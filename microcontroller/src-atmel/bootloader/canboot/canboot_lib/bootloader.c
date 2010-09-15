#include <avr/boot.h>
//#include "pgmspace.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>


#include "../config.h"
#include "bootloader.h"

#include "can.h"
#include "util.h"


unsigned char Station_id;


typedef struct
{
	unsigned char cmd;
	unsigned int index;
	unsigned int size;
	unsigned int address;
} sdo_message;

typedef struct
{
	unsigned int data[4];
} sdo_data_message;


//unsigned char Device_info_msg[] __attribute__ ((section (".progdata"))) =
unsigned char Device_info_msg[] PROGMEM =
{
	SDO_CMD_REPLY,
	SDO_TYPE_UINT32_RO,
	(unsigned char)(SPM_PAGESIZE),
	(unsigned char)(SPM_PAGESIZE >> 8),
	(FLASHEND + 1) / 1024, //changed this from Atmega number to real Flash-size in kB
	0
};

//unsigned char Flash_info_msg[] __attribute__ ((section (".progdata"))) =
unsigned char Flash_info_msg[] PROGMEM =
{
	SDO_CMD_REPLY,
	SDO_TYPE_STRING_WO,
#if (FLASHEND >= 0xffff)
	0xff,0xff //dirty hack : return 65535 bytes instead of 65536 because we used to small sized integer...
#else
	(unsigned char)((unsigned char)FLASHEND + 1),
	((unsigned int)FLASHEND + 1) >> 8
#endif
};



void bootloader(void) {
	uint16_t Address;
	uint16_t Size;
	unsigned char x;
	
	asm volatile(
		"eor r1,r1    \n\t"
		"out 0x3f, r0 \n\t"
		"out 0x3e, %B0\n\t"
		"out 0x3d, %A0\n\t"
		::"w" (RAMEND)
	);

#ifdef TURN_OFF_WATCHDOG
	//disable watchdog (it is NOT turned off on mega 644 through reset)
	wdt_reset();
	MCUSR = 0;
	_WD_CONTROL_REG = _BV(_WD_CHANGE_BIT) | _BV(WDE);
	_WD_CONTROL_REG = 0;
#endif

	//don't use library function to read eeprom
	//because it wouldn't end up in bootloader section
	EEAR = EEPR_ADDR_NODE;
	EECR = (1<<EERE);
	Station_id = EEDR;
		
	can_init();
	
	Tx_msg.addr_src = Station_id;
	Tx_msg.addr_dst = 0;
	Tx_msg.port_src = PORT_MGT;
	Tx_msg.port_dst = PORT_MGT;
	Tx_msg.dlc = 1;
	Tx_msg.data[0] = FKT_MGT_AWAKE;
	
	can_transmit();
	
	unsigned char count = 20;
	#if defined(TOGGLE_MCP_LED)
		unsigned char toggle = 0x1C;
	#elif defined(TOGGLE_PORT_LED)
		DDR_LED |= (1<<BIT_LED);
	#endif
	while (count--) {
		#if defined(TOGGLE_MCP_LED)
			mcp_write(BFPCTRL, toggle);
			toggle ^= 0x10;
		#elif defined(TOGGLE_PORT_LED)
			PORT_LED ^= (1<<BIT_LED);
		#endif
		_delay_ms(100);
		
		if (can_get_nb()) {
			goto sdo_server;
		}
	}
	
	start_app:
	asm volatile(JUMP_OPCODE " __vectors\r\t");
	
	sdo_server:
	
	while (1) {
		if (Rx_msg.port_dst == PORT_SDO_CMD) {
			sdo_message * msg = (sdo_message*)Rx_msg.data;
			
			Tx_msg.port_src = PORT_SDO_CMD;
			Tx_msg.addr_dst = Rx_msg.addr_src;
			Tx_msg.port_dst = Rx_msg.port_src;
			
			
			if (msg->cmd == SDO_CMD_READ) {
				switch (msg->index) {
					case 0xFF00:	//device information
						my_memcpy_P(sizeof(Device_info_msg), Tx_msg.data, Device_info_msg);
						Tx_msg.dlc = sizeof(Device_info_msg);
						break;
					case 0xFF01:	//upload flash
						my_memcpy_P(sizeof(Flash_info_msg), Tx_msg.data, Flash_info_msg);
						Tx_msg.dlc = sizeof(Flash_info_msg);
						break;
					case 0xFF02:
						goto start_app;
					default:
						Tx_msg.dlc = 1;
						Tx_msg.data[0] = SDO_CMD_ERROR_INDEX;
						break;
				}
				can_transmit();
			} else if (msg->cmd == SDO_CMD_WRITE_BLK) {
				if (msg->index == 0xFF01) {
					Address = msg->address;
					Size = msg->size;
					Tx_msg.dlc = 1;
					Tx_msg.data[0] = SDO_CMD_WRITE_BLK_ACK;
					can_transmit();
					goto programm;
				} else {
					Tx_msg.dlc = 1;
					Tx_msg.data[0] = SDO_CMD_ERROR_INDEX;
					can_transmit();
				}
			}
		}
		while (!can_get_nb());
	}

	programm:

	while (1) {
		while (!can_get_nb());
		if (Rx_msg.port_dst != PORT_SDO_DATA) {
			boot_rww_enable ();
			goto sdo_server;
		} else {
			for (x = 0; x < 4; x++) {
				boot_page_fill(Address, ((sdo_data_message*)Rx_msg.data)->data[x]);
				Address += 2;
			}

			if ((Address % SPM_PAGESIZE) == 0) {
				boot_page_erase (Address - SPM_PAGESIZE);
				boot_spm_busy_wait ();      // Wait until the memory is erased.
				boot_page_write (Address - SPM_PAGESIZE);     // Store buffer in flash page.
				boot_spm_busy_wait();       // Wait until the memory is written.
			}

			can_transmit();		//this will repeat the ACK message
		}
	}
}

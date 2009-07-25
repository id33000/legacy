onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -format Logic /top_top_sch_tb/clk50
add wave -noupdate -format Logic /top_top_sch_tb/clk20
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/winkel
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/winkel_diag
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/visual_winkel
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/freeze_diag
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/counter_diag
add wave -noupdate -format Logic /top_top_sch_tb/rdy_diag
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/sram_read
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/sram_pos
add wave -noupdate -format Logic /top_top_sch_tb/b10code
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/b8_code_diag
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/decode_data
add wave -noupdate -format Literal -radix unsigned /top_top_sch_tb/decode_counter
add wave -noupdate -format Logic /top_top_sch_tb/decode_rdy
add wave -noupdate -format Logic /top_top_sch_tb/decode_synchron
add wave -noupdate -format Logic /top_top_sch_tb/decode_crc_ok
add wave -noupdate -format Logic /top_top_sch_tb/decode_crc_er
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {133935 ps} 0}
configure wave -namecolwidth 289
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {80092196 ps} {96892196 ps}

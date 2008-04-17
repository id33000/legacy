# Makefile for Entropium PRNG
ALGO_NAME := ENTROPIUM

# comment out the following line for removement of PRNG from the build process
PRNGS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := entropium.o sha256-asm.o
$(ALGO_NAME)_TEST_BIN := main-entropium-test.o debug.o uart.o serial-tools.o \
                         sha256-asm.o entropium.o cli.o performance_test.o
                         
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"


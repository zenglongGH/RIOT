include ../Makefile.tests_common

# some boards have not enough rom and/or ram
BOARD_BLACKLIST += nucleo32-f031 nucleo32-f042 nucleo32-l031 pic32-clicker
BOARD_INSUFFICIENT_MEMORY += arduino-duemilanove arduino-uno

# override PRNG if desired (see sys/random for alternatives)
# USEMODULE += prng_minstd

USEMODULE += fmt
USEMODULE += printf_float
USEMODULE += random
USEMODULE += shell
USEMODULE += xtimer

FEATURES_OPTIONAL += periph_hwrng

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py

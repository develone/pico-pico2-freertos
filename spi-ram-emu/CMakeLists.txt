add_library(spi-ram-emu INTERFACE)
target_include_directories(spi-ram-emu INTERFACE ${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib)
target_sources(spi-ram-emu INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib/logic.c
	${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib/logic.h
	${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib/pio_spi.c
	${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib/pio_spi.h
	${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib/sram.c
	${CMAKE_CURRENT_LIST_DIR}/spi-ram-emu-lib/sram.h
		 
)



# STM32 BLUEPILL CLONE STM32F103C6T6


board_runner_args(jlink "--device=STM32F103C6" "--speed=4000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
## Module 5 Custom Boards - Task 1:

```text
Create a custom board using the "Copy/Rename" method
Build the hello world sample for said board
Place the board directory in <project_root>/boards/
Push and tag it as l5-task1
```

### 1. Copy folder from:
```bash
cp -r ~/zephyr/boards/others/stm32_min_dev ~/zephyr-course/boards/stm32_bluepill
```
### 2. Should look like:
```shell
boards/
└── stm32_bluepill
    ├── board.cmake
    ├── board.yml
    ├── doc
    │   ├── img
    │   │   ├── stm32_min_dev.jpg
    │   │   └── stm32_min_dev_pinout_blue.jpg
    │   └── index.rst
    ├── Kconfig
    ├── Kconfig.defconfig
    ├── Kconfig.stm32_bluepill
    ├── revision.cmake
    ├── stm32_bluepill_defconfig
    ├── stm32_bluepill.dts
    ├── support
    │   └── openocd.cfg
    └── twister.yaml
```

### 3 . Modify CMake Path
```cmake
list(APPEND BOARD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
```
### 4. Overlay for the 10K RAM Version:
> inside : `app/boards/stm32_bluepill.overlay`

```cpp
&sram0 {
	reg = <0x20000000 DT_SIZE_K(10)>;
};
```

### 5. Build with the new board config
```bash
west build --board stm32_bluepill app -p
```

### 6. Flash it
```bash
st-flash --connect-under-reset --reset write build/zephyr/zephyr.bin 0x8000000
```
Confirmed the LED blink speed changes in the board.
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

---

## Module 5 Custom Boards - Task 2:

```text
Create a custom board using the "From Scratch" method
Build the hello world sample for said board
It must also print a message ("Board Initialized") before entering the main entry point of the application
Push anad tag it as l5-task2
```

### 1. Create new folder with the new board definitions
```bash
 ~/zephyr-course/boards/bluepill_c6
```
Should look like: 
```shell
boards/bluepill_c6/
├── bluepill_c6_defconfig
├── bluepill_c6.dts
├── board.cmake
├── board.yml
├── Kconfig.bluepill_c6
├── Kconfig.defconfig
└── twister.yaml
```


### 2. Modify de LED and the 10K RAM Version:
> inside : `app/boards/bluepill.dts`

```dts
	leds {
		compatible = "gpio-leds";
		led: led {
			gpios = <&gpioc 13 GPIO_ACTIVE_LOW>;
			label = "LED PIN 13";
		};
	};
```   
```dts
&sram0 {
	reg = <0x20000000 DT_SIZE_K(10)>;
};
```
### 3. In `app/src/main.cpp`, add a `SYS_INIT` that prints before `main()`:
```cpp
   static int board_initialized_msg(void)
   {
   	printk("Board Initialized\n");
   	return 0;
   }
   SYS_INIT(board_initialized_msg, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
```

> **Note — `prj.conf` change:** `CONFIG_PRINTK=y` must be set in `app/prj.conf`.
> Without it, `printk()` output (including `Board Initialized`) is not sent to the console.


### 4. Build with the new board config and flash it
```bash
west build --board bluepill_c6 app -p
```
```bash
st-flash --connect-under-reset --reset write build/zephyr/zephyr.bin 0x8000000
```
Confirmed the LED blink speed changes in the board.

### 5. Open the serial console with the Arduino as USB-Serial converter and power-cycle the board:
```bash
   minicom -D /dev/ttyACM0 -b 115200
```

You should see `Board Initialized` before the `LED state: ON/OFF` lines.

> **Note — viewing the boot log:** `Board Initialized` is printed once during
> boot, before `main()`. Open the serial console **first** (minicom or `cat`
> with `stty -F /dev/ttyACM0 115200 raw -echo`), then unplug and replug the
> 5V power of the Blue Pill. Opening the port after the board has already
> booted misses this message.
>
> Expected output after a power cycle:
> ```
> Board Initialized
> *** Booting Zephyr OS build v4.2.0 ***
> [00:00:00.000,000] <inf> main: LED state: OFF
> ```

---
> Notes: Before every build execute: `source ~PATH .venv/bin/activate`
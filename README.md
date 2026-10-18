# Module 6 – Task 1: `my_driver` Sensor Driver

A sensor driver that controls the on-board LED of the Blue Pill. `sensor_sample_fetch` turns the LED on, and `sensor_channel_get` turns it off.

## Summary of Steps

1. **Devicetree:** `app/app.overlay` declares the `my_driver0` node with `compatible = "my,driver"`. The binding lives in `dts/bindings/my,driver.yaml`.
2. **CMake:** `app/CMakeLists.txt` adds `DTS_ROOT` and the `drivers/my_driver` subdirectory.
3. **Kconfig:** `app/Kconfig` defines `MY_DRIVER`. `app/prj.conf` enables `CONFIG_SENSOR=y` and `CONFIG_MY_DRIVER=y`.
4. **Driver** (`app/drivers/my_driver/my_driver.c`):
   - `sample_fetch` turns the LED on.
   - `channel_get` turns the LED off.
   - `init` configures the pin and logs `==DRIVER_INITIALIZED==`.
5. **Application** (`app/src/main.cpp`): calls `sensor_sample_fetch` and `sensor_channel_get` alternately, once per second.
6. **Board:** `boards/bluepill_c6/` is the custom board used for the build.

## Build, Flash, and Monitor

Before every build, activate the environment:

```bash
source ~/IOMICO/.venv/bin/activate
```

**1. Build**

```bash
cd ~/IOMICO/zephyr-course
west build -b bluepill_c6 app -p
```

**2. Flash**

```bash
st-flash --connect-under-reset --reset write build/zephyr/zephyr.bin 0x8000000
```

**3. Open the serial console**

Open minicom first, then power-cycle the board (unplug and replug its power) so the boot messages are captured:

```bash
minicom -D /dev/ttyACM0 -b 115200
```

Expected output:

```
==DRIVER_INITIALIZED==
*** Booting Zephyr OS build v4.2.0 ***
[00:00:00.000,000] <inf> main: sample_fetch ret=0 (LED ON)
[00:00:01.000,000] <inf> main: channel_get ret=0 (LED OFF)
[00:00:02.000,000] <inf> main: sample_fetch ret=0 (LED ON)
```

To exit minicom: `Ctrl-A`, then `Q`.
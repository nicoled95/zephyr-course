# Zephyr Training Environment

Welcome to the Zephyr RTOS training! This repository includes a ready-to-use
development environment based on Zephyr 4.3.0, which you can set up in one of
three ways:

---

## Manual Zephyr Setup

Follow the following guide:
- [Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#).

Make sure to select appropriate OS and to perform all steps till
[Build the Blinky Sample](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#build-the-blinky-sample).

---

## Task: Configurable Heartbeat LED

Added a configurable heartbeat LED, decoupled from the board's default LED
alias:

- **`app/app.overlay`** — adds an `app-led` alias pointing to the board's
  actual LED node (`&led`), so the app never hardcodes a board-specific pin.
- **`app/Kconfig`** — adds `CONFIG_APP_HEARTBEAT_PERIOD_MS` (default 500,
  range 100–2000).
- **`main.cpp`** — uses `DT_ALIAS(app_led)` for the GPIO and
  `CONFIG_APP_HEARTBEAT_PERIOD_MS` for the sleep duration.

### app.overlay not auto-detected
On this setup, `app/app.overlay` wasn't picked up automatically by `west
build`. Fixed permanently by adding this line to `app/CMakeLists.txt`,
**before** `find_package(Zephyr REQUIRED)`:
```cmake
set(EXTRA_DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/app.overlay")
```
Alternative (without editing `CMakeLists.txt`), pass it per-build instead:
```bash
west build --board stm32_min_dev@blue app -p -- -DEXTRA_DTC_OVERLAY_FILE="app.overlay"
```

### Verifying the period is configurable
```bash
west build -t menuconfig   # change APP_HEARTBEAT_PERIOD_MS, save & exit
west build --board stm32_min_dev@blue app  # rebuild 
st-flash --connect-under-reset --reset write build/zephyr/zephyr.bin 0x8000000
```
Confirmed the LED blink speed changes accordingly (e.g. 500ms → 2000ms).
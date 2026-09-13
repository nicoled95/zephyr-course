# Zephyr Training Environment

Development environment for the Zephyr RTOS training, based on Zephyr 4.2.0.

---

## Environment Setup

Before running any `west` command, activate the Python virtual environment
(shared across workspaces):

```bash
source ~/IOMICO/zephyrproject/.venv/bin/activate
```

You'll know it's active when you see `(.venv)` at the start of your shell
prompt. Required every time you open a new terminal session.

---

## Hardware Notes (Blue Pill clone — STM32F103C6T6)

My board is a Blue Pill clone with an **STM32F103C6T6** chip (32 KB Flash /
10 KB RAM), not the C8T6 (64 KB Flash / 20 KB RAM) that Zephyr's
`stm32_min_dev` board target assumes by default. Two adjustments are needed:

### 1. RAM overlay
Without correcting the RAM size, Zephyr's heap init writes past the
physical 10 KB of RAM and the app crashes with a Usage Fault before
reaching `main()` (confirmed via GDB — crash inside `sys_heap_init`).

`app/boards/stm32_min_dev_stm32f103xb_blue.overlay`
```dts
&sram0 {
    reg = <0x20000000 DT_SIZE_K(10)>;
};
```

### 2. Flashing with st-flash instead of `west flash`
`west flash` (OpenOCD runner) fails on this hardware with
`Error: error writing to flash at address 0x08000000` — an incompatibility
between OpenOCD's STM32F1 flash algorithm and this ST-Link V2 clone.
`st-flash` (from `stlink-tools`) works reliably instead:

```bash
west build --board stm32_min_dev@blue app -p
st-flash --connect-under-reset --reset write build/zephyr/zephyr.bin 0x8000000
```

> Note: a wire connects the ST-Link's RST pin to the board's reset pin, but
> `st-flash` still reports `NRST is not connected`, and a plain
> `st-flash --reset write ...` fails with `Can not connect to target`.
> `--connect-under-reset` is required regardless — it may take 2–3 attempts
> to succeed.

---

## Task: LED Subsystem Kconfig Menu

Reproduced the following menu structure in `app/Kconfig`:

```
[*] LED Subsystem  --->
     LED blink sleep time (1s (medium))  --->
     [ ] Advanced LED settings  --->
          (100) LED brightness (0-100)
          (500) LED fade duration (ms)
          Expert settings  --->
               [ ] Enable LED debugging
               [ ] Custom blink pattern
```

The blink sleep time is a `choice` between `LED_BLINK_SLEEP_TIME_SLOW` (2s),
`_MEDIUM` (1s, default), and `_FAST` (500ms) — mapped to milliseconds in
`main.cpp` via `#if defined(...)`.

### Three ways to configure a Kconfig value

**1. Edit `prj.conf` (persistent, part of the project)**
```conf
CONFIG_LED_BLINK_SLEEP_TIME_FAST=y
```
Rebuild normally afterward: `west build --board stm32_min_dev@blue app -p`

**2. Pass it on the command line (one-off override)**
```bash
west build --board stm32_min_dev@blue app -p -- -DCONFIG_LED_BLINK_SLEEP_TIME_FAST=y
```
Useful for quick tests without editing files.

**3. Interactive menuconfig**
```bash
west build --board stm32_min_dev@blue app -t menuconfig
```
Opens a text UI to navigate the menu tree and toggle/select values directly.
Save and exit, then rebuild:
```bash
west build --board stm32_min_dev@blue app
```

> Note: once a build directory already exists, `--board` can be omitted —
> `west build -t menuconfig` reuses the board from the existing build config.
> This only works after at least one successful build with `--board` set.

### Other useful build targets

**`-t ram_report`** — prints a breakdown of RAM usage by symbol/section,
useful for tracking down overflows like the one we hit with the real 10 KB
SRAM on this chip:
```bash
west build -t ram_report
```
(there's also `-t rom_report` for Flash usage.)
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

## Task 1 — Notes (hardware-specific setup)

My board is a Blue Pill clone with an **STM32F103C6T6** chip (32 KB Flash / 10 KB RAM),
not the C8T6 (64 KB Flash / 20 KB RAM) that Zephyr's `stm32_min_dev` board target assumes
by default. Two adjustments were needed to get this running:

### 1. RAM overlay
Without correcting the RAM size, Zephyr's heap init writes past the physical 10 KB of RAM
and the app crashes with a Usage Fault before reaching `main()` (confirmed via GDB —
crash inside `sys_heap_init`). Fixed with a board overlay:

`app/boards/stm32_min_dev_stm32f103xb_blue.overlay`
```dts
&sram0 {
    reg = <0x20000000 DT_SIZE_K(10)>;
};
```

### 2. Flashing with st-flash instead of `west flash`
`west flash` (OpenOCD runner) fails on this hardware with
`Error: error writing to flash at address 0x08000000` — likely an incompatibility
between OpenOCD's STM32F1 flash algorithm and this particular ST-Link V2 clone.
`st-flash` (from `stlink-tools`) works reliably instead:

```bash
west build --board stm32_min_dev@blue app -p
st-flash --connect-under-reset --reset write build/zephyr/zephyr.bin 0x8000000
```

Note: a wire is connected between the ST-Link's RST pin and the board's reset
pin, but `st-flash` still reports `NRST is not connected` and a plain
`st-flash --reset write ...` fails with `Can not connect to target`. The
`--connect-under-reset` flag is required regardless — it may take 2–3 attempts
to succeed.
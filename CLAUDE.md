# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a **2026 全国大学生电子设计竞赛 (NUEDC)** entry based on the **TI MSPM0G3507** microcontroller (ARM Cortex-M0+, 48-pin LQFP). The repo spans embedded firmware, PCB design, and reference materials.

## Repository Structure

```
TI/
├── Project/
│   ├── empty/          # Fresh CCS empty template (bootstrapped from MSPM0 SDK)
│   └── TEST/           # Active firmware development — the real application
├── PCB_JLC/            # PCB design (立创EDA / JLC EDA format, .eprj2/.epro2)
└── 资料/               # References — datasheets, schematics, BOM, competition docs
```

## Build & Flash (Firmware)

The CCS projects use **TI ARM Clang Compiler** (TICLANG 4.0.4.LTS), **not GCC**. The toolchain is managed by Eclipse CDT with GNU Make.

- **Build**: Open in **Code Composer Studio** (CCS Theia 70.5.1). Build outputs `Debug/<ProjName>.out` (ELF) and `Debug/<ProjName>.map`. SysConfig runs as a pre-build step to regenerate `ti_msp_dl_config.c`/`.h`.
- **Flash/Debug**: Debug probe is **SEGGER J-Link** via SWD (PA19/SWDIO, PA20/SWCLK). Configured in `targetConfigs/MSPM0G3507.ccxml`.
- **Clean**: `make clean` from `Debug/`, or CCS clean command.
- **Key dependencies**: MSPM0 SDK v2.10.00.04, SysConfig v1.26.2, CMSIS headers.

## Firmware Architecture

Each CCS project follows the same DriverLib pattern:

- **`empty.syscfg`** — TI SysConfig file. Defines GPIO instances, peripheral assignments, pin mux, and clock config. Opened with SysConfig GUI. **Do not manually edit `ti_msp_dl_config.c`/`.h`** — they are auto-generated from this file at build time.
- **`empty.c`** — Application entry point (`main()`). Calls `SYSCFG_DL_init()` then enters the main loop.
- **`Debug/ti_msp_dl_config.c`** / **`Debug/ti_msp_dl_config.h`** — Auto-generated DriverLib init code (GPIO, clock tree, pinmux). Never hand-edit.
- **`Debug/startup_mspm0g350x_ticlang.o`** — Startup vector table from MSPM0 SDK (linked as a prebuilt object).
- **`Debug/device_linker.cmd`** — Linker command file (memory layout, sections).
- **Compiler flags**: `-march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mthumb -O2 -gdwarf-3 -Wall`
- **`Debug/compile_commands.json`** — Clangd-compatible compilation database for LSP support.

## TEST Project — Active Firmware

Located at `Project/TEST/`. Controls an LED on **PA14** (active high) and is the primary development target. Already has a local `CLAUDE.md` with additional details.

## PCB Design

Located at `PCB_JLC/`. Designed in 立创EDA. The main board (`MSPM0G3507参考.eprj2`) is a two-board system:
- **主控板** (Main control board) — MSPM0G3507, power regulation (RT9013-33GB LDO), USB-UART (CH9102F), MPU6050 (I2C gyro/accelerometer), SWD debug header
- **拓展板** (Expansion board) — A4950 motor drivers, additional I/O

## Pin Assignments (TEST project)

| Pin  | Function  | Notes                       |
|------|-----------|-----------------------------|
| PA14 | GPIO OUT  | LED (active high)           |
| PA19 | SWDIO     | Debug data (J101 13:14 ON)  |
| PA20 | SWCLK     | Debug clock (J101 15:16 ON) |

## Key Peripherals (PCB)

| Chip       | Function          | Interface |
|------------|-------------------|-----------|
| MPU6050    | 6-axis IMU        | I2C       |
| CH9102F    | USB-to-UART       | UART      |
| A4950      | DC motor driver   | PWM/GPIO  |
| RT9013-33GB| 3.3V LDO regulator| —         |

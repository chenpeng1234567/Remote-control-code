# RC Handheld Remote Controller 🎮

Firmware for a handheld wireless remote controller built on the **STM32F103RGT6**, running **FreeRTOS** and an **LVGL** touch GUI. It drives a robot chassis over a 2.4 GHz **NRF24L01** link — sending joystick and mode commands downstream while displaying live telemetry (chassis battery, target coordinates) upstream.

## ✨ Features

- **Wireless control** — bidirectional NRF24L01 link (SPI): joystick direction, AUTO/MANUAL mode and emergency-stop commands sent to the chassis
- **Live telemetry** — receives chassis battery level and target X/Z coordinates from the chassis
- **Dual battery monitoring** — remote battery measured locally via ADC, chassis battery reported over the air
- **Two-page touch UI (LVGL 8.1)**
  - **Control page** — AUTO/MANUAL mode banner, STOP/GO indicator, target coordinates (mm), RC & chassis battery percentage
  - **System Monitor page** — battery arc gauge, battery bar, mode/state/uptime info, color-coded battery icon (green / yellow / red)
- **FreeRTOS task layout** — GUI task, wireless task and LED heartbeat task running concurrently
- **Debug-friendly** — 115200 baud debug UART and USMART runtime function-call debugging

## 🔧 Hardware

| Component | Details |
|---|---|
| MCU | STM32F103RGT6 (ARM Cortex-M3 @ 72 MHz, 1 MB Flash, 96 KB SRAM) |
| Display | 320×240 LCD with resistive touch |
| Wireless | NRF24L01 2.4 GHz module (SPI) |
| Input | Analog joystick (2× ADC channels), AUTO/MANUAL switch, e-stop button, page keys |
| Board | Based on the ALIENTEK (正点原子) STM32F103 development board |

## 🖼️ UI Overview

```
┌───────────────────────────┐   ┌───────────────────────────┐
│         [ AUTO ]          │   │      SYSTEM MONITOR  🔋   │
│───────────────────────────│   │           │   BASE ████ 80%│
│  X : 123 mm  Y : 456 mm   │   │   ╭───╮   │   MODE : AUTO │
│  RC : 90 %   CHAS : 80 %  │   │   │85%│   │   STATE : GO   │
│───────────────────────────│   │   ╰───╯   │   TIME : 01:23 │
│          [ GO ]           │   │                           │
└───────────────────────────┘   └───────────────────────────┘
        Control page                 Monitor page
        LEFT key to return      RIGHT key to enter
```

## 📡 Radio Protocol

**Remote → Chassis** (ASCII, 32-byte packet):

```
X:%4d Y:%4d %d %d %d
```

| Field | Meaning |
|---|---|
| `X` | Joystick X direction: `1` forward, `2` backward, `0` neutral |
| `Y` | Joystick Y direction: `1` right, `2` left, `0` neutral |
| flag 1 / 2 | AUTO / MANUAL mode select |
| flag 3 | Emergency stop |

**Chassis → Remote**:

| Packet | Meaning |
|---|---|
| `P<0-100>` | Chassis battery percentage |
| `X<x>Z<z>` | Target coordinates in mm (`X` supports negative values) |

## 📁 Project Structure

```
LVGL_stm32f103rgt6/
├── User/                # Application code (main.c, lvgl_demo.c, tasks)
├── Projects/MDK-ARM/    # Keil MDK-ARM project (atk_f103.uvprojx)
├── Drivers/
│   ├── BSP/             # Board support: LCD, touch, ADC, NRF24L01, KEY, SPI...
│   ├── CMSIS/           # CMSIS core & device files
│   └── STM32F1xx_HAL_Driver/
├── Middlewares/
│   ├── FreeRTOS/        # FreeRTOS V10.4.6
│   ├── LVGL/            # LVGL V8.1.1 + porting layer
│   ├── MALLOC/          # Memory management
│   └── USMART/          # Runtime debug component
└── Output/              # Build artifacts
```

Key source files:

- [main.c](LVGL_stm32f103rgt6/User/main.c) — clock, peripherals and RTOS bring-up
- [lvgl_demo.c](LVGL_stm32f103rgt6/User/lvgl_demo.c) — FreeRTOS tasks, LVGL UI, radio packet handling and coordinate parser

## 🚀 Build & Flash

1. Open `LVGL_stm32f103rgt6/Projects/MDK-ARM/atk_f103.uvprojx` in **Keil MDK5** (ARMCC / AC5 toolchain)
2. Build the project (F7)
3. Flash to the board via ST-Link / J-Link (or download the hex from `Output/`)

## 🙏 Acknowledgements

- [LVGL](https://github.com/lvgl/lvgl) — Light and Versatile Graphics Library
- [FreeRTOS](https://www.freertos.org/) — Real-time operating system
- [ALIENTEK](https://www.alientek.com/) — Board BSP libraries and reference examples

## 📄 License

Released under the [MIT License](LICENSE).

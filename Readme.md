# 🕹️ Chip-8 Emulator

A **Chip-8** interpreter/emulator written in C++ using SDL2.  
Play classic games – all in a pixel‑perfect 64×32 window with keyboard input and sound.

---

## ✨ Features

- ✔️ Full Chip‑8 instruction set (35 opcodes)
- ✔️ Realistic **60 Hz** execution timing
- ✔️ **SDL2** graphics – smooth, hardware‑accelerated rendering
- ✔️ **Beep sound** (440 Hz) when the sound timer is active
- ✔️ Standard **keyboard mapping** (1‑4, Q‑R, A‑F, Z‑V)
- ✔️ Accurate **delay** and **sound** timers
- ✔️ Clean, modular code – audio and emulation logic are separated
- ✔️ Supports both **file‑based** and **in‑memory** ROM loading

---

## 🎮 Key Mapping

The emulator uses the traditional Chip‑8 keypad layout mapped to your physical keyboard:

| Chip‑8 Key | Physical Key |
| ---------- | ------------ |
| 0x1        | `1`          |
| 0x2        | `2`          |
| 0x3        | `3`          |
| 0x4        | `4`          |
| 0x5        | `Q`          |
| 0x6        | `W`          |
| 0x7        | `E`          |
| 0x8        | `R`          |
| 0x9        | `A`          |
| 0xA        | `S`          |
| 0xB        | `D`          |
| 0xC        | `F`          |
| 0xD        | `Z`          |
| 0xE        | `X`          |
| 0xF        | `C`          |
|            | `V`          |

> **Note:** Some games may expect different keys (e.g., `R` and `S` for movement in `br8kout.ch8`).  
> The mapping above is the standard one; you can adjust it if needed.

---

## 🛠️ Building from Source

### Prerequisites

- **C++17** compiler (g++, clang, or MSVC)
- **SDL2** development libraries

## 🚀 Usage

> ./chip8 ./roms/rom_name.ch8

### Example

> ./chip8 roms/PONG.ch8

## 📜 License

This project is licensed under the MIT License.

## 📧 Contact

For questions or suggestions, please open an issue on GitHub.

Feel free to give it a try and improve it if you want! 🎮

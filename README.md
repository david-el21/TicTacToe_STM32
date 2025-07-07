# Embedded Tic Tac Toe Game – STM32 (Serial Version)

This is an embedded implementation of the classic **Tic Tac Toe (X and O)** game using an **STM32F103** microcontroller with a serial interface and LED visualization.

## 🎯 Features

- **3×6 LED Matrix**: 18 LEDs used to represent the game grid
- **Serial Interface**: 
  - Game status and instructions sent over UART
  - Player input received from terminal (serial monitor)
- **Embedded Game Logic**:
  - Turn-based system for 2 players
  - Move validation and board state updates
  - Win/draw condition detection and board reset

## 🧰 Development Tools

- STM32CubeMX for peripheral and pin configuration
- Code written in C, using modular `.c/.h` files
- Compiled and built with `make` inside Visual Studio Code (VSCode)
- UART (serial) used as main interface for input/output

## 🔧 Hardware Requirements

- **STM32F103C8T6** ("Blue Pill")
- 18 individual LEDs connected to GPIO pins
- Resistors and basic wiring for LEDs
- USB-to-serial converter (if needed for debugging)
- No LCD or graphical display used

## 📄 How It Works

Each move updates the LED matrix to reflect X or O placement. All interaction is handled via the serial console: players are prompted to choose a position, and the system verifies, updates the board, and announces results through UART messages.

## 🚀 Future Improvements

- Add rotary encoder or matrix keypad for local input
- Integrate timer interrupts for smoother navigation
- Add a game replay or score tracking feature

## 👤 Author

**Florian David** 
**Coman Rares Andrei**
Embedded Systems – Group 433E  
University POLITEHNICA of Bucharest – 2025

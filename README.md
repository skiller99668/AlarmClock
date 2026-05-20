# AlarmClock
 
A custom ESP32-based alarm clock with a math problem wake-up challenge, LED sunrise simulation, and audio playback.
 
## Features
 
- **Math problem alarm** — the alarm won't stop until you solve a randomly generated arithmetic problem on the keypad
- **LED sunrise simulation** — 15 minutes before the alarm, a WS2812B LED strip gradually brightens from dim amber to warm white to simulate a natural sunrise
- **LCD display** — shows the current time, alarm time, and math problem
- **4x4 keypad input** — used to set the time, set the alarm, and answer the math problem
- **Audio** — passive buzzer for alarm sound, with DFPlayer Mini + MP3 playback planned
- **RTC timekeeping** — DS3231 module keeps accurate time even when unplugged
## Hardware
 
| Component | Part |
|---|---|
| Microcontroller | ESP32 DevKit (CP2102) |
| Display | LCD1602 with I2C adapter |
| Input | 4x4 membrane keypad |
| Lighting | WS2812B LED strip |
| Audio | Passive buzzer (DFPlayer Mini + speaker planned) |
| Timekeeping | DS3231 RTC module |
| Power | USB-C 5V |
 
## Wiring
 
| Component | ESP32 Pin |
|---|---|
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| RTC SDA | GPIO 21 |
| RTC SCL | GPIO 22 |
| Keypad R1-R4 | GPIO 19, 18, 5, 17 |
| Keypad C1-C4 | GPIO 16, 4, 0, 2 |
| Buzzer | GPIO 12 |
| LED strip DIN | GPIO 15 |
 
All I2C devices (LCD and RTC) share the same SDA/SCL pins at address `0x27` (LCD) and `0x68` (RTC).
 
## Software
 
Built with PlatformIO in VSCode. Simulated using Wokwi.
 
### Dependencies
 
```ini
lib_deps =
    marcoschwartz/LiquidCrystal_I2C
    chris--a/Keypad
    adafruit/RTClib
    fastled/FastLED
```
 
### File Structure
 
```
src/
├── main.cpp              # setup, loop, state management
├── alarmSequence.cpp     # alarm trigger, buzzer, LED sunrise logic
├── alarmSequence.h
├── equationMaker.cpp     # math problem generation and answer checking
├── equationMaker.h
├── setTime.cpp           # time and alarm setting via keypad
└── setTime.h
```
 
## Usage
 
### Setting the time
On boot, press `D` to enter time setting mode. Enter the hour (0-23) followed by `#`, then the minute followed by `#`.
 
### Setting the alarm
Press `A` to enter alarm setting mode. Enter the hour followed by `#`, then the minute followed by `#`.

### Checking alarm
Press `B` to check when an alarm was set, or if an alarm has been set at all.
 
### Alarm sequence
1. 15 minutes before the alarm, the LED strip begins brightening
2. At the alarm time, the buzzer sounds and a math problem appears on the LCD
3. Solve the problem using the keypad and press `#` to submit
4. The alarm stops when the correct answer is entered
### Keypad layout
```
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```
 
In Clock Mode: 
- `A` — Set alarm/Cancel alarm
- `B` — View set alarm time 
- `D` — Set new time

In Alarm/Time Setting Mode:
- `#` — Confirm / Submit
- `A` — Backspace
- `D` — cancel

When Alarm Sounds (Equation Mode):
- `#` — Confirm / Submit
- `A` — Backspace
- `*` — Stop Alarm 
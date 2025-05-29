# Touchscreen Pomodoro Timer using Arduino GIGA R1 WiFi and Display Shield

## Overview

This project is a productivity-enhancing timer built using the Arduino GIGA R1 WiFi and its official Display Shield. It applies the **Pomodoro Technique**, a time management method that alternates between focused work sessions and short breaks. The timer helps users concentrate for 40 minutes followed by a 20-minute break, promoting balance, reducing procrastination, and improving focus.

## Motivation

Many students struggle with time management, focus, and procrastination—especially those affected by **ADHD**. These issues can lead to stress, low academic performance, and difficulty staying on task. Research shows that procrastination is strongly linked to poor grades, low motivation, and mental fatigue.

This project aims to offer a simple and effective solution: a timer that blends into a student’s workspace and offers structure with minimal interaction. By using common objects like a **pen holder** or **touch interaction**, it reinforces good study habits without being intrusive.

## How It Works

The timer comes in two versions:

### 1. Push Button Version
- Press a pen or finger on a tactile button to start a 40-minute work session.
- A buzzer sounds (2 kHz for 3 seconds) when the session ends.
- Press the button again to start a 20-minute break.
- After the break, the system resets and waits for the next session.
- Includes touchscreen controls (Stop, Resume, End, 20 MIN, 40 MIN).

### 2. Ultrasonic Sensor Version
- Uses an HC-SR04 sensor to detect whether a pen is in place.
- Removing the pen (distance > 10 cm) starts the 40-minute timer.
- Replacing the pen (distance < 10 cm) starts the 20-minute break.
- The buzzer sounds after each timer completes.
- Fully automatic with no need for direct touch.

## Features

- Touchscreen UI with real-time countdown
- STOP, RESUME, END, 20 MIN, and 40 MIN controls
- 2 kHz buzzer for notifications
- Idle mode with dark screen for energy efficiency
- Designed in Arduino C++ and fully open-source

## Applications

This project helps students:
- Develop self-discipline and focus
- Structure their study sessions
- Reduce procrastination
- Improve academic performance
- Manage energy and reduce screen fatigue

## Libraries Used

Installable via Arduino Library Manager:

- Arduino_GigaDisplay_GFX
- Arduino_GigaDisplayTouch

## Implementation Language

The code is written in C++ using the Arduino development environment.

##  Pictures

![UI Images](docs/ui-example.png) 
![timer](https://github.com/user-attachments/assets/2c0c77ee-deeb-4eb7-bcf1-50dc723c4b87)
![timer2](https://github.com/user-attachments/assets/45b0814c-3b1e-4dcb-b8bb-8ca5ecaf4581)

---

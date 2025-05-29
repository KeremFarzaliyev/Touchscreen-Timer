# 🕒 Touchscreen Pomodoro Timer (Arduino GIGA R1 WiFi + Display Shield)

This project is a touchscreen-based **Pomodoro-style timer** built using the Arduino GIGA R1 WiFi and GIGA Display Shield. It features two versions for triggering the timer: one using a **push button** and another using an **ultrasonic distance sensor**.

---

## 📦 Features

- ⏱️ 40-minute work + 20-minute break timer cycle  
- 🖐️ Touchscreen interface with STOP, RESUME, END, 20 MIN, 40 MIN buttons  
- 🔊 2 kHz buzzer alarm for 3 seconds when each timer ends  
- 🔌 Idle mode (screen clears when not in use)  
- 👆 Button-activated version for tactile pen presses  
- 📡 Sensor-activated version using object distance (>10 cm)

---

## 📁 Project Versions

### 1. `push_button_timer.ino`
- Uses a **tactile button** (e.g. pressed by a pen) on digital pin `D2`
- Ideal for physical activation when removing the pen

### 2. `ultrasonic_timer.ino`
- Uses an **HC-SR04 ultrasonic sensor**
  - `TRIG` → pin `D9`
  - `ECHO` → pin `D8`
- Automatically starts timer when object is removed (distance > 10 cm)
- Starts 20-min break timer when object is returned (distance < 10 cm)

---

## 🔧 Hardware Required

- Arduino GIGA R1 WiFi  
- GIGA Display Shield  
- Passive Buzzer (connected to D30)  
- Either:
  - Tactile push button (connected to D2 + GND), or  
  - HC-SR04 ultrasonic sensor (TRIG to D9, ECHO to D8)  
- Jumper wires and optional breadboard  

---

## 📦 Libraries Used

Install these from the Arduino Library Manager:

- `Arduino_GigaDisplay_GFX`  
- `Arduino_GigaDisplayTouch`

---

## 🧠 How It Works

- Pressing the button or removing the pen triggers a **40-minute work timer**
- After time is up, a **buzzer sounds**
- Press the button (or return the object) to start a **20-minute break**
- After break ends, system resets to idle mode
- Touchscreen can also be used to manually pause/resume/end timers

---

## 📸 Screenshots

![UI Screenshot](docs/ui-example.png) *(Optional if you upload screenshots)*

---

## 📄 License

This project is open source under the [MIT License](LICENSE).

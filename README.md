# automotive-cruise-control-pid
🚗 Automotive Cruise Control System (PID)
📌 Project Overview

This project implements a closed-loop cruise control system using a discrete PID controller. The system regulates vehicle speed through PWM motor control on Arduino and was validated using MATLAB/Simulink modeling.

⚙️ Technologies Used

Arduino (Embedded C)

MATLAB

Simulink

Tinkercad (Hardware Simulation)

🧠 Control Strategy

Discrete PID controller

Anti-windup protection

Ramp limiter (acceleration limit)

Derivative filtering

Road slope disturbance simulation

📊 System Model

The vehicle dynamics were modeled in Simulink using a first-order transfer function:

G(s) = K / (T s + 1)

The controller was tuned to ensure:

Stable response

Minimal overshoot

Reduced steady-state error

Realistic automotive acceleration behavior

🔬 Features

Speed setpoint adjustment (+/- 10 km/h)

Double-click reset

LCD real-time display

PWM motor control

Disturbance rejection (road slope simulation)

📷 Simulation

Project tested using Tinkercad hardware simulation and MATLAB/Simulink dynamic modeling.

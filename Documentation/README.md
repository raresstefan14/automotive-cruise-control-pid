
System Description

The cruise control system is based on a closed-loop PID controller implemented in discrete time.

Main components:

Speed reference input (setpoint)

Ramp limiter for acceleration control

PID controller (Kp, Ki, Kd)

PWM motor control

Vehicle dynamic model

Road slope disturbance simulation

Control equation:

u(k) = Kp·e(k) + Ki·Σe(k)·Ts + Kd·(e(k)-e(k-1))/Ts

Plant model:

G(s) = K / (T s + 1)

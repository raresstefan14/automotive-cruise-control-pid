#include <LiquidCrystal.h>

// ---------------- PIN DEFINITIONS ----------------
#define ENA 5
#define IN1 6
#define IN2 7

#define BUZZER 4

#define BTN_SET 8
#define BTN_CANCEL 9
#define BTN_RESUME 10

#define LED_GREEN 11
#define LED_RED 12

LiquidCrystal lcd(A1, A2, A3, A4, A5, 13);

// ---------------- VARIABLES ----------------
float targetSpeed = 0;
float currentSpeed = 0;
float speedCommand = 0;

bool cruiseActive = false;

// ---------------- PID PARAMETERS ----------------
float Kp = 2.6;
float Ki = 0.30;
float Kd = 0.15;

float error = 0;
float previousError = 0;
float integral = 0;

float derivativeFiltered = 0;
float derivativeAlpha = 0.7;

int pwm = 0;

// ---------------- CONTROL TIMING ----------------
unsigned long lastControlTime = 0;
const float Ts = 0.05;

// ---------------- RAMP LIMITER ----------------
float maxAccel = 20.0;

// ---------------- ROAD DISTURBANCE ----------------
float roadSlope = 0;
float slopeTimer = 0;

// ---------------- DOUBLE CLICK ----------------
unsigned long lastResumePress = 0;
const unsigned long doubleClickTime = 400;

// ---------------- MOTOR PARAMETERS ----------------
float maxSpeed = 150.0;

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(BTN_SET, INPUT_PULLUP);
  pinMode(BTN_CANCEL, INPUT_PULLUP);
  pinMode(BTN_RESUME, INPUT_PULLUP);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  lcd.begin(16,2);
  lcd.print("Cruise Ready");

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  randomSeed(analogRead(0));

  delay(1500);
  lcd.clear();
}

// ---------------- MAIN LOOP ----------------
void loop() {

  // ---------------- SERIAL INPUT ----------------
  if (Serial.available() > 0) {
    int newSpeed = Serial.parseInt();
    if (newSpeed >= 0 && newSpeed <= maxSpeed) {
      targetSpeed = newSpeed;
      cruiseActive = true;
      integral = 0;
    }
  }

  // ---------------- BUTTON CONTROL ----------------
  if (digitalRead(BTN_SET) == LOW) {
    targetSpeed += 10;
    if (targetSpeed > maxSpeed) targetSpeed = maxSpeed;
    tone(BUZZER, 1000, 150);
    cruiseActive = true;
    delay(250);
  }

  if (digitalRead(BTN_CANCEL) == LOW) {
    targetSpeed -= 10;
    if (targetSpeed < 0) targetSpeed = 0;
    tone(BUZZER, 800, 150);
    delay(250);
  }

  if (digitalRead(BTN_RESUME) == LOW) {

    unsigned long now = millis();

    if (now - lastResumePress < doubleClickTime) {
      targetSpeed = 0;
      cruiseActive = false;
      integral = 0;
      pwm = 0;
      tone(BUZZER, 500, 300);
    } 
    else {
      cruiseActive = !cruiseActive;
      tone(BUZZER, 1200, 150);
    }

    lastResumePress = now;
    delay(250);
  }

  // ---------------- CONTROL LOOP ----------------
  if (millis() - lastControlTime >= Ts * 1000) {

    lastControlTime = millis();

    if (cruiseActive) {

      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);

      float maxStep = maxAccel * Ts;

      if (speedCommand < targetSpeed) {
        speedCommand += maxStep;
        if (speedCommand > targetSpeed)
          speedCommand = targetSpeed;
      }
      else if (speedCommand > targetSpeed) {
        speedCommand -= maxStep;
        if (speedCommand < targetSpeed)
          speedCommand = targetSpeed;
      }

      error = speedCommand - currentSpeed;

      if (abs(error) < 0.5)
        error = 0;

      integral += error * Ts;
      integral = constrain(integral, -300, 300);

      float derivative = (error - previousError) / Ts;

      derivativeFiltered =
          derivativeAlpha * derivativeFiltered
        + (1 - derivativeAlpha) * derivative;

      float output =
          Kp * error
        + Ki * integral
        + Kd * derivativeFiltered;

      pwm = constrain(output, 0, 255);

      if ((pwm == 255 && error > 0) ||
          (pwm == 0 && error < 0)) {
        integral -= error * Ts;
      }

      analogWrite(ENA, pwm);

      previousError = error;

      float motorTargetSpeed = (pwm / 255.0) * 200.0;
      currentSpeed += (motorTargetSpeed - currentSpeed) * 0.12;

      slopeTimer += Ts;

      if (slopeTimer > 8) {
        slopeTimer = 0;
        roadSlope = random(-5, 6);
      }

      currentSpeed -= roadSlope * 0.02;

      if (currentSpeed < 0)
        currentSpeed = 0;
    }
    else {

      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);

      analogWrite(ENA, 0);

      currentSpeed *= 0.97;
      speedCommand = currentSpeed;
    }
  }

  // ---------------- SERIAL DEBUG OUTPUT ----------------
  Serial.print(currentSpeed);
  Serial.print(",");
  Serial.print(targetSpeed);
  Serial.print(",");
  Serial.println(pwm);

  // ---------------- LCD DISPLAY ----------------
  lcd.setCursor(0,0);
  lcd.print("Speed:");
  lcd.print(currentSpeed,0);
  lcd.print("   ");

  lcd.setCursor(0,1);
  lcd.print("Set:");
  lcd.print(targetSpeed,0);
  lcd.print("   ");
}

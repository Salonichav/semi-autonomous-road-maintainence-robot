#include <Arduino.h>
#include <ESP32Servo.h>

// =========================
// Motor Driver - L298N
// =========================
#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

#define ENA 25
#define ENB 26

// =========================
// Ultrasonic Sensor
// =========================
#define TRIG_PIN 33
#define ECHO_PIN 32

// =========================
// Servo
// =========================
#define SERVO_PIN 17

Servo myServo;

// =========================
// Motor PWM
// =========================
const int PWM_FREQUENCY = 1000;
const int PWM_RESOLUTION = 8;

const int MOTOR_SPEED = 150;

// =========================
// Obstacle threshold
// =========================
const float OBSTACLE_DISTANCE = 30.0;

// =========================
// Setup
// =========================
void setup() {
    Serial.begin(115200);

    // Motor direction pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Ultrasonic
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Servo
    myServo.attach(SERVO_PIN);

    // Initial safe state
    stopMotors();
    myServo.write(90);

    // ESP32 Core 3.x PWM setup
    ledcAttach(ENA, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttach(ENB, PWM_FREQUENCY, PWM_RESOLUTION);

    Serial.println("Robot initialized.");
}

// =========================
// Read ultrasonic distance
// =========================
float readDistance() {

    // Ensure clean trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Send 10 us trigger pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Measure echo pulse
    unsigned long duration = pulseIn(
        ECHO_PIN,
        HIGH,
        30000
    );

    // Timeout / invalid reading
    if (duration == 0) {
        return -1.0;
    }

    // Convert microseconds to centimeters
    float distance = duration * 0.0343 / 2.0;

    return distance;
}

// =========================
// Set motor speed
// =========================
void setMotorSpeed(int speed) {

    speed = constrain(speed, 0, 255);

    ledcWrite(ENA, speed);
    ledcWrite(ENB, speed);
}

// =========================
// Move forward
// =========================
void moveForward(int speed) {

    // Left motor
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    // Right motor
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    setMotorSpeed(speed);
}

// =========================
// Stop motors
// =========================
void stopMotors() {

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    ledcWrite(ENA, 0);
    ledcWrite(ENB, 0);
}

// =========================
// Main loop
// =========================
void loop() {

    float distance = readDistance();

    Serial.print("Distance: ");

    if (distance < 0) {
        Serial.println("Invalid / Timeout");
    } else {
        Serial.print(distance);
        Serial.println(" cm");
    }

    // =========================
    // Obstacle detected
    // =========================
    if (distance > 0 && distance < OBSTACLE_DISTANCE) {

        Serial.println("Obstacle detected!");
        Serial.println("Stopping robot.");

        stopMotors();

        // Move servo to inspection position
        myServo.write(90);
    }

    // =========================
    // No obstacle
    // =========================
    else {

        Serial.println("Path clear.");

        moveForward(MOTOR_SPEED);

        // Normal servo position
        myServo.write(0);
    }

    delay(50);
}

#include <Arduino.h>
#include <ESP32Servo.h>

// Motor pins
#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27
#define ENA 25
#define ENB 26

#define TRIG_PIN 33
#define ECHO_PIN 32

#define SERVO_PIN 17

Servo myservo;

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myservo.attach(SERVO_PIN);

  // PWM setup (LEDC) ESP32
  ledcSetup(0, 1000, 8);     // channel, freq, resolution
  ledcSetup(1, 1000, 8);

  ledcAttachPin(ENA,0);
  ledcAttachPin(ENB,1);
}

long readDistance(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long dur=pulseIn(ECHO_PIN, HIGH, 30000);
  return dur*0.034/2;
}

void forward(int spd){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  ledcWrite(0, spd);
  ledcWrite(1, spd);
}

void stopMotors(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  ledcWrite(0,0);
  ledcWrite(1,0);
}

void loop(){
  long d = readDistance();
  Serial.println(d);

  if(d >0 && d<30){
    stopMotors();
    myservo.write(90);
  }else{
    forward(150);
    myservo.write(0);
  }
  delay(100);
}
#include <AFMotor.h>
#include <NewPing.h>

AF_DCMotor motor1(3, MOTOR12_1KHZ);
AF_DCMotor motor2(4, MOTOR12_1KHZ);

#define trigkanan A4
#define echokanan A5

#define trigtengah A2
#define echotengah A3

#define trigkiri A0
#define echokiri A1
// #define MaxDistance 100

// ini untuk menyeimbangkan ketika robot maju
#define kecepatanKanan 255
#define kecepatanKiri 255

void setup() {
    Serial.begin(9600);
    pinMode(trigkanan, OUTPUT);
    pinMode(echokanan, INPUT);

    pinMode(trigtengah, OUTPUT);
    pinMode(echotengah, INPUT);

    pinMode(trigkiri, OUTPUT);
    pinMode(echokiri, INPUT);
}

void loop() {
    long duration_kanan, duration_tengah, duration_kiri, kanan, tengah, kiri;
    digitalWrite(trigkanan, LOW);
    delayMicroseconds(10);
    digitalWrite(trigkanan, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigkanan, LOW);
    duration_kanan = pulseIn(echokanan, HIGH);
    kanan = duration_kanan / 29 / 2;

    digitalWrite(trigtengah, LOW);
    delayMicroseconds(10);
    digitalWrite(trigtengah, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigtengah, LOW);
    duration_tengah = pulseIn(echotengah, HIGH);
    tengah = duration_tengah / 29 / 2;

    digitalWrite(trigkiri, LOW);
    delayMicroseconds(10);
    digitalWrite(trigkiri, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigkiri, LOW);
    duration_kiri = pulseIn(echokiri, HIGH);
    kiri = duration_kiri / 29 / 2;

    if ((tengah <= 17) && (kiri <= 22)) {
        belokkanan();
    } else if ((tengah <= 17) && (kanan <= 22)) {
        belokkiri();
    } else if (kiri <= 11) {
        belokkanan();
    } else if (kanan <= 11) {
        belokkiri();
    } else if (tengah <= 17) {
        maju();  // sesuaikan dengan dominan nya
    } else {
        maju();
    }
}

void maju() {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor1.setSpeed(kecepatanKanan);
    motor2.setSpeed(kecepatanKiri);
}

void belokkanan() {  // dengan beda arah putaran motor
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
    motor1.setSpeed(100);
    motor2.setSpeed(140);
}

void belokkiri() {  // dengan beda arah putaran motor
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
    motor1.setSpeed(140);
    motor2.setSpeed(100);
}

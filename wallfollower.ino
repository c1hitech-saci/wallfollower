#include <AFMotor.h>

// uncomment baris di bawah ini untuk mengaktifkan mode debug atau feedback
#define DEBUG
#define FEEDBACK

// pin sensor ultrasonik
#define PIN_TRIG_SENSOR_KIRI A0
#define PIN_ECHO_SENSOR_KIRI A1
#define PIN_TRIG_SENSOR_DEPAN A2
#define PIN_ECHO_SENSOR_DEPAN A3
#define PIN_TRIG_SENSOR_KANAN A4
#define PIN_ECHO_SENSOR_KANAN A5

// jarak aman dengan dinding dalam satuan cm
#define JARAK_DENGAN_DINDING 5
#define JARAK_TOLERANSI 2

// interval eksekusi fungsi dalam satuan milisecond
// uncomment baris di bawah ini untuk mengaktifkan interval
// #define SENSOR_READ_INTERVAL 100
// #define CONTROL_INTERVAL 1000

// delay dalam satuan milisecond untuk setiap aksi robot
#define DELAY_SIKU_SIKU 1000
#define DELAY_BALIK_ARAH 2000

// kecepatan motor mulai dari 0 - 255
// bisa ditambah atau dikurangi sesuai kebutuhan
// atau menambahkan variable kecepatan baru
#define SPEED_MAX 255
#define SPEED_MEDIUM 180
#define SPEED_MIN 0

// deklarasi motor
// atur pin motor sesuai dengan pin yang digunakan
AF_DCMotor motorKiri(1);   // pin M1
AF_DCMotor motorKanan(2);  // pin M2

// ------------------ START: global variable ------------------
/**
 * Variable global yang digunakan untuk menyimpan data jarak sensor
 * variable ini akan berubah setiap interval waktu SENSOR_READ_INTERVAL
 */
long jarakSensorKiri;
long jarakSensorDepan;
long jarakSensorKanan;
// ------------------ END: global variable ------------------

// ------------------ START: inline function ------------------
inline void logikaKontrol() __attribute__((always_inline));
#ifdef CONTROL_INTERVAL
inline void kontrolRobot(unsigned long waktuSekarang) __attribute__((always_inline));
#endif
inline long ambilJarakSensor(int pinTrigger, int pinEcho) __attribute__((always_inline));
inline bool dekatDenganDinding(int jarakSensor) __attribute__((always_inline));
inline bool jauhDenganDinding(int jarakSensor) __attribute__((always_inline));
inline bool titikNetral(int jarakSensor) __attribute__((always_inline));
inline void printSensor() __attribute__((always_inline));
inline void setSpeed(int kecepatanKiri, int kecepatanKanan) __attribute__((always_inline));
inline void resolveMotorState(int kecepatanKiri, int kecepatanKanan) __attribute__((always_inline));
#ifdef SENSOR_READ_INTERVAL
inline void updateJarakSensorKiri(unsigned long waktuSekarang) __attribute__((always_inline));
inline void updateJarakSensorDepan(unsigned long waktuSekarang) __attribute__((always_inline));
inline void updateJarakSensorKanan(unsigned long waktuSekarang) __attribute__((always_inline));
#endif
// ------------------ END: inline function ------------------

// ------------------ START: control robot ------------------
/**
 * Fungsi untuk mengontrol robot
 * Silakan modifikasi sesuai dengan logika kontrol robot
 */

void logikaKontrol() {
    if (jauhDenganDinding(jarakSensorDepan)) {
        if (titikNetral(jarakSensorKiri) && titikNetral(jarakSensorKanan)) {
            // maju
            setSpeed(SPEED_MAX, SPEED_MAX);
        } else if (titikNetral(jarakSensorKiri) && dekatDenganDinding(jarakSensorKanan)) {
            // belok kanan sedikit
            setSpeed(SPEED_MAX, SPEED_MEDIUM);
        } else if (dekatDenganDinding(jarakSensorKiri) && titikNetral(jarakSensorKanan)) {
            // belok kiri sedikit
            setSpeed(SPEED_MEDIUM, SPEED_MAX);
        } else if (dekatDenganDinding(jarakSensorKiri) && dekatDenganDinding(jarakSensorKanan)) {
            // maju
            setSpeed(SPEED_MAX, SPEED_MAX);
        }
    } else if (titikNetral(jarakSensorDepan)) {
        if (titikNetral(jarakSensorKiri) && titikNetral(jarakSensorKanan)) {
            // maju pelan
            setSpeed(SPEED_MEDIUM, SPEED_MEDIUM);
        } else if (titikNetral(jarakSensorKiri) && dekatDenganDinding(jarakSensorKanan)) {
            // belok kanan
            setSpeed(SPEED_MIN, SPEED_MEDIUM - 20);
        } else if (dekatDenganDinding(jarakSensorKiri) && titikNetral(jarakSensorKanan)) {
            // belok kiri
            setSpeed(SPEED_MEDIUM - 20, SPEED_MIN);
        } else if (dekatDenganDinding(jarakSensorKiri) && dekatDenganDinding(jarakSensorKanan)) {
            // maju pelan
            setSpeed(SPEED_MEDIUM, SPEED_MEDIUM);
        }
    } else if (dekatDenganDinding(jarakSensorDepan)) {
        // berhenti dulu, biarkan sensor mengupdate jarak
        setSpeed(SPEED_MIN, SPEED_MIN);
#ifndef SENSOR_READ_INTERVAL
        jarakSensorKiri = ambilJarakSensor(PIN_TRIG_SENSOR_KIRI, PIN_ECHO_SENSOR_KIRI);
        jarakSensorDepan = ambilJarakSensor(PIN_TRIG_SENSOR_DEPAN, PIN_ECHO_SENSOR_DEPAN);
        jarakSensorKanan = ambilJarakSensor(PIN_TRIG_SENSOR_KANAN, PIN_ECHO_SENSOR_KANAN);
#endif
        delay(100);

        // deteksi apakah harus belok kiri, kanan, atau balik arah
        if (dekatDenganDinding(jarakSensorKiri) && dekatDenganDinding(jarakSensorKanan)) {
            // balik arah
            setSpeed(SPEED_MAX, -SPEED_MAX);
            delay(DELAY_BALIK_ARAH);
            setSpeed(0, 0);
            delay(200);
        } else if ((titikNetral(jarakSensorKiri) || dekatDenganDinding(jarakSensorKiri)) && jauhDenganDinding(jarakSensorKanan)) {
            // belok kanan
            setSpeed(SPEED_MAX, -SPEED_MAX);
            delay(DELAY_SIKU_SIKU);
            setSpeed(0, 0);
            delay(200);
        } else if (jauhDenganDinding(jarakSensorKiri) && (titikNetral(jarakSensorKanan) || dekatDenganDinding(jarakSensorKanan))) {
            // belok kiri
            setSpeed(-SPEED_MAX, SPEED_MAX);
            delay(DELAY_SIKU_SIKU);
            setSpeed(0, 0);
            delay(200);
        }
    } else {
        // berhenti dulu, biarkan sensor mengupdate jarak
        setSpeed(SPEED_MIN, SPEED_MIN);
    }
}

/**
 * Jangan ubah isi fungsi ini
 */
#ifdef CONTROL_INTERVAL
void kontrolRobot(unsigned long waktuSekarang) {
    static unsigned long waktuTerakhir = 0;
    if (waktuSekarang - waktuTerakhir >= CONTROL_INTERVAL) {
        logikaKontrol();
        waktuTerakhir = waktuSekarang;
    }
}
#endif
// ------------------ END: control robot ------------------

// ------------------ START: setup ------------------
void setup() {
#ifdef FEEDBACK
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
#endif
#ifdef DEBUG
    // ------------------ START: inisialisasi serial ------------------
    Serial.begin(9600);
    Serial.println("Wall Follower Robot - C1 Hitech R&D");
    Serial.println("===================================");
    // ------------------ END: inisialisasi serial ------------------
#endif
#ifdef FEEDBACK
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
#endif
#ifdef DEBUG
    Serial.println("Inisialisasi pin");
#endif
    // ------------------ START: inisialisasi pin ------------------
    pinMode(PIN_TRIG_SENSOR_KIRI, OUTPUT);
    pinMode(PIN_ECHO_SENSOR_KIRI, INPUT);
    pinMode(PIN_TRIG_SENSOR_DEPAN, OUTPUT);
    pinMode(PIN_ECHO_SENSOR_DEPAN, INPUT);
    pinMode(PIN_TRIG_SENSOR_KANAN, OUTPUT);
    pinMode(PIN_ECHO_SENSOR_KANAN, INPUT);
    // ------------------ END: inisialisasi pin ------------------
#ifdef FEEDBACK
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
#endif
#ifdef DEBUG
    Serial.println("Inisialisasi motor");
#endif
    // ------------------ START: inisialisasi motor ------------------
    motorKiri.setSpeed(255);
    motorKanan.setSpeed(255);
    motorKiri.run(RELEASE);
    motorKanan.run(RELEASE);
    // ------------------ END: inisialisasi motor ------------------
#ifdef FEEDBACK
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
#endif
#ifdef DEBUG
    Serial.println("Reset global variable");
#endif
    // ------------------ START: reset global variable ------------------
    jarakSensorKiri = 0;
    jarakSensorDepan = 0;
    jarakSensorKanan = 0;
    // ------------------ END: reset global variable ------------------
#ifdef FEEDBACK
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
#endif
#ifdef DEBUG
    Serial.println("Setup selesai");
    Serial.println("===================================");
#endif
#ifdef FEEDBACK
#ifdef DEBUG
    Serial.println("Robot akan berjalan setelah 3 detik");
#endif
    delay(3000);
    digitalWrite(LED_BUILTIN, LOW);
#endif
}
// ------------------ END: setup ------------------

// ------------------ START: loop ------------------
void loop() {
#if defined(SENSOR_READ_INTERVAL) || defined(CONTROL_INTERVAL)
    unsigned long waktuSekarang = millis();
#endif

#ifdef SENSOR_READ_INTERVAL
    updateJarakSensorKiri(waktuSekarang);
    updateJarakSensorDepan(waktuSekarang);
    updateJarakSensorKanan(waktuSekarang);
#endif
#ifndef SENSOR_READ_INTERVAL
    jarakSensorKiri = ambilJarakSensor(PIN_TRIG_SENSOR_KIRI, PIN_ECHO_SENSOR_KIRI);
    jarakSensorDepan = ambilJarakSensor(PIN_TRIG_SENSOR_DEPAN, PIN_ECHO_SENSOR_DEPAN);
    jarakSensorKanan = ambilJarakSensor(PIN_TRIG_SENSOR_KANAN, PIN_ECHO_SENSOR_KANAN);
#endif

#ifdef DEBUG
    printSensor();
#endif

#ifdef CONTROL_INTERVAL
    kontrolRobot(waktuSekarang);
#endif
#ifndef CONTROL_INTERVAL
    logikaKontrol();
#endif
}
// ------------------ END: loop ------------------

// ------------------ START: fungsi untuk mengambil jarak sensor ------------------
/**
 * Fungsi untuk mengambil jarak sensor
 * @param pinTrigger pin trigger sensor
 * @param pinEcho pin echo sensor
 * @return jarak sensor dalam satuan cm
 */
long ambilJarakSensor(int pinTrigger, int pinEcho) {
    digitalWrite(pinTrigger, LOW);
    delayMicroseconds(2);
    digitalWrite(pinTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinTrigger, LOW);
    return (pulseIn(pinEcho, HIGH) / 29 / 2);  // rumus: jarak = waktu / 29 / 2
}

bool dekatDenganDinding(int jarakSensor) {
    return jarakSensor <= (JARAK_DENGAN_DINDING + JARAK_TOLERANSI) && !(jarakSensor >= (JARAK_DENGAN_DINDING - JARAK_TOLERANSI));
}

bool jauhDenganDinding(int jarakSensor) {
    return jarakSensor >= (JARAK_DENGAN_DINDING - JARAK_TOLERANSI) && !(jarakSensor <= (JARAK_DENGAN_DINDING + JARAK_TOLERANSI));
}

bool titikNetral(int jarakSensor) {
    return jarakSensor >= (JARAK_DENGAN_DINDING - JARAK_TOLERANSI) && jarakSensor <= (JARAK_DENGAN_DINDING + JARAK_TOLERANSI);
}

#ifdef DEBUG
/**
 * Fungsi untuk menampilkan jarak sensor untuk setiap sudut ke serial monitor
 */
void printSensor() {
    Serial.print("Sensor Kiri: ");
    Serial.print(jarakSensorKiri);
    Serial.print(" mm | Sensor Depan: ");
    Serial.print(jarakSensorDepan);
    Serial.print(" mm | Sensor Kanan: ");
    Serial.print(jarakSensorKanan);
    Serial.println(" mm");
}
#endif
// ------------------ END: fungsi untuk mengambil jarak sensor ------------------

// ------------------ START: fungsi untuk menggerakkan robot ------------------

/**
 * setSpeed adalah fungsi untuk menggerakkan robot fungsi ini akan mengubah kecepatan motor dan arah motor dengan idempoten
 * kecepatan motor dan arah motor hanya akan diubah jika kecepatan motor atau arah motor berbeda dengan kecepatan motor atau arah motor sebelumnya
 *
 * @param kecepatanKiri kecepatan motor kiri range -255 sampai 255
 * @param kecepatanKanan kecepatan motor kanan range -255 sampai 255
 */
void setSpeed(int kecepatanKiri, int kecepatanKanan) {
    static int kecepatanKiriLama = 0;
    static int kecepatanKananLama = 0;

    if (kecepatanKiri != kecepatanKiriLama || kecepatanKanan != kecepatanKananLama) {
        resolveMotorState(kecepatanKiri, kecepatanKanan);
        motorKiri.setSpeed(abs(kecepatanKiri));
        motorKanan.setSpeed(abs(kecepatanKanan));
        kecepatanKiriLama = kecepatanKiri;
        kecepatanKananLama = kecepatanKanan;
    }
}

/**
 * Fungsi untuk menentukan arah motor
 * @param kecepatanKiri kecepatan motor kiri range -255 sampai 255
 * @param kecepatanKanan kecepatan motor kanan range -255 sampai 255
 */
void resolveMotorState(int kecepatanKiri, int kecepatanKanan) {
    if (kecepatanKiri > 0) {
        motorKiri.run(FORWARD);
    } else if (kecepatanKiri < 0) {
        motorKiri.run(BACKWARD);
    } else {
        motorKiri.run(RELEASE);
    }

    if (kecepatanKanan > 0) {
        motorKanan.run(FORWARD);
    } else if (kecepatanKanan < 0) {
        motorKanan.run(BACKWARD);
    } else {
        motorKanan.run(RELEASE);
    }
}
// ------------------ END: fungsi untuk menggerakkan robot ------------------

// ------------------ START: update jarak sensor ------------------
/**
 * Fungsi untuk mengupdate jarak sensor
 * disarankan untuk tidak mengubah isi fungsi ini
 */

#ifdef SENSOR_READ_INTERVAL
void updateJarakSensorKiri(unsigned long waktuSekarang) {
    static unsigned long waktuTerakhir = 0;
    if (waktuSekarang - waktuTerakhir >= SENSOR_READ_INTERVAL) {
        jarakSensorKiri = ambilJarakSensor(PIN_TRIG_SENSOR_KIRI, PIN_ECHO_SENSOR_KIRI);
        waktuTerakhir = waktuSekarang;
    }
}

void updateJarakSensorDepan(unsigned long waktuSekarang) {
    static unsigned long waktuTerakhir = 0;
    if (waktuSekarang - waktuTerakhir >= SENSOR_READ_INTERVAL) {
        jarakSensorDepan = ambilJarakSensor(PIN_TRIG_SENSOR_DEPAN, PIN_ECHO_SENSOR_DEPAN);
        waktuTerakhir = waktuSekarang;
    }
}

void updateJarakSensorKanan(unsigned long waktuSekarang) {
    static unsigned long waktuTerakhir = 0;
    if (waktuSekarang - waktuTerakhir >= SENSOR_READ_INTERVAL) {
        jarakSensorKanan = ambilJarakSensor(PIN_TRIG_SENSOR_KANAN, PIN_ECHO_SENSOR_KANAN);
        waktuTerakhir = waktuSekarang;
    }
}
#endif
// ------------------ END: update jarak sensor ------------------
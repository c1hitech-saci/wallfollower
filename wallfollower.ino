#include <AFMotor.h>

// pin sensor ultrasonik
#define pinTrigSensorKiri A0
#define pinEchoSensorKiri A1
#define pinTrigSensorDepan A2
#define pinEchoSensorDepan A3
#define pinTrigSensorKanan A4
#define pinEchoSensorKanan A5

// jarak aman dengan dinding dalam satuan mm
#define jarakAmanDenganDinding 100

// interval eksekusi fungsi dalam satuan milisecond
#define sensorReadInterval 100
#define controlInterval 1000

// delay dalam satuan milisecond untuk setiap aksi robot
#define delayMaju 1000
#define delaySikuSiku 1000
#define delayBalikArah 2000

// kecepatan motor mulai dari 0 - 255
// bisa ditambah atau dikurangi sesuai kebutuhan
// atau menambahkan variable kecepatan baru
#define speedMax 255
#define speedMedium 200
#define speedMin 0

// deklarasi motor
// atur pin motor sesuai dengan pin yang digunakan
AF_DCMotor motorKiri(1);  // pin M1
AF_DCMotor motorKanan(2); // pin M2

// ------------------ START: global variable ------------------
/**
 * Variable global yang digunakan untuk menyimpan data jarak sensor
 * variable ini akan berubah setiap interval waktu sensorReadInterval
 */
float jarakSensorKiri;
float jarakSensorDepan;
float jarakSensorKanan;
// ------------------ END: global variable ------------------

void setup()
{
  // ------------------ START: inisialisasi serial ------------------
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Wall Follower Robot - C1 Hitech R&D");
  Serial.println("===================================");
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  // ------------------ END: inisialisasi serial ------------------

  // ------------------ START: inisialisasi pin ------------------
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Inisialisasi pin");
  pinMode(pinTrigSensorKiri, OUTPUT);
  pinMode(pinEchoSensorKiri, INPUT);
  pinMode(pinTrigSensorDepan, OUTPUT);
  pinMode(pinEchoSensorDepan, INPUT);
  pinMode(pinTrigSensorKanan, OUTPUT);
  pinMode(pinEchoSensorKanan, INPUT);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  // ------------------ END: inisialisasi pin ------------------

  // ------------------ START: inisialisasi motor ------------------
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Inisialisasi motor");
  motorKiri.setSpeed(255);
  motorKanan.setSpeed(255);
  motorKiri.run(RELEASE);
  motorKanan.run(RELEASE);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  // ------------------ END: inisialisasi motor ------------------

  // ------------------ START: reset global variable ------------------
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Reset global variable");
  jarakSensorKiri = 0;
  jarakSensorDepan = 0;
  jarakSensorKanan = 0;
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  // ------------------ END: reset global variable ------------------

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Setup selesai");
  Serial.println("===================================");
  Serial.println("Robot akan berjalan setelah 3 detik");
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  // ambil waktu sekarang dalam satuan milisecond
  // waktu sekarang digunakan untuk mengupdate jarak sensor dan mengontrol robot dengan interval waktu tertentu
  // hal ini dilakukan agar robot tidak terlalu cepat mengupdate jarak sensor dan mengontrol robot
  // sekaligus robot bisa melakukan semi multitasking (mengupdate jarak sensor dan mengontrol robot secara bersamaan)
  unsigned long waktuSekarang = millis();

  // update jarak sensor, waktu interval update sensor ditentukan oleh variable sensorReadInterval
  updateJarakSensorKiri(waktuSekarang);
  updateJarakSensorDepan(waktuSekarang);
  updateJarakSensorKanan(waktuSekarang);

  printSensor();

  // kontrol robot, waktu interval kontrol robot ditentukan oleh variable controlInterval
  kontrolRobot(waktuSekarang);
}

// ------------------ START: control robot ------------------
/**
 * Fungsi untuk mengontrol robot
 * Silakan modifikasi sesuai dengan logika kontrol robot
 */

void kontrolRobot(unsigned long waktuSekarang)
{
  static unsigned long waktuTerakhir = 0;
  if (waktuSekarang - waktuTerakhir >= controlInterval)
  { // Ubah logika kontrol robot di dalam if block ini
    if (jarakSensorKiri >= jarakAmanDenganDinding && jarakSensorDepan >= jarakAmanDenganDinding && jarakSensorKanan >= jarakAmanDenganDinding)
    {
      maju(speedMax, speedMax);
    }
    else if (jarakSensorKiri < jarakAmanDenganDinding && jarakSensorDepan >= jarakAmanDenganDinding && jarakSensorKanan >= jarakAmanDenganDinding)
    {
      maju(speedMax, speedMedium);
    }
    else if (jarakSensorKiri >= jarakAmanDenganDinding && jarakSensorDepan >= jarakAmanDenganDinding && jarakSensorKanan < jarakAmanDenganDinding)
    {
      maju(speedMedium, speedMax);
    }
    else if (jarakSensorKiri >= jarakAmanDenganDinding && jarakSensorDepan < jarakAmanDenganDinding && jarakSensorKanan >= jarakAmanDenganDinding)
    {
      mundur(speedMax, speedMax);
    }
    else if (jarakSensorKiri < jarakAmanDenganDinding && jarakSensorDepan >= jarakAmanDenganDinding && jarakSensorKanan < jarakAmanDenganDinding)
    {
      mundur(speedMax, speedMax);
    }
    else if (jarakSensorKiri < jarakAmanDenganDinding && jarakSensorDepan < jarakAmanDenganDinding && jarakSensorKanan >= jarakAmanDenganDinding)
    {
      mundur(speedMax, speedMax);
    }
    else if (jarakSensorKiri >= jarakAmanDenganDinding && jarakSensorDepan < jarakAmanDenganDinding && jarakSensorKanan < jarakAmanDenganDinding)
    {
      mundur(speedMax, speedMax);
    }
    else if (jarakSensorKiri < jarakAmanDenganDinding && jarakSensorDepan < jarakAmanDenganDinding && jarakSensorKanan < jarakAmanDenganDinding)
    {
      mundur(speedMax, speedMax);
    }
    else
    {
      berhenti();
    }
    delay(delayMaju);

    // Jangan hapus kode di bawah ini
    waktuTerakhir = waktuSekarang;
  }
}
// ------------------ END: control robot ------------------

// ------------------ START: fungsi untuk mengambil jarak sensor ------------------
/**
 * Fungsi untuk mengambil jarak sensor
 * @param pinTrigger pin trigger sensor
 * @param pinEcho pin echo sensor
 * @return jarak sensor dalam satuan mm
 */
int ambilJarakSensor(int pinTrigger, int pinEcho)
{
  digitalWrite(pinTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);
  return (pulseIn(pinEcho, HIGH) * 0.0343 / 2); // rumus: jarak = waktu * kecepatan suara / 2
}

/**
 * Fungsi untuk menampilkan jarak sensor
 */
void printSensor()
{
  Serial.print("Sensor Kiri: ");
  Serial.print(jarakSensorKiri);
  Serial.print(" mm | Sensor Depan: ");
  Serial.print(jarakSensorDepan);
  Serial.print(" mm | Sensor Kanan: ");
  Serial.print(jarakSensorKanan);
  Serial.println(" mm");
}
// ------------------ END: fungsi untuk mengambil jarak sensor ------------------

// ------------------ START: fungsi untuk menggerakkan robot ------------------
/**
 * Buat aksi-aksi robot di sini
 * Silakan modifikasi sesuai kebutuhan
 */

void maju(int kecepatanKiri, int kecepatanKanan)
{
  motorKiri.setSpeed(kecepatanKiri);
  motorKanan.setSpeed(kecepatanKanan);
  motorKiri.run(FORWARD);
  motorKanan.run(FORWARD);
}

void mundur(int kecepatanKiri, int kecepatanKanan)
{
  motorKiri.setSpeed(kecepatanKiri);
  motorKanan.setSpeed(kecepatanKanan);
  motorKiri.run(BACKWARD);
  motorKanan.run(BACKWARD);
}

void berhenti()
{
  motorKiri.run(RELEASE);
  motorKanan.run(RELEASE);
}
// ------------------ END: fungsi untuk menggerakkan robot ------------------

// ------------------ START: update jarak sensor ------------------
/**
 * Fungsi untuk mengupdate jarak sensor
 * disarankan untuk tidak mengubah isi fungsi ini
 */

void updateJarakSensorKiri(unsigned long waktuSekarang)
{
  static unsigned long waktuTerakhir = 0;
  if (waktuSekarang - waktuTerakhir >= sensorReadInterval)
  {
    jarakSensorKiri = ambilJarakSensor(pinTrigSensorKiri, pinEchoSensorKiri);
    waktuTerakhir = waktuSekarang;
  }
}

void updateJarakSensorDepan(unsigned long waktuSekarang)
{
  static unsigned long waktuTerakhir = 0;
  if (waktuSekarang - waktuTerakhir >= sensorReadInterval)
  {
    jarakSensorDepan = ambilJarakSensor(pinTrigSensorDepan, pinEchoSensorDepan);
    waktuTerakhir = waktuSekarang;
  }
}

void updateJarakSensorKanan(unsigned long waktuSekarang)
{
  static unsigned long waktuTerakhir = 0;
  if (waktuSekarang - waktuTerakhir >= sensorReadInterval)
  {
    jarakSensorKanan = ambilJarakSensor(pinTrigSensorKanan, pinEchoSensorKanan);
    waktuTerakhir = waktuSekarang;
  }
}
// ------------------ END: update jarak sensor ------------------
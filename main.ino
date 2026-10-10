#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

#include <DHT.h>

// =====================================================
//                    TEAM SETTINGS
// =====================================================

#define TEAM_NUMBER 1

// =====================================================
//                    I2C
// =====================================================

#define I2C_SDA 21
#define I2C_SCL 22

// =====================================================
//                    BMP180
// =====================================================

Adafruit_BMP085 bmp;

// =====================================================
//                    MPU6050
// =====================================================

Adafruit_MPU6050 mpu;

// =====================================================
//                    DHT22
// =====================================================

#define DHT_PIN 4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
//                    STATUS LED
// =====================================================

#define LED_PIN 13

// =====================================================
//                    LoRa PINS
// =====================================================

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23

#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

// =====================================================
//                    LoRa PARAMETERS
// =====================================================

#define LORA_FREQUENCY 433E6

#define LORA_SF       7
#define LORA_BW       125E3
#define LORA_CR       5
#define LORA_SYNC     0xA5

// =====================================================
//                    PACKET SETTINGS
// =====================================================

unsigned long packetNumber = 0;

unsigned long lastSend = 0;

// Send every 1 second
const unsigned long SEND_INTERVAL = 1000;

// =====================================================
//                       SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("       CANSAT LoRa TRANSMITTER");
  Serial.println("========================================");

  // ===================================================
  // I2C
  // ===================================================

  Wire.begin(I2C_SDA, I2C_SCL);

  // ===================================================
  // BMP180
  // ===================================================

  Serial.println();
  Serial.println("Checking BMP180...");

  if (!bmp.begin())
  {
    Serial.println("BMP180 not detected!");

    while (1)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(200);

      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  }

  Serial.println("BMP180 OK");

  // ===================================================
  // MPU6050
  // ===================================================

  Serial.println();
  Serial.println("Checking MPU6050...");

  if (!mpu.begin())
  {
    Serial.println("MPU6050 not detected!");

    while (1)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(400);

      digitalWrite(LED_PIN, LOW);
      delay(400);
    }
  }

  Serial.println("MPU6050 OK");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // ===================================================
  // DHT22
  // ===================================================

  Serial.println();
  Serial.println("Checking DHT22...");

  dht.begin();

  delay(2000);

  float dhtTemperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(dhtTemperature) || isnan(humidity))
  {
    Serial.println("DHT22 reading failed!");
  }
  else
  {
    Serial.println("DHT22 OK");
  }

  // ===================================================
  // SPI
  // ===================================================

  SPI.begin(
    LORA_SCK,
    LORA_MISO,
    LORA_MOSI,
    LORA_SS
  );

  // ===================================================
  // LoRa PINS
  // ===================================================

  LoRa.setPins(
    LORA_SS,
    LORA_RST,
    LORA_DIO0
  );

  // ===================================================
  // LoRa INITIALIZATION
  // ===================================================

  Serial.println();
  Serial.println("Starting LoRa...");

  while (!LoRa.begin(LORA_FREQUENCY))
  {
    Serial.println("LoRa initialization failed...");

    delay(1000);
  }

  // ===================================================
  // LoRa PARAMETERS
  // ===================================================

  LoRa.setSpreadingFactor(LORA_SF);

  LoRa.setSignalBandwidth(LORA_BW);

  LoRa.setCodingRate4(LORA_CR);

  LoRa.setSyncWord(LORA_SYNC);

  LoRa.enableCrc();

  LoRa.setTxPower(17);

  // ===================================================
  // READY
  // ===================================================

  Serial.println();
  Serial.println("LoRa initialization OK!");

  Serial.println("----------------------------------------");
  Serial.println("Team       : The Electrophiles");
  Serial.println("Frequency  : 433 MHz");
  Serial.println("SF         : 7");
  Serial.println("Bandwidth  : 125 kHz");
  Serial.println("Coding     : 4/5");
  Serial.println("Sync Word  : 0xA5");
  Serial.println("CRC        : ENABLED");
  Serial.println("----------------------------------------");

  Serial.println("TRANSMITTER READY");
  Serial.println();

  // Ready indication
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);

    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

// =====================================================
//                       LOOP
// =====================================================

void loop()
{
  // ===================================================
  // SEND INTERVAL
  // ===================================================

  if (millis() - lastSend < SEND_INTERVAL)
  {
    return;
  }

  lastSend = millis();

  packetNumber++;

  // ===================================================
  // READ BMP180
  // ===================================================

  float bmpTemperature =
    bmp.readTemperature();

  float pressure =
    bmp.readPressure();

  float altitude =
    bmp.readAltitude(1013.25);

  // ===================================================
  // READ MPU6050
  // ===================================================

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mpuTemp;

  mpu.getEvent(
    &accel,
    &gyro,
    &mpuTemp
  );

  // Acceleration
  float ax = accel.acceleration.x;
  float ay = accel.acceleration.y;
  float az = accel.acceleration.z;

  // Gyroscope
  float gx = gyro.gyro.x;
  float gy = gyro.gyro.y;
  float gz = gyro.gyro.z;

  // ===================================================
  // READ DHT22
  // ===================================================

  float dhtTemperature =
    dht.readTemperature();

  float humidity =
    dht.readHumidity();

  // If DHT reading fails, use -999
  if (isnan(dhtTemperature))
  {
    dhtTemperature = -999;
  }

  if (isnan(humidity))
  {
    humidity = -999;
  }

  // ===================================================
  // TIME
  // ===================================================

  unsigned long totalSeconds =
    millis() / 1000;

  unsigned int seconds =
    totalSeconds % 60;

  unsigned int minutes =
    (totalSeconds / 60) % 60;

  unsigned int hours =
    (totalSeconds / 3600) % 24;

  unsigned int milliseconds =
    millis() % 1000;

  char timeString[20];

  snprintf(
    timeString,
    sizeof(timeString),
    "%02u:%02u:%02u:%03u",
    hours,
    minutes,
    seconds,
    milliseconds
  );

  // ===================================================
  // CREATE TELEMETRY PACKET
  // ===================================================

  char packet[400];

  snprintf(
    packet,
    sizeof(packet),

    "%s; P-%03lu; Ti-%s;"
    "A-%.1f; Pr-%.2f; T-%.1f;"
    "Ro-%.1f; Pi-%.1f; Y-%.1f;"
    "AX-%.2f; AY-%.2f; AZ-%.2f;",

    "The Electrophiles",

    packetNumber,

    timeString,

    altitude,

    pressure,

    dhtTemperature,

    gx,

    gy,

    gz,

    ax,

    ay,

    az
  );

  // ===================================================
  // PACKET LENGTH
  // ===================================================

  int packetLength =
    strlen(packet);

  // ===================================================
  // SERIAL OUTPUT
  // ===================================================

  Serial.println();
  Serial.println("========================================");

  Serial.print("Packet length: ");
  Serial.print(packetLength);
  Serial.println(" bytes");

  Serial.println("TX DATA:");
  Serial.println("----------------------------------------");

  Serial.println(packet);

  Serial.println("----------------------------------------");

  // ===================================================
  // TRANSMIT LoRa
  // ===================================================

  LoRa.beginPacket();

  LoRa.print(packet);

  LoRa.endPacket();

  Serial.println("Packet transmitted");

  // ===================================================
  // LED FLASH
  // =====================================================

  digitalWrite(LED_PIN, HIGH);

  delay(50);

  digitalWrite(LED_PIN, LOW);

  Serial.println("========================================");
}

#include <SPI.h>
#include <SD.h>
#include <dht.h>
#include <Arduino.h>
#include "HX711.h"
#include "RTClib.h"
#include <RF24.h>


#define Sensor1 5 // Defines the pin # the sensor inside the hive
#define Sensor2 6 // Defines the pin # the sensor outside the hive


// Pin definitions
#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);

RTC_PCF8523 rtc;

// Create an instance of the HX711 class
HX711 scale;
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

const int chipSelect = 10;
File myFile;
dht DHT1;
dht DHT2;

const byte address[6] = "00001";

void setup() {
  Serial.begin(57600);

  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");


  #ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
    rtc.start();
    float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
    float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
    float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
    float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours
  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
    int offset = round(deviation_ppm / drift_unit);
  // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  Serial.print("Offset is "); Serial.println(offset); // Print to control offset

// Radio

  // Start the radio
  radio.begin();

  // Set the data rate to 250 kbps
  radio.setDataRate(RF24_250KBPS);

  // Set the channel to 0
  radio.setChannel(100);

  // Set the power level to minimum
  radio.setPALevel(RF24_PA_MIN);

  // Set the address of the receiver
  radio.openWritingPipe(address);

  // Start the PCF8523 RTC
  Wire.begin();

  // Weight scale 

   Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
       
  scale.set_scale(9800);
  // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0
}

void loop() {

    DateTime now = rtc.now();

    // calculate a date which is 7 days, 12 hours and 30 seconds into the future
    DateTime future (now - TimeSpan(0,0,0,2));

    int readData1 = DHT1.read22(Sensor1); // DHT22/AM2302
    float temperature1 = DHT1.temperature;
    float humidity1 = DHT1.humidity-5;

    // Read temperature and humidity from the second sensor
    int readData2 = DHT2.read22(Sensor2); // DHT22/AM2302
    float temperature2 = DHT2.temperature;
    float humidity2 = DHT2.humidity-5;

      // Read the weight from the scale and print it to the serial monitor
    float weight = scale.get_units();

  String radiomessage1 = String(future.month()) + "/" + String(future.day()) + "/" + String(future.year()) + " " + String(future.hour()) + ":" + String(future.minute()) + ":" + String(future.second());

  char charArray1[radiomessage1.length() + 1];
  radiomessage1.toCharArray(charArray1, radiomessage1.length() + 1);
  radio.write(charArray1, sizeof(charArray1));

  String radiomessage2 = String((temperature1 * 9.0) / 5.0 + 32.0) + ","+ String(humidity1) + ","+ String((temperature2 * 9.0) / 5.0 + 32.0) + "," + String(weight);

  char charArray2[radiomessage2.length() + 1];
  radiomessage2.toCharArray(charArray2, radiomessage2.length() + 1);
  radio.write(charArray2, sizeof(charArray2));

  // Open the file for writing
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    // Read temperature and humidity from the first sensor
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" lbs");

    myFile.print(future.year(), DEC);
    myFile.print('/');
    myFile.print(future.month(), DEC);
    myFile.print('/');
    myFile.print(future.day(), DEC);
    myFile.print(' ');
    myFile.print(future.hour(), DEC);
    myFile.print(':');
    myFile.print(future.minute(), DEC);
    myFile.print(':');
    myFile.print(future.second(), DEC);
    myFile.print(",");

    // Write temperature and humidity from the first sensor to file
    // myFile.print("Inside Hive - Temperature: ");
    myFile.print((temperature1 * 9.0) / 5.0 + 32.0); // Celsius to Fahrenheit
    // myFile.print(" F\tHumidity: ");
    myFile.print(",");
    myFile.print(humidity1);
    // myFile.println("%");

    // Write temperature and humidity from the second sensor to file
    // myFile.print("Outside Hive - Temperature: ");
    myFile.print(",");
    myFile.print((temperature2 * 9.0) / 5.0 + 32.0); // Celsius to Fahrenheit
    // myFile.println("%");

    // myFile.print("Weight: ");
    myFile.print(",");
    myFile.print(weight);
    myFile.println();

    // Close the file
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt for writing");
  }

  Serial.println(String(future.month()) + "/" + String(future.day()) + "/" + String(future.year()) + " " + String(future.hour()) + ":" + String(future.minute()) + ":" + String(future.second()));

  Serial.println(String((temperature1 * 9.0) / 5.0 + 32.0) + ","+ String(humidity1) + ","+ String((temperature2 * 9.0) / 5.0 + 32.0) + "," + String(weight));


  delay(5000); // Wait for 5 seconds before repeating the loop
}

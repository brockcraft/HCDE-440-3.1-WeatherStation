/*A4 WeatherStation Example Code
   
   Incorporates MPL115A2 barmeric pressure sensor and
   DHT22 temperature and humidity sensor to report to Adafruit.io feeds

   DEBUG MODE IS TO PREVENT FLOODING THE SERVER
   LEAVE AT 1 WHILE TESTING
   SET TO 0 TO POST VALUES TO THE SERVER

   brc 2019
*/
// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

#define DEBUG 0 // Set to 1 for debug mode - won't connect to server. Set to 0 to post data

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "arduino_secrets.h"

#include <Wire.h>  // for I2C communications
#include <Adafruit_Sensor.h>  // the generic Adafruit sensor library used with both sensors
#include <DHT.h>   // temperature and humidity sensor library
#include <DHT_U.h> // unified DHT library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPL115A2.h> // Barometric pressure sensor library

// pin connected to DH22 data line
#define DATA_PIN 12

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// create MPL115A2 instance
Adafruit_MPL115A2 mpl115a2;

// create OLED Display instance on an ESP8266
// set OLED_RESET to pin -1 (or another), because we are using default I2C pins D4/D5.
#define OLED_RESET -1

Adafruit_SSD1306 display(OLED_RESET);

// set up the all of the data feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *kpa = io.feed("kpa");

// the refresh interval in for posting new data, in ms
#define SERVER_REFRESH_DELAY 10000

void setup() {

  // start the serial connection
  Serial.begin(115200);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));


  // wait for serial monitor to open
  while (! Serial);

  Serial.println("Weather Station Started.");
  Serial.print("Debug mode is: ");

  if (DEBUG) {
    Serial.println("on.");
  } else {
    Serial.println("off.");
  }

  // set up the OLED display
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Starting up.");
  display.display();
  // init done

  // initialize dht22
  dht.begin();

  // initialize MPL115A2
  mpl115a2.begin();

  Serial.println("MPL115 Sensor Test"); Serial.println("");
  mplSensorDetails();

  if (!DEBUG) {
    // connect to io.adafruit.com
    Serial.print("Connecting to Adafruit IO");

    Serial.println("Debug mode off. Connecting to server.");
    io.connect();

    // wait for a connection
    while (io.status() < AIO_CONNECTED) {
      Serial.print(".");
      delay(500);
    }

    // we are connected
    Serial.println();
    Serial.println(io.statusText());
  }
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  if (!DEBUG) {
    io.run();
  }

  //-------------GET THE TEMPERATURE--------------//
  // the Adafruit_Sensor library provides a way of getting 'events' from sensors
  //getEvent returns data from the sensor and we can update that on Adafruit.IO
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(fahrenheit);

  //-------------GET THE HUMIDITY--------------//
  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  //-------------GET THE PRESSURE--------------//
  // The Adafruit_Sensor library doesn't support the MPL1152, so we'll just grab data from it
  // with methods provided by its library

  float pressureKPA = 0, temperatureC = 0;

  pressureKPA = mpl115a2.getPressure();
  Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.println(" kPa");

  // save pressure to Adafruit IO
  kpa->save(pressureKPA); // assign the "pressure" feed the value of pressureKPa

  //-------------UPDATE THE DISPLAY--------------//
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp.   : ");
  display.print(fahrenheit);
  display.println("'F");
  display.print("Humidity: ");
  display.print(event.relative_humidity);
  display.println("%");
  display.print("Pressure: ");
  display.print(pressureKPA);
  display.println(" kPa");
  display.display();

  //-------------DON'T FLOOD THE ADAFRUIT.IO SERVER--------------//
  // wait a few seconds, as defined above
  if (!DEBUG) {
    delay(SERVER_REFRESH_DELAY);
  }

}

void mplSensorDetails(void)
{
  float pressureKPA = 0, temperatureC = 0;

  mpl115a2.getPT(&pressureKPA, &temperatureC);
  Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.print(" kPa  ");
  Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C both measured together");

  pressureKPA = mpl115a2.getPressure();
  Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.println(" kPa");

  temperatureC = mpl115a2.getTemperature();
  Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C");
  delay(5000);
}

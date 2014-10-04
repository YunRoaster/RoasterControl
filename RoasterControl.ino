/*
  This roaster app is derived from the original Yun example
  Temperature web interface

 The original example showed how to serve data from an analog input
 via the Arduino Yún's built-in webserver using the Bridge library.

 We left the TMP36 code in place thinking that we might get one and use
 it as an ambient temperature log.

 The circuit:
 * TMP36 temperature sensor on analog pin A1
 * SD card attached to SD card slot of the Arduino Yún
 * RyanJMcClaughlin.com digital thermocouple shield

 Prepare your SD card with an empty folder in the SD root
 named "arduino" and a subfolder of that named "www".
 This will ensure that the Yún will create a link
 to the SD to the "/mnt/sd" path.

 In this sketch folder is a basic webpage and a copy of zepto.js, a
 minimized version of jQuery.  When you upload your sketch, these files
 will be placed in the /arduino/www/TemperatureWebPanel folder on your SD card.

 You can then go to http://arduino.local/sd/TemperatureWebPanel
 to see the output of this sketch.

 You can remove the SD card while the Linux and the
 sketch are running but be careful not to remove it while
 the system is writing to it.


 Original example created  6 July 2013
 by Tom Igoe
 Extended in September 2014
 by Phil West

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/TemperatureWebPanel

 */

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
// #include <SPI.h>
#include "MAX6675.h"

#define HEATER 6  // This is the red LED on the TC board, so no need for TC_LED
#define SD_CS 10 //is this correct for the YUN?
#define PWM_PERIOD 1667

int duty_cycle = 800;
float pct_pwr = (100 * duty_cycle) / PWM_PERIOD;
String TCstring;
static unsigned long start_time = millis();
static unsigned long display_time;


//int TC_LED = 13; // Status LED Pin
//int CS = 10; // CS pin on MAX6675  --we use THERMOCUOPELX_CS for this
int THERMOCOUPLE1_CS = 7;
int THERMOCOUPLE2_CS = 8;

int SO = 12; // SO pin of MAX6675
int CK = 13; // SCK pin of MAX6675
int units = 2; // Units to readout temp (0 = raw, 1 = ˚C, 2 = ˚F)
float temperature0 = 0.0; // Temperature output variable
float temperature1 = 0.0; // Temperature output variable
float temperature2 = 0.0; // Temperature output variable

// Initialize the MAX6675 Library for our chip
// MAX6675 temp(THERMOCOUPLE1_CS, SO, CK, units);
MAX6675 temp1(THERMOCOUPLE1_CS, SO, CK, units);
MAX6675 temp2(THERMOCOUPLE2_CS, SO, CK, units);



void slowPWM(const int duty_cycle)
{
  static int state = 0;
  static unsigned long next_time = 0;

  if (!duty_cycle) {
    // reset everything
    next_time = 0;
    digitalWrite(HEATER, state = 0);
//    digitalWrite(TC_LED, state = 0);
    return;
  }

  if (millis() >= next_time) {
    digitalWrite(HEATER, state);
//    digitalWrite(TC_LED, state);
    next_time =  millis() + (state ? duty_cycle : (PWM_PERIOD - duty_cycle));
    state = !state;
  }
}



// Listen on default port 5555, the webserver on the Yún
// will forward there all the HTTP requests for us.
YunServer server;
String startString;
long hits = 0;

void setup() {

  // Initialize Serial
  Bridge.begin();
  Serial.begin(9600);

  // Wait until a Serial Monitor is connected.
  while (!Serial) { //fast blink LED
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
  }
  Serial.println("Serial Comms Initialized...\n");


  //pinMode (TC_LED, OUTPUT);
  pinMode (HEATER, OUTPUT);

  digitalWrite(HEATER, LOW);



  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  digitalWrite(13, HIGH);

  // using A0 and A2 as vcc and gnd for the TMP36 sensor:
  pinMode(A0, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A2, LOW);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();

  // get the time that this sketch started:
  Process startTime;
  startTime.runShellCommand("date");
  while (startTime.available()) {
    char c = startTime.read();
    startString += c;
  }
}

void loop()
{
  /*   char carray[message.length() + 1]; //determine size of the array
     message.toCharArray(carray, sizeof(carray)); //put readStringinto an array
     float pct_pwr = atof(carray); //convert the array into a float
   }
  */
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client)
  {
    // read the command
    //string should end with a colon even if no argument is passed
    String command = client.readStringUntil(':');
    String command2 = client.readString();

    command.trim();        //kill whitespace
    command2.trim();        //kill whitespace
    Serial.print("Command is: ");
    Serial.print(command);
    Serial.print(", Command2 is: ");
    Serial.println(command2);

    // is "CSVData" the command?
    if (command == "CSVData")
    {
      Serial.println(" In the 'CSVData' command processor... ");
      display_time = millis() - start_time;
      int sensorValue = analogRead(A1);
      // convert the reading to millivolts:
      float voltage = sensorValue *  (5000 / 1024);
      // convert the millivolts to temperature celsius:
      float temperature0 = (voltage - 500) / 10;
      // convert to degrees F
      temperature0 = (temperature0 * 1.8) + 32.0;

      // **** Read MAX6675 using native library instead of Arduino SPI
      // Read the temp from the MAX6675
      temperature1 = temp1.read_temp();

      temperature2 = temp2.read_temp();

      //Now, write out the CSV string with Time, temp0, temp1, temp2
      client.print(display_time);
      client.print(", ");
      client.print(pct_pwr);
      client.print(", ");
      client.print(temperature1);
      client.print(", ");
      client.println(temperature2);

    }
    else if (command == "temperature")
    {
      Serial.println(" In the 'temperature' command processor... ");
      //First, we'll get/read all the data.
      //Then, write out a formatted CSV string with important variables
      //The, write out human-readable text with embedde variables

      // get the time from the server:

      display_time = millis() - start_time;
      int sensorValue = analogRead(A1);
      // convert the reading to millivolts:
      float voltage = sensorValue *  (5000 / 1024);
      // convert the millivolts to temperature celsius:
      float temperature0 = (voltage - 500) / 10;
      // convert to degrees F
      temperature0 = (temperature0 * 1.8) + 32.0;


      // **** Read MAX6675 using native library instead of Arduino SPI
      // Read the temp from the MAX6675
      temperature1 = temp1.read_temp();

      temperature2 = temp2.read_temp();

      // print the other information:
      client.print("Current display time ");
      client.println(display_time);
      client.print("<br>Current TMP36 temperature: ");
      client.print(temperature0);
      client.print(" degrees F");
    }
    else if (command == "pct_pwr")
    {
      Serial.print("... In the pct_pwr command processor ...");
      char carray[command2.length() + 1]; //determine size of the array
      command2.toCharArray(carray, sizeof(carray)); //put readStringinto an array
      pct_pwr = atof(carray); //convert the array into a float
      if (pct_pwr > 100.0 ) { pct_pwr = 100.0; }
      duty_cycle = (pct_pwr * PWM_PERIOD) / 100.0;
      Serial.print("Setting power to ");
      Serial.print(pct_pwr);
      Serial.print(" percent");
      Serial.print(" and duty cycle to ");
      Serial.print(duty_cycle);
      Serial.println(" ");

    }
    else
    {
      Serial.println(" Command unrecognized... ");
    }

  }

  // Close connection and free resources.
  client.stop();
  hits++;
  slowPWM(duty_cycle);
  delay(50); // Poll every 50ms
}



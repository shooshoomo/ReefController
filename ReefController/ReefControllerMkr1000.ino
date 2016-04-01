//controls led rgb pwm lights
//monitors temp to control lights and fan if needed
//displays basic info to lcd screen
//uses rtc
//uses built in wifi atwin1500
//uses dallas one wire probe
//ada fruit lcd

//define pins
int DS18S20Pin = 0;
int FanPin = 7;
int PumpPin = 8;
int BLEDPin = 3;
int RLEDPin = 4;
int GLEDPin = 5;
int HeartBeatLEDPin = 6;
//pins for i2c used for rtc
//12 SCL & 11 SDA

//includes here
//wifi
#include <SPI.h>
#include <WiFi101.h>
char ssid[] = "myWifi";      // your network SSID (name)
char pass[] = "myWifiPassword";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);

//lcd
//needed for I2C
#include <Wire.h>
//=========================================
//next 2 needed for adafruit display
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7


// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
//from ADAFruit
//#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;

//setup temp prob
//one wire
#include <OneWire.h>
//Temperature chip i/o
OneWire ds(DS18S20Pin);

//include for watchdog timer
//#include <wdt.h>

//define variables
// variable to set output pin to
//0 is off 255 is full on pwm
int ROutput = 0;
int GOutput = 0;
int BOutput = 0;

//led mode
//-1 auto
//0 off
// 1 manual values
int LEDMode = -1;

int RManualValue = 60;
int GManualValue = 60;
int BManualValue = 255;

//Fan mode
//-1 auto
//0 off
// 1 manual values
int FanMode = -1;
bool FanOn = 0;
int FanOnTemp = 79;
int FanOffTemp = 77;

//Pumps On
//0 off
// 1 on
bool PumpOn = 1;

float TankTemp;
DateTime CurrentDateTime;
String FormattedTime;
String FormattedDate;
DateTime RebootDateTime;
String FormattedRebootDateTime;

//used to convert decimals into ints for display
char buf[2];

//LED Dimming vales can be changed below
//idea of light values as an array from Krusduino
//https://code.google.com/archive/p/dangerduino/downloads
byte rled[96] = {
	1, 1, 1, 1, 1, 1, 1, 1,  //0 - 1
	1, 1, 1, 1, 1, 1, 1, 1,  //2 - 3
	1, 1, 1, 1, 1, 1, 1, 1,  //4 - 5
	1, 1, 1, 1, 1, 1, 1, 1,  //6 - 7
	1, 16, 25, 55, 80, 110, 140, 190,  //8 - 9
	190, 190, 190, 190, 190, 190, 190, 190,  //10 - 11
	190, 190, 190, 190, 191, 192, 193, 194,  //12 - 13
	190, 190, 190, 190, 190, 190, 190, 190,  //14 - 15
	190, 190, 190, 190, 190, 190, 190, 190,  //16 - 17
	190, 190, 190, 190, 190, 190, 190, 190,  //18 - 19
	190, 110, 90, 70, 55, 32, 16, 1,  //20 - 22
	0, 0, 0, 0, 0, 0, 0, 0    //22 - 23
};
byte gled[96] = {
	1, 1, 1, 1, 1, 1, 1, 1,  //0 - 1
	1, 1, 1, 1, 1, 1, 1, 1,  //2 - 3
	1, 1, 1, 1, 1, 1, 1, 1,  //4 - 5
	1, 1, 1, 1, 1, 1, 1, 1,  //6 - 7
	1, 16, 32, 55, 80, 110, 140, 190,  //8 - 9
	190, 190, 190, 190, 190, 190, 190, 190,  //10 - 11
	190, 190, 190, 190, 195, 196, 197, 198,  //12 - 13
	190, 190, 190, 190, 190, 190, 190, 190,  //14 - 15
	190, 190, 190, 190, 190, 190, 190, 190,  //16 - 17
	190, 190, 190, 190, 190, 190, 190, 190,  //18 - 19
	190, 110, 90, 70, 55, 32, 16, 1,  //20 - 22
	0, 0, 0, 0, 0, 0, 0, 0    //22 - 23
};
byte bled[96] = {
	1, 1, 1, 1, 1, 1, 1, 1,  //0 - 1
	1, 1, 1, 1, 1, 1, 1, 1,  //2 - 3
	1, 1, 1, 1, 1, 1, 1, 1,  //4 - 5
	1, 1, 1, 1, 1, 1, 1, 1,  //6 - 7
	1, 16, 32, 55, 80, 110, 140, 190,  //8 - 9
	190, 190, 190, 190, 190, 190, 190, 190,  //10 - 11
	190, 190, 190, 190, 199, 190, 191, 192,  //12 - 13
	190, 190, 190, 190, 190, 190, 190, 190,  //14 - 15
	190, 190, 190, 190, 190, 190, 190, 190,  //16 - 17
	190, 190, 190, 190, 190, 190, 190, 190,  //18 - 19
	190, 110, 90, 70, 55, 32, 16, 1,  //20 - 22
	25, 25, 25, 20, 20, 10, 10, 0    //22 - 23
};


void setup() {
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);
	lcd.setBacklight(RED);
	lcd.print("ReefTroller");
	lcd.setCursor(0, 1);
	lcd.print("Please Wait");

	//configure outputs
	pinMode(FanPin, OUTPUT);
	pinMode(PumpPin, OUTPUT);
	pinMode(RLEDPin, OUTPUT);
	pinMode(GLEDPin, OUTPUT);
	pinMode(BLEDPin, OUTPUT);

	//set outputs to defaults
	setOutputs();

	RebootDateTime = rtc.now();

	//start up wifi
	// attempt to connect to Wifi network:
	while (status != WL_CONNECTED) {
		status = WiFi.begin(ssid, pass);
		// wait 1 seconds for connection:
		delay(1000);
	}

	//start up web client
	server.begin();

	if (!rtc.begin()) {
		Serial.println("Couldn't find RTC");
	}

	//wdt_enable(WDTO_8S);

	//setup routine is done so go green
	lcd.setBacklight(GREEN);
	lcd.clear();

	//setup is done, turn on led on pin 6
	pinMode(HeartBeatLEDPin, OUTPUT);
	digitalWrite(HeartBeatLEDPin, HIGH);

}

void loop() {
	//gather data
	gatherData();

	//process controller logic
	processLogic();

	//set outputs
	setOutputs();

	//update lcd
	updateLCD();

	//respond to client requests
	processClient();

	//blink 6 to show it's alive
	digitalWrite(HeartBeatLEDPin, CurrentDateTime.second() % 2);

	//reset the watchdog timer
	//wdt_reset();

}

//gather data from probes, etc
void gatherData() {
	TankTemp = getTemp();
	CurrentDateTime = rtc.now();
}

//do any calculations based on variables
void processLogic() {
	if (TankTemp > FanOnTemp) {
		FanOn = 1;
	}
	if (TankTemp < FanOffTemp) {
		FanOn = 0;
	}

	//set LEDs Outputs -1 auto, 0 off, 1 manual values
	byte RTarget, GTarget, BTarget;
	if (LEDMode == -1) { //automatic
						 //time based light logic
						 //figure out where in the array we are based on what time it is.
		int TimeSector, NextTimeSector, TimeSectorSecond;

		TimeSector = (CurrentDateTime.hour() * 60 + CurrentDateTime.minute()) / 15;
		TimeSectorSecond = (CurrentDateTime.minute() % 15) * 60 + CurrentDateTime.second();
		//roll over to midnight
		if (TimeSector == 95) { NextTimeSector = 0; }
		else { NextTimeSector = TimeSector + 1; }

		RTarget = GetLedTarget(rled[TimeSector], rled[NextTimeSector], TimeSectorSecond);
		GTarget = GetLedTarget(gled[TimeSector], gled[NextTimeSector], TimeSectorSecond);
		BTarget = GetLedTarget(bled[TimeSector], bled[NextTimeSector], TimeSectorSecond);
	}
	else if (LEDMode == 1) { //manual
		RTarget = RManualValue;
		GTarget = GManualValue;
		BTarget = BManualValue;
	}
	else { //off
		RTarget = 0;
		GTarget = 0;
		BTarget = 0;
	}

	//if temp is too high dim/shutoff lights
	if (TankTemp > (FanOnTemp + 1)) {
		ROutput = RTarget*.5;
		GOutput = GTarget*.5;
		BOutput = BTarget*.5;
	}
	else {
		ROutput = RTarget;
		GOutput = GTarget;
		BOutput = BTarget;
	}

	//create formatted time/date string
	FormattedDate = "";
	if (CurrentDateTime.month() < 10) {
		FormattedDate += '0';
	}
	FormattedDate += CurrentDateTime.month();
	FormattedDate += '/';
	if (CurrentDateTime.day() < 10) {
		FormattedDate += '0';
	}
	FormattedDate += CurrentDateTime.day();
	FormattedDate += '/';
	FormattedDate += CurrentDateTime.year();

	FormattedTime = "";
	if (CurrentDateTime.hour() < 10) {
		FormattedTime += '0';
	}
	FormattedTime += CurrentDateTime.hour();
	FormattedTime += ':';
	if (CurrentDateTime.minute() < 10) {
		FormattedTime += '0';
	}
	FormattedTime += CurrentDateTime.minute();
	FormattedTime += ':';
	if (CurrentDateTime.second() < 10) {
		FormattedTime += '0';
	}
	FormattedTime += CurrentDateTime.second();

	FormattedRebootDateTime = "";
	if (RebootDateTime.month() < 10) {
		FormattedRebootDateTime += '0';
	}
	FormattedRebootDateTime += RebootDateTime.month();
	FormattedRebootDateTime += '/';
	if (RebootDateTime.day() < 10) {
		FormattedRebootDateTime += '0';
	}
	FormattedRebootDateTime += RebootDateTime.day();
	FormattedRebootDateTime += '/';
	FormattedRebootDateTime += RebootDateTime.year();
	FormattedRebootDateTime += " ";

	if (RebootDateTime.hour() < 10) {
		FormattedRebootDateTime += '0';
	}
	FormattedRebootDateTime += RebootDateTime.hour();
	FormattedRebootDateTime += ':';
	if (RebootDateTime.minute() < 10) {
		FormattedRebootDateTime += '0';
	}
	FormattedRebootDateTime += RebootDateTime.minute();
	FormattedRebootDateTime += ':';
	if (RebootDateTime.second() < 10) {
		FormattedRebootDateTime += '0';
	}
	FormattedRebootDateTime += RebootDateTime.second();


}

byte GetLedTarget(byte pt1, byte pt2, int lstep) {
	byte result;
	float fresult; //math done with float for increased precision
				   //if the 2 targets are the same of if it's the first minute just return the first target
	if (pt1 == pt2 or lstep == 0)
	{
		result = pt1;
	}
	else if (pt1 < pt2) {
		//getting brighter
		// 1/15 of the difference * the minute of the quarter added to pt1
		fresult = ((float(pt2 - pt1) / 900.0) * float(lstep)) + float(pt1);
		result = byte(fresult);
	}
	else {
		//getting dimmer
		fresult = -((float(pt1 - pt2) / 900.0) * float(lstep)) + float(pt1);
		result = byte(fresult);
	}
	//result= pt1;  
	return result;
}

void updateLCD() {
	lcd.setCursor(0, 0);
	lcd.print(FormattedTime);
	lcd.print(" ");
	sprintf(buf, "%.1f", TankTemp, DEC);
	lcd.print(TankTemp);
	lcd.print(" F");
	lcd.setCursor(0, 1);
	lcd.print("R");
	lcd.print(ROutput);
	lcd.print(" G");
	lcd.print(GOutput);
	lcd.print(" B");
	lcd.print(BOutput);

	if (TankTemp > FanOnTemp + 1) {
		lcd.setBacklight(RED);
	}
	else {
		lcd.setBacklight(GREEN);
	}

}

void setOutputs() {
	// relays are backwards, drive low to turn on
	if (FanMode == -1) {
		if (FanOn == 0) {
			digitalWrite(FanPin, HIGH);
		}
		if (FanOn == 1) {
			digitalWrite(FanPin, LOW);
		}
	}
	if (FanMode == 0) {
		digitalWrite(FanPin, HIGH);
	}
	if (FanMode == 1) {
		digitalWrite(FanPin, LOW);
	}

	if (PumpOn == 0) {
		digitalWrite(PumpPin, HIGH);
	}
	if (PumpOn == 1) {
		digitalWrite(PumpPin, LOW);
	}


	analogWrite(RLEDPin, ROutput);
	analogWrite(GLEDPin, GOutput);
	analogWrite(BLEDPin, BOutput);

}

float getTemp() {
	//returns the temperature from one DS18S20 in DEG Farenheit

	byte data[12];
	byte addr[8];

	if (!ds.search(addr)) {
		//no more sensors on chain, reset search
		ds.reset_search();
		return -1000;
	}

	if (OneWire::crc8(addr, 7) != addr[7]) {
		Serial.println("CRC is not valid!");
		return -1001;
	}

	if (addr[0] != 0x10 && addr[0] != 0x28) {
		Serial.print("Device is not recognized");
		return -1002;
	}

	ds.reset();
	ds.select(addr);
	ds.write(0x44, 1); // start conversion, with parasite power on at the end

	byte present = ds.reset();
	ds.select(addr);
	ds.write(0xBE); // Read Scratchpad


	for (int i = 0; i < 9; i++) { // we need 9 bytes
		data[i] = ds.read();
	}

	ds.reset_search();

	byte MSB = data[1];
	byte LSB = data[0];

	float tempRead = ((MSB << 8) | LSB); //using two's compliment
	float TemperatureSum = tempRead / 16;
	TemperatureSum = (TemperatureSum * 9.0) / 5.0 + 32.0;
	return TemperatureSum;

}


void processClient() {
	WiFiClient client = server.available();   // listen for incoming clients

	if (client) {                             // if you get a client
		String currentLine = "";                // make a String to hold incoming data from the client
		String RequestString = "";  //used to determine what the request is
		while (client.connected()) {            // loop while the client's connected
			if (client.available()) {             // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then

				if (c == '\n') {                    // if the byte is a newline character
													//check for get line
					if (currentLine.startsWith("GET /")) {
						RequestString = currentLine;
					}
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {  //process their request
													  // Check to see what the client request was 
													  //-1 auto, 0 off, 1 manual/on values
						if (RequestString.startsWith("GET /led=-1")) {
							LEDMode = -1;
							Serial.println("Set LED to Auto");
						}
						if (RequestString.startsWith("GET /led=0")) {
							LEDMode = 0;
							Serial.println("Set LED to OFF");
						}
						if (RequestString.startsWith("GET /led=1")) {
							LEDMode = 1;
							Serial.println("Set LED to Manual");
						}
						if (RequestString.startsWith("GET /rled=")) {
							RManualValue = currentLine.substring(10).toInt();
						}
						if (RequestString.startsWith("GET /gled=")) {
							GManualValue = currentLine.substring(10).toInt();
						}
						if (RequestString.startsWith("GET /bled=")) {
							BManualValue = currentLine.substring(10).toInt();
						}
						if (RequestString.startsWith("GET /fan=-1")) {
							FanMode = -1;
						}
						if (RequestString.startsWith("GET /fan=0")) {
							FanMode = 0;
						}
						if (RequestString.startsWith("GET /fan=1")) {
							FanMode = 1;
						}
						if (RequestString.startsWith("GET /pump=1")) {
							PumpOn = 1;
						}
						if (RequestString.startsWith("GET /pump=0")) {
							PumpOn = 0;
						}
						processLogic();
						//now respond with a json formatted status
						client.println("HTTP/1.1 200 OK");
						client.println("Content-Type: application/json");
						client.println("Connection: close");  // the connection will be closed after completion of the response
						client.println("Cache-Control: no-cache, no-store, must-revalidate");
						client.println("Pragma: no-cache");
						client.println("Expires: 0");
						//client.println("Refresh: 60");  // refresh the page automatically every x sec
						client.println();
						client.print("{\"led\":\"");
						client.print(LEDMode);
						client.print("\",\"rled\":\"");
						client.print(ROutput);
						client.print("\",\"gled\":\"");
						client.print(GOutput);
						client.print("\",\"bled\":\"");
						client.print(BOutput);
						client.print("\",\"tankdate\":\"");
						client.print(FormattedDate);
						client.print("\",\"tanktime\":\"");
						client.print(FormattedTime);
						client.print("\",\"lastreboot\":\"");
						client.print(FormattedRebootDateTime);
						client.print("\",\"tanktemp\":\"");
						sprintf(buf, "%.1f", TankTemp, DEC);
						client.print(TankTemp);
						//client.println("&deg;F<br/>");
						client.print("\",\"fanmode\":\"");
						client.print(FanMode);
						client.print("\",\"fan\":\"");
						client.print(FanOn);
						client.print("\",\"pump\":\"");
						client.print(PumpOn);
						client.print("\"}");
						client.println();
						client.println();
						break;
					}
					else {      // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				}
				else if (c != '\r') {    // if you got anything else but a carriage return character,
					currentLine += c;      // add it to the end of the currentLine
				}


			}
		}
		// close the connection:
		delay(10);
		client.stop();
		Serial.println("client disconnected");
	}
}

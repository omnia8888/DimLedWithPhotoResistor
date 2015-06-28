/*
 Name:		DimLedWithPhotoResistor.ino
 Created:	6/5/2015 9:38:47 PM
 Author:	Micke
*/
#include <LedControl.h>
#include "MickesLedControl.h"

#define PHOTO_CELL_PIN 0
#define LED_1_PIN 6
#define LED_2_PIN 5

#define LEDCONTROL_DATA_PIN 12
#define LEDCONTROL_CLOCK_PIN 11
#define LEDCONTROL_CS_PIN 10

#define NUM_OF_LEDCONTROLS 1

//Analog Threshold values for brightness to change. In range between 0 and photocellValueMax
int lightThreshholds[3] = { 100, 300, 600 }; 
//Values to be returned from brightness control function. Must be one more than light Thresholds
int brightnessLevels[4] = { 10, 80, 150, 240 }; 
//How much under or over the sensor vale has to be to change light level
int thresholdSensitivity = 30;
//How much the currentPhotocell will increase or decrease at on time.
int photocellValueStepping = 10;
//The maximum that currentPhotcellValue will reach:
int photocellValueMax = 1023;
//How often the brightness update will occur.
long brightnessUpdateIntervall = 100;

int currentPhotocellReading = 0;
int currentLedBrightness = 0;
int photocellValue = 0;
byte lightLevel = 0;
long brightnessLevelTimer = 0;

int photocellReading;     // the analog reading from the sensor divider
int LEDbrightness;        
int PWMCounter = 0;

int photoCreadMax = 0;
int photoCreadMin = 1023;

MickesLedControl MickeLC(LEDCONTROL_DATA_PIN, LEDCONTROL_CLOCK_PIN,
	LEDCONTROL_CS_PIN, NUM_OF_LEDCONTROLS);

void setup(void) {
	// We'll send debugging information via the Serial monitor
	Serial.begin(9600);
	//pinMode(LED_2_PIN, OUTPUT);
	
	MickeLC.initLedDisplay(0, 5);
	//lastPhotocellReading = analogRead(PHOTO_CELL_PIN);

}

void loop(void) {
	int steppedReading;
	//digitalWrite(LED_2_PIN, HIGH);
	photocellReading = analogRead(PHOTO_CELL_PIN);
	

	Serial.print("Analog reading = ");
	Serial.println(photocellReading);     // the raw analog reading

	steppedReading = setbrightnesslevel(photocellReading);

	Serial.print("Stepped reading = ");
	Serial.println(steppedReading);  

	Serial.print("Current reading = ");
	Serial.println(currentPhotocellReading);


	// LED gets brighter the darker it is at the sensor
	// that means we have to -invert- the reading from 0-1023 back to 1023-0
	//photocellReading = 1023 - photocellReading;

	//steppedReading = 1023 - steppedReading;
	//now we have to map 0-1023 to 0-255 since thats the range analogWrite uses
	//LEDbrightness = map(photocellReading, 100, 800, 255, 0);
	
	LEDbrightness = steppedReading;
	
	Serial.print("Led brightness = ");
	Serial.println(LEDbrightness);
	analogWrite(LED_1_PIN, LEDbrightness);

	//Serial.print("PWM Counter = ");
	//Serial.println(PWMCounter);
	//analogWrite(LED_2_PIN, PWMCounter);

	Serial.print("light level = ");
	Serial.println(lightLevel);


	Serial.println();

	//delay(100);
}

int setbrightnesslevel(int newPhotocellReading)
{
	
	int nextLightThreshold;

	//Timer that controls how often brightness update will occur
	if (brightnessLevelTimer == 0)
	{
		//Update timer on first run
		brightnessLevelTimer = millis() + brightnessUpdateIntervall;
	}
	else if (brightnessLevelTimer > millis())
	{
		//not time yet for update
		return(brightnessLevels[lightLevel]);
	}

	//Check if brightess level has ony changed in range of stepping value.
	if (newPhotocellReading > currentPhotocellReading - photocellValueStepping && newPhotocellReading < currentPhotocellReading + photocellValueStepping)
	{
		Serial.println("Only small change");
		currentPhotocellReading = newPhotocellReading;
	}


	//Check if lightbrightness have increased.
	if (newPhotocellReading > currentPhotocellReading)
	{
		Serial.println("Brightness increased");
		//Increase the current photocell value
		currentPhotocellReading = currentPhotocellReading += photocellValueStepping;
		

		
		Serial.print("Next light threshold = ");
		Serial.println(nextLightThreshold);

		//Check if ligth level max reached or not
		if (lightLevel < sizeof(brightnessLevels))
		{
			//Check if lightthreshold value with threshold sesitivity is reached
			nextLightThreshold = lightThreshholds[lightLevel] + thresholdSensitivity;
			if (currentPhotocellReading > nextLightThreshold)
			{
				//Increase light level and retur ledBrightness
				lightLevel++;

			}
		}
	}
	//Check if lightbrightness have decreased.
	if (newPhotocellReading < currentPhotocellReading)
	{
		Serial.println("Brightness decreased");
		//Decrease the current photocell value
		currentPhotocellReading = currentPhotocellReading - photocellValueStepping;
		//Check that not max value is reached and if so decrease value to Max value again.

		//Check if ligth level min reached or not
		if (lightLevel > 0)
		{
			//Check if lightthreshold value with threshold sesitivity is reached
			nextLightThreshold = lightThreshholds[lightLevel] - thresholdSensitivity;
			if (currentPhotocellReading < nextLightThreshold)
			{
				//Decrease light level and retur ledBrightness
				lightLevel--;
			}
		}
	}
	//Update timer
	brightnessLevelTimer = millis() + brightnessUpdateIntervall;
	return(brightnessLevels[lightLevel]);

}
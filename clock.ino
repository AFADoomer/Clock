// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PIXEL_PIN D2
#define PIXEL_COUNT 50
#define PIXEL_TYPE WS2811

#define FLASH_PIXEL_COUNT 10

struct colorRGB {
    int R;
    int G;
    int B;
};

struct mover {
    String strName;
    struct colorRGB colorMover;
    String posLastLat;
    String posLastLong;
    long timeCheckIn;
    int clockPosition;
};

struct location {
    String strName;
    struct colorRGB colorLocation;
} location;

struct mover movers[] {
    "Jay", 0, 0, 4, "", "", 0, 0,
    "Leanne", 4, 0, 4, "", "", 0, 0,
    "Max", 50, 25, 4, "", "", 0, 0,
    "Lady", 50, 10, 10, "", "", 0, 0,
    "Default", 0, 0, 0, "", "", 0, 0//Final Entry is default color (off)
};

struct location locations[] {
    "Home", 10, 10, 10,
    "Work", 0, 0, 10,
    "Traveling", 10, 0, 10,
    "Holiday", 0, 10, 0,
    "Unknown", 10, 10, 0,
    "Mortal Peril", 20, 0, 0,
    "Default", 0, 0, 0 //Final Entry is default color (off)
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int currentLED = 0;
String currentMover = "";
String currentLocation = "";

struct mover getMover(String nameLookup) {
    int i = 0;
    
    for (i = 0; i <= 4; i++) {
        String nameCurrent = movers[i].strName;
        if ((strcmp(nameCurrent.toUpperCase(), nameLookup.toUpperCase()) == 0) ||
            (strcmp(nameCurrent, "Default") == 0))
            return movers[i];
    }
    
    return movers[i];
}

/*
struct colorRGB getMoverColor(String nameLookup) {
    int i = 0;
    String nameCurrent = "";

    for (i = 0; i < 4; i++) {
        String nameCurrent = movers[i].strName;
        if ((strcmp(nameCurrent.toUpperCase(), nameLookup.toUpperCase()) == 0) ||
            (strcmp(nameCurrent, "Default") == 0))
            return movers[i].colorMover;
    }

    return getMoverColor("Default");
}

String getMoverPrettyName(String nameLookup) {
    int i = 0;
    String nameCurrent = "";

    for (i = 0; i < 4; i++) {
        String nameCurrent = movers[i].strName;
        if (strcmp(nameCurrent.toUpperCase(), nameLookup.toUpperCase()) == 0)
        {
            return movers[i].strName;
        }
    }
    return nameLookup;
}

void setMoverLocation(String nameLookup, String posLat, String posLong) {
    int i = 0;
    String nameCurrent = "";

    for (i = 0; i < 4; i++) {
        String nameCurrent = movers[i].strName;
        if (strcmp(nameCurrent.toUpperCase(), nameLookup.toUpperCase()) == 0)
        {
            movers[i].posLastLat = posLat;
            movers[i].posLastLong = posLong;
            movers[i].timeCheckIn = millis();
        }
    }
}

void setMoverClockDirection(String nameLookup, int intClockPosition) {
    int i = 0;
    String nameCurrent = "";

    for (i = 0; i < 4; i++) {
        String nameCurrent = movers[i].strName;
        if (strcmp(nameCurrent.toUpperCase(), nameLookup.toUpperCase()) == 0)
        {
            movers[i].clockPosition = intClockPosition * 6 + i;
        }
    }
}
*/

void turnLEDOn(int start = 0, int end = PIXEL_COUNT, colorRGB colorNew = { 255, 255, 255 }) {
    if (start != 0 && end == PIXEL_COUNT)
        end = start;

	for(int i = start; i <= end; i++) {
    		strip.setPixelColor(i, strip.Color(colorNew.R, colorNew.G, colorNew.B));
    }
}

void turnLEDOff(int start = 0, int end = PIXEL_COUNT) {
    turnLEDOn(start, end, { 0, 0, 0 });
}

void flashNotification(colorRGB colorFlash, int intFlashes = 5){
	for(int i = 0; i < intFlashes; i++) {
		delay(250);
		turnLEDOn(0, FLASH_PIXEL_COUNT, colorFlash);
		strip.show();
		delay(250);
		turnLEDOff(0, FLASH_PIXEL_COUNT);
		strip.show();
	}
}

int LEDControl(String command) { 
	turnLEDOff();
	
    char copyStr[64];
    command.toCharArray(copyStr,64);
    char *p = strtok(copyStr, "|");

    currentMover = p;
    p = strtok(NULL,"|");
    String posLat = p;
    p = strtok(NULL,"|");
    String posLong = p;
    p = strtok(NULL,"|");
    int R = atof(p);
    p = strtok(NULL,"|");
    int G = atof(p);
    p = strtok(NULL,"|");
    int B = atof(p);
    p = strtok(NULL,"|");
    int currentClockDirection = atof(p);

	struct colorRGB colorCurrent;
    colorCurrent = { R, G, B };
    
    currentLED = currentClockDirection;

    if ((strcmp(currentMover, "") != 0) && 
    	(strcmp(currentMover.toUpperCase(), "NULL") != 0)) {
        struct mover currentMoverData = getMover(currentMover);
    	    
        if (R + G + B == 0)
            colorCurrent = currentMoverData.colorMover;
  //      	colorCurrent = getMoverColor(currentMover);

        currentMoverData.posLastLong = posLong;
        currentMoverData.posLastLat = posLat;
        
        currentMoverData.clockPosition = currentClockDirection;
        
        currentMover = currentMoverData.strName;
/*
    	setMoverLocation(currentMover, posLat, posLong);
    	setMoverClockDirection(currentMover, currentClockDirection);
    	
        currentMover = getMoverPrettyName(currentMover);
*/
	    currentLocation = String(posLat + ", " + posLong);
    }
    turnLEDOn(currentLED, currentLED, colorCurrent);

/*
    currentLED = atof(p);
    p = strtok(NULL,"|");
    int R = atof(p);
    p = strtok(NULL,"|");
    int G = atof(p);
    p = strtok(NULL,"|");
    int B = atof(p);
    p = strtok(NULL,"|");
    currentMover = p;
	
	struct colorRGB colorCurrent;

	if (strcmp(currentMover, "") == 0) {
	    colorCurrent = { R, G, B };
	    currentLocation = "";
	} else {
    	colorCurrent = getMoverColor(currentMover);
    	currentLocation = String(posLastLong + ", " + posLastLat);
	}
    turnLEDOn(currentLED, currentLED, colorCurrent);
*/
	
    return 1;   
}

/*
int updateClock(String command) {
  int commaIndex = command.indexOf(' ');
  int secondCommaIndex = command.indexOf(' ', commaIndex+1);
  String name = command.substring(0, commaIndex);
  String loc = command.substring(commaIndex+1, secondCommaIndex);
  int person = number_for_key(name, people);
  if(person != 0){
	  flashB(number_for_key(name, colors));
	  int place = number_for_key(loc, places);
	  if(place != 0){
		  allMineOff(person);
		  strip.setPixelColor(7+person+6*place, strip.Color(255, 255, 255));
		  strip.show();
	  }
  }
  return 1;
}
*/

void setup() {
    Particle.function("LEDControl", LEDControl);
    Particle.variable("getLED", currentLED);
    Particle.variable("getMover", currentMover);
    Particle.variable("getPos", currentLocation);

	strip.begin();
    strip.show();
	startupSwirl();
	strip.setBrightness(80);
	turnLEDOff();
}

void loop() {
    strip.show();
}

/*
int number_for_key(String key, struct entry dict[]){
    for(int j=0; j<6; j++) {
        String name = dict[j].str;
        if (strcmp(name, key) == 0)
            return dict[j].n;
    }
    return 0;
}
*/

void startupSwirl() { // outer hands counterclockwise, inner hands clockwise
	int swirldelay = 500;

    colorRGB flashColor = {0, 50, 50};

//	strip.setPixelColor(0, strip.Color(255, 0, 0));
//	strip.show();
//	delay(swirldelay);
	flashNotification(flashColor, 2);
	/*
	strip.setPixelColor(0, strip.Color(0, 255, 0));
	strip.show();
	delay(swirldelay);
	strip.setPixelColor(0, strip.Color(0, 0, 255));
	strip.show();
	*/

	/*
    for(int i=0; i<50; i = i + 5) {
    	allOff();
        strip.setPixelColor(i, strip.Color(255, 255, 255));
        strip.show();
        delay(swirldelay);
    }
    */
    
	/*
	for(int j=0; j<2; j++) {
    	for(int i=14; i<50; i+=3) {
    		allOff();
    		strip.setPixelColor(i, strip.Color(255, 255, 255));
    		strip.setPixelColor(62-i, strip.Color(255, 255, 255));
    		strip.show();
    		delay(swirldelay);
    		
    		allOff();
    		strip.setPixelColor(i+2, strip.Color(255, 255, 255));
    		strip.setPixelColor(62-i, strip.Color(255, 255, 255));
    		strip.show();
    		delay(swirldelay);
    	}
	}
	*/
}

void allMineOff(int n) {
	for(int i=7+n; i<PIXEL_COUNT; i+=6) {
		//strip.setPixelColor(i, strip.Color(0, 0, 0));
		turnLEDOff(i);
	}
	strip.show();
}

void debug(String message, int value) {
    char msg [50];
    sprintf(msg, message.c_str(), value);
    Spark.publish("DEBUG", msg);
}

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

//LED Controls
#define PIXEL_PIN D0
#define PIXEL_COUNT 50
#define PIXEL_TYPE WS2811

#define MOVER_COUNT 6
#define LOCATION_COUNT 6
#define LOCATION_ARRAY_COUNT 7

#define FLASH_PIXEL_COUNT 14

//Distance calculation
#define pi 3.14159265358979323846

//Photoresistor
#define PHOTO A0
#define POWER A1

//MP3 Player
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00

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
    int intClockPosition;
};

struct location {
    String strName;
    int intDirection;
    struct colorRGB colorLocation;
    String posLat;
    String posLong;
    double radius; //in miles
} location;

struct mover movers[] {
    "Max", 50, 25, 4, "", "", 0,
    "", 0, 0, 0, "", "", 0,
    "Jay", 0, 0, 20, "", "", 0,
    "Leanne", 4, 0, 4, "", "", 0,
    "", 0, 0, 0, "", "", 0,
    "Lady", 50, 10, 10, "", "", 0,
    "Default", 0, 0, 0, "", "", 0 //Final Entry is default color (off)
};

struct location locations[] {
    "Home", 2, 10, 10, 10, "37.088567", "-76.420840", 0.25,
    "Work", 1, 0, 10, 10, "37.070425", "-76.368069", 0.5,
    "Work", 1, 0, 10, 10, "37.062589", "-76.495912", 1.0,
    "Traveling", 0, 30, 0, 20, "", "", 0,
    "Holiday", 3, 0, 10, 0, "", "", 0,
    "Unknown", 4, 10, 10, 0, "", "", 0,
    "Mortal Peril", 5, 20, 0, 0, "", "", 0,
    "Default", 4, 10, 10, 0, "", "", 0 //Final Entry is default (Unknown again)
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

String currentMover = "";
String currentLocation = "";
String currentClockStatus = "";
String currentState = "";
String debugString = "";
double moveDistance = 0;

int lightLevel = 0;

struct colorRGB pixelStatus[PIXEL_COUNT];

int getMoverIndex(String nameLookup) {
    for (int i = 0; i <= MOVER_COUNT; i++) {
        String nameCurrent = movers[i].strName;
        if (strcmp(nameCurrent.toUpperCase(), nameLookup.toUpperCase()) == 0)
            return i;
    }
    return MOVER_COUNT;
}

struct mover getMover(String nameLookup) {
    return movers[getMoverIndex(nameLookup)];
}

struct location getClockLocation(String statusLookup) {
    for (int i = 0; i <= LOCATION_ARRAY_COUNT; i++) {
        String statusCurrent = locations[i].strName;
        if (strcmp(statusCurrent.toUpperCase(), statusLookup.toUpperCase()) == 0)
            return locations[i];
    }
    return locations[LOCATION_ARRAY_COUNT];
}

void turnLEDOn(int start = 0, int end = PIXEL_COUNT, colorRGB colorNew = { 255, 255, 255 }, int fadeSteps = 1, bool override = true, int stepJump = 1) {
    int deltaR = 0, deltaG = 0, deltaB = 0;
    
    if (start != 0 && end == PIXEL_COUNT)
        end = start;

    for (int f = 1; f <= fadeSteps; f++) {
    	for (int i = start; i <= end; i = i + stepJump) {
    	    if (override || (pixelStatus[i].R == 0 && pixelStatus[i].G == 0 && pixelStatus[i].B == 0)) {
    	            if (fadeSteps > 1 && f == 1) {
            	        struct colorRGB colorOld = pixelStatus[i];
                        deltaR = (colorNew.R - colorOld.R) / fadeSteps;
                        deltaG = (colorNew.G - colorOld.G) / fadeSteps;
                        deltaB = (colorNew.B - colorOld.B) / fadeSteps;
                    }
                    
                    strip.setPixelColor(i, strip.Color(colorNew.R - (deltaR * (fadeSteps - f)), colorNew.G - (deltaG * (fadeSteps - f)), colorNew.B - (deltaB * (fadeSteps - f))));
        		if (override) pixelStatus[i] = colorNew;
    	    }
        }
        if (fadeSteps > 1) {
            strip.show();
            delay(25);
        }
    }
    strip.show();
}

void dimLEDs(float newLightLevel) {
    if (newLightLevel != lightLevel) {
    	for (int i = 0; i < PIXEL_COUNT; i++) {
            struct colorRGB colorNew = pixelStatus[i];
    
            float ratio = (600 + newLightLevel) / 3200;

            if (colorNew.R) colorNew.R = constrain(colorNew.R * ratio, 4, 255);
            if (colorNew.G) colorNew.G = constrain(colorNew.G * ratio, 4, 255);
            if (colorNew.B) colorNew.B = constrain(colorNew.B * ratio, 4, 255);
    
            strip.setPixelColor(i, strip.Color(colorNew.R, colorNew.G, colorNew.B));
        }
		lightLevel = newLightLevel;
    }
}

void turnLEDOff(int start = 0, int end = PIXEL_COUNT, int fadeSteps = 1, int stepJump = 1) {
    turnLEDOn(start, end, { 0, 0, 0 }, fadeSteps, 1, stepJump);
}

void turnMoverLEDOff(int intMoverIndex) {
	for (int i = FLASH_PIXEL_COUNT + intMoverIndex; i < PIXEL_COUNT; i += 6) {
		turnLEDOff(i, i, 10);
	}
	strip.show();
}

void flashNotification(colorRGB colorFlash, int intFlashes = 5){
	for (int i = 0; i < intFlashes; i++) {
		turnLEDOn(0, FLASH_PIXEL_COUNT - 1, colorFlash, 10);
		strip.show();
		delay(500);
		turnLEDOff(0, FLASH_PIXEL_COUNT - 1, 10);
		strip.show();
		if (intFlashes > 1) delay(250);
	}
}

void innerSwirl(colorRGB colorFlash, int intFlashes = 1){
    int startLED = PIXEL_COUNT - 2;
    
    for (int j = 0; j < intFlashes; j++) {
	    for (int i = startLED; i >= 15; i = i - 3) {
    		turnLEDOn(i, i, colorFlash);
	    	strip.show();
		    delay(50);
    	}
	    for (int k = startLED; k >= 15; k = k - 3) {
		    turnLEDOff(k, k);
    		strip.show();
		    delay(50);
    	}
    }
}

void innerFlash(colorRGB colorFlash, int duration = 50){
    int startLED = PIXEL_COUNT - 2;
    
    turnLEDOn(15, startLED, colorFlash, 10, 0, 3);
    delay(duration);
	turnLEDOff(15, startLED, 10, 3);
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::                                                                         :*/
/*::  This routine calculates the distance between two points (given the     :*/
/*::  latitude/longitude of those points). It is being used to calculate     :*/
/*::  the distance between two locations using GeoDataSource(TM) products.   :*/
/*::                                                                         :*/
/*::  Definitions:                                                           :*/
/*::    South latitudes are negative, east longitudes are positive           :*/
/*::                                                                         :*/
/*::  Passed to function:                                                    :*/
/*::    lat1, lon1 = Latitude and Longitude of point 1 (in decimal degrees)  :*/
/*::    lat2, lon2 = Latitude and Longitude of point 2 (in decimal degrees)  :*/
/*::    unit = the unit you desire for results                               :*/
/*::           where: 'M' is statute miles (default)                         :*/
/*::                  'K' is kilometers                                      :*/
/*::                  'N' is nautical miles                                  :*/
/*::  Worldwide cities and other features databases with latitude longitude  :*/
/*::  are available at http://www.geodatasource.com                          :*/
/*::                                                                         :*/
/*::  For enquiries, please contact sales@geodatasource.com                  :*/
/*::                                                                         :*/
/*::  Official Web site: http://www.geodatasource.com                        :*/
/*::                                                                         :*/
/*::           GeoDataSource.com (C) All Rights Reserved 2015                :*/
/*::                                                                         :*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  Function prototypes                                           :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double);
double rad2deg(double);

double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dist = dist * 60 * 1.1515;
  switch(unit) {
    case 'M':
      break;
    case 'F':
        dist = dist * 5280;
        break;
    case 'K':
      dist = dist * 1.609344;
      break;
    case 'N':
      dist = dist * 0.8684;
      break;
  }
  return (dist);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
  return (deg * pi / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
  return (rad * 180 / pi);
}

int checkLocationRadius(struct mover currentMover) {
    float posLat = currentMover.posLastLat.toFloat();
    float posLong = currentMover.posLastLong.toFloat();

    for (int i = 0; i <= LOCATION_ARRAY_COUNT; i++) {
        if (locations[i].radius != 0) {
            double thisDistance = distance(posLat, posLong, locations[i].posLat.toFloat(), locations[i].posLong.toFloat(), 'M');
            if (thisDistance <= locations[i].radius){
                return i;
            }
        }
    }    
    return -1;
}

void trailLED(int trailStart = FLASH_PIXEL_COUNT, int trailEnd = PIXEL_COUNT, struct colorRGB trailColor = {4, 4, 4}) {
    int i;
    //colorRGB flashColor = {0, 10, 10};
/*    
    if (start > end) {
        for(i = end; i >= start; i -= 3) {
            turnLEDOn(i, i, flashColor, false);
            strip.show();
            delay(50);
            if (!pixelStatus[i]) turnLEDOff(i, i);
            strip.show();
            delay(50);
            turnLEDOn(i + 1, i + 1, flashColor, false);
            strip.show();
            delay(50);
            if (!pixelStatus[i + 1]) turnLEDOff(i + 1, i + 1);
            strip.show();
            delay(50);
        }
    } else {
    */
    /*
        for(i = trailStart; i <= trailEnd; i += 3) {
            turnLEDOn(i, i, flashColor, false);
            strip.show();
            delay(50);
            if (!pixelStatus[i]) turnLEDOff(i, i);
            strip.show();
            delay(50);
            i = i + 2;
            turnLEDOn(i, i, flashColor, false);
            strip.show();
            delay(50);
            if (!pixelStatus[i]) turnLEDOff(i, i);
            strip.show();
            delay(50);
        }
        */
        int swirlDelay = 1;
        
        int move = 3;
        
        int dist = abs(trailStart - trailEnd);
        int dir = trailStart > trailEnd ? -1 : 1;

        int current = 0;

        while (current != dist) {
            int j = trailStart + current * dir * move;

            if (j > PIXEL_COUNT) j = FLASH_PIXEL_COUNT + current % 50;
            if (j <= FLASH_PIXEL_COUNT) j = PIXEL_COUNT - (FLASH_PIXEL_COUNT - j);
            
            turnLEDOn(j, j, trailColor, 10, false);
            strip.show();
            delay(swirlDelay);
            turnLEDOn(j, j, pixelStatus[j], 10, false);
            strip.show();
            delay(swirlDelay);
            turnLEDOn(j + dir, j + dir, trailColor, 10, false);
            strip.show();
            delay(swirlDelay);
            turnLEDOn(j + dir, j + dir, pixelStatus[j + dir], 10, false);
            strip.show();
            delay(swirlDelay);

            current = current + dir;
        }
/*
        for(int t = trailStart; i < trailEnd; i += move * dir) {
            int j = i;
            if (i > PIXEL_COUNT) j = FLASH_PIXEL_COUNT + i % 50;
            if (i < FLASH_PIXEL_COUNT) j = PIXEL_COUNT - i;

            turnLEDOn(j, j, trailColor, 10, false);
            strip.show();
            delay(swirlDelay);
            turnLEDOn(j, j, pixelStatus[j], 10, false);
            strip.show();
            delay(swirlDelay);
            turnLEDOn(j + dir, j + dir, trailColor, 10, false);
            strip.show();
            delay(swirlDelay);
            turnLEDOn(j + dir, j + dir, pixelStatus[j + dir], 10, false);
            strip.show();
            delay(swirlDelay);
        }
        */
    //}
}

//MP3 Player Controls - See https://community.particle.io/t/a-great-very-cheap-mp3-sound-module-without-need-for-a-library/20111/23
void playSound(int intFolder = 0, int intSong = 1) {
    execute_CMD(0x0F, intFolder, intSong); 
    delay(500);
    execute_CMD(0x0D, 0, 0);
}

void setVolume(int volume = 1) {
    execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
}

void execute_CMD(byte CMD, byte Par1, byte Par2) {
    int16_t checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
    
    byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge, Par1, Par2, checksum >> 8, checksum & 0xFF, End_Byte};
    
    for (byte k = 0; k < 10; k++) {
        Serial1.write(Command_line[k]);
    }
}

int LEDControl(String command) { 
    char copyStr[64];
    command.toCharArray(copyStr,64);
    char *p = strtok(copyStr, "|");
    
    String parameters[8];
    
    String posLat = "", posLong = "", oldPosLat = "", oldPosLong = "";
    
    moveDistance = 0;
    
    int c = 0;
    while (strcmp(p, "") != 0) {
        parameters[c] = p;
        p = strtok(NULL,"|");
        c++;
    }
    
    currentMover = parameters[0];
    //If the second parameter is coordinates, treat it as such, otherwise, it's the
    // String entry for the clock position/location of the mover
    if (atof(parameters[1]) != 0) {
        currentClockStatus = "";
        posLat = parameters[1];
        posLong = parameters[2];
    } else {
        currentClockStatus = parameters[1];
        posLat = "";
        posLong = "";
    }

    int Vol = atof(parameters[3]);
    int R = atof(parameters[4]);
    int G = atof(parameters[5]);
    int B = atof(parameters[6]);
    
    struct location currentClockLocation;

	struct colorRGB colorCurrent;
	struct colorRGB colorClockDirection;
	
    if (Vol > 0) setVolume(Vol);

    colorCurrent = { R, G, B };
    
    if ((strcmp(currentMover, "") != 0) && 
    	(strcmp(currentMover.toUpperCase(), "NULL") != 0)) {
        int currentMoverIndex = getMoverIndex(currentMover);
        struct mover *currentMoverData;
        currentMoverData = &movers[currentMoverIndex];
        
        int oldLED = currentMoverData->intClockPosition;
        
        currentMover = currentMoverData->strName;

        if (strcmp(currentClockStatus.trim(), "") == 0) {
            int locationIndex = -1;
            double rangeTravel = 0.5;

            oldPosLat = currentMoverData->posLastLat;
            oldPosLong = currentMoverData->posLastLong;
            
            currentMoverData->posLastLat = posLat;
            currentMoverData->posLastLong = posLong;

            //Check radiuses
            locationIndex = checkLocationRadius(*currentMoverData);
            
            //TODO...
            //Check holiday indicators (How?  Maybe just leave up to IFTTT)

            if (locationIndex == -1) {
                //Check traveling indicators (large change in distance between check-ins)
                if (strcmp(oldPosLat, "") != 0 || strcmp(oldPosLong, "") != 0)
                    moveDistance = distance(posLat.toFloat(), posLong.toFloat(), oldPosLat.toFloat(), oldPosLong.toFloat(), 'M');

                if (moveDistance > rangeTravel) { //Assume if you move more than a half mile (~800m) you are traveling...
                    currentClockLocation = getClockLocation("Traveling");
                } else {
                    currentClockLocation = getClockLocation("Unknown"); //Fall through to unknown
                }
            }
            else
                currentClockLocation = getClockLocation(locations[locationIndex].strName);
        } else {
            currentClockLocation = getClockLocation(currentClockStatus);
        }
        
        currentClockStatus = currentClockLocation.strName; //To prettify the name case and give name of known location if address is within range
        colorClockDirection = currentClockLocation.colorLocation;
        
	    currentLocation = String(currentClockStatus + ", " + posLat + ", " + posLong);

        if (R + G + B == 0)
            colorCurrent = currentMoverData->colorMover;
            
        int currentLED = (currentClockLocation.intDirection * MOVER_COUNT) + getMoverIndex(currentMover) + PIXEL_COUNT - (LOCATION_COUNT * MOVER_COUNT);

        if (currentLED != oldLED) {
            currentMoverData->intClockPosition = currentLED;
            turnMoverLEDOff(getMoverIndex(currentMover));
            
            if (millis() < 10000 || oldLED == 0) {
                turnLEDOn(currentLED, currentLED, colorCurrent, 10);
            } else {
                playSound();
                //if (oldLED) trailLED(oldLED, currentLED);
                //turnLEDOn(currentLED, currentLED, colorClockDirection);
            	//innerSwirl(colorClockDirection, 1);
        	    innerFlash(colorClockDirection, 500);
                turnLEDOn(currentLED, currentLED, colorCurrent, 10);
        	    flashNotification(colorCurrent, 3);
            }
            
            Particle.publish("CurrentMove", String(currentMover + ", " + currentLocation), 60, PRIVATE);

        	strip.show();
        }
        
        currentState = String(currentMover + "|" + currentClockStatus + "|" + currentLocation);
    }

    return 1;   
}

void setup() {
    Particle.function("LEDControl", LEDControl);
    Particle.variable("getLoc", currentClockStatus);
    Particle.variable("getMover", currentMover);
    Particle.variable("getInfo", currentState);
    Particle.variable("debugString", debugString);

    pinMode(PHOTO, INPUT);
    pinMode(POWER, OUTPUT);

    digitalWrite(POWER, HIGH);
    
	strip.begin();
    strip.show();
	startupSwirl();
	strip.setBrightness(80);
	turnLEDOff();
	
	//MP3 player initialization
    Serial1.begin(9600);
    delay(500);
    execute_CMD(0x3F, 0, 0);
    delay(500);
    setVolume(1);

    //Hard-code the rotten dogs
	LEDControl("Max|Holiday");
	loop();
    LEDControl("Lady|Holiday");
}

void loop() {
    int lightLevel = analogRead(PHOTO);
    
    dimLEDs(lightLevel);
    
    strip.show();
    delay(50);
}

void startupSwirl() { // outer hands counterclockwise, inner hands clockwise
	int swirldelay = 10;

	for(int j=0; j<2; j++) {
    	for(int i=14; i<50; i+=3) {
    		turnLEDOff();
    		strip.setPixelColor(i, strip.Color(100, 100, 100));
    		strip.setPixelColor(62-i, strip.Color(100, 100, 100));
    		strip.show();
    		delay(swirldelay);
    		
    		turnLEDOff();
    		strip.setPixelColor(i+2, strip.Color(100, 100, 100));
    		strip.setPixelColor(62-i, strip.Color(100, 100, 100));
    		strip.show();
    		delay(swirldelay);
    	}
	}
	
}
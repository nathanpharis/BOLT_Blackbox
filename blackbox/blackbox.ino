//============================================================================================================================================
//MURI SPS Blackbox 
//Calculation of Optically Measured Particles with an Automated SPS System (COMPASS)
//Written by Nathan Pharis - phari009 Spring 2020
//============================================================================================================================================
//
//Version Description: SPS standalone configuration. Takes in 5V and outputs a serial string as noted in the BOLT campaign documentation.
//
//=============================================================================================================================================
//=============================================================================================================================================
//           ____                                    
//          / ___|___  _ __ ___  _ __   __ _ ___ ___ 
//         | |   / _ \| '_ ` _ \| '_ \ / _` / __/ __|
//         | |__| (_) | | | | | | |_) | (_| \__ \__ \
//          \____\___/|_| |_| |_| .__/ \__,_|___/___/
//                              |_|                                                                                                                                                                                                                                                                                            
//=============================================================================================================================================
//=============================================================================================================================================

/*  Teensy 3.5/3.6 pin connections:
     ------------------------------------------------------------------------------------------------------------------------------------------------------------------
     Component                    | Pins used         

     UBlox Neo m8n                | UART 1 (0,1)          
     SPS30 A                      | UART 2 (9,10)
     SPS30 B                      | UART 3 (7,8)
     Data Stream                  | UART 5 (33,34)
     Thermocouple A               | SPI 0 (11,12,13,15)
     Thermocouple B               | SPI 0 (11,12,13,20)
     Pressure sensor              | A9 (23)
     SD Logging                   | SPI 1 (5,20,31,32)
     OLED                         | I2C 0 (18,19)
     OPC Heater                   | (35,36)

     
     ------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
/////////////////////////////
//////////Libraries//////////
/////////////////////////////
#include <SPI.h>                                                        //SPI Library for R1
#include <SD.h>                                                         //SD Library for logging
#include <UbloxGPS.h>                                                   //GPS Library
#include <Arduino.h>                                                    //Arduino kit
#include <SPS.h>                                                        //Library for SPS
#include <SFE_MicroOLED.h>                                              //Library for OLED
#include <Adafruit_MAX31856.h>                                          //Adafruit Library
#include <LatchRelay.h>                                                 //Heater relay

////////////////////////////////////
//////////Pin Definitions///////////
////////////////////////////////////

#define SENSOR_HEATER_ON 35                                             //Latching Relay pins for heaters
#define SENSOR_HEATER_OFF 36
#define HONEYWELL_PRESSURE A9                                           //Analog Honeywell Pressure Sensor
#define THERMOCOUPLE_A 15                                               //Chip Select pin for SPI for the thermocouples
#define THERMOCOUPLE_B 20
#define SD_B 32                                                         //Chip select pin for SD B
#define UBLOX_SERIAL Serial1                                            //Serial Pins
#define SPSA_SERIAL Serial2
#define SPSB_SERIAL Serial3                                           
#define DATA_SERIAL Serial5                                         
#define PIN_RESET 17                                                    //The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used

//////////////////////////////
//////////Constants///////////
//////////////////////////////
//Bauds
#define DATA_BAUD 115200
#define SPS_BAUD 115200
#define UBLOX_BAUD 9600

//Values
#define MINUTES_TO_MILLIS 60000                                         //MATH TIME
#define PSI_TO_ATM  0.068046                                            //Pounds per square inch to atmospheres   
#define C2K 273.15                                                      //Degrees Celsius to Kelvin
#define DC_JUMPER 1                                                     //The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define START 0x42                                                      //The start, or beginning byte
#define STOP 0x53                                                       //The ending, or stop byte
#define NoFix 0x00
#define Fix 0x01

//Control
#define HIGH_TEMP 16                                                    //Thermal control
#define LOW_TEMP 10
#define LOG_RATE 1000
#define SCREEN_UPDATE_RATE 2000

///////////////////////////////////
//////////Data Management//////////
///////////////////////////////////
//Data Log
unsigned long logCounter = 0;
File Flog;                                                             //Variables needed to establish the flight log
static String data;
String Fname = "";
boolean SDcard = true;

static boolean FlightlogOpen = false;                                   //SD for Flight Computer
const int chipSelect = BUILTIN_SDCARD; 

//Data Transfer


////////////////////////////////
//////////Power Relays//////////
////////////////////////////////
LatchRelay sensorHeatRelay(SENSOR_HEATER_ON,SENSOR_HEATER_OFF);        //Declare latching relay objects and related logging variables
String sensorHeat_Status = "";

////////////////////////////////////////////////
//////////Environment Sensor Variables//////////
////////////////////////////////////////////////
//Thermocouple Temp Sensors
Adafruit_MAX31856 thermocoupleA = Adafruit_MAX31856(THERMOCOUPLE_A);
Adafruit_MAX31856 thermocoupleB = Adafruit_MAX31856(THERMOCOUPLE_B);
float t1 = -127.00;                                                    //Temperature values
float t2 = -127.00;
bool coldSensor = false;

//Honeywell Pressure Sensor
float pressureSensor;                                                  //Analog number given by sensor
float pressureSensorVoltage;                                           //Voltage calculated from analog number
float PressurePSI;                                                     //PSI calculated from voltage
float PressureATM;                                                     //ATM calculated from PSI

//GPS
UbloxGPS GPS(&UBLOX_SERIAL);
uint8_t FixStatus= NoFix;


////////////////////////
//////////OPCs//////////
////////////////////////
SPS SpsA(&SPSA_SERIAL);  
SPS SpsB(&SPSB_SERIAL);   
String OPCdata = "";

////////////////////////////////////////////////
//////////MicroOLED Object Declaration//////////
////////////////////////////////////////////////
MicroOLED oled(PIN_RESET, DC_JUMPER);                                  //Object I2C declaration
bool finalMessage[2] = {false,false};
unsigned short screen = 0;
unsigned long oledTime = 0;
unsigned long screenUpdateTimer = 0;

void setup() {
  analogReadResolution(13);

  initOLED(oled);                                                      //Initialize OLED Screen
  
  initData();                                                          //Initialize SD
  oledPrintNew(oled, "Data Init");

  initGPS();                                                           //Initialize GPS
  oledPrintAdd(oled, "GPSInit");
  delay(1000);

  initRelays();                                                        //Initialize Relays
  oledPrintAdd(oled, "RlyInit");

  initOPCs();                                                          //Initialize OPCs
  oledPrintNew(oled, "OPCInit");
  delay(1000);

  initTemp();                                                          //Initialize Temp Sensors
  oledPrintAdd(oled, "TmpInit");
  
  Serial.println("Setup Complete");
  oledPrintNew(oled, " Setup Success");
}

void loop() {
  GPS.update();                                                        //Update GPS and plantower on private loops

  if (millis() - logCounter >= LOG_RATE) {
      logCounter = millis();
      
      updateSensors();                                                   //Updates and logs all sensor data
      actHeat();                                                         //Controls active heating
      oledUpdate();                                                      //Update screen
  }   
}

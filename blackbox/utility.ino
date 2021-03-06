// GPS   
int getGPStime() 
{
  return (GPS.getHour() * 3600 + GPS.getMinute() * 60 + GPS.getSecond());
}

void FixCheck(){                                                        //Check if gps fix is good
  if (GPS.getFixAge() < 4000) 
  {
    FixStatus = Fix;
  }
  else if(GPS.getFixAge() > 4000)
  {
    FixStatus = NoFix;
  }
  else
  {
    FixStatus = NoFix;
  }
}

String flightTimeStr() {                                                //Returns the flight time as a usable string for print statements  
  unsigned long t = millis() / 1000;
  String fTime = "";
  fTime += (String(t / 3600) + ":");
  t %= 3600;
  fTime += String(t / 600);
  t %= 600;
  fTime += (String(t / 60) + ":");
  t %= 60;
  fTime += (String(t / 10) + String(t % 10));
  return fTime;
}

float flightMinutes() {                                                 //Return time in minutes
  float minutes = millis() / 1000;
  minutes = minutes / 60;
  return minutes;
}

void openFlightlogA() {                                                  //Open flight log
  if (!FlightlogOpenA&&SDcardA) {
    //add .c_str() next to Fname
    FlogA = sdA.open(FnameA.c_str(), FILE_WRITE);
    FlightlogOpenA = true;
  }
}
void closeFlightlogA() {                                                 //Close flight log
  if (FlightlogOpenA&&SDcardA) {
    FlogA.close();
    FlightlogOpenA = false;
  }
}

void openFlightlogB() {                                                  //Open flight log
  if (!FlightlogOpenB&&SDcardB) {
    //add .c_str() next to Fname
    FlogB = sdB.open(FnameB.c_str(), FILE_WRITE);
    FlightlogOpenB = true;
  }
}
void closeFlightlogB() {                                                 //Close flight log
  if (FlightlogOpenB&&SDcardB) {
    FlogB.close();
    FlightlogOpenB = false;
  }
}

//void openFlightlog() {                                                  //Open flight log
//  if (!FlightlogOpen&&SDcard) {
//    //add .c_str() next to Fname
//    Flog = SD.open(Fname.c_str(), FILE_WRITE);
//    FlightlogOpen = true;
//  }
//}
//void closeFlightlog() {                                                 //Close flight log
//  if (FlightlogOpen&&SDcard) {
//    Flog.close();
//    FlightlogOpen = false;
//  }
//}

void printData(){
  Serial.println();
  Serial.println("Measurement Update");
  Serial.println("========================================================================================");
  Serial.println("             Time");
  Serial.print("Flight Time String: ");
  Serial.println(flightTimeStr());
  Serial.print("Flight Minutes: ");
  Serial.println(String(flightMinutes()));
  Serial.println("------------------------------");
  Serial.println("             GPS");
  Serial.print("Latitude: ");
  Serial.println(String(GPS.getLat(), 4));
  Serial.print("Longitude: ");
  Serial.println(String(GPS.getLon(), 4));  
  Serial.print("Altitude: ");
  Serial.println(String(GPS.getAlt_feet(), 1));
  Serial.print("Date and Time: ");
  Serial.println(String(GPS.getMonth()) + "/" + String(GPS.getDay()) + "/" + String(GPS.getYear()) + " " + String(GPS.getHour()) + ":" + String(GPS.getMinute()) + ":" + String(GPS.getSecond()));
  Serial.print("Satellites and Fix Age: ");
  Serial.println((String(GPS.getSats()) + ", " + String(GPS.getFixAge())));
  Serial.println("------------------------------");
  Serial.println("          Temperature");
  Serial.println("   t1       t2"); 
  Serial.println((String(t1,4) + ", " +String(t2,4)));
  Serial.println("------------------------------");
  Serial.println("           Pressure");
  Serial.print("Pressure(PSI): ");
  Serial.println(String(PressurePSI,6));
  Serial.print("Pressure(ATM): ");
  Serial.println(String(PressureATM,6));
  Serial.println("------------------------------");
  Serial.println("       System Statuses");
  Serial.print("Sensor Heater Relay: ");
  Serial.println(sensorHeat_Status);
  Serial.println("------------------------------");
  Serial.println("             OPCs");
  Serial.print("SPS A: ");
      if (SpsA.getLogQuality()){                                       //OPC Statuses
        Serial.println("Good Log");
      } else {
        Serial.println("Bad Log");
      }
  Serial.print("SPS B: ");
      if (SpsB.getLogQuality()){                                       //OPC Statuses
        Serial.println("Good Log");
      } else {
        Serial.println("Bad Log");
      }      
  Serial.println("Raw data:");          
  Serial.println(OPCdata);
  Serial.println("========================================================================================");  
}

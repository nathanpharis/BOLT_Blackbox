//OLED functions
void oledOn(MicroOLED &named){ named.command(DISPLAYON); }              //Turn on Display
                                                                        
void oledOff(MicroOLED &named){ named.command(DISPLAYOFF); }            //Turn off Display

void oledPrintNew(MicroOLED &named, String message){                    //Print new page
  named.clear(PAGE);
  named.setCursor(0, 0);
  named.print(message);
  named.display();
}

void oledPrintAdd(MicroOLED &named, String message){                    //Add to page
  named.print(message);
  named.display();
}

void oledUpdate(){                                                      //Update screen
  if ((millis()>14400000)&&!finalMessage[1]){                           //Recovery screen
    oledOn(oled);
    oled.setFontType(0);
    oledPrintNew(oled, "If Found  Call:JamesFlaten    (651).399.2423");
    finalMessage[1] = true; 
  } 
  else if ((GPS.getAlt_feet()>2000)&&!finalMessage[0]) {              //Turn off screen in flight
    oledPrintNew(oled, "");
    oledOff(oled);
    finalMessage[0] = true; 
  }
  else if(millis()-screenUpdateTimer>=SCREEN_UPDATE_RATE){            //Initialization screen
     String localDataPrint = "";
     
     screenUpdateTimer = millis();

    if (screen == 0) {                                                 //There are two screens that this system will cycle through    
      localDataPrint += "GPSALT:";                                     //Print GPS satellite count
      
      if (GPS.getAlt_feet()>=1000) {
        localDataPrint += String(GPS.getAlt_feet(),2);
      }
      else if (GPS.getAlt_feet() > 100) {
        localDataPrint += String(GPS.getAlt_feet(),2) + " ";
      }
      else if (GPS.getAlt_feet() > 10) {
        localDataPrint += String(GPS.getAlt_feet(),2) + "  ";
      }
      else{
        localDataPrint += String(GPS.getAlt_feet(),2) + "   ";
      }
      localDataPrint += "SAT: ";
      if (GPS.getSats()<10) {
        localDataPrint += '0' + String(GPS.getSats()) + " ";
      } else {
        localDataPrint += String(GPS.getSats()) + " ";
      }
       
     screen++; 
     oledPrintNew(oled, localDataPrint);
     
    } 
    else if (screen == 1) {
      if ((millis() - oledTime) < LOG_RATE) {                      //Indicate proper logging
        localDataPrint += "GoodLog";
      } else {
        localDataPrint += "BadLog!";
       }

      if (t2<=-10) {
        localDataPrint += String(t2,2) + " ";
      }
      else if (t2>=10 || (-10<t2 && t2<0)) {
        localDataPrint += String(t2,2) + "  ";                            //Internal temp
      }
      else if (t2>=0 && t2<10) {
        localDataPrint += String(t2,2) + "   ";
      }
            
      localDataPrint += String(flightMinutes(),2);                        //Master timer

         
      screen++;
      oledPrintNew(oled, localDataPrint);  
    }
    
    else if (screen == 2) {
      if (SpsA.getLogQuality()){
        localDataPrint+= "SA1 ";
      } else {
        localDataPrint += "SA0 ";
      }
      if (SpsB.getLogQuality()){
        localDataPrint+= "SB1 ";
      } else {
        localDataPrint += "SB0 ";
      }

      screen = 0;
      oledPrintNew(oled, localDataPrint);
    }
    else {
       oledPrintNew(oled, "Error");
       screen = 0;
    }
  }
}

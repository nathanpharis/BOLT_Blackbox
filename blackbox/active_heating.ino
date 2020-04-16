void actHeat(){  
  float sensTemp;
  
  if (t1 < -100) {
    sensTemp = t2;
  } 
  else {
    sensTemp = t1;
  }
  
  if(-100 < sensTemp && sensTemp < LOW_TEMP){
    coldSensor = true;    
  }
  if(sensTemp > HIGH_TEMP){
    coldSensor = false;
  }

  if(coldSensor && sensorHeatRelay.getState()==false){
    sensorHeatRelay.setState(true);
  }
  else if(!coldSensor && sensorHeatRelay.getState()==true){
    sensorHeatRelay.setState(false);
  }
}

//Function to generate the output packet

void sendDataPacket(){
  uint16_t checksum = 0;                          //Initialize the necessary local variables
  String packet = "";
  byte outputBytes[82];
  byte checksumBytes[2];
  
  packet += String(BEGIN);                        //Initialize the packet
  packet += String(SYSTEM_ID);

  packet += String(packetNum++);                  //Increase the packet number
  
  packet += String(millis());                     //Add the relative time stamp
  
  packet += String(GPS.getHour());                //Add the absolute time stamp
  packet += String(GPS.getMinute());
  packet += String(GPS.getSecond());
  
  packet += String(GPS.getLat(), 4);              //Add the GPS location
  packet += String(GPS.getLon(), 4);
  packet += String(GPS.getAlt_feet(), 1);
  
  packet += String(t1);                           //Add the temperatures of SPSA and SPSB, respectively
  packet += String(t2);
  
  packet += String(PressurePSI);                  //Add the pressure around the SPS systems
  
  packet += String(uint16_t(SpsA.getTot()));                //Add the SPSA data
  packet += String(SpsA.SPSdata.nums[0]);
  packet += String(SpsA.SPSdata.nums[1]);
  packet += String(SpsA.SPSdata.nums[2]);
  packet += String(SpsA.SPSdata.nums[3]);
  packet += String(SpsA.SPSdata.nums[4]);
  
  packet += String(uint16_t(SpsB.getTot()));                //Add the SPSB data
  packet += String(SpsB.SPSdata.nums[0]);
  packet += String(SpsB.SPSdata.nums[1]);
  packet += String(SpsB.SPSdata.nums[2]);
  packet += String(SpsB.SPSdata.nums[3]);
  packet += String(SpsB.SPSdata.nums[4]);
  
  packet.getBytes(outputBytes, 79);               //Pass the packet to the output array as bytes
  
  for (unsigned short i = 0; i < 79; i++){        //Calculate the checksum
    checksum += outputBytes[i];
  }

  memcpy(&checksum, &checksumBytes, 2);           //Pass the checksum bytes to a staging array
  
  outputBytes[79] = checksumBytes[0];             //Add the checksum to the output
  outputBytes[80] = checksumBytes[1];
  
  outputBytes[81] = STOP;                         //Add the stop byte
  
  for (unsigned short i = 0; i < 82; i++){        //Send the data
    DATA_SERIAL.write(outputBytes[i]);
  }
}

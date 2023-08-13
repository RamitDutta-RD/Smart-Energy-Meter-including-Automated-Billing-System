/* Project Group 2 */
/* Smart Energy Meter for measuring power consumption */

#include <ArduinoJson.h>
#include <SoftwareSerial.h>

/* ACS712 Model         Optimized Current Range              Output Sensitivity
-------------------------------------------------------------------------------

ACS712 ELC-05           +/- 5A                              185 mV/A           */

int mVperAmp = 185; 


#define samplerate 89

String str;

int voltages[samplerate], currents[samplerate];
float tempV, tempI;

double Voltage = 0, Amps = 0, Power=0, PF;
SoftwareSerial linkSerial(10, 11); // RX, TX

void setup()
{
  Serial.begin(115200);
  linkSerial.begin(4800);


}

void loop()
{

  ///////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////Sampling/////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////
  
  
  for (int i = 0; i < samplerate; i++)
  {
    voltages[i] = analogRead(A2);
    currents[i] = analogRead(A1);
    
  }


  ///////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////calculating avg //////////////////////////////
  //////////////////////////////////////////////////////////Now/////////////////////////
  
  Voltage = 0;
  Amps = 0;
  
    for (int i = 0; i < samplerate; i++)      
  {
    Voltage += voltages[i];
    Amps+= currents[i];
  }
  
  Voltage/=samplerate;
  Amps/=samplerate;


  ///////////////////////////////////////////////////////////////////////////////////
  ///////////// eliminating avg from the samples to keep the ac part only ///////////
  ///////////////////////////////////////////////////////////////////////////////////
  
      for (int i = 0; i < samplerate; i++)
  {
    voltages[i]-=Voltage;
    currents[i]-=Amps;
  }


  ///////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////Scale Convertion///////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////
  
  Voltage = 0;
  Amps = 0;
  Power = 0;
  
  for (int i = 0; i < samplerate; i++) // in one cycle
  {

    tempV = 5*(11.0*220.0/6)*(voltages[i])/1023.0; // 220 - 6V transformer and 10k and 1k resistor in series in the 6V outlet

    tempI = (5000.0*(currents[i])) / (1023.0*mVperAmp) ; // Gets current

    Power += tempV*tempI;
    
    Voltage = Voltage + pow(tempV,2);
    Amps = Amps + pow(tempI, 2);
  }


  //////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////rms values ///////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////
  
  Voltage = sqrt(Voltage/samplerate);
  Amps = sqrt (Amps / samplerate);
  Power/=samplerate;
  float pf = (Power/(Voltage*Amps)); 

  Serial.print(Voltage);
  Serial.print(' ');
  Serial.print(Amps);
  Serial.print(' ');
  Serial.print(Power);
  Serial.print(' ');
  Serial.println (pf); 
  //display.clearDisplay();
  
  str = "Real power : " + String (Power) + "Watt" + "  " + "Current :" + String (Amps) + "A" + "  " + "Supply Voltage:" + String (Voltage) + "Volts" + "  " + "Power Factor :" + String (pf) ;
  Serial.print (str);

   
  StaticJsonDocument<400> doc;
  doc["Voltage"] = Voltage;
  doc["Current"] = Amps;
  doc["Real Power"] = Power;
  doc["PF"] = pf;
  serializeJson(doc, linkSersial);
   
  delay (10000);

}

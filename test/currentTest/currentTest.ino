/* Author: Sakib Chowdhury */

/* ACS712 Model         Optimized Current Range	            Output Sensitivity
-------------------------------------------------------------------------------

ACS712 ELC-05           +/- 5A                              185 mV/A

ACS712 ELC-20           +/- 20A                             100 mV/A

ACS712 ELC-30           +/- 30A                             66 mV/A */


uint8_t sensitivity = 185;    
uint16_t sensorOffset = 1660;           // in mV
double adcVoltage = 0;                  // in mV
double currentAmps = 0;                 // in Amps
uint8_t currentSensorPin = A0; 
uint16_t adcVal = 0;


/* 
void calibrate(){

    for (uint8_t i = 0; i< 100; i++){
        adcVal = analogRead(currentSensorPin);
        sensorOffset += (adcVal / 1024.0)*3300;        // 3300 means 3.3V --> written for ESP8266
    }

    sensorOffset /= 100;


    Serial.println("Calibration -------------------------------------> DONE");
    Serial.println(sensorOffset);
    
}
*/

void setup(){
    Serial.begin(115200);
    Serial.println("Setup ---------------------------------> OK ");
    //calibrate();
}


void loop(){
    adcVal = analogRead(currentSensorPin);
    adcVoltage = (adcVal / 1024.0)*3300;        // 3300 means 3.3V --> written for ESP8266
    currentAmps = (adcVoltage - sensorOffset)/sensitivity; 

    Serial.print("sensor reading ---------------------> "); Serial.println(adcVal);
    Serial.print("adcVltage reading ---------------------> "); Serial.println(adcVoltage);
    Serial.print("current in Amps --------------------> "); Serial.println(currentAmps);
    Serial.println("");Serial.println("");

    delay(500);

}

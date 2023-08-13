uint8_t zeroCrossPin = 2;
uint8_t flag = 0;
uint8_t mVperAmp = 185;
uint16_t RawValue = 0;
uint16_t ACSoffset = 2500;

float Vcorr = 1.0;
float ccorr = 1.0;

#define MAXLIMITOFARRAY 200

uint8_t samplerate = 0;


uint16_t voltages[MAXLIMITOFARRAY], currents[MAXLIMITOFARRAY];
float tempV, tempI;
uint16_t voltage_offset = 512, current_offset = 512;
double Voltage = 0, Amps = 0, Power=0, PF;


/*
algorithm: 1
Step1: calibrate currentSensor offset value
Step2: attach Interrupt
Step3: after interrupt is called, calculate 3 values of current, if they are increasing/decreasing consequtively, know the polarity of power factor
Step4: detach interrupt 
Step5: attach Interrupt with another ISR. This time we'll find the zero cross of the current
Step6: when the code enters ISR,measure currentTime by millis function.
Step7: keep calculating current value in a while loop. when the current falls inside a tolerance limit of the zero value, when this occurs,
        immediately measure the time difference between previous time and current time through millis() function 
Step8: convert time_difference to seconds. theta = 2*pi*line_frequency*time_difference
Step9: pf = cos(theta)
Step10: Measure voltageMax ---> unnecessary actually, since 230V is almost constant 
Step11: Measure currentMax 
Step12: 


*/

/************************************************************************************************************************************/
/*********************************************************** algorithm 2 ************************************************************/
/************************************************************************************************************************************/


void setup(){
    Serial.begin(9600);
    Serial.println("setup -------------------------> Done !! ");
    attachInterrupt(digitalPinToInterrupt(zeroCrossPin), callback, FALLING);
    
}


void loop(){
    uint8_t i = 0; 
    while (flag == 0) Serial.println("stuck in flag = 0");
    while (flag > 0 && flag < 3){
        voltages[i] = analogRead(A2);
        currents[i] = analogRead(A1);
        //Serial.println(i);
        i = i+1;
    }
    samplerate = i;
    Serial.print("sampleRate------------------> "); Serial.println(samplerate);
    Serial.print("i --------------------------> "); Serial.println(i);
    Voltage = 0;
    Amps = 0;

    for (i = 0; i < samplerate; i++)
    {
        Voltage += voltages[i];
        Amps+= currents[i];
        delayMicroseconds(1);
    }

    Voltage/=samplerate;
    Amps/=samplerate;

    Serial.println("average voltage/current------------------> OK");

     for (i = 0; i < samplerate; i++)
    {
        voltages[i]-=Voltage;
        currents[i]-=Amps;
    }
    Serial.println("Average erased --------------------------> OK");
    Voltage = 0;
    Amps = 0;
    Power = 0;

    for (i = 0; i < samplerate; i++)
    {
        //Serial.println(voltages[i]);
        tempV = 5*(11.0*220.0/12)*(voltages[i])/1023.0*Vcorr;
        tempI = (5000.0*(currents[i])) / (1023.0*mVperAmp)*ccorr ; // Gets you mV
        Power += tempV*tempI;
        
        Voltage = Voltage + pow(tempV,2);
        Amps = Amps + pow(tempI, 2);
    }
    Serial.println("RMS CALCULATION --------------------------> OK");
    Voltage = sqrt(Voltage/samplerate);
    Amps = sqrt (Amps / samplerate);
    Power/=samplerate;
    float pf = (Power/(Voltage*Amps));

    Serial.println(samplerate);
    Serial.print("voltage: ");Serial.println(Voltage);
    Serial.print("Current: "); Serial.println(Amps);
    Serial.print("Power: "); Serial.println(Power);
    Serial.print("PF: "); Serial.println (pf); 

    delay (2000); 
    flag = 0; 
    
}

void callback() {

    if (flag < 3){
        flag = flag+1; 
    }
}

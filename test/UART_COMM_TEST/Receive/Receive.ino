#include <SoftwareSerial.h>

SoftwareSerial receive(13,15);
void setup(){
    Serial.begin(115200);
    receive.begin(4800);

}

void loop(){
    if (receive.available()){
        Serial.write(receive.read());
    }
}
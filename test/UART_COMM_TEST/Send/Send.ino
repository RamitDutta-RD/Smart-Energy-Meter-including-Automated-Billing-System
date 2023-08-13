#include <SoftwareSerial.h>

SoftwareSerial send(2,3);
void setup(){
    //Serial.begin(115200);
    send.begin(9600);

}

void loop(){
    send.println("hello world");
    delay(1000);
}
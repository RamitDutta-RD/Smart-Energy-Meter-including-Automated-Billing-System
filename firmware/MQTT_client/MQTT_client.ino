#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP8266WiFi.h>

double volt, curr, rpower, pf;

/************************************************************************************/
/***************************** Defining Wifi ****************************************/
/************************************************************************************/

#define SSID "Galaxy A51FFCC"
#define PASS "zgyk2512"

/************************************************************************************/
/***************************** MQTT params ******************************************/
/************************************************************************************/

#define HOST "io.adafruit.com"
#define PORT 1883
#define USRNAME "bueteeeproject"
#define KEY "aio_VrpD56mWsuat7us66n8obl5LceeL"

/***********************************************************************************/
/***************************** MQTT instance ***************************************/
/***********************************************************************************/

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USRNAME, KEY);

/***********************************************************************************/
/****************************** Topic **********************************************/
/***********************************************************************************/

Adafruit_MQTT_Publish voltage = Adafruit_MQTT_Publish(&mqtt, USRNAME "/feeds/voltage");
Adafruit_MQTT_Publish current = Adafruit_MQTT_Publish(&mqtt, USRNAME "/feeds/current");
Adafruit_MQTT_Publish realPower = Adafruit_MQTT_Publish(&mqtt, USRNAME "/feeds/realPower");
Adafruit_MQTT_Publish powerFactor = Adafruit_MQTT_Publish(&mqtt, USRNAME "/feeds/powerFactor");

/***********************************************************************************/
/*********************************** MQTT Connection *******************************/
/***********************************************************************************/

void MQTT_connect()
{
    int8_t ret;
    if (mqtt.connected())
    {
        return;
    }
    Serial.print("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0)
    { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        mqtt.disconnect();
        delay(5000); // wait 5 seconds
        retries--;
        if (retries == 0)
        {
            // basically die and wait for WDT to reset me
            while (1)
                ;
        }
    }
    Serial.println("MQTT Connected!");
}

// Declare the "link" serial port
// Please see SoftwareSerial library for detail
SoftwareSerial linkSerial(13, 15); // RX, TX

void setup()
{
    // Initialize "debug" serial port
    // The data rate must be much higher than the "link" serial port
    Serial.begin(115200);
    while (!Serial)
        continue;

    // Initialize the "link" serial port
    // Use the lowest possible data rate to reduce error ratio
    linkSerial.begin(4800);

    delay(10);
    Serial.println("Setup Started... ");

    Serial.print("connecting to ");
    Serial.println(SSID);
    WiFi.begin(SSID, PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println(" ");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Setup Done ... ");
}

void loop()
{
    // Check if the other Arduino is transmitting
    MQTT_connect();
    if (linkSerial.available())
    {
        // Allocate the JSON document
        // This one must be bigger than for the sender because it must store the strings
        StaticJsonDocument<500> doc;

        // Read the JSON document from the "link" serial port
        DeserializationError err = deserializeJson(doc, linkSerial);

        if (err == DeserializationError::Ok)
        {
            // Print the values
            // (we must use as<T>() to resolve the ambiguity)

            volt = doc["Voltage"].as<double>();
            curr = doc["Current"].as<double>();
            rpower = doc["Real Power"].as<double>();
            pf = doc["PF"].as<double>();

            Serial.print("Voltage = ");
            Serial.println(volt);
            Serial.print("Current = ");
            Serial.println(curr);
            Serial.print("Power = ");
            Serial.println(rpower);
            Serial.print("PF = ");
            Serial.println(pf);



            Serial.print("publish to voltage topic ---------------------------------------> ");
            if (!voltage.publish(volt))
            {
                Serial.println(F(" FAILED"));
            }
            else
            {
                Serial.println(F(" OK"));
            }



            Serial.print("publish to current topic ---------------------------------------> ");
            if (!current.publish(curr))
            {
                Serial.println(F(" FAILED"));
            }
            else
            {
                Serial.println(F(" OK"));
            }


            Serial.print("publish to real power topic ------------------------------------> ");
            if (!realPower.publish(rpower))
            {
                Serial.println(F(" FAILED"));
            }
            else
            {
                Serial.println(F(" OK"));
            }


            Serial.print("publish to power factor topic ----------------------------------> ");
            if (!powerFactor.publish(pf))
            {
                Serial.println(F(" FAILED"));
            }
            else
            {
                Serial.println(F(" OK"));
            }
        }
        else
        {
            // Print error to the "debug" serial port
            Serial.print("deserializeJson() returned ");
            Serial.println(err.c_str());

            // Flush all bytes in the "link" serial port buffer
            while (linkSerial.available() > 0)
                linkSerial.read();
        }
    }
}

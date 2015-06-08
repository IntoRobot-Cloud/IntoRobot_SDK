/*
************************************************************************
* @author  :  IntoRobot Team
* @version :  V1.0.0
* @date     :  03-30-15
************************************************************************
 This application example code is in the public domain.

   control Led througt the IntoRobot Cloud

this example is writted for arduino with WiFi shield

*/
#include <SPI.h>
#include <WiFi.h>
#include <IntoRobot.h>


char ssid[] = "yourNetwork"; //  your network SSID (name)
char pass[] = "secretPassword";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;

//intorobot topic
#define CMD_LIGHTSWITCH "channel/lightSwitch_0/cmd/switch" 
#define DATA_LIGHTSTATE  "channel/lightSwitch_0/data/light"

const char *device_id = "your_device_id";         //device id, get from the IntoRobot Cloud
const char *access_token = "your_access_token";  //access token, get from the IntoRobot Cloud
WiFiClient wiFiclient;
IntorobotClass IntoRobot(device_id, access_token, wiFiclient);

void setup() 
{
    pinMode(13, OUTPUT);  
    //Initialize serial and wait for port to open:
    Serial.begin(9600);

    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) 
    {
        Serial.println("WiFi shield not present");
        while (true);
    }

    //get the wifi firmware version
    String fv = WiFi.firmwareVersion();
    if ( fv != "1.1.0" )
    {Serial.println("Please upgrade the firmware");}

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) 
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
    }
    Serial.println("Connected to wifi");

    //until the arduino connnet to the IntoRobot Cloud
    while(!IntoRobot.connected())
    {
        IntoRobot.process();
    }
    //subscribe the Led Control topic
    IntoRobot.subscribe(CMD_LIGHTSWITCH, NULL, LightSwitchCb);
}

void loop() 
{
    IntoRobot.process();
}

//the Led Control CallBack Function
void LightSwitchCb(uint8_t * payload, uint32_t len)
{
    IntoRobot.printf("LightSwitchCb  data = %c", payload[0]);
    if(!memcmp(payload,"1",1))
    {
        digitalWrite(13, HIGH);// Turn ON the LED pins  
        IntoRobot.publish(DATA_LIGHTSTATE, (uint8_t *)"1", 1);

    }
    else if(!memcmp(payload,"0",1))
    {
        digitalWrite(13, LOW);// Turn ON the LED pins  
        IntoRobot.publish(DATA_LIGHTSTATE, (uint8_t *)"0", 1);
    }
}


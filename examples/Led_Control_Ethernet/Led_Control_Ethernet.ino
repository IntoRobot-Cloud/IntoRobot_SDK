/*
************************************************************************
* @author  :  IntoRobot Team
* @version :  V1.0.0
* @date     :  03-30-15
************************************************************************
 This application example code is in the public domain.

   control Led througt the IntoRobot Cloud

this example is writted for arduino with Ethernet shield

*/
#include <SPI.h>
#include <Ethernet.h>
#include <IntoRobot.h>

// Update these with values suitable for your network.
byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 100 };

//intorobot topic
#define CMD_LIGHTSWITCH "channel/lightSwitch_0/cmd/switch" 
#define DATA_LIGHTSTATE  "channel/lightSwitch_0/data/light"

const char *device_id = "your_device_id";         //device id, get from the IntoRobot Cloud
const char *access_token = "your_access_token";  //access token, get from the IntoRobot Cloud
EthernetClient ethClient;
IntorobotClass IntoRobot(device_id, access_token, ethClient);

void setup() 
{
    pinMode(13, OUTPUT);  
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) 
    {
        Serial.println("Failed to configure Ethernet using DHCP");
        // no point in carrying on, so do nothing forevermore:
        // try to congifure using IP address instead of DHCP:
        Ethernet.begin(mac, ip);
    }
    // give the Ethernet shield a second to initialize:
    delay(1000);
    Serial.println("ethernet connected...");

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


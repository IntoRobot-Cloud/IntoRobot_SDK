/**
 ******************************************************************************
 * @file     : intorobot.cpp
 * @author   : robin
 * @version  : V1.0.0
 * @date     : 6-December-2014
 * @brief    :    
 ******************************************************************************
  Copyright (c) 2013-2014 IntoRobot Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */
#include "IntoRobot.h"
#include <stdarg.h>

//firmware  info
#define INTOROBOT_FIRMWARE_VER   "2.0.1"                           //arduino library version
#define INTOROBOT_PRODUCT_ID     "553c6699084d55640e332113"        //arduino  product  id
#define INTOROBOT_APPLICATION_ID "FFFFFFFFFFFFFFFFFFFFFFFF"        //arduino application id 

//sevice info
#define INTOROBOT_SERVER_IP     "52.74.196.79"
#define INTOROBOT_SERVER_DOMAIN "iot.intorobot.net"
#define INTOROBOT_SERVER_PORT   1883
#define INTOROBOT_API_VER       "v1"

//publish
//last will
#define INTOROBOT_MQTT_WILLTOPIC   "platform/default/info/accident"
#define INTOROBOT_MQTT_WILLQOS     0
#define INTOROBOT_MQTT_WILLRETAIN  true
#define INTOROBOT_MQTT_WILLMESSAGE "off-line"

//device info
#define INTOROBOT_MQTT_INFOTOPIC   "firmware/default/info/state"
#define INTOROBOT_MQTT_INFOQOS     0
#define INTOROBOT_MQTT_INFORETAIN  true

//device debug info
#define INTOROBOT_MQTT_SENDBUGTOPIC   "firmware/default/info/debug"
#define INTOROBOT_MQTT_SENDDEBUGQOS     0

//receive debug 
#define INTOROBOT_MQTT_RECDEBUGTOPIC   "firmware/default/action/debug"
#define INTOROBOT_MQTT_RECDEBUGQOS     0


volatile uint8_t INTOROBOT_CLOUD_CONNECT = 1;     //default connect platform system
volatile uint8_t INTOROBOT_CLOUD_CONNECTED = 0;   //connected
volatile uint8_t INTOROBOT_CLOUD_RECONNECT = 0;   //whether reconnected

const char *intorobot_device_id;      //user device_id
const char *intorobot_access_token;   //user acces_token
struct _callbacklist callbacklist;
Cloud_Debug_Buffer  IntoRobot_Debug_tx_buffer;
Cloud_Debug_Buffer  IntoRobot_Debug_rx_buffer;

/*********************************************************************************
  *Function		:   IntorobotClass::IntorobotClass(void)    
  *Description	:   consturctor function
  *Input              :   none     
  *Output		:   none      
  *Return		:   none     
  *author		:   robot    
  *date			:   2015-02-01        
  *Others		:         
**********************************************************************************/
IntorobotClass::IntorobotClass(const char *device_id, const char *access_token, Client& mqttclient)
{
	memset(&IntoRobot_Debug_tx_buffer,0,sizeof(IntoRobot_Debug_tx_buffer));
    memset(&IntoRobot_Debug_rx_buffer,0,sizeof(IntoRobot_Debug_rx_buffer));

	_verbose = false;
    
    intorobot_access_token = access_token;
    intorobot_device_id = device_id;
    
    ApiMqttClient = MqttClientClass((char *)INTOROBOT_SERVER_DOMAIN, INTOROBOT_SERVER_PORT, ApiMqttClientCallBack, mqttclient);
}

/*********************************************************************************
  *Function		:    bool IntorobotClass::connected(void)   
  *Description	:    whether board connected intorobot system
  *Input              :    none      
  *Output		:    none     
  *Return		:    the result fo connecting    
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
bool IntorobotClass::connected(void)
{
    if(INTOROBOT_CLOUD_CONNECTED)
    {return true;}
    else
    {return false;}
}

/*********************************************************************************
  *Function		:    void IntorobotClass::connect(void)   
  *Description	:    connect the  intorobot system
  *Input              :    none  
  *Output		:    none     
  *Return		:    none    
  *author		:    robot    
  *date			:    2015-02-01       
  *Others		:         
**********************************************************************************/
void IntorobotClass::connect(void)
{
    INTOROBOT_CLOUD_CONNECT = 1;
}

/*********************************************************************************
  *Function		:    void IntorobotClass::disconnect(void)   
  *Description	:   disconnect the intorobot system
  *Input              :    none       
  *Output		:    none     
  *Return		:    none    
  *author		:    robot    
  *date			:    2015-02-01       
  *Others		:         
**********************************************************************************/
void IntorobotClass::disconnect(void)
{
    ApiMqttClient.disconnect();
	INTOROBOT_CLOUD_CONNECT = 0;
}

/*********************************************************************************
  *Function		:    uint8_t IntorobotClass::publish(const char *topic, char *payload)   
  *Description	:    publish the topic 
  *Input              :         
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::publish(const char *topic, char *payload)
{
    return publish(topic, (uint8_t *)payload, strlen(payload), 0, true); 
}

/*********************************************************************************
  *Function		:   uint8_t IntorobotClass::publish()
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::publish(const char* topic, uint8_t* payload, unsigned int plength)
{
    return publish(topic, payload, plength, 0, true); 
}

/*********************************************************************************
  *Function		:     uint8_t IntorobotClass::publish()  
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::publish(const char* topic, uint8_t* payload, unsigned int plength, uint8_t retained)
{
    return publish(topic, payload, plength, 0, retained); 
}

/*********************************************************************************
  *Function		:      uint8_t IntorobotClass::publish() 
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::publish(const char* topic, uint8_t* payload, unsigned int plength, uint8_t qos, uint8_t retained)
{
    String fulltopic;
    
    fill_mqtt_topic(fulltopic, topic, NULL);    
    if (_verbose) 
    {
        Serial.println(fulltopic.c_str());
    }
    return ApiMqttClient.publish(fulltopic.c_str(), payload, plength, qos, retained); 
}

/*********************************************************************************
  *Function		:   uint8_t IntorobotClass::subscribe()    
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::subscribe(const char* topic, const char *device_id, void (*callback)(uint8_t*, uint32_t))
{
    return subscribe(topic, device_id, callback, 0);
}

/*********************************************************************************
  *Function		:     uint8_t IntorobotClass::subscribe()         
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::subscribe(const char* topic, const char *device_id, void (*callback)(uint8_t*, uint32_t), uint8_t qos)
{
    String fulltopic;

    addsubcallback((char *)topic, (char *)device_id, callback, qos);
    fill_mqtt_topic(fulltopic, topic, device_id);    
    if (_verbose) 
    {
        Serial.println(fulltopic.c_str());
    }
    return ApiMqttClient.subscribe(fulltopic.c_str(), qos);
}

/*********************************************************************************
  *Function		:       uint8_t IntorobotClass::unsubscribe()       
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
uint8_t IntorobotClass::unsubscribe(const char *topic, const char *device_id)
{
    String fulltopic;

    delsubcallback((char *)topic, (char *)device_id);
    fill_mqtt_topic(fulltopic, topic, device_id);
    return ApiMqttClient.unsubscribe(fulltopic.c_str());
}

/*********************************************************************************
  *Function		:    int IntorobotClass::deviceInfo(char *product_id, char *device_id, char *access_token)
  *Description	:    get the device info
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
int IntorobotClass::deviceInfo(char *product_id, char *device_id, char *access_token)
{
    if((product_id==NULL) || (device_id==NULL) || (access_token==NULL))
    {
        return false;
    }

    strcpy(product_id,INTOROBOT_PRODUCT_ID);
    strcpy(device_id,intorobot_device_id);
    strcpy(access_token,intorobot_access_token);
    return true;
}

/*********************************************************************************
  *Function		:   void IntorobotClass::process(void)    
  *Description	:   
  *Input              :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void IntorobotClass::process(void)
{
    if(INTOROBOT_CLOUD_CONNECT)
    {
        //判断后台是否联通
        if(!INTOROBOT_CLOUD_CONNECTED)
        {
            String fulltopic;
            char temp[256];

            if (_verbose) 
            {
                Serial.println("intorobot cloud connecting");
            }
            //connect intorobot cloud
            fill_mqtt_topic(fulltopic, INTOROBOT_MQTT_WILLTOPIC, NULL);
            memset(temp,0,sizeof(temp));
            strcpy(temp,&intorobot_device_id[1]);
		    if (_verbose) 
            {
                Serial.println(intorobot_device_id);
                Serial.println(intorobot_access_token);
            }
            if(ApiMqttClient.connect(temp, intorobot_access_token, intorobot_device_id, fulltopic.c_str(), INTOROBOT_MQTT_WILLQOS, INTOROBOT_MQTT_WILLRETAIN, INTOROBOT_MQTT_WILLMESSAGE))
            {
                if (_verbose) 
                {
                    Serial.println("mqtt connecting");
                }
                //publish  default topic
                memset(temp,0,sizeof(temp));
                strcat(temp,"{\"key\":\"online\"}");
                publish(INTOROBOT_MQTT_INFOTOPIC, (uint8_t*)temp, strlen(temp),true);

                memset(temp,0,sizeof(temp));
                sprintf(temp,"{\"product_id\":\"%s\",\"application_id\":\"%s\",\"firmware_version\":\"%s\"}", INTOROBOT_PRODUCT_ID, INTOROBOT_APPLICATION_ID, INTOROBOT_FIRMWARE_VER);
                publish(INTOROBOT_MQTT_INFOTOPIC, (uint8_t*)temp, strlen(temp),false);

                //subscibe default topic
                if(INTOROBOT_CLOUD_RECONNECT==0)
                {
                    if (_verbose) 
                    {
                        Serial.println("cloud connect first");
                    }
                    subscribe(INTOROBOT_MQTT_RECDEBUGTOPIC, NULL, SystemDebugCallback);
                }
                else  //resubscribe topic
                {
                    INTOROBOT_CLOUD_RECONNECT = 0;
                    if (_verbose) 
                    {
                        Serial.println("cloud reconnected ");
                    }
                    resubscribe();
                }

                INTOROBOT_CLOUD_CONNECTED=1;
            }
        }
        else
        {
            if(false == ApiMqttClient.loop())
            {
                if (_verbose) 
                {
                    Serial.println("mqtt connect false ");
                }
                INTOROBOT_CLOUD_CONNECTED=0;
                INTOROBOT_CLOUD_RECONNECT=1;//重连平台
            }
            else
            {
                sendDebug(); 
            }//如果有调试信息 发送调试信息
        }
    }
    else
    {
        //如果连接断开
        if(ApiMqttClient.connected()) 
        {
            ApiMqttClient.disconnect();
        }
    }
}

/*********************************************************************************
  *Function		:    size_t IntorobotClass::printf(const char *fmt, ...
  *Description	:    
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
#define MAX_PRINTF_MESSAGE_LENGTH 128
size_t IntorobotClass::printf(const char *fmt, ...) 
{
    char buffer[MAX_PRINTF_MESSAGE_LENGTH]; 
    va_list args;   

    va_start(args,fmt); 
    vsprintf(buffer,fmt,args); 

    return print(buffer);
} 

/*********************************************************************************
  *Function		:    void SpacebrewYun::verbose(boolean verbose = true) 
  *Description	:    
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void IntorobotClass::verbose(bool verbose = true) 
{
    _verbose = verbose;
}

/*********************************************************************************
  *Function		:    void IntorobotClass::sendDebug(void)   
  *Description	:    send debug info to the platform system
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void IntorobotClass::sendDebug(void)
{
    u32 n;
    String s_debug_info="";

    for(n=0; (n<MQTT_MAX_PACKET_SIZE)&&(IntoRobot_Debug_tx_buffer.tail!=IntoRobot_Debug_tx_buffer.head); n++)
    {
        s_debug_info += (char)IntoRobot_Debug_tx_buffer.buffer[IntoRobot_Debug_tx_buffer.tail];
        IntoRobot_Debug_tx_buffer.tail = (unsigned int)(IntoRobot_Debug_tx_buffer.tail + 1) % Cloud_DEBUG_BUFFER_SIZE;
    }

    if(n)
    {
        publish(INTOROBOT_MQTT_SENDBUGTOPIC, (uint8_t *)s_debug_info.c_str(), n, false);
    }
}

/*********************************************************************************
  *Function		:   void IntorobotClass::fill_mqtt_topic(String &fulltopic, const char *topic, const char *device_id)    
  *Description	:   fill mqtt topic
  *Input		:          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void IntorobotClass::fill_mqtt_topic(String &fulltopic, const char *topic, const char *device_id)
{
    char temp[128];
    
    memset(temp,0,sizeof(temp));
    if(device_id == NULL)
    {sprintf(temp,"%s/%s/", INTOROBOT_API_VER, intorobot_device_id);}
    else
    {sprintf(temp,"%s/%s/", INTOROBOT_API_VER, device_id);}
    fulltopic=temp;
    fulltopic+=topic;
}

/*********************************************************************************
  *Function		:    size_t IntorobotClass::write(uint8_t byte)   
  *Description	:    
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
size_t IntorobotClass::write(uint8_t byte)
{
    IntoRobot_Debug_tx_buffer.buffer[IntoRobot_Debug_tx_buffer.head] = byte;   
    IntoRobot_Debug_tx_buffer.head++;
    /* To avoid buffer overflow */
    if(IntoRobot_Debug_tx_buffer.head == Cloud_DEBUG_BUFFER_SIZE)
    {
        IntoRobot_Debug_tx_buffer.head = 0;
    }
    return 1;
}

/*********************************************************************************
  *Function		:    int IntorobotClass::read(void) 
  *Description	:    
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
int IntorobotClass::read(void)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (IntoRobot_Debug_rx_buffer.head == IntoRobot_Debug_rx_buffer.tail)
    {
        return -1;
    }
    else
    {
        unsigned char c = IntoRobot_Debug_rx_buffer.buffer[IntoRobot_Debug_rx_buffer.tail];
        IntoRobot_Debug_rx_buffer.tail = (unsigned int)(IntoRobot_Debug_rx_buffer.tail + 1) % Cloud_DEBUG_BUFFER_SIZE;
        return c;
    }
}

/*********************************************************************************
  *Function		:    int IntorobotClass::available(void)
  *Description	:    
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
int IntorobotClass::available(void)
{
    return (unsigned int)(Cloud_DEBUG_BUFFER_SIZE + IntoRobot_Debug_rx_buffer.head - IntoRobot_Debug_rx_buffer.tail) % Cloud_DEBUG_BUFFER_SIZE;
}

/*********************************************************************************
  *Function		:       void resubscribe(void)
  *Description	:   
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void IntorobotClass::resubscribe(void)
{
    //删除一个
    for (int i = 0 ; i < callbacklist.total_callbacks; i++)
    {
        subscribe(callbacklist.callbacknode[i].topic, callbacklist.callbacknode[i].device_id, 
        callbacklist.callbacknode[i].callback, callbacklist.callbacknode[i].qos);
    }
}

/*********************************************************************************
  *Function		:     CB getsubcallback(char * fulltopic)
  *Description	:   
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
CB getsubcallback(char * fulltopic)
{
    char topictmp[128]={0};
    
    //首先判断是否有相同的topic，如果有相同的，覆盖，如果没有，则在尾部增添
    for (int i = 0 ; i < callbacklist.total_callbacks; i++)
    {
        memset(topictmp, 0, sizeof(topictmp));
        if(callbacklist.callbacknode[i].device_id == NULL)
        {sprintf(topictmp,"%s/%s/", INTOROBOT_API_VER, intorobot_device_id);}
        else
        {sprintf(topictmp,"%s/%s/", INTOROBOT_API_VER, callbacklist.callbacknode[i].device_id);}
        strcat(topictmp,callbacklist.callbacknode[i].topic);
        if (strcmp(fulltopic, topictmp) == 0)
        {
            return callbacklist.callbacknode[i].callback;
        }
    }
    return NULL;
}

/*********************************************************************************
  *Function		:    void addsubcallback()   
  *Description	:   
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void addsubcallback(char *topic, char *device_id, void (*callback)(uint8_t*, uint32_t), uint8_t qos)
{
    int if_found_topic = 0;
    int i = 0;

    //首先判断是否有相同的topic，如果有相同的，覆盖，如果没有，则在尾部增添
    for (i = 0 ; i < callbacklist.total_callbacks; i++)
    {
        if ((topic == callbacklist.callbacknode[i].topic)&&(device_id == callbacklist.callbacknode[i].device_id))
        {
            if_found_topic = 1;
            break;
        }
    }

    if (if_found_topic)
    {
        callbacklist.callbacknode[i].callback = callback; 
        callbacklist.callbacknode[i].qos = qos; 
    }
    else
    {
        if (callbacklist.total_callbacks == MAX_CALLBACK_NUM)
        {
            return;
        }
        else
        {
            callbacklist.callbacknode[callbacklist.total_callbacks].callback = callback; 
            callbacklist.callbacknode[callbacklist.total_callbacks].qos = qos; 
            callbacklist.callbacknode[callbacklist.total_callbacks].topic = topic;
            callbacklist.callbacknode[callbacklist.total_callbacks].device_id= device_id;
            callbacklist.total_callbacks ++;
        }
    }
}

/*********************************************************************************
  *Function		:    void delsubcallback(char * topic, char *device_id)   
  *Description	:   
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void delsubcallback(char * topic, char *device_id)
{
    //删除一个
    for (int i = 0 ; i < callbacklist.total_callbacks; i++)
    {
        if ((topic == callbacklist.callbacknode[i].topic) && (device_id == callbacklist.callbacknode[i].device_id))
        {
            memcpy(&callbacklist.callbacknode[i], &callbacklist.callbacknode[i+1], (callbacklist.total_callbacks - 1 - i) * sizeof(struct _callbacknode));
            memset(&callbacklist.callbacknode[callbacklist.total_callbacks-1], 0, sizeof(struct _callbacknode));
            callbacklist.total_callbacks--;
            return;
        }
    }
}

/*********************************************************************************
  *Function		:    void ApiMqttClientCallBack(char *topic, uint8_t *payload, uint32_t length)  
  *Description	:    the mqtt client callbck function
  *Input		      :          
  *Output		:         
  *Return		:        
  *author		:        
  *date			:           
  *Others		:         
**********************************************************************************/
void ApiMqttClientCallBack(char *topic, uint8_t *payload, uint32_t length)
{
    CB CallBack=getsubcallback(topic);
    if(CallBack)
    {
        uint8_t *pData = (uint8_t *)malloc(length);
        memset(pData, 0, length);
        memcpy(pData, payload, length);
        CallBack(pData,length);
        free(pData);
    }
}

/*********************************************************************************
  *Function		:     system reset
  *Description	:     server control system reset
  *Input		      :     payload:  the point of payload   len
                               len: the len of payload
  *Output		:     none
  *Return		:     none
  *author		:     robin
  *date			:     2015-03-27      
  *Others		:         
**********************************************************************************/
void SystemDebugCallback(uint8_t *payload, uint32_t len)
{
    u32 n;
    
    for( n=0; n<len; n++)
    {
        IntoRobot_Debug_rx_buffer.buffer[IntoRobot_Debug_rx_buffer.head] = payload[n];   
        IntoRobot_Debug_rx_buffer.head++;
        /* To avoid buffer overflow */
        if(IntoRobot_Debug_rx_buffer.head == Cloud_DEBUG_BUFFER_SIZE)
        {
            IntoRobot_Debug_rx_buffer.head = 0;
        }
    }
}


#include "Wifi.h"

/*----------------------------------------------------------------------------------
                    FUNCTIONS DEFINITIONS FOR THE WIFI DRIVER
------------------------------------------------------------------------------------*/

/*DESCRIPTION :
    THIS FUNCTION AIMS TO INITIALIZE THE WHOLE WIFI CONNECTION 
    INCLUDING THE ESP MODULE AND THE MQTT CONNECTION
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE                                                                            */

void WIFI_Init(void)
{
#if ESP_USED == ESP32WROOM32
  ESP32WROOM32_InitWifi();
#endif  
}


/*DESCRIPTION :
    THIS FUNCTION AIMS TO CLOSE ALL THE CONNECTIONS OF THE MQTT CONNECTIONS
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE  */
void WIFI_CloseConnections(void)
{
#if ESP_USED == ESP32WROOM32
  ESP32WROOM32_MQTT_CloseConnections();
#endif   
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO SEND DATA TO A CERTAIN TOPIC 
  ARGUMENTS   :
    IT TAKES THE REQUIRED TOPIC AND THE DATA TO BE PUBLISHED 
    AND THE QUALITY OF SERVICE 
  RETURN VALUE :
    NONE                                                                            */

void WIFI_SendData(unsigned char* topic , unsigned char* message, unsigned char qos)
{
 #if ESP_USED == ESP32WROOM32
  ESP32WROOM32_MQTT_PublishData(topic , message, qos);
 #endif  
}

/*DESCRIPTION :
    HIS FUNCTION AIMS TO SUBSCRIBE TO CERTAIN TOPIC
  ARGUMENTS   :
    IT TAKES THE REQUIRED TOPIC TO BE SUBSCRIBED AND THE QUALITY OF SERVICE 
  RETURN VALUE :
    NONE                                                                            */

void WIFI_ReceiveData(unsigned char* topic , unsigned char qos)
{
  ESP32WROOM32_MQTT_SubscribeTopic(topic, qos);
}
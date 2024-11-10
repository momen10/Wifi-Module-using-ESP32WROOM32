

#ifndef WIFI_H

/*----------------------------------------------------------------------------------
                        THE USED ESP MODULE CONFIGURATIONS 
------------------------------------------------------------------------------------*/

#define ESP32WROOM32   0x01
#define ESP8266        0x02

//CONFIGURE THE REQUIRED ESP MODULE  
#define ESP_USED       ESP32WROOM32

/*----------------------------------------------------------------------------------
                  THE REQUIRED FILES FOR THE REQUIRED ESP MODULE 
------------------------------------------------------------------------------------*/

#if ESP_USED == ESP32WROOM32
#include "ESP32WROOM32_Interface.h"
#endif

/*----------------------------------------------------------------------------------
                 FUNCTIONS DECLARATIONS FOR THE WIFI DRIVER APIS
------------------------------------------------------------------------------------*/

void WIFI_Init(void);
void WIFI_CloseConnections(void);
void WIFI_SendData(unsigned char* topic , unsigned char* message, unsigned char qos);
void WIFI_ReceiveData(unsigned char* topic , unsigned char qos);

/*----------------------------------------------------------------------------------*/

#define WIFI_H
#endif
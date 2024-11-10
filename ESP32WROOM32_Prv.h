/*
 * NAME : ESP32WROOM32_Prv.h
 *
 * Created on: Sep 22, 2024
 *
 * Author: Mo'men Ahmed
 */

#ifndef HAL_ESP32WROOM32_Priv
#define HAL_ESP32WROOM32_Priv

/*------------------------------------------------------------------------------
      HERE ARE SOME AT COMMANDS NEEDED FOR THE WIFI AND MQTT CONNECTIONS.
--------------------------------------------------------------------------------*/

//BASIC AT COMMANDS
unsigned char* AT_Test_Cmd     = "AT\r\n";
unsigned char* AT_Reset_Cmd    = "AT+RST\r\n";
unsigned char* AT_Echo_Off_Cmd = "ATE0\r\n";
unsigned char* AT_Echo_On_Cmd  = "ATE1\r\n";

//WIFI RELATED AT COMMANDS
unsigned char* AT_Wifi_Disable_mode_Cmd          = "AT+CWMODE=0\r\n";
unsigned char* AT_Wifi_Station_mode_Cmd          = "AT+CWMODE=1\r\n";
unsigned char* AT_Wifi_SoftAP_mode_Cmd           = "AT+CWMODE=2\r\n";
unsigned char* AT_Wifi_StationAndSoftAP_mode_Cmd = "AT+CWMODE=3\r\n";

//AT COMMAND TO CONNECT TO WIFI : AT+CWJAP="username","password"
//YOY HAVE TO PUT ESCAPE CHARACTER '\' BEFORE THE FOLLOWING SPECIAL CHARACTERS ->  , " \ 
unsigned char* AT_New_Wifi_Connect_Cmd      = "AT+CWJAP=";
unsigned char* AT_Last_Wifi_Connect_Cmd     = "AT+CWJAP\r\n";
unsigned char* AT_Disconnect_AP_Cmd         = "AT+CWQAP\r\n";

//MQTT RELATED AT COMMANDS
unsigned char* AT_MQTT_User_Config_Cmd    = "AT+MQTTUSERCFG=0,";

unsigned char* AT_MQTT_Conn_Broker_Cmd    = "AT+MQTTCONN=0,";

unsigned char* AT_MQTT_Publish_Data_Cmd   = "AT+MQTTPUB=0,";

unsigned char* AT_MQTT_Subscribe_Data_Cmd = "AT+MQTTSUB=0,";

unsigned char* AT_MQTT_Clean_Cmd          = "AT+MQTTCLEAN=0\r\n";

/*------------------------------------------------------------------------------*/

#endif
/*
 * NAME : ESP32WROOM32_Interface.h
 *
 * Created on: Sep 22, 2024
 *
 * Author: Mo'men Ahmed
 */
 
#ifndef HAL_ESP32WROOM32_Interface
#define HAL_ESP32WROOM32_Interface

#define NUM_STATES          0x06
#define REC_BUFFER_LENGTH   500

/*----------------------------------------------------------------------------
    HERE ARE DIFFERENT ENUMS FOR THE ESP MODULE, WIFI AND MQTT CONNECTIONS       
------------------------------------------------------------------------------*/

typedef enum{
  QOS_0,
  QOS_1,
  QOS_2
}Qos_Type;

//HERE ARE THE CERTAIN REQUESTS THAT THE SERVER WILL ASK THE METER ABOUT
typedef enum{
  No_Request, 
  Asking_For_Voltage_Request,
  Asking_For_Current_Request
}Received_Request_Type;

//WIFI MODES FOR ESP MODULE
typedef enum {
	Disable_Wifi,
        Station_Mode, 
	SoftAP_Mode,
        StationAndSoftAP_Mode
}ESP32WROOM32_WIFI_Mode;

//ECHO MODES FOR ESP MODULE
typedef enum{
  DISABLE_ECHO ,
  ENABLE_ECHO
}ESP32WROOM32_Echo_Mode;

//ESP CONFIGURATIONS
typedef struct {
  ESP32WROOM32_Echo_Mode echo_mode;
  ESP32WROOM32_WIFI_Mode wifi_mode;
}ESP32WROOM32_Config;

//SCHEME MODE FOR MQTT CONNECTION
typedef enum{
  MQTT_TCP=1 ,
  MQTT_TLS_NO_CERT,
  MQTT_TLS_WITH_CERT
}MQTT_Scheme_Mode;

//RECONNECT MODE FOR MQTT CONNECTION
typedef enum{
  NOT_RECONNECT ,
  RECONNECT_AUTO
}MQTT_Reconnect_Mode;

typedef enum{
  ZERO_STATE,
  ESP_INIT,
  AP_CONNECTED,
  MQTT_USER_INIT,
  MQTT_BROKER_CONNECTED,
  IDLE_STATE
}WIFI_STATES;

typedef enum {
  ERROR_REC_OK, RECEIVED_OK
}Check_Rec_OK_State;

typedef enum {
  ERROR_REC_REQ_STRING, RECEIVED_REQ_STRING
}Check_Rec_String_State;

/*------------------------------------------------------------------------------
          HERE ARE SOME CONFIGURATION STRUCTURES FOR MQTT CONNECTION                      
--------------------------------------------------------------------------------*/
typedef struct {
  unsigned char*         Client_ID;
  MQTT_Scheme_Mode       Scheme_Mode;
}ESP32WROOM32_MQTT_Config;

typedef struct{
  unsigned char*             Broker_host_name;
  unsigned short             Broker_Port_Num;
  MQTT_Reconnect_Mode        Reconnect_Mode ;
}ESP32WROOM32_MQTT_Broker_Config;

/*------------------------------------------------------------------------------
          HERE ARE THE DECLARATIONS OF THE INRERFACE FUNCTIONS THAT
          WILL BE CALLED BY THE UPPER LAYER WHICH IS THE WIFI LAYER
--------------------------------------------------------------------------------*/

void                  ESP32WROOM32_Init                 (ESP32WROOM32_Config * ESP_Struct);

//MQTT RELATED APIS
void                  ESP32WROOM32_MQTT_Init            (ESP32WROOM32_MQTT_Config * MQTT_Struct);
void                  ESP32WROOM32_MQTT_ConnectBroker   (ESP32WROOM32_MQTT_Broker_Config* Broker_Struct);
void                  ESP32WROOM32_MQTT_PublishData     (unsigned char* topic , unsigned char* data, unsigned char qos);
void                  ESP32WROOM32_MQTT_SubscribeTopic  (unsigned char* topic , unsigned char qos);
     
void                  ESP32WROOM32_MQTT_CloseConnections(void);

//WIFI RELATED APIS
void                  ESP32WROOM32_TestATCmd            (void);
void                  ESP32WROOM32_ResetCmd             (void);
void                  ESP32WROOM32_SendATCmd            (unsigned char * command);
void                  ESP32WROOM32_ConnectNewAP         (unsigned char* username, unsigned char* password);
void                  ESP32WROOM32_ConnecLasttAP        (void);
void                  ESP32WROOM32_DisconnectAP         (void);
Check_Rec_OK_State    ESP32WROOM32_ValidateOK           (void);
Received_Request_Type ESP32WROOM32_ValidateString       (unsigned char * str);

void                  ESP32WROOM32_InitWifi             (void);

void                  Clear_Array                      (unsigned char volatile * arr, unsigned short  size);

/*------------------------------------------------------------------------------*/

#endif // !HAL_ESP32WROOM32_Config

 
 

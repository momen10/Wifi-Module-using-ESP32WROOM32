/*
 * NAME : ESP32WROOM32_Prog.c
 *
 * Created on: Sep 22, 2024
 *
 * Author: Mo'men Ahmed
 */

/*------------------------------------------------------------------------------
                 THE NEEDED FILES FOR THE ESP32WROOM32 APIS 
--------------------------------------------------------------------------------*/

#include "ESP32WROOM32_Prv.h"
#include "ESP32WROOM32_Interface.h"
#include "ESP32WROOM32_Config.h"
#include "lib_uart.h"
#include <string.h>

/*------------------------------------------------------------------------------
                        SOME NEEDED DEFINITIONS 
--------------------------------------------------------------------------------*/
            
#define WAITING_THRESHOLD_REC_OK     (500000UL)
#define FAILED_THRESHOLD              0x03

/*------------------------------------------------------------------------------
        SOME NEEDED VARIABLES FOR THE IMPLEMENTATION OF THE FUNCTIONS
--------------------------------------------------------------------------------*/
WIFI_STATES current_state = ZERO_STATE;

extern uint16_t Rec_buffer_index;
extern uint8_t  Rec_buffer_full ;
uint16_t Rec_buffer_indices [NUM_STATES] ;
uint8_t Check_Rec_OK ;
uint8_t Counter_Failed ;
volatile uint8_t Rec_String [REC_BUFFER_LENGTH] = {0};

extern uint8_t* AT_Wifi_Connect_Cmd;
extern uint8_t* AT_Reset_Cmd;
extern uint8_t* AT_MQTT_Conn_Broker_Cmd;
extern uint8_t* AT_MQTT_User_Config_Cmd;
extern uint8_t* AT_MQTT_Publish_Data_Cmd;
extern uint8_t* AT_MQTT_Subscribe_Data_Cmd;

extern uint8_t* REQ_TOPIC ;


/*------------------------------------------------------------------------------
                        FUNCTIONS DEFINITIONS 
--------------------------------------------------------------------------------*/


void Clear_Array (uint8_t volatile * arr, uint16_t size)
{
  for (uint16_t i = 0; i< size ; i++)
  {
    arr[i] = 0;
  }
}


/*DESCRIPTION :
    THIS FUNCTION AIMS TO INITIALIZE THE USED MICROCONTROLLER AND THE USED
    ESP MODULE, 
    INITIALIZE THE UART USED IN THE MICROCONTROLLER
    SENT THE AT COMMANDS RELATED TO THE ECHO MODE AND THE WIFI MODE FOR THE ESP  
  ARGUMENTS   :
    IT TAKES THE CONFIG STRUCT OF THE ESP MODULE THAT INCLUDES THE ECHO MODE 
    AND THE WIFI MODE                                                           */

void ESP32WROOM32_Init(ESP32WROOM32_Config * ESP_Struct)
{
   //GPIO CONFIGURATIONS FOR UART PINS
  GPIO_InitType GPIO_TX2_PIN ={
    .GPIO_Pin  = TX_PIN,
    .GPIO_Mode = GPIO_Mode_OUTPUT_CMOS
  };
  
    GPIO_InitType GPIO_RX2_PIN ={
    .GPIO_Pin  = RX_PIN,
    .GPIO_Mode = GPIO_Mode_INPUT
  };
  
  GPIOA_Init(RX_PORT, &GPIO_RX2_PIN);
  GPIOBToF_Init(TX_PORT, &GPIO_TX2_PIN);
  
  //UART CONFIGURATIONS
  UART_InitType My_UART = {
    .Mode     = UART_MODE_Msk,
    .Parity   = UART_PARITY_NONE,
    .WordLen  = UART_WORDLEN_8B,
    .FirstBit = UART_FIRSTBIT_LSB,
    .Baudrate = 115200
  }; 
  
  UART_Init(UART_USED, &My_UART);
  
  //ENABLING THE INTERRUPT OF RECEIVING IN UART    
  UART_INTConfig(UART_USED, UART_CTRL_RXIE_Msk , ENABLE);
  
  
  //ECHO CONFIGURATIONS
  switch (ESP_Struct -> echo_mode)
  {
  case (DISABLE_ECHO):
    ESP32WROOM32_SendATCmd(AT_Echo_Off_Cmd);
    break;
  case (ENABLE_ECHO):
    ESP32WROOM32_SendATCmd(AT_Echo_On_Cmd);
    break;    
  }
  
  //WIFI CONFIGURATIONS : STATION / SOFTAP / STATION+SOFTAP
  switch (ESP_Struct -> wifi_mode)
  {
    case(Station_Mode):
      ESP32WROOM32_SendATCmd(AT_Wifi_Station_mode_Cmd);
      break;
    case(SoftAP_Mode):
      ESP32WROOM32_SendATCmd(AT_Wifi_SoftAP_mode_Cmd);
      break;
    case(StationAndSoftAP_Mode):
      ESP32WROOM32_SendATCmd(AT_Wifi_StationAndSoftAP_mode_Cmd);
      break;
    case(Disable_Wifi):
      ESP32WROOM32_SendATCmd(AT_Wifi_Disable_mode_Cmd);
      break;      
  }
  
    if(Check_Rec_OK == RECEIVED_OK)
    {
      Rec_buffer_indices[ESP_INIT-1] = Rec_buffer_index;
      current_state = ESP_INIT;
      Counter_Failed = 0;
    }
    else
    {
      Counter_Failed++;
      if(Counter_Failed == FAILED_THRESHOLD)
      {
        current_state = ZERO_STATE;
        for (uint8_t i =0; i < NUM_STATES ; i++)
        {
          Rec_buffer_indices[i] = 0;
        }  
        ESP32WROOM32_ResetCmd();        
      }
    }
  
    if (Rec_buffer_full == 1)
    {
      Clear_Array(Rec_String , REC_BUFFER_LENGTH);
      Rec_buffer_full = 0;      
    }
}


void ESP32WROOM32_ResetCmd(void)
{
  ESP32WROOM32_SendATCmd(AT_Reset_Cmd);
}


/*DESCRIPTION  :
    THIS FUNCTION AIMS TO CONNECT TO NEW ACCESS POINT
    IT TAKES THE USERNAME AND PASSWORD OF THE REQUIRED ACCESS POINT 
    AND SEND THE AT COMMAND TO CONNECT TO THIS ACCESS POINT 
  ARGUMENTS    :
    IT TAKES THE USERNAME AND PASSWORD OF THE REQUIRED ACCESS POINT
  RETURN VALUE :
    NONE   
                                                                     */
void ESP32WROOM32_ConnectNewAP(unsigned char* username, unsigned char* password)
{
    uint8_t Connect_AP_Cmd_Str[100] = {0};  
  
    strcpy((char*)Connect_AP_Cmd_Str   , (const char*) AT_New_Wifi_Connect_Cmd);
    strcat((char*)Connect_AP_Cmd_Str   , (const char*) username);
    strcat((char*)Connect_AP_Cmd_Str   , (const char*) password);
    
    ESP32WROOM32_SendATCmd(Connect_AP_Cmd_Str);
    
    if(Check_Rec_OK == RECEIVED_OK)
    {
      Counter_Failed = 0;
      Rec_buffer_indices[AP_CONNECTED-1] = Rec_buffer_index ;
      if (Rec_buffer_indices[AP_CONNECTED-1] >  (Rec_buffer_indices[ESP_INIT-1] + 4))
      {
          current_state = AP_CONNECTED;
      }  
    }
    else
    {
      Counter_Failed++;
      if(Counter_Failed == FAILED_THRESHOLD)
      {
        current_state = ZERO_STATE;
        for (uint8_t i =0; i < NUM_STATES ; i++)
        {
          Rec_buffer_indices[i] = 0;
        }
        ESP32WROOM32_ResetCmd();                
      }
    }   
    
    if (Rec_buffer_full == 1)
    {
      Clear_Array(Rec_String , REC_BUFFER_LENGTH);
      Rec_buffer_full = 0;      
    }    
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO CONNECT TO THE LAST CONNECTED AP 
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE    
                                                                    */
void ESP32WROOM32_ConnecLasttAP(void)
{
  ESP32WROOM32_SendATCmd(AT_Last_Wifi_Connect_Cmd);
   
  if(Check_Rec_OK == RECEIVED_OK)
  {
     current_state = AP_CONNECTED;
  }
}


/*DESCRIPTION :
    THIS FUNCTION AIMS TO DISCONNECT FROM THE CONNECTED ACCESS POINT
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE  */

void ESP32WROOM32_DisconnectAP(void)
{
  ESP32WROOM32_SendATCmd(AT_Disconnect_AP_Cmd);
  
  if(Check_Rec_OK == RECEIVED_OK)
  {
     current_state = ESP_INIT;
  }   
}

/*DESCRIPTION :
    THIS FUNCTION AIMS SEND AT BASIC COMMAND
    TO TEST WHETHER IT RESPONDS BY OK OR NOT 
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE                                                                        */                            

void ESP32WROOM32_TestATCmd(void)
{
//   Check_Rec_OK = ERROR_REC_OK;  
   UART_SendString(UART_USED, AT_Test_Cmd);
  
//   uint32_t counter_waiting = 0;
 //  Check_Rec_OK = ESP32WROOM32_ValidateOK();
 // while (Check_Rec_OK != RECEIVED_OK)
 // {
 // // counter_waiting ++;
 // // if (counter_waiting == WAITING_THRESHOLD_REC_OK)
 // // {
 // //   counter_waiting = 0;
 // //   break;
 // // }
 //   Check_Rec_OK = ESP32WROOM32_ValidateOK();
 // }
}


/*DESCRIPTION :
    THIS FUNCTION AIMS TO SEND ANY AT COMMAND
  ARGUMENTS   :
    IT TAKES THE REQUIRED COMMAND AS A STRING AND SEND IT BY UART 
  RETURN VALUE :
    NONE                                                                        */

 uint32_t counter_waiting ;
void ESP32WROOM32_SendATCmd(uint8_t * command)
{
     Check_Rec_OK = ERROR_REC_OK;  
     
     counter_waiting = 0;
    
     UART_SendString(UART_USED, command);
        
     Check_Rec_OK = ESP32WROOM32_ValidateOK();
    while (Check_Rec_OK != RECEIVED_OK)
    {
        counter_waiting ++;
      if (counter_waiting == WAITING_THRESHOLD_REC_OK)
      {
        counter_waiting = 0;
        break;
      }
      Check_Rec_OK = ESP32WROOM32_ValidateOK();
    }
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO CHECK WHETHER THE MICROCONTROLLER
    HAS RECEIVED "OK" FROM THE ESP MODULE SUCCESSFULLY OR NOT  
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    IT RETURNS AN ENUM INDICATIONG WHETHER
    THE ESP HAS RESPONDES WITH "OK" OR NOT                                      */

Check_Rec_OK_State ESP32WROOM32_ValidateOK (void)
{
  Check_Rec_OK_State Received_OK = ERROR_REC_OK ;
  
  //SPECIAL CASE IF THE INDEX AT THE BEGENNING OF THE BUFFER
  if (Rec_buffer_index == 0)
  {
    //CHECK ON THE LAST 4 CHARACTERS OF THE BUFFER
      if (Rec_String[REC_BUFFER_LENGTH - 4] == 'O' && Rec_String[REC_BUFFER_LENGTH - 3] == 'K'
      && Rec_String[REC_BUFFER_LENGTH - 2] == 0x0D && Rec_String[REC_BUFFER_LENGTH - 1] == 0X0A)
      {
          Received_OK = RECEIVED_OK;
      }    
  }
  else
  {
     //GENERAL CASE IF THE INDEX IS IN THE MIDDLE OF THE BUFFER
      if (Rec_String[Rec_buffer_index - 4] == 'O' && Rec_String[Rec_buffer_index - 3] == 'K'
      && Rec_String[Rec_buffer_index - 2] == 0x0D && Rec_String[Rec_buffer_index - 1] == 0X0A)
      {
          Received_OK = RECEIVED_OK;
      }
  }  
 
  return Received_OK;
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO CHECK WHETHER THE MICROCONTROLLER HAS RECEIVED
    ANY REQUIRED STRING FROM THE ESP MODULE SUCCESSFULLY OR NOT 
  ARGUMENTS   :
    IT TAKES THE REQUIRED STRING THAT WILL BE SEARCHED FOR IN THE 
    RECEIVING BUFFER
  RETURN VALUE :
    IT RETURNS AN ENUM INDICATIONG WHETHER
    THE ESP HAS RESPONDES WITH SPECEFIC STRING OR NOT                           */

Received_Request_Type ESP32WROOM32_ValidateString (unsigned char * str)
{
  uint8_t string_length = 0 ;
  uint8_t i = 0 ;
  Received_Request_Type  Rec_string_state;
  if(*str == 'V')
  {
      Rec_string_state = Asking_For_Voltage_Request;
  }
  else if (*str == 'C')
  {
      Rec_string_state = Asking_For_Current_Request;
  }
  
  
//GETTING THE LENGTH OF THE REQUIRED STRING
  while(str[i] != '\0')
  {
    string_length ++;
    i++;
  }

  
//GENERAL CASE WHEN THE STRING HAS BEEN RECEIVED AT THE MIDDLE OF THE BUFFER
    if ( Rec_buffer_index > string_length)
    {
      for (uint8_t j = 0; j < string_length; j++)
      {
          uint8_t ch = Rec_String[Rec_buffer_index - string_length - 2];
          if (Rec_String[Rec_buffer_index - string_length - 2 + j] != str[j])
          {
              Rec_string_state = No_Request;
              break;
          }
      }      
    }
    else 
    {
      /*SPECIAL CASE WHEN THE STRING HAS BEEN RECEIVED :
        PART AT THE END AND PART AT THE BEGINNING*/

         uint8_t i = 0;
         while ( i < string_length - Rec_buffer_index)
         {      
          if (Rec_String[Rec_buffer_index + REC_BUFFER_LENGTH - string_length + i] != str[i])
          {
              Rec_string_state = No_Request;
              break;
          }     
          i++;
         }
         
         while(i < string_length)
         {
           if (Rec_String[ i - Rec_buffer_index] != str[i])
          {
              Rec_string_state = No_Request;
              break;
          }     
          i++;          
         }       
    }
    return Rec_string_state;
}


/*DESCRIPTION :
    THIS FUNCTION AIMS TO INITIALIZE THE MQTT CONNECTION
  ARGUMENTS   :
    IT TAKES THE CONFIG STRUCT OF THE MQTT CONNECTIONS
    INCLUDING THE CLIENT ID AND THE SCHEME MODE
  RETURN VALUE :
    NONE                                                                        */
 void ESP32WROOM32_MQTT_Init(ESP32WROOM32_MQTT_Config * MQTT_Struct)
{
 // uint8_t scheme_ascii = (MQTT_Struct -> Scheme_Mode) + '0' ;
 // uint8_t scheme_str[4] = {0};
 // scheme_str[0] = scheme_ascii;
 // const uint8_t comma_str[3] = {0};
 // strcat((char*)comma_str  , (char const*)",\"");
 // strcat((char*)scheme_str , (char const*)comma_str);
 // 
 // //SEND THE COMMAND : "AT+MQTTUSERCFRG=0,"
 // uint8_t MQTT_user_cmd_str[100] = {0};
 // strcpy((char*)MQTT_user_cmd_str , (const char*) AT_MQTT_User_Config_Cmd);
 // 
 // strcat((char*)MQTT_user_cmd_str , (char const*)scheme_str);
 //
 // strcat((char*)MQTT_user_cmd_str , (char const*)(MQTT_Struct -> Client_ID));
 //
 // strcat((char*)MQTT_user_cmd_str , (char const*)"\",\"\",\"\",0,0,\"\"\r\n");

  ESP32WROOM32_SendATCmd("AT+MQTTUSERCFG=0,1,\"MO2\",\"\",\"\",0,0,\"\"\r\n");

//  ESP32WROOM32_SendATCmd(MQTT_user_cmd_str);
  
  if(Check_Rec_OK == RECEIVED_OK)
  {
    Counter_Failed = 0;
    Rec_buffer_indices[MQTT_USER_INIT-1] = Rec_buffer_index;
    if(Rec_buffer_indices[MQTT_USER_INIT-1] > (Rec_buffer_indices[AP_CONNECTED-1] + 4))
    {
      current_state = MQTT_USER_INIT;      
    }
  }
  else
  {
    Counter_Failed++;
    if(Counter_Failed == FAILED_THRESHOLD)
    {
      current_state = ZERO_STATE;
      for (uint8_t i =0; i < NUM_STATES ; i++)
      {
        Rec_buffer_indices[i] = 0;
      } 
        ESP32WROOM32_ResetCmd();              
    }
  }
  
  if (Rec_buffer_full == 1)
  {
    Clear_Array(Rec_String , REC_BUFFER_LENGTH);
    Rec_buffer_full = 0;    
  }  
}


/*DESCRIPTION :
    THIS FUNCTION AIMS TO CONNECT TO THE REQUIRED BROKER IN THE MQTT CONNECTION
  ARGUMENTS   :
    IT TAKES THE BROKER CONFIG STRUCT THAT INCLUDES ITS HOST NAME, PORT NUMBER 
    AND RECONNECT MODE
  RETURN VALUE :
    NONE                                                                        */

void ESP32WROOM32_MQTT_ConnectBroker(ESP32WROOM32_MQTT_Broker_Config* Broker_Struct)
{
//    ESP32WROOM32_SendATCmd("AT+MQTTCLEAN=0\r\n");
  
  //SEND THE LISTENING PORT NUMBER OF BROKER
  uint8_t  i = 0;
  uint8_t  digits[4];
  uint8_t  digits_ascii[5]={0};
  uint16_t port = (Broker_Struct -> Broker_Port_Num);
  while( port != 0)
  {
    digits[i] = port % 10;
    
    digits_ascii[3-i] = digits[i] + '0';
    
    port /= 10;
    
    i++;
  }
  
  uint8_t reconnect_ascii = (Broker_Struct -> Reconnect_Mode) + '0' ;
  uint8_t reconnect_str[4] ={0};
  reconnect_str[0] = ',';
  reconnect_str[1] = reconnect_ascii;

  //SEND THE MQTT CONNECT COMMAND : "AT+MQTTCONN=0,"
  uint8_t MQTT_conn_broker_cmd_str[100] = {0};
  strcpy((char*)MQTT_conn_broker_cmd_str   , (const char*) AT_MQTT_Conn_Broker_Cmd);
  strcat((char*)MQTT_conn_broker_cmd_str   , (char const*)"\"");
  strcat((char*)MQTT_conn_broker_cmd_str   , (char const*)(Broker_Struct -> Broker_host_name));
  strcat((char*)MQTT_conn_broker_cmd_str   , (char const*)"\",");
  strcat((char*)MQTT_conn_broker_cmd_str   , (char const*)digits_ascii);
  strcat((char*)MQTT_conn_broker_cmd_str   , (char const*)reconnect_str);
  strcat((char*)MQTT_conn_broker_cmd_str   , (char const*)"\r\n");

  ESP32WROOM32_SendATCmd(MQTT_conn_broker_cmd_str);
  
    if(Check_Rec_OK == RECEIVED_OK)
    {
       Counter_Failed = 0;
       Rec_buffer_indices[MQTT_BROKER_CONNECTED-1] = Rec_buffer_index;
        if(Rec_buffer_indices[MQTT_BROKER_CONNECTED-1] > (Rec_buffer_indices[MQTT_USER_INIT-1] + 4))
        {
          current_state = MQTT_BROKER_CONNECTED;      
        }
    }
    else
    {
      Counter_Failed++;
      if(Counter_Failed == FAILED_THRESHOLD)
      {
        current_state = ZERO_STATE;
        for (uint8_t i =0; i < NUM_STATES ; i++)
        {
          Rec_buffer_indices[i] = 0;
        }
        ESP32WROOM32_ResetCmd();                
      }
    }  

    if (Rec_buffer_full == 1)
    {
      Clear_Array(Rec_String , REC_BUFFER_LENGTH);
      Rec_buffer_full = 0;      
    }    
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO PUBLISH DATA TO A CERTAIN TOPIC
  ARGUMENTS   :
    IT TAKES THE REQUIRED TOPIC AND THE DATA TO BE PUBLISHED 
    AND THE QUALITY OF SERVICE 
  RETURN VALUE :
    NONE                                                                       */

void ESP32WROOM32_MQTT_PublishData(unsigned char* topic , unsigned char* data, unsigned char qos)
{
  uint8_t qos_ascii = (qos + '0');
  uint8_t retain_str[5] = {0};
  retain_str[0] = qos_ascii;
  strcat((char*)retain_str, (const char*)",0\r\n");
  
  //SEND THE MQTT PUBLISH COMMAND : "AT+MQTTPUB=0,"
  uint8_t MQTT_Pub_Cmd_str [100]= {0};
  strcpy((char*)MQTT_Pub_Cmd_str  , (const char*) AT_MQTT_Publish_Data_Cmd);
  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)"\"");
  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)topic);
  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)"\",\"");
  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)data);
  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)"\",");
//  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)"0,0\r\n");
  
  strcat((char*)MQTT_Pub_Cmd_str  , (char const*)retain_str);

  ESP32WROOM32_SendATCmd(MQTT_Pub_Cmd_str);
}


/*DESCRIPTION :
    HIS FUNCTION AIMS TO SUBSCRIBE TO CERTAIN TOPIC
  ARGUMENTS   :
    IT TAKES THE REQUIRED TOPIC TO BE SUBSCRIBED AND THE QUALITY OF SERVICE 
  RETURN VALUE :
    NONE                                                                        */

void ESP32WROOM32_MQTT_SubscribeTopic(unsigned char* topic , unsigned char qos)
{
  uint8_t qos_str[4] = {0};
  uint8_t qos_ascii = (qos + '0');
  
  qos_str[0] = qos_ascii;
  qos_str[1] = '\r';
  qos_str[2] = '\n';
  //SEND THE MQTT SUBSCRIBE COMMAND : "AT+MQTTSUB=0,"
  uint8_t MQTT_Sub_Cmd_Str[100] = {0};
  strcpy((char*)MQTT_Sub_Cmd_Str   , (const char*) AT_MQTT_Subscribe_Data_Cmd);
  strcat((char*)MQTT_Sub_Cmd_Str   , (char const*)"\"");
  strcat((char*)MQTT_Sub_Cmd_Str   , (char const*)topic);
  strcat((char*)MQTT_Sub_Cmd_Str   , (char const*)"\",");
  strcat((char*)MQTT_Sub_Cmd_Str   , (char const*)qos_str);
  
  ESP32WROOM32_SendATCmd(MQTT_Sub_Cmd_Str);
  
  if(Check_Rec_OK == RECEIVED_OK)
  {
    Counter_Failed = 0;    
    Rec_buffer_indices[IDLE_STATE-1] = Rec_buffer_index;
    if(Rec_buffer_indices[IDLE_STATE-1] > (Rec_buffer_indices[MQTT_BROKER_CONNECTED-1] + 4 ))
    {
      current_state = IDLE_STATE;      
    }
  } 
    else
    {
      Counter_Failed++;
      if(Counter_Failed == FAILED_THRESHOLD)
      {
        current_state = ZERO_STATE;
        for (uint8_t i =0; i < NUM_STATES ; i++)
        {
          Rec_buffer_indices[i] = 0;
        }  
        ESP32WROOM32_ResetCmd();                
      }
    }

    if (Rec_buffer_full == 1)
    {
      Clear_Array(Rec_String , REC_BUFFER_LENGTH);
      Rec_buffer_full = 0;
    }  
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO CLOSE THE MQTT CONNECTION WITH THE BROKER
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE                                                                        */
void ESP32WROOM32_MQTT_CloseConnections(void)
{
  ESP32WROOM32_SendATCmd(AT_MQTT_Clean_Cmd);
}

/*DESCRIPTION :
    THIS FUNCTION AIMS TO INITIALIZE THE WIFI
    IT CONTAINS THE COMPLETE STATE MACHINE FOR THE MQTT CONNECTION
       FIRST  : ZERO STATE AND YOU HAVE TO INITIALIZE THE ESP MODULE
       SECOND : ESP INIT STATE AND YOU HAVE TO CONNECT TO AN ACCESS POINT
       THIRD  : AP CONNECTED MODE AND YOU HAVE TO 
                INITIALIZE THE MQTT CONNECTION
       FOURTH : MQTT USER INIT STATE AND YOU HAVE 
                TO CONNECT TO THE REQUIRED BROKER
       FIFTH  : MQTT BROKER CONNECTED AND YOU HAVE TO
                SUBSCRIBE FOR A CERTAIN TOPIC
       SIXTH  : IDLE STATE WHICH THE SYSTEM IS READY
                TO RECEIVE THE MESSAGES ABOUT THIS TOPIC
  ARGUMENTS   :
    NONE 
  RETURN VALUE :
    NONE   */
void ESP32WROOM32_InitWifi(void)
{  
  //ESP CONFIGURATIONS
  ESP32WROOM32_Config ESP_Used = 
  { 
    .echo_mode = ENABLE_ECHO ,
    .wifi_mode = Station_Mode 
  };
  
  //MQTT USER CONFIGURATIONS
   ESP32WROOM32_MQTT_Config MQTT_User = {
   .Client_ID     = "Mo2",
   .Scheme_Mode   = MQTT_TCP, 
 };
 
 //MQTT BROKER CONFIGURATIONS
  ESP32WROOM32_MQTT_Broker_Config MQTT_Broker = {
    .Broker_host_name    = "test.mosquitto.org",
    .Broker_Port_Num     = 1883,
    .Reconnect_Mode      = RECONNECT_AUTO
  }; 
  
  
  while(current_state != IDLE_STATE)
  {
    switch (current_state)
    {
      case ZERO_STATE:
            ESP32WROOM32_Init(&ESP_Used);   
        break;
      case ESP_INIT:
        //    ESP32WROOM32_ConnecLasttAP();
            ESP32WROOM32_ConnectNewAP(REQ_AP_Username, REQ_AP_Password);
        break;
      case AP_CONNECTED:
            ESP32WROOM32_MQTT_Init(&MQTT_User);
        break;
      case MQTT_USER_INIT:
            ESP32WROOM32_MQTT_ConnectBroker(&MQTT_Broker);
        break;
      case MQTT_BROKER_CONNECTED:
            ESP32WROOM32_MQTT_SubscribeTopic(REQ_TOPIC_SERIAL_NUM, QOS_1);
        break;
    }
  }

}









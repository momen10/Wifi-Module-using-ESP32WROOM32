/*
 * NAME : ESP32WROOM32_Config.h
 *
 * Created on: Sep 22, 2024
 *
 * Author: Mo'men Ahmed
 */

#ifndef HAL_ESP32WROOM32_Config
#define HAL_ESP32WROOM32_Config

#include "target.h"
#include "lib_uart.h"
#include "lib_gpio.h"

/*----------------------------------------------------------------------------------
        HERE ARE DIFFERENT CONFIGURATIONS BASED ON THE USED MICROCONTROLLER  
------------------------------------------------------------------------------------*/
//CONFIGURE UART (IN MC) YOU WILL USE FOR COMM (UARTx)
#define UART_USED  UART2

/*GPIO CONFIGURATIONS
FOR PINS : (GPIO_Pin_x) 
FOR PORTS: (GPIOx)*/
#define TX_PORT  GPIOB
#define TX_PIN   GPIO_Pin_4
#define RX_PORT  GPIOA
#define RX_PIN   GPIO_Pin_14
/*----------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
    HERE ARE SOME STRINGS THAT CAN BE CONFIGURED AND USED IN THE MQTT CONNECTIONS
------------------------------------------------------------------------------------*/

//REQUIRED ACCESS POINT USERNAME AND PASSWORD
unsigned char* REQ_AP_Username              = "\"Galaxy A326F77\",";
unsigned char* REQ_AP_Password              = "\"momen2002\"\r\n";

//REQUIRED TOPIC TO BE SUBSCRIBED
unsigned char* REQ_TOPIC_SERIAL_NUM         = "1234";
unsigned char* REQ_TOPIC_PUBLISH_VOLT       = "esp32/1234/volt";
unsigned char* REQ_TOPIC_PUBLISH_CURRENT    = "esp32/1234/current";

//HERE ARE THE CERTAIN PARAMTERS THAT THE SERVER WILL ASK THE METER ABOUT
unsigned char* ASKING_FOR_VOLTAGE           = "Voltage" ;
unsigned char* ASKING_FOR_CURRENT           = "Current" ;

/*----------------------------------------------------------------------------------*/

#endif // !HAL_ESP32WROOM32_Config



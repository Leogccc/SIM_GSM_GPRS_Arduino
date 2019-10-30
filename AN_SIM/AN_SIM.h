
#ifndef __AN_SIM_H__
#define __AN_SIM_H__

#include "GPRS_Shield_Arduino.h"

 #define DEBUG_AN_SIM_L
//#define DEBUG_AN_SIM_L(x) Serial.println(x)  	


#define RETRY_COUNT 10
#define PARAM_COUNT 46
#define RESP_BUFFER_SIZE 550

//const char paramKey1[] PROGMEM =  {"idData"}; 
const char paramKey2[] PROGMEM =  {"idSis"};
const char paramKey3[] PROGMEM =  {"mo"};
const char paramKey4[] PROGMEM =  {"dd"}; 
const char paramKey5[] PROGMEM =  {"ma"};
const char paramKey6[] PROGMEM =  {"aa"};
const char paramKey7[] PROGMEM =  {"hh"};
const char paramKey8[] PROGMEM =  {"mm"}; 
const char paramKey9[] PROGMEM =  {"od"};
const char paramKey10[] PROGMEM = {"tc"}; 
const char paramKey11[] PROGMEM = {"ca"};
const char paramKey12[] PROGMEM = {"t0"};
const char paramKey13[] PROGMEM = {"t0"};
const char paramKey14[] PROGMEM = {"t1"}; 
const char paramKey15[] PROGMEM = {"t2"};
const char paramKey16[] PROGMEM = {"t3"}; 
const char paramKey17[] PROGMEM = {"t4"};
const char paramKey18[] PROGMEM = {"t5"}; 
const char paramKey19[] PROGMEM = {"t6"};
const char paramKey20[] PROGMEM = {"t7"}; 
const char paramKey21[] PROGMEM = {"t8"};
const char paramKey22[] PROGMEM = {"v00"}; 
const char paramKey23[] PROGMEM = {"v01"};
const char paramKey24[] PROGMEM = {"v02"}; 
const char paramKey25[] PROGMEM = {"v03"};
const char paramKey26[] PROGMEM = {"v04"}; 
const char paramKey27[] PROGMEM = {"v05"};
const char paramKey28[] PROGMEM = {"v06"}; 
const char paramKey29[] PROGMEM = {"v07"};
const char paramKey30[] PROGMEM = {"v08"}; 
const char paramKey31[] PROGMEM = {"v10"};
const char paramKey32[] PROGMEM = {"v12"}; 
const char paramKey33[] PROGMEM = {"v13"};
const char paramKey34[] PROGMEM = {"v14"}; 
const char paramKey35[] PROGMEM = {"v15"};
const char paramKey36[] PROGMEM = {"v16"}; 
const char paramKey37[] PROGMEM = {"v17"};
const char paramKey38[] PROGMEM = {"v18"}; 
const char paramKey39[] PROGMEM = {"e0"};
const char paramKey40[] PROGMEM = {"e1"}; 
const char paramKey41[] PROGMEM = {"e2"};
const char paramKey42[] PROGMEM = {"e3"}; 
const char paramKey43[] PROGMEM = {"e4"};
const char paramKey44[] PROGMEM = {"e5"}; 
const char paramKey45[] PROGMEM = {"e6"};
const char paramKey46[] PROGMEM = {"e07"};
const char paramKey47[] PROGMEM = {"e08"};                   


const __FlashStringHelper* const queryParamKeys[PARAM_COUNT] PROGMEM =
{
  /*(const __FlashStringHelper*)paramKey1,*/
    (const __FlashStringHelper*)paramKey2,
    (const __FlashStringHelper*)paramKey3,
    (const __FlashStringHelper*)paramKey4,
    (const __FlashStringHelper*)paramKey5,
    (const __FlashStringHelper*)paramKey6,
    (const __FlashStringHelper*)paramKey7,
    (const __FlashStringHelper*)paramKey8,
    (const __FlashStringHelper*)paramKey9,
    (const __FlashStringHelper*)paramKey10,
    (const __FlashStringHelper*)paramKey11,
    (const __FlashStringHelper*)paramKey12,
    (const __FlashStringHelper*)paramKey13,
    (const __FlashStringHelper*)paramKey14,
    (const __FlashStringHelper*)paramKey15,
    (const __FlashStringHelper*)paramKey16,
    (const __FlashStringHelper*)paramKey17,
    (const __FlashStringHelper*)paramKey18,
    (const __FlashStringHelper*)paramKey19,
    (const __FlashStringHelper*)paramKey20,
    (const __FlashStringHelper*)paramKey21,
    (const __FlashStringHelper*)paramKey22,
    (const __FlashStringHelper*)paramKey23,
    (const __FlashStringHelper*)paramKey24,
    (const __FlashStringHelper*)paramKey25,
    (const __FlashStringHelper*)paramKey26,
    (const __FlashStringHelper*)paramKey27,
    (const __FlashStringHelper*)paramKey28,
    (const __FlashStringHelper*)paramKey29,
    (const __FlashStringHelper*)paramKey30,
    (const __FlashStringHelper*)paramKey31,
    (const __FlashStringHelper*)paramKey32,
    (const __FlashStringHelper*)paramKey33,
    (const __FlashStringHelper*)paramKey34,
    (const __FlashStringHelper*)paramKey35,
    (const __FlashStringHelper*)paramKey36,
    (const __FlashStringHelper*)paramKey37,
    (const __FlashStringHelper*)paramKey38,
    (const __FlashStringHelper*)paramKey39,
    (const __FlashStringHelper*)paramKey40,
    (const __FlashStringHelper*)paramKey41,
    (const __FlashStringHelper*)paramKey42,
    (const __FlashStringHelper*)paramKey43,
    (const __FlashStringHelper*)paramKey44,
    (const __FlashStringHelper*)paramKey45,
    (const __FlashStringHelper*)paramKey46,
    (const __FlashStringHelper*)paramKey47  
};


enum rtc_simbolos {yy=0, MM, dd, hh, mm, ss, zz}; // rtc ="yy/MM/dd,hh:mm:ss±zz"

class AN_SIM
{
private:
        GPRS gprs;

public:
    
    AN_SIM(uint8_t Rx=8, uint8_t Tx=7,  uint32_t baud_rate=19200 );
    ~AN_SIM();


void begin(void) ; // Configura o SIM900 para acessar a internet via GSM/GPRS usando HTTP

bool http_client_get(
    const __FlashStringHelper * url,
    const __FlashStringHelper *path ,
    const __FlashStringHelper* const queryParamKeys[],
    char * paramValue_array[] ,
    char * responseBuffer );      // faz uma requisicao a um servidor(URL com envio de dados) e retorna uma String de resposta do servidor 


bool getDateTimeRTC (char * buffer) { return gprs.getDateTime(buffer);}

bool getSignalStrength(int *buffer) { return gprs.getSignalStrength(buffer) ;}

friend bool parsing_RTC(char rtc[], char* rtc_parsed[]); // recebe a string rtc(retorno de GPRS::getDateTime() ), e faz o parseamento armazenando a saida em rtc_parsed
} ;


#endif  


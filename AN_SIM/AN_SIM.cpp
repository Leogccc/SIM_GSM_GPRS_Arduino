
#include "AN_SIM.h"


AN_SIM::AN_SIM( uint8_t Rx, uint8_t Tx, uint32_t baud_rate):gprs(Tx, Rx, baud_rate)
{
 
}

AN_SIM::~AN_SIM()
{
} 


void AN_SIM::begin(void) {

//apn, login, senha, pin
static const char apn[]   PROGMEM = {"smart.m2m.vivo.com.br"}; // "smart.m2m.vivo.com.br" ; meta.vivo.com.br
static const char user[] PROGMEM = "vivo" ; //meta
static const char pwd[] PROGMEM ="vivo" ;
//static const char pin[]  PROGMEM  ="8486" ;

int i;

  DEBUG_AN_SIM_L(F("Start HTTP !"));
  DEBUG_AN_SIM_L(F("if something is failing you may want to debug, check sim900.h\r\n"));

  for (i = 0; i < RETRY_COUNT; i++)
  {
      DEBUG_AN_SIM_L(F("Initializing ..."));
      if (gprs.init() != false)
          break;

      delay(500);
  }

  if (i < RETRY_COUNT)
      DEBUG_AN_SIM_L(F("OK"));
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return;
  }

  for (i = 0; i < RETRY_COUNT; i++)
  {
      DEBUG_AN_SIM_L(F("Check if network is registered..."));
      if (gprs.isNetworkRegistered() != false)
          break;

      delay(2000);
  }

  if (i < RETRY_COUNT)
      DEBUG_AN_SIM_L(F("OK"));
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return;
  }

for (i = 0; i < RETRY_COUNT; i++) {

  DEBUG_AN_SIM_L(F("opening bearer ..."));
  if (gprs.openBearer((const __FlashStringHelper *)apn,(const __FlashStringHelper *)user,(const __FlashStringHelper *)pwd) != false)
        break;
  else
  {
      gprs.closeBearer() ;
  }

}

  if (i < RETRY_COUNT) {
      DEBUG_AN_SIM_L(F("OK, ip address is: "));
      DEBUG_AN_SIM_L(gprs.getIPAddress()); }
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
  }


}



bool AN_SIM::http_client_get( const __FlashStringHelper * url, 
 const __FlashStringHelper *path , 
 const __FlashStringHelper* const queryParamKeys[],
 char * paramValue_array[] ,
  char* responseBuffer )

 {

  int i ;

  DEBUG_AN_SIM_L(F("initializing HTTP service ..."));

   for (i = 0; i < RETRY_COUNT; i++)
  {
      if (gprs.httpInitialize() != false) break;
      else gprs.httpTerminate() ;
  }

 if (i < RETRY_COUNT)
      DEBUG_AN_SIM_L(F("OK"));
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return false;
  }



  for (i = 0; i < RETRY_COUNT; i++)
  {
      DEBUG_AN_SIM_L(F("sending runtime constructed HTTP GET request"));
      if (gprs.httpSendGetRequest(url, path,PARAM_COUNT,queryParamKeys,paramValue_array,HTTP_DEFAULT_PORT)!=-1) break;
  }

  if (i < RETRY_COUNT)
      DEBUG_AN_SIM_L(F("OK"));
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return false;
  }



  DEBUG_AN_SIM_L(F("fetching HTTP GET response ..."));
  if (gprs.httpReadResponseData(responseBuffer, RESP_BUFFER_SIZE ) != false)
  {
      DEBUG_AN_SIM_L(F("OK, received data:"));
      DEBUG_AN_SIM_L(responseBuffer);
  }
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return false;
  }

  DEBUG_AN_SIM_L(F("terminating HTTP service ..."));
  if (gprs.httpTerminate() != false)
     DEBUG_AN_SIM_L(F("OK"));
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return false;
  }

 /*  DEBUG_AN_SIM_L(F("closing bearer ..."));
  if (gprs.closeBearer() != false)
      DEBUG_AN_SIM_L(F("OK"));
  else
  {
      DEBUG_AN_SIM_L(F("failed"));
      return;
  } */

return true; // tudo ocorreu corretamente, a solicitacao foi um sucesso (envio de dados para o server) e tbm a resposta do servidor foi recebida com sucesso

}



bool parsing_RTC( char rtc[], char* rtc_parsed[]) {
//Format rtc is "yy/MM/dd,hh:mm:ss±zz", where characters indicate year (two last digits),month, day, hour, minutes, seconds and time zone
//time zone indicates the difference, expressed in quarters of an hour, between the local time and GMT; range -47...+48).

// Ex rtc:"14/11/13,21:14:41+04" => para não criar outro vetor pro parsing rtc vai ser transformado em um vetor de várias substrings concatenadas:"14\011\013\021\014\041+04"
// e rtc_parsed vai apontar para cada subtring 
char * pch;
rtc_parsed[yy]= rtc ; // primeira substring yy

pch=strchr(rtc_parsed[yy],'/');
if(pch!=NULL)    { (*pch)='\0'; rtc_parsed[MM]= pch+1; } 
else return false;
pch=strchr(rtc_parsed[MM],'/');
if(pch!=NULL)   { (*pch)='\0'; rtc_parsed[dd]= pch+1; }
else return false;
pch=strchr(rtc_parsed[dd],',');
if(pch!=NULL)  { (*pch)='\0'; rtc_parsed[hh]= pch+1; }
else return false;
pch=strchr(rtc_parsed[hh],':');
if(pch!=NULL)   { (*pch)='\0'; rtc_parsed[mm]= pch+1; }
else return false;
pch=strchr(rtc_parsed[mm],':'); 
if(pch!=NULL)  { (*pch)='\0'; rtc_parsed[ss]= pch+1; }
else return false;
pch=strpbrk(rtc_parsed[ss],"+-"); 
if(pch!=NULL)  {(*pch)='\0';  rtc_parsed[zz]= pch+1; }
else return false;

return true;

// "yy/MM/dd,hh:mm:ss±zz"=>"rtc_parsed[0]/rtc_parsed[1]/rtc_parsed[2],rtc_parsed[3]:rtc_parsed[4]:rtc_parsed[5]rtc_parsed[6]"
// Obs: /, e : não existem mais, foram substituidos por "\0"
}



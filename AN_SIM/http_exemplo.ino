
#include "AN_SIM.h"

char responseBuffer[RESP_BUFFER_SIZE]; // vetor que armazena a resposta da requisicao http
char rtc[30]={0};
char* rtc_parsed[7];
int forca_sinal = 99; //mapeamento da intensidade do sinal recebido  em dbm         // inicia com valor não detectavel ou não conhecido =99

 const char url[] PROGMEM = {"http://www.acquanativa.com.br"};
 const char path[] PROGMEM = {"/analytics/includes/pisdata.inc.php"};

/*char param1_valor[3]={0}; */
char param2_valor[14]={0};
char param3_valor[2]={0};
char param4_valor[3]={0};
char param5_valor[3]={0};
char param6_valor[5]={0};
char param7_valor[3]={0};
char param8_valor[3]={0};
char param9_valor[10]={0};
char param10_valor[10]={0};
char param11_valor[10]={0};
char param12_valor[2]={0};
char param13_valor[2]={0};
char param14_valor[2]={0};
char param15_valor[2]={0};
char param16_valor[2]={0};
char param17_valor[2]={0};
char param18_valor[2]={0};
char param19_valor[2]={0};
char param20_valor[2]={0};
char param21_valor[2]={0};
char param22_valor[8]={0};
char param23_valor[8]={0};
char param24_valor[8]={0};
char param25_valor[8]={0};
char param26_valor[8]={0};
char param27_valor[8]={0};
char param28_valor[8]={0};
char param29_valor[8]={0};
char param30_valor[8]={0};
char param31_valor[8]={0};
char param32_valor[8]={0};
char param33_valor[8]={0};
char param34_valor[8]={0};
char param35_valor[8]={0};
char param36_valor[8]={0};
char param37_valor[8]={0};
char param38_valor[8]={0};
char param39_valor[2]={0};
char param40_valor[2]={0};
char param41_valor[2]={0};
char param42_valor[2]={0};
char param43_valor[2]={0};
char param44_valor[2]={0};
char param45_valor[2]={0};
char param46_valor[2]={0};
char param47_valor[2]={0};


AN_SIM telemetria(8,7,19200); //rx, tx, baud_rate (SOFTWARE SERIAL)// Arduino communicates with SIM900 GSM shield at a baud rate of 19200 // Make sure that corresponds to the baud rate of your module


float od = 0 ;
int i=0;


void setup() {
  Serial.begin(19200) ;
  // Give time to your GSM shield log on to network
  delay(10000);
  telemetria.begin() ;
}



void loop() { 
  
String od_str= String(od /*+ 5*cos(3.14*(++i)) */);
od_str.toCharArray(param9_valor,10);


strcpy(param2_valor ,"7811926092019");       
strcpy(param3_valor ,"1");
strcpy(param4_valor ,"23");
strcpy(param5_valor ,"10");
strcpy(param6_valor ,"2019");

strcpy(param7_valor ,"12");

//strcpy(param9_valor ,"7.98");
strcpy(param10_valor ,"35");
strcpy(param11_valor ,"30");
strcpy(param27_valor ,"1");
strcpy(param47_valor ,"1");


strcpy(param22_valor ,"1");
strcpy(param23_valor ,"2");
strcpy(param24_valor ,"3");
strcpy(param25_valor ,"4");
strcpy(param26_valor ,"5");
strcpy(param27_valor ,"6");
strcpy(param28_valor ,"7");
strcpy(param29_valor ,"8");
strcpy(param30_valor ,"9");

strcpy(param31_valor ,"1.5");
strcpy(param32_valor ,"2.5");
strcpy(param33_valor ,"3.5");
strcpy(param34_valor ,"4.5");
strcpy(param35_valor ,"5.5");
strcpy(param36_valor ,"6.5");
strcpy(param37_valor ,"7.5");
strcpy(param38_valor ,"8.5");



if(telemetria.getDateTimeRTC(rtc)){  
  
  Serial.print(F("RTC:  "));
  Serial.print(rtc); 
   
parsing_RTC(rtc, rtc_parsed);     

strcpy(param4_valor ,rtc_parsed[dd]);
strcpy(param5_valor ,rtc_parsed[MM]);
strcpy(param6_valor ,rtc_parsed[yy]);
strcpy(param7_valor ,rtc_parsed[hh]); 
strcpy(param8_valor ,rtc_parsed[mm]); 

/* Serial.print(param4_valor); Serial.print("@"); 
Serial.print(param5_valor); Serial.print("@"); 
Serial.print(param6_valor); Serial.print("@"); 
Serial.print(param7_valor); Serial.print("@"); 
Serial.print(param8_valor); Serial.println("@");   */

sim900_clean_buffer(rtc,sizeof(rtc)); 
} 


if(telemetria.getSignalStrength(&forca_sinal) ) {

Serial.print(F("; Sinal: "));  

/* 
SIM900:
Possible values are (Mapeamento):
0 => -113 dBm or less
1 =>  -111 dBm
2...30 => -109... -53 dBm
31 => -51 dBm or greater
99 => not known or not detectable 

Classificação do sinal:
2 a 9 => Marginal
10 a 14 => OK
15 a 19 => Good
20 a 30 => Excellent
*/
if ( (forca_sinal >=2) && (forca_sinal <=9) ) Serial.println(F("Fraco"));  
else if ( (forca_sinal >= 10) && (forca_sinal <=14) ) Serial.println(F("Normal"));  
else if ( (forca_sinal >= 15) && (forca_sinal <=19) ) Serial.println(F("Bom"));  
else if ( (forca_sinal >= 20) && (forca_sinal <=30))Serial.println(F("Ótimo"));  
else Serial.println(F("Erro")); 

}
else forca_sinal= 99;



static char * paramValue[PARAM_COUNT]={
param2_valor,
param3_valor,
param4_valor,
param5_valor,
param6_valor,
param7_valor,
param8_valor,
param9_valor,
param10_valor,
param11_valor,
param12_valor,
param13_valor,
param14_valor,
param15_valor,
param16_valor,
param17_valor,
param18_valor,
param19_valor,
param20_valor,
param21_valor,
param22_valor,
param23_valor,
param24_valor,
param25_valor,
param26_valor,
param27_valor,
param28_valor,
param29_valor,
param30_valor,
param31_valor,
param32_valor,
param33_valor,
param34_valor,
param35_valor,
param36_valor,
param37_valor,
param38_valor,
param39_valor,
param40_valor,
param41_valor,
param42_valor,
param43_valor,
param44_valor,
param45_valor,
param46_valor,
param47_valor,
};



Serial.println(F("  Request:"));

if (telemetria.http_client_get( (const __FlashStringHelper *) url, 
                            (const __FlashStringHelper *) path,
                            queryParamKeys, 
                            paramValue ,
                            responseBuffer) ) 
                            {
                              Serial.print(F("Resposta:"));
                              Serial.println(responseBuffer);
                            }
else Serial.println(F("Falha")); // ocorreu falha  no envio(request) e/ou recebimento(resposta do server) de dados


sim900_clean_buffer(responseBuffer,sizeof(responseBuffer));

//delay(300000);
delay(15000);

}


/*
 * GPRS_Shield_Arduino.cpp
 * A library for SeeedStudio seeeduino GPRS shield 
 *  
 * Copyright (c) 2015 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : lawliet zou
 * Create Time: April 2015
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// #include <stdio.h>
#include "GPRS_Shield_Arduino.h"


GPRS* GPRS::inst;

GPRS::GPRS(uint8_t tx, uint8_t rx, uint32_t baudRate):gprsSerial(rx,tx)
{
    inst = this;
    sim900_init(&gprsSerial, baudRate);
}

bool GPRS::init(void)
{
     if(!sim900_check_with_cmd(F("AT\r\n"),"OK\r\n",CMD)){
		return false;
    } 
    
    if(!sim900_check_with_cmd(F("AT+CFUN=1\r\n"),"OK\r\n",CMD)){
        return false;
    }
	
    if(!checkSIMStatus()) {
		return false;
    }
    return true;
}

bool GPRS::checkPowerUp(void)
{
  return sim900_check_with_cmd(F("AT\r\n"),"OK\r\n",CMD);
  //We dont need here any pin, as this library has to work also with SIM800L 
}

void GPRS::powerUpDown(uint8_t pin)
{
  // power on pulse for SIM900 Shield
  digitalWrite(pin,LOW);
  delay(1000);
  digitalWrite(pin,HIGH);
  delay(2000);
  digitalWrite(pin,LOW);
  delay(3000);
}

void GPRS::powerReset(uint8_t pin)
{
  // reset for SIM800L board.
  // RST pin has to be OUTPUT, LOW and with a NPN transistor 
  digitalWrite(pin,HIGH);
  delay(1000);
  digitalWrite(pin,LOW);
  delay(3000);  
}
  
  
bool GPRS::checkSIMStatus(void)
{
    char gprsBuffer[32];
    int count = 0;
    sim900_clean_buffer(gprsBuffer,32);
    while(count < 3) {
        sim900_send_cmd(F("AT+CPIN?\r\n"));
        sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
        if((NULL != strstr(gprsBuffer,"+CPIN: READY"))) {
            break;
        }
        count++;
        delay(300);
    }
    if(count == 3) {
        return false;
    }
    return true;
}

bool GPRS::isNetworkRegistered(void)
{
    char gprsBuffer[32];
    int count = 0;
    sim900_clean_buffer(gprsBuffer,32);
    while(count < 3) {
        sim900_send_cmd(F("AT+CREG?\r\n"));
        sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
		//Check if home network (0,1) or roaming (0,5) is enabled
		if( (NULL != strstr(gprsBuffer,"+CREG: 0,1")) || (NULL != strstr(gprsBuffer,"+CREG: 0,5")) ) {
			break;
		}
        count++;
        delay(300);
    }
    if(count == 3) {
        return false;
    }
    return true;
}




bool GPRS::getDateTime(char *buffer)
{
	//If it doesn't work may be for two reasons:
	//		1. Your carrier doesn't give that information
	//		2. You have to configurate the SIM900 IC.
	//			- First with SIM900_Serial_Debug example try this AT command: AT+CLTS?
	//			- If response is 0, then it is disabled.
	//			- Enable it by: AT+CLTS=1
	//			- Now you have to save this config to EEPROM memory of SIM900 IC by: AT&W
	//			- Now, you have to power down and power up again the SIM900 
	//			- Try now again: AT+CCLK?
	//			- It should work
	
	//AT+CCLK?						--> 8 + CR = 9
	//+CCLK:"14/11/13,21:14:41+04"	--> CRLF + 29+ CRLF = 33
	//								
	//OK							--> CRLF + 2 + CRLF =  6

    byte i = 0;
    char gprsBuffer[50];
    char *p,*s;
	sim900_flush_serial();
    sim900_send_cmd(F("AT+CCLK?\r"));
    sim900_clean_buffer(gprsBuffer,50);
    sim900_read_string_until(gprsBuffer, sizeof(gprsBuffer), "OK\r\n");
    if(NULL != ( s = strstr(gprsBuffer,"+CCLK:"))) {
        s = strstr((char *)(s),"\"");
        s = s + 1;  //We are in the first character  (inicio do ano)
        p = strstr((char *)(s),"\""); //p is last character "
        if (NULL != s) {
            i = 0;
            while (s < p) {
              buffer[i++] = *(s++);
            }
            buffer[i] = '\0';       //Ex: buffer[]="14/11/13,21:14:41+04"     
        }
        return true;
    }  
    return false;
}



bool GPRS::getVcc(char *buffer)
{
	//AT+CBC            --> 6 + CR
	//+CBC: 0,100,4241	--> CRLF + 16 + CRLF
	//
	//OK			    --> CRLF + 2 + CRLF
	
    byte i = 0;
    char gprsBuffer[50];
    char *p,*s;
	sim900_flush_serial();
    sim900_send_cmd(F("AT+CBC\r"));
    sim900_clean_buffer(gprsBuffer,50);
    sim900_read_buffer(gprsBuffer,50,DEFAULT_TIMEOUT);
    if(NULL != ( s = strstr(gprsBuffer,"+CBC:"))) {
        s = strstr((char *)(s),",");
        s = s + 1; 
        s = strstr((char *)(s), ","); 
        s = s + 1; //We are in the first Vcc character
        p = s + 4; //p is last character
        if (NULL != s) {
            i = 0;
            while (s < p) {
              buffer[i++] = *(s++);
            }
            buffer[i] = '\0';            
        }
        return true;
    }  
    return false;
}


bool GPRS::getSignalStrength(int *buffer)
{
	//AT+CSQ						--> 6 + CR = 10
	//+CSQ: <rssi>,<ber>			--> CRLF + 5 + CRLF = 9						
	//OK							--> CRLF + 2 + CRLF =  6

	byte i = 0;
	char gprsBuffer[26];
	char *p, *s;
	char buffers[4];
	sim900_flush_serial();
	sim900_send_cmd(F("AT+CSQ\r"));
	sim900_clean_buffer(gprsBuffer, 26);
	sim900_read_buffer(gprsBuffer, 26, DEFAULT_TIMEOUT);
	if (NULL != (s = strstr(gprsBuffer, "+CSQ:"))) {
		s = strstr((char *)(s), " ");
		s = s + 1;  //We are in the first phone number character 
		p = strstr((char *)(s), ","); //p is last character """
		if (NULL != s) {
			i = 0;
			while (s < p) {
				buffers[i++] = *(s++);
			}
			buffers[i] = '\0';
		}
		*buffer = atoi(buffers);
		return true;
	}
	return false;
}

bool GPRS::sleep(void)
{
    return sim900_check_with_cmd(F("AT+CSCLK=2\r\n"),"OK\r\n",CMD);
}


bool GPRS::wake(void)
{
    unsigned long timerStart;
	bool ok = false;
	//First, send AT dummy command to wake up
	sim900_send_cmd(F("AT\r\n"));
	//Second delay almost 100 ms
	delay(200);
	sim900_flush_serial();
	//Third, send the second AT command to check that it is alive
    timerStart = millis();
    while(!ok && ((unsigned long) (millis() - timerStart) < 3000UL) ) {  //Until 3 seconds maximum
		ok = sim900_check_with_cmd(F("AT\r\n"),"OK",CMD);
		delay(300);
    }
	if (ok) {
		//Four, exit sleep mode
		return sim900_check_with_cmd(F("AT+CSCLK=0\r\n"),"OK",CMD);
	} else {
		return false;
	}
}


//Here is where we ask for APN configuration, with F() so we can save MEMORY
bool GPRS::join(const __FlashStringHelper *apn, const __FlashStringHelper *userName, const __FlashStringHelper *passWord)
{
	byte i;
    char *p, *s;
    char ipAddr[32];
    //Select multiple connection
    //sim900_check_with_cmd("AT+CIPMUX=1\r\n","OK",DEFAULT_TIMEOUT,CMD);

    //set APN. OLD VERSION
    //snprintf(cmd,sizeof(cmd),"AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n",_apn,_userName,_passWord);
    //sim900_check_with_cmd(cmd, "OK\r\n", DEFAULT_TIMEOUT,CMD);

    //It is the user who need to call gprs.close and gprs.disconnect
	//sim900_check_with_cmd("AT+CIPSHUT\r\n", "SHUT OK\r\n", CMD, 2000, 1000);  /**Reset the IP session if any**/	
	
    sim900_send_cmd(F("AT+CSTT=\""));
    if (apn) {
      sim900_send_cmd(apn);
    }
	sim900_send_cmd(F("\",\""));
    if (userName) {
		sim900_send_cmd(userName);
    }
	sim900_send_cmd(F("\",\""));
    if (passWord) {
		sim900_send_cmd(passWord);
    }
    if (!sim900_check_with_cmd(F("\"\r\n"), "OK\r\n", CMD)) {
		return false;
	}
 

    //Brings up wireless connection
    if (!sim900_check_with_cmd(F("AT+CIICR\r\n"),"OK\r\n", CMD)) {
		return false;
	}
	
    //Get local IP address
    sim900_send_cmd(F("AT+CIFSR\r\n"));
    sim900_clean_buffer(ipAddr,32);
    sim900_read_buffer(ipAddr,32);
	//Response:
	//AT+CIFSR\r\n       -->  8 + 2
	//\r\n				 -->  0 + 2
	//10.160.57.120\r\n  --> 15 + 2 (max)   : TOTAL: 29 
	//Response error:
	//AT+CIFSR\r\n       
	//\r\n				 
	//ERROR\r\n
    if (NULL != strstr(ipAddr,"ERROR")) {
		return false;
	}
    s = ipAddr + 11;
    p = strstr((char *)(s),"\r\n"); //p is last character \r\n
    if (NULL != s) {
        i = 0;
        while (s < p) {
            ip_string[i++] = *(s++);
        }
        ip_string[i] = '\0';            
    }
    _ip = str_to_ip(ip_string);
    if(_ip != 0) {
        return true;
    }
    return false;
} 

void GPRS::disconnect()
{
	sim900_check_with_cmd(F("AT+CIPSHUT\r\n"), "SHUT OK\r\n", CMD);
}

bool GPRS::connect(Protocol ptl,const char * host, int port, int timeout, int chartimeout)
{
    //char cmd[64];
	char num[4];
    char resp[96];
	
    //sim900_clean_buffer(cmd,64);
    if(ptl == TCP) {
		sim900_send_cmd(F("AT+CIPSTART=\"TCP\",\""));
		sim900_send_cmd(host);
		sim900_send_cmd(F("\","));
		itoa(port, num, 10);
		sim900_send_cmd(num);
		sim900_send_cmd(F("\r\n"));
//        sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",host, port);
    } else if(ptl == UDP) {
		sim900_send_cmd(F("AT+CIPSTART=\"UDP\",\""));
		sim900_send_cmd(host);
		sim900_send_cmd(F("\","));
		itoa(port, num, 10);
		sim900_send_cmd(num);
		sim900_send_cmd(F("\r\n"));

	//        sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n",host, port);
    } else {
        return false;
    }
    

    //sim900_send_cmd(cmd);
    sim900_read_buffer(resp, 96, timeout, chartimeout);
	//Serial.print("Connect resp: "); Serial.println(resp);    
    if(NULL != strstr(resp,"CONNECT")) { //ALREADY CONNECT or CONNECT OK
        return true;
    }
    return false;
}

//Overload with F() macro to SAVE memory
bool GPRS::connect(Protocol ptl,const __FlashStringHelper *host, const __FlashStringHelper *port, int timeout, int chartimeout)
{
    char resp[96];

    if(ptl == TCP) {
        sim900_send_cmd(F("AT+CIPSTART=\"TCP\",\""));   //%s\",%d\r\n",host, port);
    } else if(ptl == UDP) {
        sim900_send_cmd(F("AT+CIPSTART=\"UDP\",\""));   //%s\",%d\r\n",host, port);
    } else {
        return false;
    }
    sim900_send_cmd(host);
    sim900_send_cmd(F("\","));
    sim900_send_cmd(port);
    sim900_send_cmd(F("\r\n"));
//	Serial.print("Connect: "); Serial.println(cmd);
    sim900_read_buffer(resp, 96, timeout, chartimeout);
//	Serial.print("Connect resp: "); Serial.println(resp);    
    if(NULL != strstr(resp,"CONNECT")) { //ALREADY CONNECT or CONNECT OK
        return true;
    }
    return false;
}

bool GPRS::is_connected(void)
{
    // the largest possible value here is:
    // OK\r\n
    // STATE: SERVER LISTENING\r\n
    // 40 byte should be fine
    char resp[40];
    sim900_send_cmd(F("AT+CIPSTATUS\r\n"));
    sim900_read_buffer(resp,sizeof(resp),DEFAULT_TIMEOUT);
    if(NULL != strstr(resp,"STATE: CONNECT OK")) {
        // OK\r\nSTATE: CONNECT OK
        return true;
    } else {
        // e.g:
        // OK\r\nSTATE: TCP CLOSED
        return false;
    }
}

bool GPRS::close()
{
    // if not connected, return
    if (!is_connected()) {
        return true;
    }
    return sim900_check_with_cmd(F("AT+CIPCLOSE\r\n"), "CLOSE OK\r\n", CMD);
}

int GPRS::readable(void)
{
    return sim900_check_readable();
}

int GPRS::wait_readable(int wait_time)
{
    return sim900_wait_readable(wait_time);
}

int GPRS::wait_writeable(int req_size)
{
    return req_size+1;
}

int GPRS::send(const char * str, int len)
{
    //char cmd[32];
	char num[4];
    if(len > 0){
        //snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%d\r\n",len);
		//sprintf(cmd,"AT+CIPSEND=%d\r\n",len);
		sim900_send_cmd(F("AT+CIPSEND="));
		itoa(len, num, 10);
		sim900_send_cmd(num);
		if(!sim900_check_with_cmd(F("\r\n"),">",CMD)) {
        //if(!sim900_check_with_cmd(cmd,">",CMD)) {
            return 0;
        }
        /*if(0 != sim900_check_with_cmd(str,"SEND OK\r\n", DEFAULT_TIMEOUT * 10 ,DATA)) {
            return 0;
        }*/
        delay(500);
        sim900_send_cmd(str);
        delay(500);
        sim900_send_End_Mark();
        if(!sim900_wait_for_resp("SEND OK\r\n", DATA, DEFAULT_TIMEOUT * 10, DEFAULT_INTERCHAR_TIMEOUT * 10)) {
            return 0;
        }        
    }
    return len;
}

boolean GPRS::send(const __FlashStringHelper* str)
{
    if(!sim900_check_with_cmd(F("AT+CIPSEND\r\n"),">",CMD)) {
        return false;
    }

    sim900_send_cmd(str);
    sim900_send_End_Mark();

    if(!sim900_wait_for_resp("SEND OK\r\n", DATA, DEFAULT_TIMEOUT, DEFAULT_INTERCHAR_TIMEOUT)) {
        return false;
    }

    return true;
}

boolean GPRS::send(const char * str)
{
	if(!sim900_check_with_cmd(F("AT+CIPSEND\r\n"),">",CMD)) {
		return false;
	}
	delay(500);
	sim900_send_cmd(str);
	delay(500);
	sim900_send_End_Mark();
	if(!sim900_wait_for_resp("SEND OK\r\n", DATA, DEFAULT_TIMEOUT * 10, DEFAULT_INTERCHAR_TIMEOUT * 10)) {
		return false;
	}   
    return true;
}


int GPRS::recv(char* buf, int len)
{
    sim900_clean_buffer(buf,len);
    sim900_read_buffer(buf,len);   //Ya he llamado a la funcion con la longitud del buffer - 1 y luego le estoy añadiendo el 0
    return strlen(buf);
}


void GPRS::listen(void)
{
	gprsSerial.listen();
}

bool GPRS::isListening(void)
{
	return gprsSerial.isListening();
}



uint32_t GPRS::str_to_ip(const char* str)
{
    uint32_t ip = 0;
    char* p = (char*)str;
    for(int i = 0; i < 4; i++) {
        ip |= atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        ip <<= 8;
        p++;
    }
    return ip;
}

char* GPRS::getIPAddress()
{
    //I have already a buffer with ip_string: snprintf(ip_string, sizeof(ip_string), "%d.%d.%d.%d", (_ip>>24)&0xff,(_ip>>16)&0xff,(_ip>>8)&0xff,_ip&0xff);
    return ip_string;
}

unsigned long GPRS::getIPnumber()
{
    return _ip;
}
/* NOT USED bool GPRS::gethostbyname(const char* host, uint32_t* ip)
{
    uint32_t addr = str_to_ip(host);
    char buf[17];
    //snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr>>24)&0xff, (addr>>16)&0xff, (addr>>8)&0xff, addr&0xff);
    if (strcmp(buf, host) == 0) {
        *ip = addr;
        return true;
    }
    return false;
}
*/

bool GPRS::getLocation(const __FlashStringHelper *apn,const __FlashStringHelper *user,const __FlashStringHelper *pwd, float *longitude, float *latitude)
{
	int i = 0;
    char gprsBuffer[80];
	char buffer[20];
    char *s;

    if (openBearer(apn,user,pwd) == false)
        return false;

	//AT+CIPGSMLOC=1,1
	sim900_flush_serial();
	sim900_send_cmd(F("AT+CIPGSMLOC=1,1\r"));
	sim900_clean_buffer(gprsBuffer,sizeof(gprsBuffer));
	sim900_read_buffer(gprsBuffer,sizeof(gprsBuffer),2*DEFAULT_TIMEOUT,6*DEFAULT_INTERCHAR_TIMEOUT);
	//Serial.println(gprsBuffer);

	if(NULL != ( s = strstr(gprsBuffer,"+CIPGSMLOC:")))
	{
		s = strstr((char *)s, ",");
		s = s+1;
		//Serial.println(*s);
		i=0;
		while(*(++s) !=  ',')
			buffer[i++]=*s;
		buffer[i] = 0;
		*longitude = atof(buffer);

		i=0;
		while(*(++s) !=  ',')
			buffer[i++]=*s;
		buffer[i] = 0;
		*latitude = atof(buffer);
		return true;
	}
	return false;
}


bool GPRS::openBearer(const __FlashStringHelper *apn,const __FlashStringHelper *user,const __FlashStringHelper *pwd)
{
    const uint8_t checkBearerRetryCount = 3;
    int i;

    //send AT+SAPBR=3,1,"Contype","GPRS"
    if (sim900_check_with_cmd(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r"),"OK\r\n", CMD) == false)
        return false;

    //send AT+SAPBR=3,1,"APN","<apn>"
    sim900_send_cmd(F("AT+SAPBR=3,1,\"APN\",\""));
    if (apn) {
      sim900_send_cmd(apn);
    }

    if (sim900_check_with_cmd(F("\"\r"),"OK\r\n", CMD) == false)
        return false;

    // envia AT+SAPBR=3,1,"USER","<user>"

    sim900_send_cmd(F("AT+SAPBR=3,1,\"USER\",\""));
    if (user) {
      sim900_send_cmd(user);
    }

    if (sim900_check_with_cmd(F("\"\r"),"OK\r\n", CMD) == false)
        return false;
    
  // envia AT+SAPBR=3,1,"PWD","<pwd>"

    sim900_send_cmd(F("AT+SAPBR=3,1,\"PWD\",\""));
    if (pwd) {
      sim900_send_cmd(pwd);
    }

    if (sim900_check_with_cmd(F("\"\r"),"OK\r\n", CMD) == false)
        return false;

    //send AT+SAPBR =1,1
    if (sim900_check_with_cmd(F("AT+SAPBR=1,1\r\n"),"OK\r\n", CMD) == false)
        return false;

    for(i = 0; i < checkBearerRetryCount; i++)
    {
        // 3 means "closed"
        uint8_t bearerStatus = 3;
        // 1 means "connected"
        queryBearer(&bearerStatus);
        if (bearerStatus == 1)
            break;
    }

    if (i >= checkBearerRetryCount)
        return false;

    return true;
}

bool GPRS::queryBearer(uint8_t * bearerStatus)
{
    char receiveBuffer[32];
    char * commaPtr;

    // send AT+SAPBR=2,1 and read "+SAPBR"
    if (sim900_check_with_cmd(F("AT+SAPBR=2,1\r\n"),"+SAPBR:", DATA) == false)
        return false;

    sim900_clean_buffer(receiveBuffer, sizeof(receiveBuffer));

    // check response which looks like:
    // +SAPBR: <cid>,<Status>,<IP_Addr>\r\nOK

    // read cid (always 1)
    if (sim900_read_string_until(receiveBuffer, sizeof(receiveBuffer), "1,") == NULL)
        return false;

    // read until next comma -> status
    commaPtr = sim900_read_string_until(receiveBuffer, sizeof(receiveBuffer), ",");
    if (commaPtr == NULL)
        return false;

    // replace comma with string termination
    *commaPtr = '\0';

    // now extract status
    *bearerStatus = (uint8_t)atol(receiveBuffer);

    // only check for ip address if we are connected (=1)
    if (*bearerStatus == 1)
    {
        char * endOfIpAddress = NULL;

        // read ip address, which is enclosed in '"', so read first '"'
        if (sim900_read_string_until(receiveBuffer, sizeof(receiveBuffer), "\"") == NULL)
            return false;

        // read second '"'
        endOfIpAddress = sim900_read_string_until(receiveBuffer, sizeof(receiveBuffer), "\"");
        if (endOfIpAddress == NULL)
            return false;

        *endOfIpAddress = '\0';

        strncpy(ip_string, receiveBuffer, sizeof(ip_string));
        _ip = str_to_ip(ip_string);
    }

    // flush rest of data which should be "\r\nOK"
    sim900_flush_serial();

    return true;;
}


bool GPRS::closeBearer(void)
{
    //TODO maybe also call queryBearer() here and check if it really was closed (as in openBearer)
    return sim900_check_with_cmd(F("AT+SAPBR=0,1\r\n"),"OK\r\n", CMD);
}

bool GPRS::ntpSyncDateTime(int8_t timezone)
{
    // e.g. "-105\0", this is not a valid value to pass to the SIM module, but we need to make sure not
    // to cross boundaries of the char array
    char timezoneString[5] = {'\0'};

    if (sim900_check_with_cmd(F("AT+CNTPCID=1\r\n"), "OK\r\n", CMD) == false)
        return false;

    sim900_send_cmd(F("AT+CNTP=\"pool.ntp.org\","));

    itoa(timezone, timezoneString, 10);

    sim900_send_cmd(timezoneString);

    if (sim900_check_with_cmd(F("\r\n"), "OK\r\n", CMD) == false)
        return false;

    return sim900_check_with_cmd(F("AT+CNTP\r\n"), "+CNTP: 1", CMD);
}

bool GPRS::httpInitialize(void)
{
    return sim900_check_with_cmd(F("AT+HTTPINIT\r\n"), "OK", CMD);
}

bool GPRS::httpTerminate(void)
{
    return sim900_check_with_cmd(F("AT+HTTPTERM\r\n"), "OK", CMD);
}

int16_t GPRS::httpSendGetRequest(const __FlashStringHelper* url, const __FlashStringHelper* path, uint16_t port)
{
    return httpSendGetRequest(url, /*"",*/ path, 0, NULL, NULL, port);
}

int16_t GPRS::httpSendGetRequest(const __FlashStringHelper * url,
                                 /*const char * pathPart1,*/
                                 const __FlashStringHelper * pathPart2,
                                 uint8_t ParametersCount,
                                 const __FlashStringHelper * const ParamKeys[],
                                 const char * const ParamValues[],
                                 uint16_t port)
{
    char receiveBuffer[32];
    char httpStatusCode[4];
    char * commaPtr = NULL;
    const __FlashStringHelper * tempParamKey;

    // 1 AT+HTTPPARA=\"CID\",1
    if (sim900_check_with_cmd(F("AT+HTTPPARA=\"CID\",1\r\n"), "OK", CMD) == false){
         DEBUG_GPRS_SHIELD_L("Erro no CID");
        return -1;
    }

    // 2 AT+HTTPPARA=\"URL\",\"<url>\"
    sim900_send_cmd(F("AT+HTTPPARA=\"URL\",\""));

    sim900_send_cmd(url);

    if (port != HTTP_DEFAULT_PORT){
        char strPort[6] = {'\0'};
        ltoa(port, strPort, 10);
        sim900_send_cmd(F(":"));
        sim900_send_cmd(strPort);
    }

    //sim900_send_cmd(pathPart1);
    sim900_send_cmd(pathPart2);

    for (int i = 0; i < ParametersCount; i++)
    {
        // first parameter
        if (i == 0)
            sim900_send_cmd("?");
        else
            sim900_send_cmd("&");

        // copies the pointers to the strings from progmem to RAM
        // got this progmem magic from http://www.gammon.com.au/progmem
        tempParamKey = (const __FlashStringHelper *)pgm_read_word(&ParamKeys[i]);

        sim900_send_cmd(tempParamKey);
        sim900_send_cmd("=");
        sim900_send_cmd(ParamValues[i]);
    }

    if (sim900_check_with_cmd(F("\"\r\n"), "OK", CMD) == false) { DEBUG_GPRS_SHIELD_L(F("Erro na URL"));  return -1; }

    // 3 AT+HTTPACTION=0
    if (sim900_check_with_cmd(F("AT+HTTPACTION=0\r\n"), "OK", DATA) == false ) { DEBUG_GPRS_SHIELD_L(F("Request falhou")); return -1;}

    // fetch additional data which looks like +HTTPACTION:<Method>,<StatusCode>,<DataLen>
    // where "Method" is always 0 (GET request)
    sim900_clean_buffer(receiveBuffer, sizeof(receiveBuffer));

    if (sim900_read_string_until(receiveBuffer, sizeof(receiveBuffer), "+HTTPACTION:0,", 15, 15000) == NULL) 
    {   DEBUG_GPRS_SHIELD_L(F("Sem resposta do comando AT+HTTPACTION=0"));
        return -1;}


    sim900_clean_buffer(receiveBuffer, sizeof(receiveBuffer));
    // let's not waste time: request should be finished after 1000ms
    sim900_read_buffer(receiveBuffer, sizeof(receiveBuffer), 1, 1000);

    sim900_clean_buffer(httpStatusCode, sizeof(httpStatusCode));
    //status codes are always 3 chars
    strncpy(httpStatusCode, receiveBuffer, 3);

    if (strcmp(httpStatusCode, "200") != 0)
        {DEBUG_GPRS_SHIELD_L(F("Código inválido: Erro na comunicao cliente-server (servidor e cliente não receberam dados)")); return -1;}

    // search for additional comma, DataLen comes after that
    commaPtr = strrchr(receiveBuffer, ',');
    if (commaPtr == NULL)
       { DEBUG_GPRS_SHIELD_L(F("Sem info do numero de dados recebidos pelo cliente: Erro no recebimento dos dados")); return -1;}

    // step over comma
    commaPtr++;

    return atol(commaPtr);
}


bool GPRS::httpReadResponseData(char* buffer, uint16_t bufferSize)
{
     char receiveBuffer[32];
     char * charFoundPtr = NULL;

    // issue command
    if (sim900_check_with_cmd(F("AT+HTTPREAD\r\n"), "+HTTPREAD:", DATA) == false)
        return false;

     sim900_clean_buffer(receiveBuffer, sizeof(receiveBuffer));

    // analyzing response which looks like:
    //+HTTPREAD:<data_len>
    //<data>
    //
    //OK

    // read first line of response, we ignore the data length as httpSendGetRequest already
    // returns this information
    if (sim900_read_string_until(receiveBuffer, sizeof(receiveBuffer),"\r\n") == NULL /*false*/)
        return false;

    sim900_clean_buffer(buffer, bufferSize);

    // read HTTP response data and the following "OK"
    // Note: at this point the response was already received (via GPRS), this is just
    // reading the data from the module. This should be done in 500ms
    charFoundPtr = sim900_read_string_until(buffer, bufferSize, "OK"/*"\r\nOK"*/, 1, 500); // charFoundPtr aponta para o caracter anterior a "OK" em buffer
     if (charFoundPtr == NULL) return false; // Erro na leitura da resposta
     
     // Deixa somente a resposta vinda do servidor em buffer
     for(unsigned int j= 0; j< bufferSize; j++) {
           
      if (buffer<=charFoundPtr){
                if (isSpace(buffer[j])) buffer[j]='\0'; // zera os caracteres space (\r, \n) em buffer (Resposta)
      }
      else buffer[j]='\0'; // zera todos os caracteres a partir de "OK" em buffer
        }

    sim900_flush_serial(); // lê tudo que tem no buffer do software Serial após ler toda a resposta 

    return true;
}



void GPRS::AT_Bypass()
{
    sim900_AT_bypass();
}					  

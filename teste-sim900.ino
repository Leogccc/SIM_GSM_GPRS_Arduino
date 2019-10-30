#include <SoftwareSerial.h>

#define AT_DELAY 2000 // delay entre o envio de comandos AT em ms

void SIM900_bearer_config(void) ; // Configura o SIM900 para acessar a internet via GSM/GPRS usando HTTP
void SIM900_http_client_get(void) ; // faz requisicao a um servidor
void resposta_AT_cmd_Serial(void) ; // mostra a resposta dos comandos AT na Serial padrão


// Configure software serial port de comunicação com o SIM900
SoftwareSerial SIM900(8,7); //rx,tx

char computerdata[550];           //we make a 20 byte character array to hold incoming data from a pc/mac/other.
byte received_from_computer = 0; //we need to know how many characters have been received.
byte serial_event = 0;           //a flag to signal when data has been received from the pc/mac/other.
byte in_char = 0;                //used as a 1 byte buffer to store inbound bytes 



void serialEvent() {                                                              //this interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received.
  received_from_computer = Serial.readBytesUntil(13, computerdata, 550);           //we read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received.
  computerdata[received_from_computer] = 13;                                       //stop the buffer from transmitting leftovers or garbage.
  serial_event = true;                                                            //set the serial event flag.
}


void setup() {
  // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  // Make sure that corresponds to the baud rate of your module
  SIM900.begin(19200);
  Serial.begin(19200) ;
  // Give time to your GSM shield log on to network
  delay(5000);
// SIM900_bearer_config();
// resposta_AT_cmd_Serial();
}



void loop() { 

  if (serial_event == true) { // Envio de comandos AT via terminal (Terminal conectado a Serial do Arduino)
      serial_event = false;                       //reset the serial event flag.                                                      
      // AT command 
      SIM900.write(computerdata, sizeof(computerdata)/sizeof(computerdata[0] ) );  
       for(int i=0; i<(sizeof(computerdata)/sizeof(computerdata[0])) ; i++) computerdata[i]=0 ; // limpa o buffer  
}

// SIM900_http_client();

resposta_AT_cmd_Serial();   
}



void SIM900_bearer_config(void) {
// "smart.m2m.vivo.com.br" ; meta.vivo.com.br
char APN[]= "smart.m2m.vivo.com.br" ;
char login[]= "vivo" ; //meta
char senha[]="vivo" ;
char PIN[]="8486" ;

// Sintax:  AT+SAPBR=<cmd_type>,<cid>[,<ConParamTag>,<ConParamValue>]  //Bearer Settings for Applications Based on IP

String  AT_APN_config = String("AT+SAPBR=3,1"); 
AT_APN_config += ",\"APN\"," ;
AT_APN_config += "\""; 
AT_APN_config.concat(APN);
AT_APN_config.concat("\""); 
AT_APN_config.concat("\r");

char AT_APN_config_array[AT_APN_config.length() +1] ;
AT_APN_config.toCharArray(AT_APN_config_array, AT_APN_config.length() +1 ) ;

String  AT_USER_config = String("AT+SAPBR=3,1"); 
AT_USER_config += ",\"USER\"," ;
AT_USER_config += "\""; 
AT_USER_config.concat(login);
AT_USER_config.concat("\""); 
AT_USER_config.concat("\r");

char AT_USER_config_array[AT_USER_config.length() +1] ;
AT_USER_config.toCharArray(AT_USER_config_array, AT_USER_config.length() +1 ) ;

String  AT_PWD_config = String ("AT+SAPBR=3,1"); 
AT_PWD_config += ",\"PWD\"," ;
AT_PWD_config += "\""; 
AT_PWD_config.concat(senha);
AT_PWD_config.concat("\""); 
AT_PWD_config.concat("\r");

char AT_PWD_config_array[AT_PWD_config.length() +1] ;
AT_PWD_config.toCharArray(AT_PWD_config_array, AT_PWD_config.length() +1 ) ;


// Setting up GPRS connection (Configure bearer profile 1) 

SIM900.write("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r") ;  //Set the connection type to GPRS  
delay(AT_DELAY);

SIM900.write(AT_APN_config_array); //
delay(AT_DELAY);

SIM900.write(AT_USER_config_array); //
delay(AT_DELAY);

SIM900.write(AT_PWD_config_array); //
delay(AT_DELAY);

//SIM900.write("AT+SAPBR=0,1\r");                           //Close the bearer (To close a GPRS context)
//delay(AT_DELAY);
  
//Enable the GPRS (Open the bearer )
SIM900.write("AT+SAPBR=1,1\r"); // To open a GPRS context.
delay(AT_DELAY);

 resposta_AT_cmd_Serial();
 
//Query if the connection is setup properly, if we get back a IP address then we can proceed
SIM900.write("AT+SAPBR=2,1\r"); // To query the GPRS context. 
delay(AT_DELAY);


}


void SIM900_http_client(void) {
  //////////////////////////////////// HTTP
 
  SIM900.write("AT+HTTPINIT\r");// Init http service  // We were allocated a IP address and now we can proceed by enabling the HTTP mode
  delay(AT_DELAY);
  resposta_AT_cmd_Serial();
   
  SIM900.write("AT+HTTPPARA=\"CID\",1\r"); // Start by setting up the HTTP bearer profile identifier
  delay(AT_DELAY);
  resposta_AT_cmd_Serial();

 // http://www.acquanativa.com.br/analytics/includes/data.inc.php?oxigenio_dissolvido=Um_Salve_pro_Elias&temperatura=0\"\r
  SIM900.write("AT+HTTPPARA=\"URL\",\"http://httpbin.org/ip\"\r"); // Set the url  to the address of the webpage you want to access
  delay(AT_DELAY);
  resposta_AT_cmd_Serial();

//SIM900.write("AT+HTTPPARA=\"PROIP\",\"74.208.99.236\"\r");// Set proxy server IP address
//delay(AT_DELAY);

//SIM900.write("AT+HTTPPARA=\"PROPORT\",80\r");// Set proxy server port
//delay(AT_DELAY);
  
  SIM900.write("AT+HTTPACTION=0\r");  //GET session start  // Start the HTTP GET session, by giving this command
  delay(AT_DELAY);
    delay(5000);
  resposta_AT_cmd_Serial();
  
  SIM900.write("AT+HTTPREAD\r");                  // The command tells the module that we want to read the received data
  delay(AT_DELAY);
   delay(5000);
  resposta_AT_cmd_Serial();
  
  SIM900.write("AT+HTTPTERM\r"); // We were allocated a IP address and now we can proceed by enabling the HTTP mode
  delay(AT_DELAY);
  resposta_AT_cmd_Serial();

// Os comandos abaixo podem ser útil mas a ordem não está correta

  
   // SIM900.write("AT+HTTPSSL=1\r");  //OPTIONAL,  ONLY IF URL is HTTPS or SSL enabled: Also Remove the http:// part in the HTTPPARA="URL",xxxx command
 // delay(AT_DELAY);
 
 
}


void resposta_AT_cmd_Serial(void) {
  delay(150); // tempo pra aguardar a resposta do SIM900 (aguarda o preenchimento do buffer)
  if(SIM900.available())
  {
    while( SIM900.available() ) {
         char GSM_resposta = SIM900.read();
         Serial.write(GSM_resposta) ;
      }
   Serial.println(" ") ;  
  }
}

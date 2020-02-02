//-------------------------------------------------------------------------------------------------------------------------
//Biblioteca e parametros do sensor biometrico

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D2, D3); //pinos de leitura do sensor biometrico

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//-------------------------------------------------------------------------------------------------------------------------
//RFID (Leitor de cartao)

#include <Wire.h> //INCLUSÃO DA BIBLIOTECA NECESSÁRIA
#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA

#define SS_PIN D4 //PINO SDA
#define RST_PIN D8 //PINO DE RESET

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

//-------------------------------------------------------------------------------------------------------------------------
//Wifi

#include <ESP8266WiFi.h>
const char WiFiPassword[] = "12345678";
const char AP_NameChar[] = "PET_Tranca" ;
 
WiFiServer server(80);
 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
String html_1 = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'><style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  background-color: #4CAF50; font-size: 120%;}</style><title>Tranca do PET</title></head><body><div id='main'><h2>Tranca do PET</h2>";
String html_2 = "<form id='F1' action='TRANCA'><input class='button' type='submit' value='DESTRAVAR' ></form><br>";
String html_4 = "</div></body></html>";
 
String request = "";

//-------------------------------------------------------------------------------------------------------------------------

//variaveis

const int pinoLedVerde = D0; //PINO DIGITAL REFERENTE AO LED VERDE
const int pinoLedVermelho = D10; //PINO DIGITAL REFERENTE AO LED VERMELHO
const int trancaRele = D9; //pino utilizado para acionar o rele
int ID; //para saber o ID do usuario cadastrado


//-------------------------------------------------------------------------------------------------------------------------
// setup do programa
void setup() {
  Serial.begin(9600); //inicia o monitor serial do arduino, com taxa de 9600

  finger.begin(57600); // defina a taxa de dados para a porta serial do sensor biometrico

  pinMode(pinoLedVerde, OUTPUT); //DEFINE O PINO COMO SAÍDA
  pinMode(pinoLedVermelho, OUTPUT); //DEFINE O PINO COMO SAÍDA
  pinMode(trancaRele, OUTPUT); //DEFINE O PINO COMO SAÍDA
  
  digitalWrite(pinoLedVerde, LOW); //LED INICIA DESLIGADO
  digitalWrite(pinoLedVermelho, LOW); //LED INICIA DESLIGADO
  digitalWrite(trancaRele, LOW); //RELE INICIA DESLIGADO
  
  if (finger.verifyPassword()) {
    Serial.println(F("Sensor biometrico encontrado!"));
    digitalWrite(pinoLedVerde, HIGH);
    delay(350);
    digitalWrite(pinoLedVerde, LOW);
  } else {
    Serial.println(F("NAO foi possivel encontrar o sensor biometrico."));
    digitalWrite(pinoLedVermelho, HIGH);
    while (1) { delay(1); }
  }


  Wire.begin(); //INICIALIZA A BIBLIOTECA WIRE
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522

  //---------------------------------------------------------------------------------------------------------------------------------------------------------
  //configuracao do WIFI

    boolean conn = WiFi.softAP(AP_NameChar, WiFiPassword);
    server.begin();

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  
  
}

void loop(){
  
  ID = leituraBiometrica(); //funcao para ler a biometria e retornar o ID da pessoa
  if(ID != -1){
    digitalWrite(pinoLedVerde, HIGH); //LIGA O LED VERDE
    digitalWrite(trancaRele, HIGH);
    delay(250);
    digitalWrite(trancaRele, LOW);
    delay(750);
    digitalWrite(pinoLedVerde, LOW); //DESLIGA O LED VERDE
  }
  leituraRfid(); //CHAMA A FUNÇÃO RESPONSÁVEL PELA VALIDAÇÃO DA TAG RFID


   // Check if a client has connected
    WiFiClient client = server.available();
    if (!client)  {  return;  }
 
    // Read the first line of the request
    request = client.readStringUntil('\r');
 
    if( request.indexOf("TRANCA") > 0 )  { 
      digitalWrite(trancaRele, HIGH);  
    }
 
 
    // Get the LED pin status and create the LED status message
    if (digitalRead(trancaRele) == HIGH) 
    {
        // the LED is on so the button needs to say turn it off
       html_2 = "<form id='F1' action='TRANCA'><input class='button' type='submit' value='DESTRAVAR' ></form><br>";
    }
 
    
    
    client.flush();
 
    client.print( header );
    client.print( html_1 );    
    client.print( html_2 );
    client.print( html_4);

    delay(500);
    digitalWrite(trancaRele, LOW); 

 
    delay(5);
}

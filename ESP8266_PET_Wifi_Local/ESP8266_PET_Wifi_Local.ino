#include <ESP8266WiFi.h>
const char WiFiPassword[] = "12345678";
const char AP_NameChar[] = "PET_Tranca" ;
 
WiFiServer server(80);
 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
String html_1 = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'><style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  background-color: #4CAF50; font-size: 120%;}</style><title>Tranca do PET</title></head><body><div id='main'><h2>Tranca do PET</h2>";
String html_2 = "<form id='F1' action='TRANCA'><input class='button' type='submit' value='DESTRAVAR' ></form><br>";
String html_4 = "</div></body></html>";
 
String request = "";
int trancaRele = D4;
 
void setup() 
{
    pinMode(trancaRele, OUTPUT); 
    digitalWrite(trancaRele, HIGH); 
 
    boolean conn = WiFi.softAP(AP_NameChar, WiFiPassword);
    server.begin();
 
} // void setup()
 
 
 
void loop() 
{
 
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client)  {  return;  }
 
    // Read the first line of the request
    request = client.readStringUntil('\r');
 
    if( request.indexOf("TRANCA") > 0 )  { 
      digitalWrite(trancaRele, LOW);  
    }
 
 
    // Get the LED pin status and create the LED status message
    if (digitalRead(trancaRele) == LOW) 
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
    digitalWrite(trancaRele, HIGH); 

 
    delay(5);
  // The client will actually be disconnected when the function returns and 'client' object is detroyed
 
}

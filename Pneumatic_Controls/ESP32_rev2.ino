

//------------------------------------------------------------------------
// WIFI SERVER SETUP
//------------------------------------------------------------------------
#include <WiFi.h>

const char* network = "OLIN-DEVICES";
const char* password = "Design&Fabric8";

WiFiServer server(80);

String header;        // storing the HTTP request


//------------------------------------------------------------------------
// VARIABLE PRESSURE ESTABLISHMENT
//------------------------------------------------------------------------
int pressure1 = 0;
int prev1 = -1;
int pressure2 = 0;
int prev2 = -1;
int pressure3 = 0;
int prev3 = -1;
int pressure4 = 0;
int prev4 = -1;


//------------------------------------------------------------------------
// ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ 
//------------------------------------------------------------------------
void setup() {

  Serial.begin(9600);

  // WIFI CONNECT
  Serial.print("Connecting to ");
  Serial.println(network);
  WiFi.begin(network, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // PRINTS IP ADDRESS
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println(IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}


//------------------------------------------------------------------------
// ~ MAIN LOOP ~ MAIN LOOP ~ MAIN LOOP ~ MAIN LOOP ~ MAIN LOOP ~ MAIN LOOP ~ 
//------------------------------------------------------------------------
 void loop() {
   WiFiClient client = server.available();                      // checking for incoming clients 
  
   if (client) {                                                // when client found
   Serial.println("New Client");
   String currentLine = "";
  
   while (client.connected()) {                                 // while client connected
     if (client.available()) {                                  // read bytes if there are any
       char c = client.read();
       Serial.write(c);
       header += c;
    
       if (c == '\n\) {                                         // if the byte says new line 
         if (currentLine.length() == 0) {                       // and current line is blank, thats the end of the client http request
          client.println("HTTP/1.1 200 ok");                    // sends http response code 
          client.println("Content-type:text/html");             // content type
          client.println("Connection: close");
          client.println();


       // Increases each Muscles Pressure
          if (header.indexOf("GET /p1/up") >= 0) {
          Serial.println("pressure 1 muscle increased by 1");
          int pressure1++;
          } else if (header.indexOf("GET /p1/down") >= 0) {
          Serial.println("pressure 1 muscle decreased by 1");
          int pressure1--;
          } else if (header.indexOf("GET /p2/up") >= 0) {
          Serial.println("pressure 2 muscle increased by 1");
          int pressure2++;
          } else if (header.indexOf("GET /p2/down") >= 0) {
          Serial.println("pressure 2 muscle decreased by 1");
          int pressure2--;
          } else if (header.indexOf("GET /p3/up") >= 0) {
          Serial.println("pressure 3 muscle increased by 1");
          int pressure3++;
          } else if (header.indexOf("GET /p3/down") >= 0) {
          Serial.println("pressure 3 muscle decreased by 1");
          int pressure3--;
          } else if (header.indexOf("GET /p4/up") >= 0) {
          Serial.println("pressure 4 muscle increased by 1");
          int pressure4++;
          } else if (header.indexOf("GET /p4/down") >= 0) {
          Serial.println("pressure 4 muscle decreased by 1");
          int pressure4--;
          }
    
       // Displays HTML web page
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link rel=\"icon\" href=\"data:,\">");
    
          // CSS styling buttons 
          client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
          client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
          client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
          client.println(".button2 {background-color: #555555;}</style></head>");         // styling for when button clicked 
    
          client.println("<body><h1>ESP32 Web Server</h1>");                              // web page heading
    
          // PRESSURE BUTTON 1
          client.println("<p>1st Muscle Pressure:</p>");              // text above button
          if (prev1 < pressure1) {
            client.println("<p><a href=\"/p1/up\"><button class=\"button\">" + String(pressure1)+ "</button><//a></p>");
            prev1++;
          } else if (prev1 > pressure1) {
            client.println("<p><a href=\"/p1/down\"><button class=\"button\">" + String(pressure1)+ "</button><//a></p>");
            prev1--;
          }

          // PRESSURE BUTTON 2
          client.println("<p>2nd Muscle Pressure:</p>");
          if (prev2 < pressure2) {
            client.println("<p><a href=\"/p2/up\"><button class=\"button\">" + String(pressure2)+ "</button><//a></p>");
            prev2++;
          } else if (prev2 > pressure2) {
            client.println("<p><a href=\"/p2/down\"><button class=\"button\">" + String(pressure2)+ "</button><//a></p>");
            prev2--;
          }

          // PRESSURE BUTTON 3
          client.println("<p>3rd Muscle Pressure:</p>");
          if (prev3 < pressure3) {
            client.println("<p><a href=\"/p3/up\"><button class=\"button\">" + String(pressure3)+ "</button><//a></p>");
            prev3++;
          } else if (prev3 > pressure3) {
            client.println("<p><a href=\"/p3/down\"><button class=\"button\">" + String(pressure3)+ "</button><//a></p>");
            prev3--;
          }

          // PRESSURE BUTTON 4
          client.println("<p>4th Muscle Pressure:</p>");
          if (prev4 < pressure4) {
            client.println("<p><a href=\"/p2/up\"><button class=\"button\">" + String(pressure4)+ "</button><//a></p>");
            prev4++;
          } else if (prev4 > pressure4) {
            client.println("<p><a href=\"/p2/down\"><button class=\"button\">" + String(pressure4)+ "</button><//a></p>");
            prev4--;
        }
  
  
        client.println(</body></html>");
        client.println();
    
        break;
     } else {
      currentLine = "";
     }
    } else if (c != '\r') {
    currentLine += c;
    }
        
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");


// debugging print statements
    Serial.println(prev1);
    Serial.println(pressure1);
  }
}

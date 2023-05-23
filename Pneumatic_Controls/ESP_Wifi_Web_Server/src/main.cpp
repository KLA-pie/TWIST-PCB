#include <Arduino.h>

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
int pressure1;
int pressure2;
int pressure3;
int pressure4;

int old_pressure = 0;

String sliderNum_str;
String sliderVal_str;
int sliderNum;
int sliderVal;
int endIdx;

//------------------------------------------------------------------------
// ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ SETUP ~ 
//------------------------------------------------------------------------
void setup() {

  Serial.begin(115200);

  delay(10);
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
  Serial.println("IP address: ");
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
        // Serial.write(c);
        // header += c;
        
        if (c == '\n') {                                         // if the byte says new line 
          if (currentLine.length() == 0) {                       // and current line is blank, thats the end of the client http request
            client.println("HTTP/1.1 200 ok");                    // sends http response code 
            client.println("Content-type:text/html");             // content type
            // client.println("Connection: close");
            client.println();

            // Displays HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // styles page
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            
            // style for buttons
            client.println("style>");
            client.println(".slider {-webkit-appearance: none; width: 100%; height: 25px; background: #d3d3d3; outline: none; opacity: 0.7;}");

            // rest of page
            client.println("</style></head><body>");

            // title 
            client.println("<h1>Pressure Control</h1>");

            // sliders
            client.println("<div class=\"slidecontainer\">");
            client.println("<p>Muscle Pressure 1:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"100\" value=\"0\" id=\"p1slider\" oninput=\"p1Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p1Value\">0</p>");
            client.println("<p>Muscle Pressure 2:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"100\" value=\"0\" id=\"p2slider\" class=\"slider\" oninput=\"p2Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p2Value\">0</p>");
            client.println("<p>Muscle Pressure 3:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"100\" value=\"0\" id=\"p3slider\" =\"slider\" oninput=\"p3Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p3Value\">0</p>");
            client.println("<p>Muscle Pressure 4:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"100\" value=\"0\" id=\"p4slider\" class=\"slider\" oninput=\"p4Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p4Value\">0</p>");

            client.println("</div>");

            client.println("<script>");
            client.println("function sendSliderData(sliderId, sliderValue) {var xmlHttp = new XMLHttpRequest(); xmlHttp.open( \"GET\", (sliderId+\",\"+String(sliderValue).padStart(4,0)+\"ENDVAL\"), false ); xmlHttp.send( null );return xmlHttp.responseText;}");
            client.println("var p1 = p1Value.innerText");
            client.println("var p2 = p2Value.innerText");
            client.println("slider.oninput = function() {output.innerHTML = this.value;}");
            client.println("</script>");
            client.println("</body>");
            client.println("</html>");
            client.println();
        
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        endIdx = currentLine.indexOf("ENDVAL");
        if (endIdx != -1) {
          sliderNum_str = currentLine.substring(endIdx-12,endIdx-11);
          sliderNum = sliderNum_str.toInt();
          sliderVal_str = currentLine.substring(endIdx-4,endIdx);
          sliderVal = sliderVal_str.toInt();
          switch (sliderNum)
          {
          case 1:
            old_pressure = pressure1;
            pressure1 = sliderVal;
            break;
          case 2:
            old_pressure = pressure2;
            pressure2 = sliderVal;
            break;
          case 3:
          old_pressure = pressure3;
          pressure3 = sliderVal;
          break;
        case 4:
          old_pressure = pressure4;
          pressure4 = sliderVal;
          break;         
        
          
          default:
            break;
          }
          if (sliderVal != old_pressure) {
              Serial.print("pressure " + String(sliderNum) + ": ");
              Serial.println(sliderVal);
            }
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");


    // debugging print statements
  }
}
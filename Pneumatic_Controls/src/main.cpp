#include <Arduino.h>
#include <Adafruit_MCP4728.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <WiFi.h>

const char* network = "OLIN-DEVICES";
const char* password = "Design&Fabric8";

WiFiServer server(80);

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

Adafruit_MCP4728 pressure_control; //Initialize the DAC

Adafruit_ADS1115 pressure_sense;  //Initialize the ADC

#define NUM_REGULATORS 4

int pressure_desired[NUM_REGULATORS] = {};
int pressure_read[NUM_REGULATORS] = {};



const int limit_kPa = 350; //Approx 10 psi TODO: Change to something reasonable
const int max_kPa = 500; //The rating of the pressure regulator, should happen at 10v
const int max_DAC_val = 3999; // Based on the gain being 5x, the output needs to be slightly scaled down
const int max_ADC_val = 23999; //0.732421875 * max 15 bit number, calculated through gains.

// Serial input command buffer
uint16_t cmd_buffer_pos = 0;
const uint8_t CMD_BUFFER_LEN = 20;
char cmd_buffer[CMD_BUFFER_LEN];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

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

  if (!pressure_control.begin(0x64)) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }
  /*
  for (int i = 0; i<NUM_REGULATORS; i++)  {
    pressure_control.setChannelValue(static_cast<MCP4728_channel_t>(i), 0, MCP4728_VREF_INTERNAL);
  }*/
  pressure_control.setChannelValue(MCP4728_CHANNEL_A, 0, MCP4728_VREF_INTERNAL);
  pressure_control.setChannelValue(MCP4728_CHANNEL_B, 0, MCP4728_VREF_INTERNAL);
  pressure_control.setChannelValue(MCP4728_CHANNEL_C, 0, MCP4728_VREF_INTERNAL);
  pressure_control.setChannelValue(MCP4728_CHANNEL_D, 0, MCP4728_VREF_INTERNAL);

  if (!pressure_sense.begin()) {
    Serial.println("Failed to find ASD1115 chip");
    while(1)  {
      delay(10);
    }
  }

  pressure_sense.setGain(GAIN_ONE);
}


int pressure_to_DAC(int kPa) {
  // A function for converting the desired pressure from KPA to the DAC value
  // Has a maximum to prevent accidentally exploding the muscle.
  // The value will be amplified to 12v?? so we should software limit it.
  // TODO: check it with jacob
  return map(constrain(kPa, 0, limit_kPa), 0, max_kPa, 0, max_DAC_val);
}

int ADC_to_pressure(int adc_value) {
  // A function for converting the desired pressure from ADC to kPa
  // TODO: check it with jacob
  return map(adc_value, 0, max_ADC_val, 0, max_kPa);
}

void set_pressures(){
  int control_signal[NUM_REGULATORS] = {};
  for(int i=0; i<NUM_REGULATORS; i++) {
    control_signal[i] = pressure_to_DAC(pressure_desired[i]);
    //pressure_control.setChannelValue(static_cast<MCP4728_channel_t>(i), control_signal, MCP4728_VREF_INTERNAL);
  }
  
  pressure_control.setChannelValue(MCP4728_CHANNEL_A, control_signal[0], MCP4728_VREF_INTERNAL);
  pressure_control.setChannelValue(MCP4728_CHANNEL_B, control_signal[1], MCP4728_VREF_INTERNAL);
  pressure_control.setChannelValue(MCP4728_CHANNEL_C, control_signal[2], MCP4728_VREF_INTERNAL);
  pressure_control.setChannelValue(MCP4728_CHANNEL_D, control_signal[3], MCP4728_VREF_INTERNAL);
}

void get_pressures()  {
  for(int i=0; i<NUM_REGULATORS; i++) {
    int adc_read = pressure_sense.readADC_SingleEnded(i);
    pressure_read[i] = ADC_to_pressure(adc_read);
  }
}

void get_control_signal() {
  for(int i=0; i<NUM_REGULATORS; i++) {
    Serial.print(pressure_to_DAC(pressure_desired[i]));
  }
  Serial.println("");
}

// Parse and execute commands sent over serial:
void parse_command_buffer() {
  Serial.print("Command read: ");
  Serial.println(cmd_buffer);


  if (strncmp(cmd_buffer, "RD", 2) == 0) {
    //Print out the Pressure Values (kPa)
    Serial.print("Pressure Values (kPa): ");
		for (int i = 0; i < NUM_REGULATORS; i++)  {
      Serial.print(pressure_read[i]);
      Serial.print(" ");
    }
    Serial.println(""); 
  }
  else if (strncmp(cmd_buffer, "CS", 2) == 0) {
    Serial.println("Control Signals: ");
    get_control_signal();
  }

  else if (strncmp(cmd_buffer, "0P", 2) == 0) {
		// Set pressure regulator 0
    int val = atoi(cmd_buffer + 2);
    pressure_desired[0] = val;
    
    Serial.print("Setting Pressure 0:");
    Serial.print(val);
    Serial.println("kPa");

  }
  else if (strncmp(cmd_buffer, "1P", 2) == 0) {
		// Set pressure regulator 1
    int val = atoi(cmd_buffer + 2);
    pressure_desired[1] = val;
    
    Serial.print("Setting Pressure 1:");
    Serial.print(val);
    Serial.println("kPa");

  }
  else if (strncmp(cmd_buffer, "2P", 2) == 0) {
		// Set pressure regulator 2
    int val = atoi(cmd_buffer + 2);
    pressure_desired[2] = val;
    
    Serial.print("Setting Pressure 2: ");
    Serial.print(val);
    Serial.println("kPa");

  }
  else if (strncmp(cmd_buffer, "3P", 2) == 0) {
		// Set pressure regulator 3
    int val = atoi(cmd_buffer + 2);
    pressure_desired[3] = val;
    
    Serial.print("Setting Pressure 3: ");
    Serial.print(val);
    Serial.println("kPa");
  }
}

void detect_serial() {
  if (Serial.available()) {
		char ch = Serial.read(); // Read character
		
		if (ch == '\r') {
		  // New line detected - end of entered command
		  Serial.println("New line detected");
		  cmd_buffer[cmd_buffer_pos] = '\0'; // Null terminate command string
		  
			parse_command_buffer(); // Parse and execute command

		  cmd_buffer_pos = 0; // Reset index back to 0
		  memset(cmd_buffer, 0, sizeof(cmd_buffer)); // Set buffer to be all zero

		} else if (cmd_buffer_pos == CMD_BUFFER_LEN - 1) {
			// Command buffer is full and needs to be reset to read the new character
		  cmd_buffer_pos = 0; // Reset index back to 0
		  memset(cmd_buffer, 0, sizeof(cmd_buffer)); // Set command to 0

		  cmd_buffer[cmd_buffer_pos] = ch; // Save the new character
		  cmd_buffer_pos++; // Increment counter position

		} else {
		  cmd_buffer[cmd_buffer_pos] = ch; // Save the new character
		  cmd_buffer_pos++; // Increment counter position
		}
  }
}

void web_server() {
  WiFiClient client = server.available();                      // checking for incoming clients 
  
  if (client) {                                                // when client found
    Serial.println("New Client");
    String currentLine = "";
    
    while (client.connected()) {                                 // while client connected
      if (client.available()) {                                  // read bytes if there are any
        char c = client.read();

        
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
            client.println("<input type=\"range\" min=\"0\" max=\"350\" value=\"0\" id=\"p1slider\" oninput=\"p1Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p1Value\">0</p>");
            client.println("<p>Muscle Pressure 2:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"350\" value=\"0\" id=\"p2slider\" class=\"slider\" oninput=\"p2Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p2Value\">0</p>");
            client.println("<p>Muscle Pressure 3:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"350\" value=\"0\" id=\"p3slider\" =\"slider\" oninput=\"p3Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
            client.println("<p id=\"p3Value\">0</p>");
            client.println("<p>Muscle Pressure 4:</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"350\" value=\"0\" id=\"p4slider\" class=\"slider\" oninput=\"p4Value.innerText = this.value\" onchange=\"sendSliderData(this.id, this.value)\">");
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
              pressure_desired[sliderNum-1] = sliderVal;
              Serial.print(pressure_desired[0]);
            }
        }
      }
      set_pressures();
      get_pressures();
      detect_serial();
    }
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");


    // debugging print statements
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  web_server();
  // set_pressures();
  // get_pressures();

  // detect_serial();
}



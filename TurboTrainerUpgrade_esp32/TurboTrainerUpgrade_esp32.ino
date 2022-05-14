// Adrian Pilkington 2021
// including web server code with inspiration/snippets from https://randomnerdtutorials.com/esp32-web-server-arduino-ide/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Wire.h>
#include <DHT.h>

// WIFIconfig.h has the 
//const char* ssid = "ssid";
//const char* password = "password";
#include "WIFIconfig.h"

#define DHTTYPE    DHT11
#define DHTPIN 32

DHT dht(DHTPIN, DHTTYPE);

// relay output pins
#define RELAY_1_PIN 25
#define RELAY_2_PIN 26
#define RELAY_3_PIN 27
#define RELAY_4_PIN 14
#define VOLTAGE_PIN 36
#define TEMP_SHUTDOWN_THRESHOLD 59
#define MINIMUM_RESISTOR_ALWAYS_CONNECTED (4.0)
#define NUM_MAXES 10
#define MINIMUM_POWER_TO_WRITE_TO_FILE 100
#define R1_VAL (4.0)
#define R2_VAL (8.0)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";
bool inExerciseProg = false;
int inExerciseProgCounter = 0;

String outputState(int output) 
{
  if (digitalRead(output)) {
    return "checked";
  }
  else {
    return "";
  }
}

String isExecerciseProgSet() 
{
  Serial.println("isExecerciseProgSet");
  if (inExerciseProg==true) 
  {
    Serial.println("in exercise true returned checked");
    return "checked";
  }
  else 
  {
    Serial.println("in exercise false returned nil");
    return "";
  }
}

String temStr;
String voltStr;
String powerStr;

String leaderStringArray[NUM_MAXES];
float maxPowerThisRun;
String maxPowerThisRunStr;
int numLines = 0;

int readPowerMaxData()
{
  numLines = 0;
  leaderStringArray[0] = "no history";
  
    // open the power leader board file, with append
  File powerFile = SPIFFS.open("/powerMax.txt", FILE_READ);
  if (!powerFile)
  {
    Serial.println("failed opening powerMax.txt");
    return 0;
  }
  
  while ((powerFile.available()) && (numLines<NUM_MAXES))
  {
    String line = powerFile.readStringUntil('\n');
    leaderStringArray[numLines] = line;
    numLines++;
  }

  Serial.println("max power leaders=");
  int lines = numLines;
  if (lines == 0) lines = 1; // always print the "no history"
  
  for (int i = 0; i < numLines; i++)
  {
      Serial.println(leaderStringArray[i]);
  }
  
  powerFile.close();
}

int writeMaxPower(float maxPower)
{
  // open the power leader board file, with FILE_WRITE which should replace existing file
  File powerFile = SPIFFS.open("/powerMax.txt", FILE_WRITE);
  if (!powerFile)
  {
    Serial.println("failed opening powerMax.txt");
    return 0;
  }

  if (numLines + 1 >= NUM_MAXES)
  {
     // shuffle all the previous MAX down one, otherwise add at the end
     for (int i = 1; i < NUM_MAXES; i++)
     {
        leaderStringArray[i-1] = leaderStringArray[i];
        Serial.print("leaderStringArray[i-1]=");
        Serial.print(leaderStringArray[i-1]);
        Serial.print(" i=");
        Serial.println(i);
     }
     leaderStringArray[NUM_MAXES-1] = maxPower;
     numLines = NUM_MAXES;
  }
  else
  {
    Serial.print("maxPower=");
    Serial.print(maxPower);
    Serial.print(" added at index=");
    Serial.println(numLines+1);
    leaderStringArray[numLines+1] = maxPower;
    numLines += 1;
  }
  
  // write the file 
  for (int i = 0; i < numLines; i++)
  { 
     powerFile.println(leaderStringArray[i]);
     Serial.print("written to file:");
     Serial.println(leaderStringArray[i]);
  }
  powerFile.close();
}

bool maxInCurrentLeaderList(float maxBiggerCurrentLeaderList)
{
  bool argBiggerThanAllCurrent = false;
  
  for (int i = 0; i < numLines; i++)
  { 
     if (maxBiggerCurrentLeaderList > (leaderStringArray[i].toInt()))
     {
       argBiggerThanAllCurrent = true;
     }
  }  
  return argBiggerThanAllCurrent;
}

String constructStringFromCurrentList()
{
   String maxStrList;
   Serial.println(numLines);
   for (int i = 0; i < numLines; i++)
   {
     maxStrList += leaderStringArray[i];
     maxStrList += " : ";
   }
   return maxStrList;
}

void setup()
{
  pinMode(RELAY_1_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, HIGH);
  pinMode(RELAY_2_PIN, OUTPUT);
  digitalWrite(RELAY_2_PIN, HIGH);
  pinMode(RELAY_3_PIN, OUTPUT);
  digitalWrite(RELAY_3_PIN, LOW);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_4_PIN, LOW);

  Serial.begin(115200);
  dht.begin();

  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  readPowerMaxData();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html");
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });
  
  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/prog1", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      Serial.println("prog1 request");
      if (inputMessage1 == "prog1")
      {
         Serial.println("inputMessage1=prog1");
         if (inputMessage2 == "1")
         {
            inExerciseProg = true;
            inExerciseProgCounter = 0;
            Serial.println("inputMessage2=true");
         }
         else
         {
            inExerciseProg = false;
            Serial.println("inputMessage2=false");
         }
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });


  // call backs for relays
  server.on("/25", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain", outputState(25));
  });
  server.on("/26", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain", outputState(26));
  });
  server.on("/27", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain", outputState(27));
  });
  server.on("/14", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain", outputState(14));
  });

  // callback for exercise preset program button enable
  server.on("/exProg", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain", isExecerciseProgSet());
  });  

  //callbacks for sensor data
  server.on("/temperature", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain",   temStr.c_str());
  });
  server.on("/volts", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain",   voltStr.c_str());
  });
  server.on("/power", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain",   powerStr.c_str());
  });
  server.on("/previousMaxes", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain",  constructStringFromCurrentList());
  });

  server.on("/currentMax", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request->send(200, "text/plain",  maxPowerThisRunStr.c_str());
  });

  // Start server
  server.begin();
}

void loop()
{
  float tem = dht.readTemperature();

  Serial.print("temperature=");
  Serial.println(tem);
  if (isnan(tem))
  {
    tem = 0;
  }
  
  //shutdown relays at threshold
  if (tem > TEMP_SHUTDOWN_THRESHOLD)
  {
    digitalWrite(RELAY_1_PIN, LOW);
    digitalWrite(RELAY_2_PIN, LOW);
    digitalWrite(RELAY_3_PIN, LOW);
    digitalWrite(RELAY_4_PIN, LOW);
    temStr = "threshold >";
    temStr += TEMP_SHUTDOWN_THRESHOLD;
    temStr += " , all relays off";
  }

  // v(out) = v(in) * (R2 / (R1 + R2))
  // R1 = 1M
  // R2 = 100K
  // v out = v in * (100000 / (100000 + 1000000))
  // measured resistor values inserted:
  // if v in = v  out /  (971000 / (971000 + 9660000) = 0.4545 / (0.091336)


  float raw_voltage_value = (analogRead(VOLTAGE_PIN) * 3.3 ) / 4095.0;
  float voltage_value = (raw_voltage_value / 0.0913366) + (2.0); ////(971.0 / (971.0 + 9660.0));
  if (raw_voltage_value < 0.1)  voltage_value = 0;
  voltStr = voltage_value;

  // resistors on relays 1 and 2 are 4ohms, 3 and 4 are 8ohms
  //
  float R1 = digitalRead(RELAY_1_PIN);
  float R2 = digitalRead(RELAY_2_PIN);
  float R3 = digitalRead(RELAY_3_PIN);
  float R4 = digitalRead(RELAY_4_PIN);

  float recip_totalR = 1.0 / MINIMUM_RESISTOR_ALWAYS_CONNECTED; // ////// always have the 4.0 ohm resistor in 

  if (R1 != 0) // check to avoid div by zero
  {
    recip_totalR += (1.0 / (R1 * R1_VAL));
  }
  if (R2 != 0) // check to avoid div by zero
  {
    recip_totalR += (1.0 / (R2 * R1_VAL));
  }
  if (R3 != 0) // check to avoid div by zero
  {
    recip_totalR += (1.0 / (R3 * R2_VAL));
  }
  if (R4 != 0) // check to avoid div by zero
  {
    recip_totalR += (1.0 / (R4 * R2_VAL));
  }

  //P=IV, V=IR, I=V/R, P=(V/R)*V
  float powerAsFloat = voltage_value * voltage_value * recip_totalR; 
  //float powerAsFloat = maxPowerThisRun+10;
  powerStr = powerAsFloat;
  
  // store the max power and this will be output on the web page and stored to the file on SPIFFS
  if (powerAsFloat > maxPowerThisRun)
  {
    maxPowerThisRun = powerAsFloat;
    // write the max power to the file
    Serial.print("Max Power now=");
    Serial.println(maxPowerThisRun);
    if ((maxPowerThisRun > MINIMUM_POWER_TO_WRITE_TO_FILE) && (maxInCurrentLeaderList(maxPowerThisRun)))
    { 
       writeMaxPower(maxPowerThisRun);
    }
  }
  maxPowerThisRunStr = maxPowerThisRun;

  // inExerciseProg causes the relays to be set in a predefined sequence
  if (inExerciseProg == true)
  {
     if (inExerciseProgCounter < 60)
     {
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, LOW);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < 120)
     {
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < 300)
     {
         digitalWrite(RELAY_1_PIN, HIGH);
         digitalWrite(RELAY_2_PIN, HIGH);
         digitalWrite(RELAY_3_PIN, LOW);
         digitalWrite(RELAY_4_PIN, LOW);
     }
     else if (inExerciseProgCounter < 350)
     {
         digitalWrite(RELAY_1_PIN, HIGH);
         digitalWrite(RELAY_2_PIN, HIGH);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < 500)
     {
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < 700)
     {
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, LOW);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     inExerciseProgCounter++;
       
  }

  while (WiFi.status() != WL_CONNECTED) //wifi broken attempt reconnect
  { 
//    server.stop(); // probably assume this has already stopped??
         
    // Connect to Wi-Fi network with SSID and password
    Serial.print("attempting to reconnect to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
  }     
  sleep(2);
}

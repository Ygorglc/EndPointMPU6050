#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(128, 32, &Wire);
#define OLED_address  0x3c 

const char* ssid = "Link Start";
const char* password = "1234#563";

IPAddress ip(192,168,1,25);
IPAddress gateway(192,168,1,1); 
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

Adafruit_MPU6050 mpu;
float accer_x[1000],
       accer_y[1000],
       accer_z[1000],
       temperature;
float  gyro_x_array[1000],
       gyro_y_array[1000],
       gyro_z_array[1000],
       time_xyz_array[1000];
float time_to_action;
         
void Accer_Gyro(){
      unsigned long timexyz = millis();
      int i = 0;
      sensors_event_t a, g, temp;

      time_to_action = millis();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
  
      display.clearDisplay();
      display.print("Coletando Dados...");
      display.println();
      display.display();
      
      while(i!=800){ 
    
        mpu.getEvent(&a, &g, &temp);

        accer_x[i] = a.acceleration.x;
        accer_y[i] = a.acceleration.y;
        accer_z[i] = a.acceleration.z;
        gyro_x_array[i] = float(g.gyro.x);
        gyro_y_array[i] = float(g.gyro.y);
        gyro_z_array[i] = float(g.gyro.z);
        time_xyz_array[i] = millis()-timexyz;
        
        i++;
        delay(5);
      }
      temperature = temp.temperature;
      DynamicJsonDocument doc(512);
      
      Serial.print(F("Stream..."));
      String buf;
      serializeJson(doc, buf);

      time_to_action = millis();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
  
      display.clearDisplay();
      display.print("Dados Coletados!");
      display.println();
      display.display();
      
      server.send(200, "application/json");
      Serial.print(F("done."));
}

void getAccer_Gyro() {
  
      DynamicJsonDocument doc(100000);
      int i = 0;
      JsonArray acceleration_x = doc.createNestedArray("acceleration_x");
      JsonArray acceleration_y = doc.createNestedArray("acceleration_y");
      JsonArray acceleration_z = doc.createNestedArray("acceleration_z");
      JsonArray gyro_x = doc.createNestedArray("gyro_x");
      JsonArray gyro_y = doc.createNestedArray("gyro_y");
      JsonArray gyro_z = doc.createNestedArray("gyro_z");
      JsonArray time_xyz = doc.createNestedArray("time");
      
      while(i!=500){ 

        acceleration_x.add(accer_x[i]);
        acceleration_y.add(accer_y[i]);
        acceleration_z.add(accer_z[i]);
        gyro_x.add(gyro_x_array[i]);
        gyro_y.add(gyro_y_array[i]);
        gyro_z.add(gyro_z_array[i]);
        time_xyz.add(time_xyz_array[i]);
        i++;
        
      }
      
      doc["temperature"] = temperature;
          //      delay(10000);
 
      Serial.print(F("Stream..."));
      String buf;
      serializeJson(doc, buf);
      server.send(200, "application/json", buf);
      Serial.print(F("done."));
      
}

void getAccer_Gyro_Unit() {
  
      DynamicJsonDocument doc(130);
      unsigned long timexyz = millis();
      int i = 0;
      sensors_event_t a, g, temp;
        
      mpu.getEvent(&a, &g, &temp);

      doc["Accer_x"] = a.acceleration.x;
      doc["Accer_y"] = a.acceleration.y;
      doc["Accer_z"] = a.acceleration.z;
      doc["Gyro_x"] = g.gyro.x;
      doc["Gyro_y"] = g.gyro.y;
      doc["Gyro_z"] = g.gyro.z;
      doc["time"] = millis()-timexyz;
      doc["temperature"] = temp.temperature;
        //      delay(10000);
      
      Serial.print(F("Stream..."));
      String buf;
      serializeJson(doc, buf);
      server.send(200, "application/json", buf);
      Serial.print(F("done."));
      
}

void getAccer_Gyro_Unit_2() {
  
      DynamicJsonDocument doc(524);
      DynamicJsonDocument doc2(10000);
      JsonArray object = doc2.createNestedArray("object");
      unsigned long timexyz = millis();
      int i = 0;
      sensors_event_t a, g, temp;
        
      mpu.getEvent(&a, &g, &temp);

      time_to_action = millis();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
  
      display.clearDisplay();
      display.print("Criando Json...");
      display.println();
      display.display();
      while(i!=9){ 
        
          doc["Accer_x"] = accer_x[i];
          doc["Accer_y"] = accer_y[i];
          doc["Accer_z"] = accer_z[i];
          doc["Gyro_x"] = gyro_x_array[i];
          doc["Gyro_y"] = gyro_y_array[i];
          doc["Gyro_z"] = gyro_z_array[i];
          doc["time"] = time_xyz_array[i];
          doc["temperature"] = temp.temperature;
        //      delay(10000);
          object.add(doc);
          i++;
      }
      Serial.print(F("Stream..."));
      String buf;
      serializeJson(object, buf);

      time_to_action = millis();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
  
      display.clearDisplay();
      display.print("Json Criado...");
      display.println();
      display.display();
      
      server.send(200, "application/json", buf);
      Serial.print(F("done."));
      
}

// Serving Hello world
void getSettings() {
     
      DynamicJsonDocument doc(512);
 
      doc["ip"] = WiFi.localIP().toString();
      doc["gw"] = WiFi.gatewayIP().toString();
      doc["nm"] = WiFi.subnetMask().toString();
 
      Serial.print(F("Stream..."));
      String buf;
      serializeJson(doc, buf);

      time_to_action = millis();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
  
      display.clearDisplay();
      display.print("IP: ");
      display.println(WiFi.localIP());
      display.print("Gateway: ");  
      display.println(WiFi.gatewayIP());
      display.print("Submask: ");
      display.println(WiFi.localIP());
      display.display();
      
      server.send(200, F("application/json"), buf);
      Serial.print(F("done."));
}
 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Welcome to the REST Web Server"));
    });
    server.on(F("/values"), HTTP_GET, getAccer_Gyro);
    server.on(F("/value"), HTTP_GET, getAccer_Gyro_Unit);
    server.on(F("/value_2"), HTTP_GET, getAccer_Gyro_Unit_2);
    server.on(F("/settings"), HTTP_GET, getSettings);
    server.on(F("/init_values"),HTTP_GET, Accer_Gyro);
}
 
// Manage not found URL
void handleNotFound() {
  
  time_to_action = millis();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  display.clearDisplay();
  display.print("Not Found: ");
  display.print("404 error: ");
  display.print(server.uri());
  display.println(server.args());
  display.display();
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}

void setup_MPU6050(){
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
  
}

void setup_wifi(){
    
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
}

void setup_restServer(){
  
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  
}

void print_display(){

  time_to_action = millis();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  display.clearDisplay();
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.print("Accer Range: ");
  display.println(mpu.getAccelerometerRange());
  display.print("Gyro Range: ");
  display.println(mpu.getGyroRange());
  display.print("Filter: ");
  display.println(mpu.getFilterBandwidth());
  display.display();

}

 
void setup(void) {
  
  Wire.begin(2, 0);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();   // clears the screen and buffer 
  Wire.begin(2, 0);
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  Serial.begin(115200);
  setup_MPU6050();
  setup_wifi();
  setup_restServer();

  print_display();

  time_to_action = millis();
  
}
 
void loop(void) {
  if(millis()- time_to_action>10000){
    print_display();
    time_to_action = millis();
  }
  
  server.handleClient();
  
}

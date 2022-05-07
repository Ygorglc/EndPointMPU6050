#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* ssid = "Link Start";
const char* password = "1234#563";

IPAddress ip(192,168,1,25);
IPAddress gateway(192,168,1,1); 
IPAddress subnet(255,255,255,0);

WebServer server(80);

Adafruit_MPU6050 mpu;
float accer_x[1000],
       accer_y[1000],
       accer_z[1000],
       temperature;
float  gyro_x_array[1000],
       gyro_y_array[1000],
       gyro_z_array[1000],
       time_xyz_array[1000];
         
void Accer_Gyro(){
      unsigned long timexyz = millis();
      int i = 0;
      sensors_event_t a, g, temp;
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
      server.send(200, "application/json");
      Serial.print(F("done."));
}
void getAccer_Gyro() {
      DynamicJsonDocument doc(107000);
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
      DynamicJsonDocument doc(524);
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
// Serving Hello world
void getSettings() {
     
      DynamicJsonDocument doc(512);
 
      doc["ip"] = WiFi.localIP().toString();
      doc["gw"] = WiFi.gatewayIP().toString();
      doc["nm"] = WiFi.subnetMask().toString();
 
      Serial.print(F("Stream..."));
      String buf;
      serializeJson(doc, buf);
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
    server.on(F("/settings"), HTTP_GET, getSettings);
    server.on(F("/init_values"),HTTP_GET, Accer_Gyro);
}
 
// Manage not found URL
void handleNotFound() {
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

 
void setup(void) {
  Serial.begin(115200);
  setup_MPU6050();
  setup_wifi();
  setup_restServer();
  
}
 
void loop(void) {
  server.handleClient();
}

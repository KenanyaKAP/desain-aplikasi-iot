#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Bonezegei_DHT11.h>

// DHT
#define DHT_PIN 14
Bonezegei_DHT11 dht(DHT_PIN);

// Display MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define DISPLAY_CS_PIN 5


// Masukan ssid dan password dari router
const char* ssid = "Suchacliche2 ";
const char* password = "boteng275";

unsigned long lastTime = 0; 
unsigned long timerDelay = 10000;

char displayLog[40];

void setup() {
  // Display MAX7219
  display.begin();
  display.setIntensity(0);
  display.setTextAlignment(PA_CENTER);
  display.setCharSpacing(1);

  // 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  display.print("Conn");

  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  display.print(WiFi.localIP());
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // cek koneksi
    if(WiFi.status() == WL_CONNECTED && dht.getData()) {
      WiFiClient client;
      HTTPClient http;
      
      float temp = dht.getTemperature();
      Serial.print("Temperature: ");
      Serial.println(temp);
      char serverName[100];
      sprintf(serverName,"http://192.168.18.126:5000/esp?temperature=%3.2f", temp);
      http.begin(client, serverName);
      
      // kirimkan HTTP POST
      int httpResponseCode = http.POST("");
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Display
      sprintf(displayLog,"Temp: %2.1f-C, HTTP Resp: %d", temp, httpResponseCode);
      displayLog[10] = 176;
      Serial.println(displayLog);
      display.displayClear();
      display.displayText(displayLog, PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    
    lastTime = millis();
  }

  display.displayAnimate();
}

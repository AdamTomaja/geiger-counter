#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <esp_task_wdt.h>
#include "config.h"

WiFiClient client;

int logInterval = 30;

int count = 0;
int lastLog = 0;

void IRAM_ATTR FALLING_ISR() {
    count++;
}

void notifyLed(int duration) {
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(duration);
  digitalWrite(STATUS_LED_PIN, LOW);
}

void connectToWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to wifi...");
    notifyLed(300);
    delay(300);
  }

  Serial.println("Wifi connected!");
  digitalWrite(STATUS_LED_PIN, LOW);
}


void postThingspeak(float cpm, float usv) {
  ThingSpeak.setField(1, cpm);
  ThingSpeak.setField(2, usv);
  ThingSpeak.setStatus("OK");

  int x = ThingSpeak.writeFields(TS_CH_ID, TS_WRITE_KEY);

  if (x == 200) {
    Serial.println("Channel update successful");
    notifyLed(1000);
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

}

void software_Reset() {
  Serial.println("resetting by software");
  delay(1000);
  esp_restart();
}

void setupPins() {
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  attachInterrupt(SENSOR_PIN, FALLING_ISR, FALLING);
}

void setup() {
  setupPins();
  Serial.println("IO initialized");

  for(int i = 0; i < 10; i++)  {
    notifyLed(100);
    delay(100);
  }

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  Serial.begin(115200);
  Serial.println("Watchdog enabled");  

  connectToWifi();
  ThingSpeak.begin(client);
  Serial.println("ThingSpeak initialized");
  lastLog = millis();
  Serial.println("System initialized");
}

void printStack()
{
  char *SpStart = NULL;
  char *StackPtrAtStart = (char *)&SpStart;
  UBaseType_t watermarkStart = uxTaskGetStackHighWaterMark(NULL);
  char *StackPtrEnd = StackPtrAtStart - watermarkStart;
  Serial.printf("=== Stack info === ");
  Serial.printf("Free Stack is:  %d \r\n", (uint32_t)StackPtrAtStart - (uint32_t)StackPtrEnd);
}

void loop() {
  esp_task_wdt_reset();

  if (WiFi.status() != WL_CONNECTED) software_Reset();

  if(millis() - lastLog >= (logInterval * 1000)) {
    float cpm = 1.0 * count * (60.0/logInterval);
    float usv = (cpm/151);
    Serial.print(cpm);
    Serial.print(" ");
    Serial.println(usv);
    postThingspeak(cpm, usv);
    count = 0;
    lastLog = millis();
    printStack();
  }
  
  delay(50);
}

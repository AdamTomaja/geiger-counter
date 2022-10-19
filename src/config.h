#ifndef CONFIG_H
#define CONFIG_H
 
#define SENSOR_PIN GPIO_NUM_13
#define STATUS_LED_PIN GPIO_NUM_2

#define WIFI_SSID "<Your WiFi SSID>"
#define WIFI_PASSWORD "<Your WiFi password>"

#define TS_CH_ID <ThingSpeak channel id>
#define TS_WRITE_KEY "<ThingSpeak write api key>"

#define PRINT_DEBUG_MESSAGES
#define WDT_TIMEOUT 10
#endif
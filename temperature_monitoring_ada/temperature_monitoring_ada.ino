#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Galaxy"
#define WLAN_PASS       "12345678"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Shivang1995"
#define AIO_KEY         "aio_hRck89APlrOLg1kUTNqOoC0AW0KZ"

/************************* DHT Sensor Setup *********************************/

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11

DHT dht(DHTPIN, DHTTYPE);

/************************* Adafruit.io Feeds *********************************/

#define TEMP_FEED       "Temperature" 
#define HUMIDITY_FEED   "Humidity"

/*************************** Sketch Code ************************************/

// WiFi client
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

// Initialize the MQTT temperature and humidity feeds
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" TEMP_FEED);
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" HUMIDITY_FEED);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize DHT sensor
  dht.begin();

  // Setup MQTT subscription (if needed)
  // Adafruit_MQTT_Subscribe subscription = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temperature");
  // mqtt.subscribe(&subscription);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (heartbeat)
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.processPackets(10000);

  // Read temperature and humidity from DHT sensor
  float temperatureValue = dht.readTemperature();
  float humidityValue = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperatureValue) || isnan(humidityValue)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Publish temperature and humidity to Adafruit IO
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.println(" °C");
  temperature.publish(temperatureValue);
  Serial.print("Humidity: ");
  Serial.print(humidityValue);
  Serial.println(" %");
  humidity.publish(humidityValue);

  // Wait some time before sending the next update
  delay(3000); // 30 seconds delay
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT... ");
    // Attempt to connect
    if (mqtt.connect() != 0) {
      Serial.print("Failed to connect. Retry in 5 seconds...");
      delay(5000);
    }
  }
}

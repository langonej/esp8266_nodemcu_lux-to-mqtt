#include <BH1750FVI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid     = "";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "";     // The password of the Wi-Fi network

const char* mqttServer = ""; // fqdn or IP of MQTT server
const int mqttPort = 1883; // mqtt port
const char* mqttUser = ""; //mqtt user
const char* mqttPassword = ""; //mqtt pw
// Create the Lightsensor instance
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  Serial.println('\n');

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

  //client.publish("tilghman/esp/shed/lux", "Hello from ESP8266");
  //client.subscribe("esp/test");

  LightSensor.begin();
}
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}
void loop()
{
  uint16_t lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.println(lux);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  char buf[16];
  // replace with what makes sense for you client.publish("example/nodemcu/sensors/lux", (itoa(lux,buf,10))); 
  client.publish("chesapeake/esp/sensor/lux", (itoa(lux,buf,10)));
  delay(1000);
}

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <dht.h>
#define dht_apin D1

dht DHT;

const char* ssid = "JioFi3_04D045";
const char* password = "ashish246";

const char* mqtt_server = "io.adafruit.com";
const char* mqtt_user = "anand30";
const char* mqtt_pass = "2fab2d6f84e44a2b8f5f954fe4d48b6d";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 13370);
  client.setCallback(callback);
  reconnect();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  DHT.read11(dht_apin);
  int h = DHT.humidity;
  int t = DHT.temperature;

  delay(1000);
  String hh = String(h);
  String msg = String(t);

  Serial.print("Publish message: ");
  Serial.println(msg);

  int numt = t;
  char cstr[16];
  itoa(numt, cstr, 10);

  int numh = h;
  char cshr[16];
  itoa(numh, cshr, 10);

  delay(1500);
  client.publish("temp", cstr);
  client.publish("humi", cshr);

}

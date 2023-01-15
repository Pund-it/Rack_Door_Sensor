
#include <Wire.h>

//Reed Switch
const int DOOR_SENSOR_PIN = D1; // Arduino pin on wemosd1mini = D1
const int DOOR_SENSOR_PIN2 = D5; // Arduino pin on wemosd1mini = D5
int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor
int currentDoorState2; // current state of door sensor
int lastDoorState2;    // previous state of door sensor


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Pund-IT";
const char* pswd = "pun!Zlrn6006";
const char* mqtt_server = "10.255.249.52";
const char* topic = "rackdoors";    // this is the [root topic]
const char* rack = "R3C2"; // Rack
const char* door = "Front"; // front/back
const char* door2 = "Rear"; // front/rear


long timeBetweenMessages = 100;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

int status = WL_IDLE_STATUS;     // the starting Wifi radio's status

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
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


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

String composeClientID() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String clientId;
  clientId += "esp-";
  clientId += macToStr(mac);
  return clientId;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = composeClientID() ;
    clientId += "-";
    clientId += String(micros() & 0xff, 16); // to randomise. sort of

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, ("connected " + composeClientID()).c_str() , true );
      // ... and resubscribe
      // topic + clientID + in
      String subscription;
      subscription += topic;
      subscription += "/";
      subscription += composeClientID() ;
      subscription += "/in";
      client.subscribe(subscription.c_str() );
      Serial.print("subscribed to : ");
      Serial.println(subscription);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" wifi=");
      Serial.print(WiFi.status());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(DOOR_SENSOR_PIN2, INPUT_PULLUP); // set arduino pin to input pull-up mode

  currentDoorState = digitalRead(DOOR_SENSOR_PIN); // read state
  currentDoorState2 = digitalRead(DOOR_SENSOR_PIN2); // read state
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Pund-IT Rack Door Sensor");
}

void loop() {
  lastDoorState = currentDoorState;              // save the last state
  lastDoorState2 = currentDoorState2;              // save the last state
  currentDoorState  = digitalRead(DOOR_SENSOR_PIN); // read new state
  currentDoorState2  = digitalRead(DOOR_SENSOR_PIN2); // read new state
  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH
    // confirm still connected to mqtt server
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    
    long now = millis();
    if (now - lastMsg > timeBetweenMessages ) {
      lastMsg = now;
      ++value;
      //Build String
      String payload = "Open";
      String pubTopic;
      pubTopic += topic ;
      pubTopic += "/openclose/";
      pubTopic += rack;
      pubTopic += "-";
      pubTopic += door;
      Serial.print("Publish topic: ");
      Serial.println(pubTopic);
      Serial.print("Publish message: ");
      Serial.println(payload);
      client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
    }

  } else if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
       long now = millis();
    if (now - lastMsg > timeBetweenMessages ) {
      lastMsg = now;
      ++value;
      //Build String
      String payload = "Closed";
      String pubTopic;
      pubTopic += topic ;
      pubTopic += "/openclose/";
      pubTopic += rack;
      pubTopic += "-";
      pubTopic += door;
      Serial.print("Publish topic: ");
      Serial.println(pubTopic);
      Serial.print("Publish message: ");
      Serial.println(payload);
      client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
    }
  }
  //second door
   if (lastDoorState2 == LOW && currentDoorState2 == HIGH) { // state change: LOW -> HIGH
    // confirm still connected to mqtt server
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    
    long now = millis();
    if (now - lastMsg > timeBetweenMessages ) {
      lastMsg = now;
      ++value;
      //Build String
      String payload = "Open";
      String pubTopic;
      pubTopic += topic ;
      pubTopic += "/openclose/";
      pubTopic += rack;
      pubTopic += "-";
      pubTopic += door2;
      Serial.print("Publish topic: ");
      Serial.println(pubTopic);
      Serial.print("Publish message: ");
      Serial.println(payload);
      client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
    }

  } else if (lastDoorState2 == HIGH && currentDoorState2 == LOW) { // state change: HIGH -> LOW
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
       long now = millis();
    if (now - lastMsg > timeBetweenMessages ) {
      lastMsg = now;
      ++value;
      //Build String
      String payload = "Closed";
      String pubTopic;
      pubTopic += topic ;
      pubTopic += "/openclose/";
      pubTopic += rack;
      pubTopic += "-";
      pubTopic += door2;
      Serial.print("Publish topic: ");
      Serial.println(pubTopic);
      Serial.print("Publish message: ");
      Serial.println(payload);
      client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
    }
  }
 
  delay(10);
}

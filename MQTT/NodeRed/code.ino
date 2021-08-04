#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "XXXXXX XXXXXXX XXXXXXXXX";
const char* password = "XXXXXXXX";
const char* mqtt_server = "XXX.XXX.X.XXX";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
String in;
int value;

void setup() {
  pinMode(4, OUTPUT);   // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  
  // initialize the ADC pin as an input
  pinMode(A0, INPUT); 
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
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

  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(5000);                       // wait for a second
  }
  
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    digitalWrite(4, LOW);
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(4, HIGH);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement... =================>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> THIS LOOP IS SHOWING VALUES,////////////////
      client.publish("outTopic", "hello world");
      client.publish("Intellic/Pyramid/Temp","0");
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

// Declare variables for the analog input from temp sensor
int sensorPin = A0;
int sensorValue = 100;
String mes = "0";



void loop() {
  // Read the sensor
  //sensorValue = analogRead(sensorPin); //////////////

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 1000) {

    sensorValue = analogRead(sensorPin);
    Serial.print("/// Raw Sensor Value:");
    Serial.println(sensorValue);
    
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    mes = String(sensorValue);
    
    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    //
    client.subscribe("in");
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    client.publish("Temp",(char*)mes.c_str());
  }
}

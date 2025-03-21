#include <WiFiS3.h>

char ssid[] = "MagentaWLAN-YL7R";          
char password[] = "xxxx"; 

// ThingSpeak settings
const char* server = "api.thingspeak.com";
String api_key = "HHLV8FR30P8QCCFB";  

struct sensor
{
  int pin;
  int value;
  int percent;
  sensor(int p) : pin(p), value(0), percent(0) {}
};
sensor moisture_sensor_1(A0);   
sensor moisture_sensor_2(A1);

WiFiClient client;

void setup() 
{
  Serial.begin(9600);
  while(!Serial)
  {
     // wait for serial to connect
  }  
  init_wifi_connection();
}

void loop() 
{  
  update_moisture_sensor(&moisture_sensor_1); 
  update_moisture_sensor(&moisture_sensor_2);
  
  //print_sensor_data(&moisture_sensor_1);
  //print_sensor_data(&moisture_sensor_2);
  
  if (WiFi.status() != WL_CONNECTED) 
  {
    init_wifi_connection();
  }
  else
  {     
    prepare_and_sent_reading();
  } 
  //delay(1000);   
  delay(20000);
}

void prepare_and_sent_reading()
{
    String post_str = "&field1=";
    post_str += moisture_sensor_1.percent;

    post_str += "&field2=";
    post_str += moisture_sensor_2.percent;

    post_str += "\r\n\r\n";

    sent_to_thing_speak(post_str);
}

void sent_to_thing_speak(String str_data)
{
  if (client.connect(server, 80)) // default port for http , connecting to server using http protocol
  {
    //String post_str = api_key;      
    
    Serial.println(str_data);

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(str_data.length());
    client.print("\n\n");
    client.print(str_data);

    Serial.println("Data sent to ThingSpeak");      
  }    
}

void init_wifi_connection()
{
  // connect to wifi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  int status = WiFi.begin(ssid, password);  
  while(status != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
    status = WiFi.status();    
  }
  Serial.println(".");

  Serial.println("Connected to wifi");
}

void update_moisture_sensor(sensor* moisture_sensor)
{
  moisture_sensor->value = analogRead(moisture_sensor->pin);  
  moisture_sensor->percent = map(moisture_sensor->value, 800, 400, 0, 100);
  
  // Constrain the value to 0-100%  
  moisture_sensor->percent = constrain(moisture_sensor->percent, 0, 100);  
}

void print_sensor_data(sensor* moisture_sensor)
{
    //Serial.print("Soil Moisture Level: ");
  Serial.print(moisture_sensor->value);          
  Serial.print("\t");
  Serial.println(moisture_sensor->percent);     
  Serial.print("\t");
}
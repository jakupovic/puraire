#include <WiFi.h>
#include "DHTesp.h"
#include <U8x8lib.h>
#include <PMsensor.h>

PMsensor PM;
DHTesp dht;

int dustPin=0;
float dustVal=0; 
int ledPower=16;
int delayTime=280;
int delayTime2=40;
float offTime=9680;
int hallEffectSensor = 10 ; // define the Hall magnetic sensor interface

const char* ssid = "elvis";
const char* password = "elviselvis";
// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
 
WiFiServer server(80);

void setup(){
  Serial.begin(115200);
  pinMode(ledPower,OUTPUT);
  pinMode(dustPin, INPUT);
  pinMode (hallEffectSensor, INPUT) ;  // define the Hall magnetic sensor line as input


/////(infrared LED pin, sensor pin)  /////
  PM.init(3, A0);          
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  // Initialize temperature sensor
  dht.setup(26, DHTesp::DHT22);
  //delay(1);
   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print  the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  u8x8.drawString(0, 1, "PurAire.com");
  
  // Define 
  String str = WiFi.localIP().toString(); 
     
  // Prepare the character array (the buffer) 
  char char_array[str.length() + 1];
   
  // Copy it over 
  str.toCharArray(char_array, str.length() + 1);
  u8x8.drawUTF8(0, 2, char_array);

}
 
void loop(){  
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }  
    // Reading temperature and humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    float temperature  = dht.getTemperature();
    float humidity = dht.getHumidity();
    float pm25 = 0;
    int err = PMsensorErrSuccess;
    if ((err = PM.read(&pm25, true, 0.1)) != PMsensorErrSuccess) {
        Serial.print("data Error = ");
        Serial.println(err);
        delay(3000);
        //return;
     }

    while (client.connected()) { 
          
      String response = String("") + "{\"pm25\":\"" + pm25 +  "\",\"temperature\":\"" + 
                        String(temperature,2) + "\",\"humidity\":\"" +
                        String(humidity,2) + "\"}" + "\r\n";
      // Define 

      String strPm25 = String("Dust: ") + pm25;
      // Prepare the character array (the buffer) 
      char pm25_char[strPm25.length() + 1];
       
      // Copy it over 
      strPm25.toCharArray(pm25_char, strPm25.length() + 1);
      u8x8.drawUTF8(0, 3, pm25_char);

      String strTemp = String("Temp: ") + String(temperature,2);
      // Prepare the character array (the buffer) 
      char temp_char[strTemp.length() + 1];
       
      // Copy it over 
      strTemp.toCharArray(temp_char, strTemp.length() + 1);
      u8x8.drawUTF8(0, 4, temp_char);
      
      String strHumidity = String("Humi: ") + String(humidity,2);
      // Prepare the character array (the buffer) 
      char humi_char[strHumidity.length() + 1];
       
      // Copy it over 
      strHumidity.toCharArray(humi_char, strHumidity.length() + 1);
      u8x8.drawUTF8(0, 5, humi_char);
      client.printf("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %u\r\n\r\n%s",
          response.length(), response.c_str());
      client.flush();
      // break out of the while loop:
      break;
    }
    // close the connection:
    client.stop();
}

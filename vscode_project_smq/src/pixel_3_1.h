//-----------------------------------------------
// Swimming Pixel Lightshow mit MQTT (ESP32)
// Karlsruhe University of Applied Science
// Informationstechnik labor
// -----------------------------------------------
// This code runs on a Lolin32 Board inside a Swimming-Pixel
// It recieves MQTT-Messages, sent from a Webapp-Launchpad.
// It has 4 Modes:
//    1.  Single Control  - Every pixel is controllable seperately
//    2.  Lightshow 1     
//    3.  Lightshow 2 
//    4.  Lightshow 3
// -----------------------------------------------
// Authors SS21:  Jannik Lassen, Johannes Neckel
// Authors WS20:  Jin Yun Ng, Ping Wen Liew, Tim Schroh, Benedikt Ochs 
// Authors SS20:  Lukas Reimold and Patrick Rodinger
//------------------------------------------------
// latest updates:
// 25.04.2021: set initial brightness to 250 (line 84)
//------------------------------------------------

//------------------------includes-------------------------------------------------
#include <Adafruit_NeoPixel.h>   
#include <WiFi.h>
#include <MQTT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <cstdio>
#include <ctime>

//------------------------Defines for Neo-Pixel ring ------------------------------
#define LED_PIN   5  // Which pin on the Arduino is connected to the NeoPixels? // On a Trinket or Gemma we suggest changing this to 1:
#define LED_COUNT 64 // How many NeoPixels are attached to the Arduino?
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//---------------------------Defines Wifi and broker Connection ------------------------------
extern const char ssid[];
extern const char pass[];
extern const char MQTT_BROKER_ADDRESS[];

//---------------------------Defines Variables and Flags ------------------------------
String prevArray[3];
String rgbArray[3];
bool lightshow_1 = false;
bool lightshow_2 = false;
bool lightshow_3 = false;
bool single_control = false;

WiFiClient net;
MQTTClient client(25000); //sets maximum message-size to ~25kB

//-------------------------------Defines MQTT topics------------------------------
const char TOPIC_PIXEL[] = "sensors/pixels/3_1";

//-----------------------------Function Prototypes----------------------------------
//Function to establish connection to WiFi and MQTT
void connect();

//Functions to control pixel LEDs
void lightshow1();
void lightshow2();
void lightshow3();
void singlecontrol();
void singlepixel(String &input);
void set_pixel_black();
void set_pixel_red();
void set_pixel_green();
void set_pixel_blue();
void set_pixel_yellow();
void set_pixel_purple();
void theaterChase(uint32_t color, int wait);

//Function gets called on receiving a message via MQTT
void messageReceived(String &topic, String &input);

//-----------------------------Display-Helper-Functions----------------------------------
void log(String message)
{
  Serial.println(message.c_str());
}

//-----------------------gets called on startup--------------------
void setup() 
{
  Serial.begin(115200);
  
  //------------------------Neo Pixel-------------------------------
  strip.begin();
  strip.setBrightness(250);
  strip.show();
  
  //--------------------WIFI & MQTT Connection----------------------------
  
  client.begin(MQTT_BROKER_ADDRESS, net); //Set the IP address directly.
  client.onMessage(messageReceived); //Set wich function to call on receiving a MQTT message
  connect(); //connect to WiFi and MQTT
}


void loop() 
{
  client.loop(); // function to check for new message arrivals

  if (!client.connected()) 
  { 
    connect(); // in case of connection loss, the client reconnects
  }
    delay(500);

  if ((lightshow_1 == true) && (single_control == false) && (lightshow_2 == false)&& (lightshow_3 == false))
  {
    lightshow1(); // go to lightshow1 function when other flags are false
  }

  else if ((lightshow_1 == false) && (single_control == false) && (lightshow_2 == true)&& (lightshow_3 == false))
  {
    lightshow2(); // go to lightshow2 function when other flags are false
  }

  if ((lightshow_1 == false) && (single_control == false) && (lightshow_2 == false)&& (lightshow_3 == true))
  {
    lightshow3(); // go to lightshow3 function when other flags are false
  }
}

void messageReceived(String &topic, String &input) 
{
  Serial.print("\nMessage received: ");
  Serial.print(input);
  if (topic == TOPIC_PIXEL)
  {
    if ((input != "lightshow1") && (input != "lightshow2") && (input != "lightshow3") && (input !="singlecontrol"))
    {
      // function to set all flags false when the message received is rgb data
      lightshow_1 = false;
      lightshow_2 = false;
      lightshow_3 = false;
      single_control = false;
      singlepixel(input);
    }

    else if ((input == "lightshow1")&&(input != "singlecontrol")&& (input != "lightshow2") && (input != "lightshow3"))
    {
      // function to set lightshow_1 flag to true and others false
      lightshow_1 = true;
      lightshow_2 = false;
      lightshow_3 = false;
      single_control = false;
      set_pixel_black();
    }

    else if ((input != "lightshow1")&&(input != "singlecontrol")&& (input == "lightshow2") && (input != "lightshow3"))
    {
      // function to set lightshow_2 flag to true and others false
      lightshow_1 = false;
      lightshow_2 = true;
      lightshow_3 = false;
      single_control = false;
    }

    else if ((input != "lightshow1")&&(input != "singlecontrol")&& (input != "lightshow2") && (input == "lightshow3"))
    {
      // function to set lightshow_3 flag to true and others false
      lightshow_1 = false;
      lightshow_2 = false;
      lightshow_3 = true;
      single_control = false;
    }

    else if ((input == "singlecontrol")&& (input != "lightshow2") && (input != "lightshow3")&& (input != "lightshow1"))
    {
      // function to set single_conntrol flag to true and others false
      lightshow_1 = false;
      lightshow_2 = false;
      lightshow_3 = false;
      single_control = true;
      singlecontrol();
    }
  }
}

void singlepixel(String &input)
{
  // function to control each pixel with RGB data message received
  String rgb = input;
  char *ptr;
  int l = 0;
  char string[rgb.length()];
  lightshow_1 = false;
  lightshow_2 = false;
  lightshow_3 = false;
  single_control = false;
  
  
  for(int i = 0; i < rgb.length(); i++)
  {
  string[i] = rgb.charAt(i);  
  }

  // initialisieren und ersten Abschnitt erstellen
  ptr = strtok(string, "rgb(,)");

  while(ptr != NULL) 
  {
    rgbArray[l] = ptr;
    ptr = strtok(NULL, "rgb(,)");
    l++;
  }

  rgbArray[1].remove(0,1);
  rgbArray[2].remove(0,1);
  
  for (int i=0; i<3; i++)
  {
    prevArray[i]=rgbArray[i]; //saving current rgb array to another array
  }  

  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(rgbArray[0].toInt(),rgbArray[1].toInt(),rgbArray[2].toInt()));   
    strip.show();   // updated pixel colors to the hardware.
  }
}


void lightshow1()
{
  // function to display lightshow1
  //theaterChase(strip.Color(127, 0, 0), 50); // Red, half brightness
  //theaterChase(strip.Color(127,  127,   127), 50); // White, half brightness
  //theaterChase(strip.Color(75, 0, 130), 50); // Purple, half brightness
  

 clock_t start;
double D=0;
start=clock();
while(D<30.7)
{
  lightshow2();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(2200);
set_pixel_red();
delay(2000);
while(D<65.7)
{
  lightshow2();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(2300);
set_pixel_red();
delay(2000);
while(D<82.5)
{
  lightshow2();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(2200);
set_pixel_red();
delay(2000);
while(D<135)
{
lightshow2();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
while(D<143.5)
{
lightshow3();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(1000);
while(D<153)
{
lightshow2();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(100);
while(D<161)
{
lightshow3();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(250);
while(D<169.5)
{
lightshow2();
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
}
delay(2000);
while(D<198)                                             
{
  for(int i=0;i<3;i++)                                    // Dreimal  Roter Wellenfront links<->rechts
  {
   if(D>=198)
  {break;}
  set_pixel_black();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_red();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_red();
 delay(100);
  set_pixel_black();
 delay(100);
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
  }
  for(int j=0;j<3;j++)                                      // Dreimal Blauer Wellenfront unten <-> oben
  {
    if(D>=198)
  break;
    set_pixel_black();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_blue();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_black();
 delay(100);
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
  
  }
  for(int j=0;j<3;j++)                                      // Dreimal Gr??ner Wellenfront links <-> rechts
  {
     if(D>=198)
  {break;}
    set_pixel_black();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_green();
 delay(100);
  set_pixel_black();
 delay(100);
  set_pixel_green();
 delay(100);
  set_pixel_black();
 delay(100);
  D=(clock()-start)/(double)CLOCKS_PER_SEC;
  }
    for(int j=0;j<3;j++)                                      // Dreimal Gelber Wellenfront unten <-> oben
  {
    if(D>=198)
    {break;}
    set_pixel_black();
    delay(100);
    set_pixel_black();
    delay(100);
    set_pixel_black();
    delay(100);
    set_pixel_yellow();
    delay(100);
    set_pixel_black();
    delay(100);
    set_pixel_black();
    delay(100);
    D=(clock()-start)/(double)CLOCKS_PER_SEC;
  }
  
}
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(250,250,250));         //  Set pixel's color (in RAM)
    strip.show();
  }
  delay(2000);
  set_pixel_black();
lightshow_1 = false;
}


void lightshow2()
{
  // function to display lightshow2
  int fadeVal=0, fadeMax=100;
  int rainbowLoops=3;
  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) 
    {

    for(int i=0; i<strip.numPixels(); i++) 
    { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(2);

    if(firstPixelHue < 65536) 
    {                              // First loop,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } 
    else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) 
    { // Last loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } 
    else 
    {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }
}

void lightshow3()
{
  // function to display lightshow3
  // function to display lightshow3
  for(int i=0; i<LED_COUNT; i++)
    {
      strip.setPixelColor(i, strip.Color(243,12,204));
      strip.setBrightness(250);
      strip.show();
    }

    for(int k=250; k>0; k--)
    {
      strip.setBrightness(k);
      strip.show();
      delay(6.9);
    }

      for(int i=0; i<LED_COUNT; i++)
    {
      strip.setPixelColor(i, strip.Color(243,12,204));
      strip.setBrightness(1);
      strip.show();
    }

    for(int j=1; j<250; j++)
    {
      strip.setBrightness(j);
      strip.show();
      delay(6.8);
    }
}

void singlecontrol()
{
  // function to return each pixel to previous single pixel colour after returning from lightshow modes
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(prevArray[0].toInt(),prevArray[1].toInt(),prevArray[2].toInt()));   
    strip.show();   // updated pixel colors to the hardware.
  }
}

void set_pixel_black()
{
  // function to set all pixels black before lightshows start
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(0,0,0));         //  Set pixel's color (in RAM)
    strip.show();
  }
}
void set_pixel_red()
{
  // function to set all pixels red 
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(255,0,0));         //  Set pixel's color (in RAM)
    strip.show();
  }
}
void set_pixel_green()
{
  // function to set all pixels green 
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(0,255,0));         //  Set pixel's color (in RAM)
    strip.show();
  }
}
void set_pixel_blue()
{
  // function to set all pixels blue 
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(0,0,255));         //  Set pixel's color (in RAM)
    strip.show();
  }
}
void set_pixel_yellow()
{
  // function to set all pixels yellow 
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(255,255,0));         //  Set pixel's color (in RAM)
    strip.show();
  }
}
void set_pixel_purple()
{
  // function to set all pixels purple 
  for(int i=0; i<LED_COUNT; i++) 
  { 
    strip.setPixelColor(i, strip.Color(243,12,204));         //  Set pixel's color (in RAM)
    strip.show();
  }
}
void theaterChase(uint32_t color, int wait) 
{
  for(int a=0; a<10; a++) 
  {  // Repeat 10 times...
    for(int b=0; b<3; b++) 
    { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) 
      {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void connect() 
{
  //--------------------Connection to WiFi---------------
  log("Checking wifi");
 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("."); 
  }
  
  Serial.print("connected");
  log("\nConnected!");
  delay(2000);
  
  //--------------------Connection to Broker---------------
  log("\nConnecting to Broker");
  
  while (!client.connect(WiFi.macAddress().c_str(), "try", "try")) 
  {
    Serial.print(".");
  }
  
  Serial.print("connected");
  log("\nConnected!");
  delay(2000);
  
  //---------------Subscribe to Topics--------------------
  client.subscribe(TOPIC_PIXEL);
}

//-----------------------------------------------
// Program: Swimming Pixel Lightshow mit MQTT (ESP32)
// Karlsruhe University of Applied Science
// Authors SS21: Jannik Lassen (laja1024), Johannes Neckel (nejo1017)
// Authors WS20: Jin Yun Ng, Ping Wen Liew, Tim Schroh, Benedikt Ochs (Extended version from Lukas Reimold and Patrick Rodinger)
// Current Status: test (08.04.2021)
//------------------------------------------------

/*
To make it easy to upload every program to the 16 ESP32s, all codes are in seperate .h files.
The only thing that needs to be changed for the different pixels is the include here in the main.cpp file
*/

const char ssid[] = "JuergenWalter";
const char pass[] = "44873763559236747268";
const char MQTT_BROKER_ADDRESS[] = "192.168.188.100";


//-----------------------------
// pixel_column_row.h
// 1_1  y    2_1  y   3_1  y   4_1  n       y: Programm Hochegladen&beschriftet
// 1_2  y    2_2  y   3_2  y   4_2  n       n: To do
// 1_3  y    2_3  y   3_3  y   4_3  n
// 1_4  y    2_4  y   3_4  y   4_4  n
//-----------------------------
#include "pixel_4_2.h"
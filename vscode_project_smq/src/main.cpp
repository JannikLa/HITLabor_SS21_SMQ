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

//-----------------------------
// pixel_column_row.h
// 1_1      2_1     3_1     4_1
// 1_2      2_2     3_2     4_2
// 1_3      2_3     3_3     4_3
// 1_4      2_4     3_4     4_4
//-----------------------------
#include "pixel_1_1.h"
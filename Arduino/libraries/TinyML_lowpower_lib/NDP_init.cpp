/*
 * Copyright (c) 2021 Syntiant Corp.  All rights reserved.
 * Contact at http://www.syntiant.com
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License (GPL) Version 2, available from the file LICENSE in the main
 * directory of this source tree, or the OpenIB.org BSD license below.  Any
 * code involving Linux software will require selection of the GNU General
 * Public License (GPL) Version 2.
 *
 * OPENIB.ORG BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <NDP.h>
#include "NDP_loadModel.h"
#include "NDP_init.h"
#include "serialFlashUtils.h"

void NDP_init(String model, byte tranceducer){ // Setting up NDP, transducer = 0 for Microphone and = 1 for sensor
  //String model = "google10.bin";                          // The factory loaded "google10.bin" has "Alexa play music" and "Alexa stop music"
  //String model = "ei_model.bin";                        // Use this line if using Edge Ipmulse model
  digitalWrite(PORSTB, LOW);                              // Resetting NDP started
  delay(100);
  digitalWrite(PORSTB, HIGH);                             // Resetting NDP finished
  digitalWrite(LED_RED, HIGH);                           // Setting Blue LED during NDP initialization to monitor status and debug
  SPI.begin();                                            // Setting up SPI bus for NDP
  SPI.beginTransaction(SPISettings(spiSpeedGeneral, MSBFIRST, SPI_MODE0));
  digitalWrite(TINYML_CS, HIGH);                          // Chip select for NDP
  NDP.spiTransfer(NULL, 0, 0x0, NULL, spiData, 1);        //Reading NDP chip ID to check SPI interface
  Serial.print("Reading NDP Chip ID (should be 0x20): "); //Displaying NDP chip ID
  Serial.println(spiData[0], HEX);
  if (spiData[0] == 0x20){
    Serial.println("NDP101 found");
    Serial.println("");
  }
  else{
    Serial.println("NDP101 NOT found.");
    Serial.println("Aborting - Going in infinite loop");
    while (1);
  }  
  if (SerialFlash.begin(FLASH_CS)){ dir();}               //Checking diractory structure in the serial flash
  Serial.println("");
  digitalWrite(LED_RED, LOW);                            // Turning off Blue LED indicates successful communication to NDP chip
  digitalWrite(LED_BLUE, HIGH);                           // Setting Red LED during NDP package loading. No SD card model loading expected.
  int loadModelFlag = loadModel(model);
  if (loadModelFlag==8){
    Serial.print("Model loaded from Serial Flash: ");
    Serial.print(model);
  }
  else{
    Serial.println("Could not load model from serial flash - Going in infinite loop");
    Serial.println("Maybe a SD card present during boot process with no Model file. Please remove SD during boot process.");
    while (1);
  }
  digitalWrite(LED_BLUE, LOW);                            // Turning off Red LED indicates clock is ok to NDP and it finished loading
  
  if (tranceducer == 0){
    digitalWrite(ENABLE_PDM, LOW);                          // Enable PDM clock with external buffer to select Microphone after the model is loaded to avoid contection, set to High earlier in the SAMD21_init
    indirectWrite(0x40011010,0x000000F0);                   // NDP <-> Sensor interface SSB/SCK/MOSI/MISO to Logic output to avoid floating gates
    indirectWrite(0x40011004,0x000000F0);                   // Above signals are set to logic high (easy to monitor)   
    }                     
  NDP.poll(); // clear any pending interrupts
}
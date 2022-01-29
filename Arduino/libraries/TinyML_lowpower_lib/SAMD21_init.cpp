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
#include <wiring_private.h>
#include "NDP_loadModel.h"
#include "SAMD21_init.h"

Uart Serial2(&sercom3, 7, 6, SERCOM_RX_PAD_3, UART_TX_PAD_2); // This sets TinyML boards Expansion board pins 6,7 for debug

void SERCOM3_Handler()                                        // This is needed for Serial2
{
    Serial2.IrqHandler();
}

void SAMD21_init(byte waitBeforeSerialPort){
  Serial.begin(115200);
  Serial2.begin(115200);
  pinPeripheral(6, PIO_SERCOM_ALT);                       //The 7th pin of TinyML board is setup for Serial2 debug. Please not 6 is Arduino pin mapping
  //pinPeripheral(7, PIO_SERCOM_ALT);                     // Since there is nothing expected from serial2 this pin is commented out
  pinMode(LED_RED, OUTPUT);                               //Red LED control of RGB LED
  pinMode(LED_BLUE, OUTPUT);                              //Green LED control of RGB LED
  pinMode(LED_GREEN, OUTPUT);                             //Blue LED control of RGB LED
  digitalWrite(LED_RED, HIGH);                            // A stuck RED LED indicates that program is waiting for Serial Port 
  if ( waitBeforeSerialPort)
      while (!Serial);                                      //Uncomment this line so that program waits for until Serial port is available for debugging
  digitalWrite(LED_RED, LOW);
  Serial.println("");
  Serial.println("                    Starting Syntiant TinyML");
  // ********** Setting SAMD21 pins for specific usage and avoiding floating nodes to save power *******
  pinMode(USER_SWITCH, INPUT_PULLUP);                     //Setting User Switch to high voltage so when it shorted to ground it can be detected
  pinMode(PORSTB, OUTPUT);                                //Reset pin for NDP
  pinMode(TINYML_CS, OUTPUT);                             //Chip select for NDP's SPI interface
  pinMode(ENABLE_PDM, OUTPUT);                            //Pin which controls microphone usage or sensor usage
  digitalWrite(ENABLE_PDM, HIGH);                         // Disable PDM clock with external buffer to avoid contention is model is sensor related
  pinMode(29, INPUT_PULLUP);                              //NDP MISO
  pinMode(10, INPUT_PULLUP);                              //Serial Flash MISO
  pinMode(30, INPUT_PULLUP);                              //SD SPI CS
  pinMode(30, INPUT_PULLUP);                              //SD SPI CS
  pinMode(15,OUTPUT);                                     //TinyML Expansion pin 5 is set as an output demonstrating GPIO responding to classifiers 
  pinMode(7, INPUT_PULLUP);                               //TinyML Expansion pin 6
  pinMode(1, INPUT_PULLUP);                               //TinyML Expansion pin 8
  pinMode(0, INPUT_PULLUP);                               //TinyML Expansion pin 9
  pinMode(4, INPUT_PULLUP);                               //unused GPIO 
  pinMode(5, INPUT_PULLUP);                               //unused GPIO
  pinMode(13, INPUT_PULLUP);                              //unused GPIO 
  pinMode(14, INPUT_PULLUP);                              //unused GPIO  
  pinPeripheral(OSC32K_OUT_PIN, PIO_AC_CLK);              // Route OSC32K to PA11 pin for NDP clock
}
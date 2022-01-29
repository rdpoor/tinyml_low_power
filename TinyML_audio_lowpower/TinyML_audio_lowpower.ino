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
#include <NDP_utils.h>
#include <Arduino.h>
#include "TinyML_init.h"
#include "NDP_loadModel.h"
#include "SAMD21_init.h"
#include "NDP_init.h"
#include "SAMD21_lowpower.h"

int match = 0; // This variable indicates which class has matched

static void wakeup_from_sleep(void) { //This routine enables functions disabled in prep_for_sleep() and polls NDP
  systick_enable();
  usb_serial_enable();
  adc_enable();
  match = NDP.poll();
}

void NDP_INT_service(){
  switch (match){
    case 0:
      break;
    case 1:
      digitalWrite(LED_BLUE, HIGH);                       // Setting up Blue LED
      Serial.println("Classifier 0 detected");            // printing on the native console
      delay(1000);                                        // Delay to keep the LED on for 1 second. Reduce it to reduce total power consumption
      digitalWrite(LED_BLUE, LOW);                        // Turning off the Blue LED
      break;
    case 2:
      digitalWrite(LED_GREEN, HIGH);
      Serial.println("Classifier 1 detected");
      delay(1000);
      digitalWrite(LED_GREEN, LOW);
      break;
    default:
      break;
  }
}

void setup(void) {
  SAMD21_init(0);                                          // Setting up SAMD21 
  NDP_init("google10.bin",0);                             // Setting up NDP "google10.bin" is factory set model for Alexa play music, "ei_model.bin" is Edge Impulse model, second parameter is 0 for Microphone and 1 for Sensor
  attachInterrupt(NDP_INT, wakeup_from_sleep, HIGH);      // Enabling interrupt based on NDP_INT signal and executing wakeup_from_sleep() routine
  NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;// Prevent the internal flash memory from powering down in sleep mode
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;                      // Select STANDBY for sleep mode
}

void loop() {
  //WARNING: The USB connection is disconnected. Double click on Reset pin to put the board in Bootloader mode for uploading next time.
  prep_for_sleep();                                       // Preparing for sleep
  __DSB();                                                // Ensure remaining memory accesses are complete
  __WFI();                                                // Enter sleep mode and Wait For Interrupt (WFI), When NDP_INT goes high, wakeup_from_sleep() runs through attachInterrupt
  NDP_INT_service();                                      // match variable is already set in the wakeup_from_sleep() routine. LEDs and GPIOs are toggled here.
}

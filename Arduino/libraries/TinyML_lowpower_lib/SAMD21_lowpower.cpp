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
#include "SAMD21_lowpower.h"

/* Enable and disable routines for ADC/System Timers/USB to save power etc 
* SPI interface, Serial interfaces etc are not disabled because the SPI code is embedded and used in multiple libraries.
* Further power saving is possible. */
void adc_enable(void) {}
void adc_disable(void) {
  // Disable ADC
  ADC->CTRLA.bit.ENABLE = 0; // Reset ADC
  ADC->CTRLA.bit.SWRST = 1;  // Reset ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1) {
    __asm("nop");
  }
}

void systick_enable(void) {
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // Enable SysTick interrupts
}
void systick_disable(void) {
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // Disable SysTick interrupts
}

void usb_serial_enable(void) {
  USBDevice.attach(); // Re-attach the native USB
}
void usb_serial_disable(void) {
  USBDevice.detach();
}

void prep_for_sleep(void) { //This routine disables certain functions before goimg to sleep 
  Serial.println(F("Entering always-on low power mode."));
  Serial.flush();
  adc_disable();
  usb_serial_disable();
  systick_disable();
}


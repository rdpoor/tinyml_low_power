/**
 * MIT License
 *
 * Copyright (c) 2022 R. Dunbar Poor
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <NDP.h>
#include <NDP_utils.h>
#include <Arduino.h>
#include "TinyML_init.h"
#include "NDP_init.h"
#include "NDP_loadModel.h"
#include "SAMD21_init.h"
#include "SAMD21_lowpower.h"

typedef enum {
  MATCH_NONE = 0,
  MATCH_PLAY_MUSIC,
  MATCH_STOP_MUSIC,
} classifier_match_t;

#define NDP_MICROPHONE 0
#define NDP_SENSOR 1

// Written at interrupt level (ndp_isr), read in service_ndp()
static volatile classifier_match_t s_match;

/**
 * @brief Called at interrupt level when the NDP asserts an interrupt.
 */
static void ndp_isr(void) { s_match = (classifier_match_t)NDP.poll(); }

void service_ndp() {
  switch (s_match) {
  case MATCH_NONE:
    break;

  case MATCH_PLAY_MUSIC:
    // 'Alexa, play music'
    digitalWrite(LED_BLUE, HIGH);
    Serial.println("Classifier 0 detected");
    delay(1000);
    digitalWrite(LED_BLUE, LOW);
    break;

  case MATCH_STOP_MUSIC:
    // 'Alexa, stop music'
    digitalWrite(LED_GREEN, HIGH);
    Serial.println("Classifier 1 detected");
    delay(1000);
    digitalWrite(LED_GREEN, LOW);
    break;

  default:
    break;
  }
}

/**
 * @brief One-time setup, called upon reboot.
 */
void setup(void) {
  // Initialize the SAMD21 host processor
  SAMD21_init(0);
  // load the the board with the model "google10.bin" it was shipped from the \
  // factory
  NDP_init("google10.bin", NDP_MICROPHONE);
  // The NDP101 will wake the SAMD21 upon detection
  attachInterrupt(NDP_INT, ndp_isr, HIGH);
  // Prevent the internal flash memory from powering down in sleep mode
  NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
  // Select STANDBY for sleep mode
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

/**
 * @brief main loop.
 *
 * This loop immediately puts the processor into low-power standby mode, then
 * waits for an interrupt from the NDP.  Upon receiving the interrupt, the
 * processor wakes, services the NDP request and goes back to sleep.
 *
 * NOTE: To conserve power, the SAMD21 disables the USB port when it sleeps
 * (which is most of the time).  When the USB port is disabled, any attempt to
 * upload a sketch will fail.
 *
 * In order to upload a sketch, double-click the reset button.  This will put
 * the SAMD21 into bootloader mode with the USB port enabled, allowing you to
 * upload a sketch.
 */
void loop() {
  // Put various peripheral modules into low power mode before entering standby.
  adc_disable();        // See SAMD21_lowpower.h
  usb_serial_disable(); // See note above about USB communications
  systick_disable();
  // Complete any memory operations and enter standby mode until an interrupt
  // is recieved from the NDP101
  __DSB();
  __WFI();
  // Arrive here after waking and having called ndp_isr().  Re-enable various
  // peripheral modules before servicing the NDP classifier data.
  systick_enable();
  usb_serial_enable();
  adc_enable();
  // process the classifier data from the NDP
  service_ndp();
  // loop (and return immediately to standby mode)
}

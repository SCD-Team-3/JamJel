/* jamjel.ino
 *
 * AUTHOR
 * Will Weaver
 * Computer Engineering Major
 * Mechanical Engineering Senior Capstone Design Team 3
 * Michigan Technological University
 *
 * SPONSOR
 * Mitch Baldwin
 * Stryker Instruments
 *
 * DESCRIPTION
 * Runs on a single Arduino Uno MCU to control the motion of the 3 piezoelectric
 * stacks in the Jamjel bone sculptor device.
 */

/******************************************************************************
 * INCLUDES                                                                   *
 ******************************************************************************/

#include <stdint.h>        // Provides types with definite sizes
#include <avr/io.h>        // Provides register aliases

#include "message.h"       // Provides interface and constants for receiving messages from the PC
#include "jamjel.h"        // Header file for this file
#include "TimerOne.h"      // Provides a convenient interface for accessing Timer 1

/******************************************************************************
 * CONSTANTS & GLOBALS                                                        *
 ******************************************************************************/

// Motion Pattern samples and statistics
mptrn_t* g_motion_pattern;

/******************************************************************************
 * FUNCTIONS                                                                  *
 ******************************************************************************/

// setSineFreq
// Modifies the frequency at which the position update routine is called based
// on the specified sine wave frequency.
// PARAMS:  new_freq - The new sine wave frequency
// RETURNS: void
void setSineFreq(uint16_t new_freq) {
  uint32_t sample_freq = new_freq * samples_per_period;
  
  // Ceiling function is applied to integer division
  // Ensures that the effective freq is always less than or equal to set freq
  uint32_t sample_prd_ms = 1000000 / sample_freq;
  if (1000000 % sample_freq != 0)
    sample_prd_ms++;
  
  // Update the rate at which the ISR is called
  Timer1.setPeriod(sample_prd_ms);
}

/******************************************************************************
 * INTERRUPT SERVICE ROUTINES                                                 *
 ******************************************************************************/

// Timer1 ISR
// Generates a new sample for each piezoelectric stack and sends it to the
// appropriate pin.
// PARAMS:  void
// RETURNS: void
ISR(TIMER1_OVF_vect) {
  
  // Write the next sequential values to each of the relevant pins
  PIEZOSTACK_A_OCR = (*piezostack_a_samples)[current_sample_index];
  PIEZOSTACK_B_OCR = (*piezostack_b_samples)[current_sample_index];
  PIEZOSTACK_C_OCR = (*piezostack_c_samples)[current_sample_index];
  
  // Increment sample index for next time, wrapping around if necessary
  current_sample_index = (current_sample_index + 1) % samples_per_period;
}

/******************************************************************************
 * SETUP                                                                      *
 ******************************************************************************/

// setup
// Is called automatically by the arduino at startup. Runs once to initialize
// the controller.
// PARAMS:  void
// RETURNS: void
void setup() {
  
  // Set all PWM pins to output
  pinMode(PIEZOSTACK_A_PIN, OUTPUT);
  pinMode(PIEZOSTACK_B_PIN, OUTPUT);
  pinMode(PIEZOSTACK_C_PIN, OUTPUT);
  
  // Configure relevant timers for Fast PWM mode with no prescaling
  // See #defines in this file for more information
  PIEZOSTACK_A_TCCRA = PIEZOSTACK_A_TCCRA_SETTINGS;
  PIEZOSTACK_A_TCCRB = PIEZOSTACK_A_TCCRB_SETTINGS;
  PIEZOSTACK_B_TCCRA = PIEZOSTACK_B_TCCRA_SETTINGS;
  PIEZOSTACK_B_TCCRB = PIEZOSTACK_B_TCCRB_SETTINGS;
  PIEZOSTACK_C_TCCRA = PIEZOSTACK_C_TCCRA_SETTINGS;
  PIEZOSTACK_C_TCCRB = PIEZOSTACK_C_TCCRB_SETTINGS;
  
  // Initialize Timer1 for the update interrupts and assign its ISR
  Timer1.initialize();
  Timer1.attachInterrupt(updatePiezoPositions, 1000000);  // Arbitrary frequency because it will be changed
  
  // Initialize proper frequency
  setSineFreq(DEFAULT_SINE_FREQ);
  
  // Get a pointer to the motion pattern structure
  g_motion_pattern = getPatternPtr();
}
 
/******************************************************************************
 * LOOP                                                                       *
 ******************************************************************************/

// loop
// Is called continuously automatically by the Arduino. Is empty because all
// processing in this application is done by the ISRs.
// PARAMS:  void
// RETURNS: void
void loop() {
  // All processing is handled by ISRs
}

/******************************************************************************
 * END OF FILE                                                                *
 ******************************************************************************/

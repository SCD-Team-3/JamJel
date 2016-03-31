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
 
#ifndef JAMJEL_H
#define JAMJEL_H
 
/******************************************************************************
 * INCLUDES                                                                   *
 ******************************************************************************/
 
/******************************************************************************
 * CONSTANTS                                                                  *
 ******************************************************************************/
 
#define DEFAULT_SINE_FREQ (100)    // The default frequency of the sine wave output
#define SAMPLES_PER_SINE  (256)    // The number of samples in a single sine wave period

// The Arduino hardwires the PWM outputs to certain physical pins:
// Timer0: Pins 5 and 6
// Timer1: Pins 9 and 10
// Timer2: Pins 3 and 11

// Timer1 is used to generate interrupts at the sample rate, so cannot be used
// for the PWM signals. Therefore, pins 3, 5, and 6 are used from Timer0 and
// Timer2 for the PWM signals.

// See pages 104-110, 153-159 in the ATmega328 datasheet for register details on
// Timer0 and Timer2.

// PIEZOSTACK_x_PIN            - Output pin number
// PIEZOSTACK_x_TCCRy          - Timer control register y
// PIEZOSTACK_x_TCCRy_SETTINGS - Value to be stored in timer control register y that configures the relevant settings
// PIEZOSTACK_x_OCR            - Register to which the duty cycle is stored (out of 255)

#define PIEZOSTACK_A_PIN            (3)
#define PIEZOSTACK_A_TCCRA          TCCR2A
#define PIEZOSTACK_A_TCCRB          TCCR2B
#define PIEZOSTACK_A_TCCRA_SETTINGS (0x23)
#define PIEZOSTACK_A_TCCRB_SETTINGS (0x01)
#define PIEZOSTACK_A_OCR            OCR2B

#define PIEZOSTACK_B_PIN            (5)
#define PIEZOSTACK_B_TCCRA          TCCR0A
#define PIEZOSTACK_B_TCCRB          TCCR0B
#define PIEZOSTACK_B_TCCRA_SETTINGS (0xA3)
#define PIEZOSTACK_B_TCCRB_SETTINGS (0x01)
#define PIEZOSTACK_B_OCR            OCR0A

#define PIEZOSTACK_C_PIN            (6)
#define PIEZOSTACK_C_TCCRA          TCCR0A
#define PIEZOSTACK_C_TCCRB          TCCR0B
#define PIEZOSTACK_C_TCCRA_SETTINGS (0xA3)
#define PIEZOSTACK_C_TCCRB_SETTINGS (0x01)
#define PIEZOSTACK_C_OCR            OCR0B

/******************************************************************************
 * MACROS                                                                     *
 ******************************************************************************/

// Useful macros
#define CLR_BYTE_MSK(t, n) (~(((t) 0xFF) << (n)))
#define CLR_BYTE(v, t, n) ((v) & CLR_BYTE_MSK((t), (n)))
 
/******************************************************************************
 * FUNCTION DECLARATIONS                                                      *
 ******************************************************************************/
 
void setSineFreq(uint16_t new_freq);

void updatePiezoPositions()
 
void setup();

void loop();

#endif // JAMJEL_H
 
/******************************************************************************
 * END OF FILE                                                                *
 ******************************************************************************/

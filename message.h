/* message.h
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
 * Provides definitions and interface for messages from the PC
 */
 
#ifndef MESSAGE_H
#define MESSAGE_H

/******************************************************************************
 * DEFINES                                                                    *
 ******************************************************************************/

// Message receiving status
// Message short format:
//   Message Type
//   Data Byte
//   Data Byte
// Message long format:
//   Message Type
//   Length (lower bits)
//   Length (upper bits)
//   Data Bytes
//   Delimiter
//   Data Bytes
//   Delimiter
//   Data Bytes
//   Delimiter

// The number of data bytes in a short message
#define MSG_SHORT_BYTES     (2)

// Message type codes
#define MSG_TYPE_PTRN   (0x01)
#define MSG_TYPE_FREQ   (0x02)
#define MSG_TYPE_AMP    (0x03)

// Delimiter code
#define MSG_DELIMITER   (0x55)

// Message type codes
#define MSGSTAT_TYPE_NONE   (0x00)
#define MSGSTAT_TYPE_PTRN   (MSG_TYPE_PTRN)
#define MSGSTAT_TYPE_FREQ   (MSG_TYPE_FREQ)
#define MSGSTAT_TYPE_AMP    (MSG_TYPE_AMP)

// Piezostack codes
#define MSGSTAT_PSTACK_ALL  (0x00)
#define MSGSTAT_PSTACK_A    (0x01)
#define MSGSTAT_PSTACK_B    (0x02)
#define MSGSTAT_PSTACK_C    (0x03)

// Byte type codes
#define MSGSTAT_BYTE_TYPE   (0x00)
#define MSGSTAT_BYTE_DATA   (0x01)
#define MSGSTAT_BYTE_LEN    (0x02)
#define MSGSTAT_BYTE_DEL    (0x03)

// Error codes
#define MSGSTAT_ERR_NONE    (0x00)
#define MSGSTAT_ERROR       (0x01)
#define MSGSTAT_ERR_TYPE    (0x02)
#define MSGSTAT_ERR_MEM     (0x04)
#define MSGSTAT_ERR_DEL     (0x08)

/******************************************************************************
 * TYPEDEFS                                                                   *
 ******************************************************************************/

typedef struct {
  uint16_t ptrn_frequency;      // Number of times all stored samples are iterated per second
  uint16_t amplitude;           // Scaling factor for motion pattern (0 = 0%, 65535 = 100%)
  uint16_t samples_per_period;  // Number of samples in each period
  uint8_t* pstack_a_samples;    // Numerical approximation of motion pattern for stack A
  uint8_t* pstack_b_samples;    // Numerical approximation of motion pattern for stack B
  uint8_t* pstack_c_samples;    // Numerical approximation of motion pattern for stack C
  uint16_t cur_sample_index;    // The index of the current samples on the output pins
} mptrn_t;

typedef struct {
  uint8_t msg_type;     // Type of the message currently being received
  uint8_t pstack;       // Piezostack # being modified (only valid for new pattern msgs)
  uint8_t byte_type;    // Type of the expected byte
  uint16_t byte_index;  // Index # of the expected byte
  uint8_t error;        // Indicates the presence of an error
} status_t;

/******************************************************************************
 * PUBLIC FUNCTION DECLARATIONS                                               *
 ******************************************************************************/

/* getPatternPtr
 * Author: Will Weaver
 * Func:   Returns a pointer to the paramters for the current motion pattern
 */
mptrn_t* inline getPatternPtr();

/* receiveByte
 * Author: Will Weaver
 * Func:   Processes a new byte received on the serial port
 * Params: new_byte : The newly received byte
 */
void inline receiveByte(uint8_t new_byte);

/******************************************************************************
 * PRIVATE FUNCTION DECLARATIONS                                              *
 ******************************************************************************/

void inline handleByte_TypeFreq(uint8_t new_byte);
void inline handleByte_TypeAmp(uint8_t new_byte);
void inline handleByte_TypeNone(uint8_t type_byte);
void inline handleByte_TypePtrn(uint8_t new_byte);
void inline handleByte_TypePtrn_ByteLen(uint8_t new_byte);
void inline handleByte_TypePtrn_ByteData(uint8_t new_byte);
void inline handleByte_TypePtrn_ByteDel(uint16_t new_byte);
void inline endMessage();
uint16_t inline replaceByte(uint16_t prev_val, uint8_t new_byte, uint8_t byte_index);

#endif // MESSAGE_H

/******************************************************************************
 * END OF FILE                                                                *
 ******************************************************************************/

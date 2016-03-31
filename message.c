/* message.c
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
 
/******************************************************************************
 * INCLUDES                                                                   *
 ******************************************************************************/
 
#include "message.h"
 
/******************************************************************************
 * GLOBALS                                                                    *
 ******************************************************************************/

status_t g_message_status;  // The current message reception status
mptrn_t g_cur_ptrn;           // Parameters for the current motion pattern
 
/******************************************************************************
 * FUNCTION DEFINITIONS                                                       *
 ******************************************************************************/

/* getPatternPtr
 * Author: Will Weaver
 * Func:   Returns a pointer to the paramters for the current motion pattern
 */
mptrn_t* inline getPatternPtr() {
  return &g_cur_ptrn;
}

/* receiveByte
 * Author: Will Weaver
 * Func:   Processes a new byte received on the serial port
 * Params: new_byte : The newly received byte
 */
void inline receiveByte(uint8_t new_byte) {
  
  // Do different things based on message type
  switch (g_message_status.msg_type) {
    case MSGSTAT_TYPE_NONE:
      handleByte_TypeNone(new_byte);
      break;
      
    case MSGSTAT_TYPE_PTRN:
      handleByte_TypePtrn(new_byte);
      break;
    
    case MSGSTAT_TYPE_FREQ:
      handleByte_TypeFreq(new_byte);
      break;
      
    case MSGSTAT_TYPE_AMP:
      handleByte_TypeAmp(new_byte);
      break;
  }
}

/* handleByte_TypeFreq
 * Author: Will Weaver
 * Func:   Processes a new frequency byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypeFreq(uint8_t new_byte) {
  
  // Process and insert the new data byte
  g_cur_ptrn.ptrn_frequency = replaceByte(g_cur_ptrn.ptrn_frequency, new_byte, g_message_status.byte_index);
  
  // End the message if all data bytes have been received
  if (g_message_status.byte_index >= MSG_SHORT_BYTES)
    endMessage();
}

/* handleByte_TypeAmp
 * Author: Will Weaver
 * Func:   Processes a new amplitude byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypeAmp(uint8_t new_byte) {
  
  // Process and insert the new data byte
  g_cur_ptrn.amplitude = replaceByte(g_cur_ptrn.amplitude, new_byte, g_message_status.byte_index);
  
  // End the message if all data bytes have been received
  if (g_message_status.byte_index >= MSG_SHORT_BYTES)
    endMessage();
}

/* handleByte_TypeNone
 * Author: Will Weaver
 * Func:   Processes a new type byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypeNone(uint8_t type_byte) {
  
  // Expecting a valid type byte
  switch (type_byte) {
    case MSG_TYPE_PTRN:
      g_message_status.msg_type = MSGSTAT_TYPE_PTRN;
      g_message_status.byte_type = MSGSTAT_BYTE_LEN;
      break;
      
    case MSG_TYPE_FREQ:
      g_message_status.msg_type = MSGSTAT_TYPE_FREQ;
      g_message_status.byte_type = MSGSTAT_BYTE_DATA;
      break;
      
    case MSG_TYPE_AMP:
      g_message_status.msg_type = MSGSTAT_TYPE_AMP;
      g_message_status.byte_type = MSGSTAT_BYTE_DATA;
      break;
      
    default:
      g_message_status.msg_type = MSGSTAT_TYPE_NONE;
      g_message_status.byte_type = MSGSTAT_BYTE_NONE;
      g_message_status.error |= MSGSTAT_ERR_TYPE;
      break;
  }
  
  // Set up for incoming message
  g_message_status.pstack = MSGSTAT_PSTACK_ALL;
  g_message_status.byte_index = 0;
}

/* handleByte_TypePtrn
 * Author: Will Weaver
 * Func:   Processes a new pattern byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypePtrn(uint8_t new_byte) {
  switch (g_message_status.byte_type) {
    case MSGSTAT_BYTE_LEN:
      handleByte_TypePtrn_ByteLen(new_byte);
      break;
      
    case MSGSTAT_BYTE_DATA:
      handleByte_TypePtrn_ByteData(new_byte);
      break;
      
    case MSGSTAT_BYTE_DEL:
      handleByte_TypePtrn_ByteDel(new_byte);
      break;
  }
}

/* handleByte_TypePtrn_ByteData
 * Author: Will Weaver
 * Func:   Processes a new pattern data byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypePtrn_ByteLen(uint8_t new_byte) {
  
  // Store received byte
  g_cur_ptrn.samples_per_period = replaceByte(g_cur_ptrn.samples_per_period, new_byte, g_message_status.byte_index);
  g_message_status.byte_index++;
  
  // Complete length received
  if (g_message_status.byte_index >= MSG_SHORT_BYTES) {
    g_message_status.byte_type = MSGSTAT_BYTE_DATA;
    g_message_status.pstack = MSGSTAT_PSTACK_A;
    g_message_status.byte_index = 0;
    
    // Free previous samples
    free(g_cur_ptrn.pstack_a_samples);
    free(g_cur_ptrn.pstacK_b_samples);
    free(g_cur_ptrn.pstack_c_samples);
    
    // Allocate space for new samples
    g_cur_ptrn.pstack_a_samples = malloc(g_cur_ptrn.samples_per_period);
    g_cur_ptrn.pstack_b_samples = malloc(g_cur_ptrn.samples_per_period);
    g_cur_ptrn.pstack_c_samples = malloc(g_cur_ptrn.samples_per_period);
    
    // Record an error if memory allocation fails
    if (!g_cur_ptrn.pstack_a_samples || !g_cur_ptrn.pstack_b_samples || !g_cur_ptrn.pstack_c_samples) {
      g_message_status.error |= MSGSTAT_ERR_MEM;
      free(g_cur_ptrn.pstack_a_samples);
      free(g_cur_ptrn.pstack_b_samples);
      free(g_cur_ptrn.pstack_c_samples);
    }
    
    // Set waveform statistics
    g_cur_ptrn.samples_per_period = len;
    g_cur_ptrn.current_sample_index = 0;
  }
}

/* handleByte_TypePtrn_ByteData
 * Author: Will Weaver
 * Func:   Processes a new pattern data byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypePtrn_ByteData(uint8_t new_byte) {
  if (!g_message_status.error) {
    
    // Store the relevant byte
    switch(g_message_status.pstack) {
      case MSGSTAT_PSTACK_A:
        g_cur_ptrn.pstack_a_samples[g_message_status.byte_index++] = new_byte;
        break;
      case MSGSTAT_PSTACK_B:
        g_cur_ptrn.pstack_b_samples[g_message_status.byte_index++] = new_byte;
        break;
      case MSGSTAT_PSTACK_C:
        g_cur_ptrn.pstack_c_samples[g_message_status.byte_index++] = new_byte;
        break;
    }
  } else {
    // If there's an error, we still need to keep track of where we're at
    g_message_status.byte_index++;
  }
  
  if (g_message_status.byte_index >= g_cur_ptrn.samples_per_period)
    g_message_status.byte_type = MSGSTAT_BYTE_DEL;
}

/* handleByte_TypePtrn_ByteDel
 * Author: Will Weaver
 * Func:   Processes a new pattern delimiter byte
 * Params: new_byte : The newly received byte
 */
void inline handleByte_TypePtrn_ByteDel(uint16_t new_byte) {
  if (new_byte == MSG_DELIMITER) {
    switch (g_message_status.pstack) {
      case MSGSTAT_PSTACK_A:
        g_message_status.pstack = MSGSTAT_PSTACK_B;
        break;
        
      case MSGSTAT_PSTACK_B:
        g_message_status.pstack = MSGSTAT_PSTACK_C;
        break;
        
      case MSGSTAT_PSTACK_C:
        endMessage();
        break;
    }
  } else {
    g_message_status.error |= MSGSTAT_ERR_DEL;
  }
  
  g_message_status.byte_index = 0;
}

/* endMessage
 * Author: Will Weaver
 * Func:   Marks the end of a message in the status variables
 */
void inline endMessage() {
  g_message_status.msg_type = MSGSTAT_TYPE_NONE;
  g_message_status.byte_type = MSGSTAT_BYTE_NONE;
  g_message_status.error = MSGSTAT_ERR_NONE;
  g_message_status.byte_index = 0;
  g_message_status.pstack = MSGSTAT_PSTACK_ALL;
}

/* replaceByte
 * Author: Will Weaver
 * Func:   Replaces a byte in a larger variable
 * Params: prev_val   : The larger variable in which the byte is being replaced
 *         new_byte   : The byte which is being inserted into the larger byte
 *         byte_index : The index of the new byte (LSB = 0)
 * Return: The new value of the larger variable
 */
uint16_t inline replaceByte(uint16_t prev_val, uint8_t new_byte, uint8_t byte_index) {
  uint8_t shift = byte_index << 3;
  prev_val &= ~(0xFF << shift);
  prev_val |= new_byte << shift;
  return prev_val;
}
 
/******************************************************************************
 * END OF FILE                                                                *
 ******************************************************************************/

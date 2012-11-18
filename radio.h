#include "efm32_int.h"

#define BANDW_1MB (0x0)
#define BANDW_2MB (0x8)
#define BANDW_250KB (0x20)

#define POW_MAX (0x6)
#define POW_HIGH (0x4)
#define POW_MEDIUM (0x2)
#define POW_LOW (0x0)

// Use this to handle radio itnerrupts
void radio_handleInterrupt(void);

// Call this once in every interation of the main loop. Services the radio.
void radio_loop(void);

// Setup the radio at a certain channel (1 - 90),
// a certain bandwidth from BANDW_1MB, BANDW_2MB or BANDW_250KB
// and with certain TX power POW_MAX, POW_HIGH, POW_MEDIUM or POW_LOW
// if you would like to do hardware acking, set hardwareACK to 1 otherwise set it to 0
void radio_setup(uint8_t channel, uint8_t bandwidth, uint8_t power, uint8_t hardwareACK);

// Send a 32 byte packet. data is pointer to the first element of the packet
void radio_sendPacket32(uint8_t * data);

// Receive a 32 byte packet. data is pointer to the first element of the packet
// The function will return 1 if data was received and will fill up the first 32 bytes of the array with it
// The function will return 0 if no data was received and will leave the array untouched
uint8_t radio_receivePacket32(uint8_t * data);

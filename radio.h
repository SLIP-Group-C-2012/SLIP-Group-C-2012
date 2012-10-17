#define BANDW_1MB (0x0)
#define BANDW_2MB (0x8)
#define BANDW_250KB (0x20)

#define POW_MAX (0x6)
#define POW_HIGH (0x4)
#define POW_MEDIUM (0x2)
#define POW_LOW (0x0)

void radio_handleInterrupt(void);

void radio_loop(void);

void radio_setup(uint8_t channel, uint8_t bandwidth, uint8_t power);

void radio_sendPacket(uint8_t * data, uint8_t size);

uint8_t radio_receivePacket(uint8_t * data, uint8_t size);

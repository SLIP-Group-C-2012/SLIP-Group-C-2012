void radio_handleInterrupt(void);

void radio_setup(uint8_t channel, uint8_t speed);

void radio_sendPacket(uint8_t * data, uint8_t size);

uint8_t radio_receivePacket(uint8_t * data, uint8_t size);

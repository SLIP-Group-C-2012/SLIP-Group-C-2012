void protocol_send(uint8_t* buff, uint8_t dest);
int protocol_recive(uint8_t* buff);
int protocol_loop();
uint8_t protocol_getaddr();
void protocol_updateaddr(uint8_t address);
void protocol_printalladdr(void);

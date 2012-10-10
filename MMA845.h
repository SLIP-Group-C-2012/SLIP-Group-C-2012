#ifndef __MMA845_H
#define __MMA845_H

int MMA845X_RegisterGet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
void MMARegWrite(uint8_t reg, uint8_t val);
uint8_t MMARegRead(uint8_t reg);
void MMARegReadN(uint8_t reg1, uint8_t n, uint8_t *array);
void MMAStandby(void);
void MMAActive(void);
void MMAInit(void);

#endif // __MMA845_H

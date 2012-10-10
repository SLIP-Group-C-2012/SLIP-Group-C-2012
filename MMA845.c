#include <stddef.h>
#include "efm32.h"
#include "NRF24.h"
#include "nRF24L01.h"
#include "efm32_usart.h"
#include "efm32_emu.h"
#include "efm32_i2c.h"
#include "config.h"
#include "MMA845XQ.h"

uint8_t rcvr_spi(void)
{
  int i;
  //while (!(USART1->STATUS & USART_STATUS_TXBL)) ;
  //USART1->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
  USART0->CTRL &= ~_USART_CTRL_TXBIL_MASK;
  USART0->CTRL |= USART_CTRL_TXBIL_HALFFULL;
  for (i = 0; i < 8; i++) {
    while(!(USART0->STATUS & USART_STATUS_TXBL)) ;
  	USART0->TXDATA = 0x07;
  }
  while (!(USART0->STATUS & USART_STATUS_TXC)) ;
  return USART_Rx(USART0);
}


/***************************************************************************//**
 * @brief
 *   Perform I2C transfer.
 *
 * @details
 *   This driver only supports master mode, single bus-master. It does not
 *   return until the transfer is complete, polling for completion.
 *
 * @param[in] seq
 *   Pointer to sequence structure defining the I2C transfer to take place. The
 *   referenced structure must exist until the transfer has fully completed.
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2CDRV_Transfer(I2C_TransferSeq_TypeDef *seq)
{
  I2C_TransferReturn_TypeDef ret;

  /* Do a polled transfer */
  ret = I2C_TransferInit(I2C0, seq);
  while (ret == i2cTransferInProgress)
  {
    ret = I2C_Transfer(I2C0);
  }

  return(ret);
}

int MMA845X_RegisterGet(I2C_TypeDef *i2c,
                         uint8_t addr,
                         uint8_t reg,
                         uint8_t *buf,
						 uint8_t len)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t regid[1];
  //uint8_t data[1];

  seq.addr = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to be read */
  regid[0] = reg;
  seq.buf[0].data = regid;
  seq.buf[0].len = 1;
  seq.buf[1].data = buf;
  seq.buf[1].len = len;
 
  ret = I2CDRV_Transfer(&seq);
  if (ret != i2cTransferDone)
  {
    return((int)ret);
  }

  return(0);
}

void MMARegWrite(uint8_t reg, uint8_t val) {
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t regid[1];
  //uint8_t data[1];

  seq.addr = MMA8451Q_ADDR;
  seq.flags = I2C_FLAG_WRITE_WRITE;
  /* Select register to be read */
  regid[0] = reg;
  seq.buf[0].data = regid;
  seq.buf[0].len = 1;
  seq.buf[1].data = &val;
  seq.buf[1].len = 1;
 
  ret = I2CDRV_Transfer(&seq);
}

uint8_t MMARegRead(uint8_t reg) {
  
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t regid[1];
  uint8_t data[1];

  seq.addr = MMA8451Q_ADDR;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to be read */
  regid[0] = reg;
  seq.buf[0].data = regid;
  seq.buf[0].len = 1;
  seq.buf[1].data = data;
  seq.buf[1].len = 1;
 
  ret = I2CDRV_Transfer(&seq);
  if (ret != i2cTransferDone)
  {
    return 0;
  }

  return data[0];
}

void MMARegReadN(uint8_t reg1, uint8_t n, uint8_t *array)
{
	MMA845X_RegisterGet(I2C0, MMA8451Q_ADDR, reg1, array, n);
}

void MMAStandby(void) {
  uint8_t r;
  r = MMARegRead(CTRL_REG1);
  MMARegWrite(CTRL_REG1, r & ~ACTIVE_MASK);
}
void MMAActive(void) {
  uint8_t r;
  r = MMARegRead(CTRL_REG1);
  MMARegWrite(CTRL_REG1, r | ACTIVE_MASK);
}

void MMAInit(void) {
  MMAStandby();
  MMARegWrite(XYZ_DATA_CFG_REG, ((MMARegRead(XYZ_DATA_CFG_REG) & ~FS_MASK) | FULL_SCALE_2G)); //2G Fullscale
  MMARegWrite(CTRL_REG4, 0);
  //MMARegWrite(CTRL_REG4, INT_EN_DRDY_MASK);
  //MMARegWrite(0x2E, INT_EN_DRDY_MASK); //Set the interrupt to route to INT1
  MMARegWrite(CTRL_REG1, (MMARegRead(CTRL_REG1) & ~DR_MASK) | DATA_RATE_80MS); // Active ODR
  MMARegWrite(CTRL_REG1, (MMARegRead(CTRL_REG1) & ~FREAD_MASK)); // 14-bit reads
  MMARegWrite(CTRL_REG1, (MMARegRead(CTRL_REG1) | LNOISE_MASK)); // Low Noise Mode
  MMARegWrite(CTRL_REG2, (MMARegRead(CTRL_REG2) & ~MODS_MASK) | 0x00); // Oversampling Mode in Active
  MMAActive();  
}              


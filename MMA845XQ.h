#ifndef __MMA845XQ_H
#define __MMA845XQ_H

#include "efm32.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MMA8451Q_ADDR 0x38

/*
**  STATUS Registers
*/
#define STATUS_00_REG         0x00
//
#define ZYXOW_MASK            0x80
#define ZOW_MASK              0x40
#define YOR_MASK              0x20
#define XOR_MASK              0x10
#define ZYXDR_MASK            0x08
#define ZDR_MASK              0x04
#define YDR_MASK              0x02
#define XDR_MASK              0x01

/*
**  XYZ Data Registers
*/
#define OUT_X_MSB_REG         0x01
#define OUT_X_LSB_REG         0x02
#define OUT_Y_MSB_REG         0x03
#define OUT_Y_LSB_REG         0x04
#define OUT_Z_MSB_REG         0x05
#define OUT_Z_LSB_REG         0x06


#define XYZ_DATA_CFG_REG      0x0E
//
#define HPF_OUT_MASK          0x10
#define FS1_MASK              0x02
#define FS0_MASK              0x01
#define FS_MASK               0x03

#define FULL_SCALE_8G         FS1_MASK
#define FULL_SCALE_4G         FS0_MASK
#define FULL_SCALE_2G         0x00

/*
**  SYSMOD System Mode Register
*/
#define SYSMOD_REG            0x0B
//
#define FGERR_MASK            0x80
#define FGT_4MASK             0x40
#define FGT_3MASK             0x20
#define FGT_2MASK             0x10
#define FGT_1MASK             0x08
#define FGT_0MASK             0x04
#define FGT_MASK              0x7C
#define SYSMOD1_MASK          0x02
#define SYSMOD0_MASK          0x01
#define SYSMOD_MASK           0x03


/*
**  ASLP_COUNT Auto Sleep Inactivity Timer Register
*/
#define ASLP_COUNT_REG        0x29

/*
**  Control Register 1
*/

#define CTRL_REG1             0x2A
#define ASLP_RATE1_MASK       0x80
#define ASLP_RATE0_MASK       0x40
#define DR2_MASK              0x20
#define DR1_MASK              0x10
#define DR0_MASK              0x08
#define LNOISE_MASK           0x04
#define FREAD_MASK            0x02
#define ACTIVE_MASK           0x01
#define ASLP_RATE_MASK        0xC0
#define DR_MASK               0x38
//                      
#define ASLP_RATE_20MS        0x00
#define ASLP_RATE_80MS        ASLP_RATE0_MASK
#define ASLP_RATE_160MS       ASLP_RATE1_MASK
#define ASLP_RATE_640MS       ASLP_RATE1_MASK+ASLP_RATE0_MASK
//
#define DATA_RATE_1250US      0x00
#define DATA_RATE_2500US      DR0_MASK
#define DATA_RATE_5MS         DR1_MASK
#define DATA_RATE_10MS        DR1_MASK+DR0_MASK
#define DATA_RATE_20MS        DR2_MASK
#define DATA_RATE_80MS        DR2_MASK+DR0_MASK
#define DATA_RATE_160MS       DR2_MASK+DR1_MASK
#define DATA_RATE_640MS       DR2_MASK+DR1_MASK+DR0_MASK

/*
**  CTRL_REG2 System Control 2 Register
*/
#define CTRL_REG2             0x2B
//
#define ST_MASK               0x80
#define BOOT_MASK             0x40
#define SMODS1_MASK           0x20
#define SMODS0_MASK           0x10
#define SLPE_MASK             0x04
#define MODS1_MASK            0x02
#define MODS0_MASK            0x01
#define SMODS_MASK            0x18
#define MODS_MASK             0x03

/*
**  CTRL_REG3 Interrupt Control Register
*/
#define CTRL_REG3             0x2C
//
#define FIFO_GATE_MASK        0x80
#define WAKE_TRANS_MASK       0x40
#define WAKE_LNDPRT_MASK      0x20
#define WAKE_PULSE_MASK       0x10
#define WAKE_FF_MT_1_MASK     0x08
#define WAKE_FF_MT_2_MASK     0x04
#define IPOL_MASK             0x02
#define PP_OD_MASK            0x01

/*
**  CTRL_REG4 Interrupt Enable Register
*/
#define CTRL_REG4             0x2D
//
//
#define INT_EN_ASLP_MASK      0x80
#define INT_EN_FIFO_MASK      0x40
#define INT_EN_TRANS_MASK     0x20
#define INT_EN_LNDPRT_MASK    0x10
#define INT_EN_PULSE_MASK     0x08
#define INT_EN_FF_MT_1_MASK   0x04
#define INT_EN_FF_MT_2_MASK   0x02
#define INT_EN_DRDY_MASK      0x01

/*
**  CTRL_REG5 Interrupt Configuration Register
*/
#define CTRL_REG5             0x2E
//
//
#define INT_CFG_ASLP_MASK     0x80
#define INT_CFG_FIFO_MASK     0x40
#define INT_CFG_TRANS_MASK    0x20
#define INT_CFG_LNDPRT_MASK   0x10
#define INT_CFG_PULSE_MASK    0x08
#define INT_CFG_FF_MT_1_MASK  0x04
#define INT_CFG_FF_MT_2_MASK  0x02
#define INT_CFG_DRDY_MASK     0x01

/*
**  FF_MT_CFG Freefall and Motion Configuration Registers
*/
#define FF_MT_CFG_1_REG       0x15
#define FF_MT_CFG_2_REG       0x19
//
#define ELE_MASK              0x80
#define OAE_MASK              0x40
#define ZEFE_MASK             0x20
#define YEFE_MASK             0x10
#define XEFE_MASK             0x08

/*
**  FF_MT_SRC Freefall and Motion Source Registers
*/
#define FF_MT_SRC_1_REG       0x16
#define FF_MT_SRC_2_REG       0x1A
//
#define EA_MASK               0x80
#define ZHE_MASK              0x20
#define ZHP_MASK              0x10
#define YHE_MASK              0x08
#define YHP_MASK              0x04
#define XHE_MASK              0x02
#define XHP_MASK              0x01

/*
**  FF_MT_THS Freefall and Motion Threshold Registers
**  TRANSIENT_THS Transient Threshold Register
*/
#define FF_MT_THS_1_REG       0x17
#define FF_MT_THS_2_REG       0x1B
#define TRANSIENT_THS_REG     0x1F
//
#define DBCNTM_MASK           0x80
#define THS6_MASK             0x40
#define THS5_MASK             0x20
#define THS4_MASK             0x10
#define THS3_MASK             0x08
#define THS2_MASK             0x04
#define TXS1_MASK             0x02
#define THS0_MASK             0x01
#define THS_MASK              0x7F

/*
**  FF_MT_COUNT Freefall Motion Count Registers
*/
#define FF_MT_COUNT_1_REG     0x18
#define FF_MT_COUNT_2_REG     0x1C

/*
**  TRANSIENT_CFG Transient Configuration Register
*/
#define TRANSIENT_CFG_REG     0x1D
//
#define TELE_MASK             0x10
#define ZTEFE_MASK            0x08
#define YTEFE_MASK            0x04
#define XTEFE_MASK            0x02
#define HPF_BYP_MASK          0x01

#define WHO_AM_I_REG 0x0D
#define INT_SOURCE 0x0C

void MMAStandby(void);
void MMAActive(void);
void MMAInit(void);

void MMARegWrite(uint8_t reg, uint8_t val);
uint8_t MMARegRead(uint8_t reg);
//void MMARegWriteN(uint8_t reg1, uint8_t n, uint8_t *array);
void MMARegReadN(uint8_t reg1, uint8_t n, uint8_t *array);

//void MMA_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif

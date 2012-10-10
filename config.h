#ifndef __CONFIG_H
#define PROSP_H
#define NODE_ID 1
#define NODE_CH 2

// #define DEMO_MODE

#ifdef RESP_DEVICE
#define RESP_ACCEL
#define MMA_INT_PIN 2
#define MMA_INT_PORT 0
#elif defined RESP_BLACK
#define RESP_ACCEL
#define MMA_INT_PIN 2
#define MMA_INT_PORT 0
#elif defined RESP_DEVKIT
#define RESP_ACCEL
#define MMA_INT_PIN 6
#define MMA_INT_PORT 3
#elif defined RESP_PRESSURE
#define MMA_INT_PIN 2
#define MMA_INT_PORT 0
#elif defined PROSP_H
#define MMA_INT_PIN 15
#define MMA_INT_PORT 0
#define RXEN_PIN 2
#define RXEN_PORT 0
#endif // RESP_DEVICE

#define RGB_PWM_TIMER_TOP 1024

void LETIMER_Setup(void);

void uart_init(USART_TypeDef *uart);

void IO_Init(void);

void InitRGBLEDPWM(void);

#endif // __CONFIG_H

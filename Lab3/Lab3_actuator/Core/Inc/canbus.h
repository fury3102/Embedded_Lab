#ifndef __CAN_H__
#define __CAN_H__

#include "main.h"

#define CAN_DATA_LENGTH 			0x08
#define CAN_TX_ID 					0x012
#define CAN_RX_ID 					0x0A2

int calc_SAE_J1850(uint8_t data[], int len);

void calc_Response();
void CAN_Transmit(CAN_HandleTypeDef *hcan);

#endif /* __CAN_H__ */

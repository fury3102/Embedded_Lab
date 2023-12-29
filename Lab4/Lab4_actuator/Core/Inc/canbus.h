/*
 * canbus.h
 *
 *  Created on: Dec 28, 2023
 *      Author: Pham Nhat Quang
 */

#ifndef INC_CANBUS_H_
#define INC_CANBUS_H_

#include "main.h"

#define CAN_TX_ID	 		0x0A2
#define CAN_RX_ID 			0x012
#define CAN_DATA_LENGTH 	0x08

int calc_SAE_J1850(uint8_t data[], int len);

void calc_Message();
void CAN_Transmit(CAN_HandleTypeDef *hcan);

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
uint8_t CANNegativeResponse(CAN_HandleTypeDef *hcan, uint8_t SID, uint8_t ResponseCode);
uint8_t readDataByIdentifierResponse(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[]);
uint8_t WriteDataByIdentifierResponse(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[]);
uint8_t securityAccessSeedGen(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[]);
uint8_t SecurityAccessKeyResponse(CAN_HandleTypeDef *hcan);


#endif /* INC_CANBUS_H_ */

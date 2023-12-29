/*
 * canbus.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Pham Nhat Quang
 */

#include "canbus.h"


extern uint8_t RxBuffer[];
uint8_t DiagnosticFlag = 0;
volatile uint8_t ReceiveFlag = 0;
uint8_t RxBuffer[CAN_DATA_LENGTH];
CAN_RxHeaderTypeDef RxHeader;

uint32_t CANTxMailboxes = CAN_TX_MAILBOX0;
CAN_TxHeaderTypeDef TxHeader;
uint8_t seed[4];
uint8_t key[16];
uint8_t receivekey[16];
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxBuffer) != HAL_OK)
	{
		Error_Handler();
	}

	if (RxHeader.StdId == 0x712)
	{
		DiagnosticFlag = 1;
	}
}


uint8_t CANNegativeResponse(CAN_HandleTypeDef *hcan, uint8_t SID, uint8_t ResponseCode)
{
	uint8_t TxBuffer[8];

	TxHeader.StdId 	= 0x7a2;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	TxBuffer[0]= 0x7f;
	TxBuffer[1]= SID;
	TxBuffer[2]= ResponseCode;
	TxBuffer[3]= 0x00;
	TxBuffer[4]= 0x00;
	TxBuffer[5]= 0x00;
	TxBuffer[6]= 0x00;
	TxBuffer[7]= 0x00;

	HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes);

}


uint8_t ReadDataByIdentifierResponse(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[])
{
	if (RxData[0] != 0x02)
	{
		CANNegativeResponse(hcan, RxData[1], 0x13);
		return 1;
	}

	uint8_t TxBuffer[8];

	TxHeader.StdId 	= 0x7a2;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	TxBuffer[0]= 0x62;
	TxBuffer[1]= RxData[2];
	TxBuffer[2]= RxData[3];
	TxBuffer[3]= RxHeader->StdId >> 8;
	TxBuffer[4]= RxHeader->StdId & 0xff;
	TxBuffer[5]= 0x00;
	TxBuffer[6]= 0x00;
	TxBuffer[7]= 0x00;
	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes) == HAL_OK)
		{
			HAL_GPIO_TogglePin(GPIOB, LEDB_Pin);
		}

	return 1;
}

uint8_t WriteDataByIdentifierResponse(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[])
{

	if (RxData[0] && 0x0f < 0x03)
	{
		CANNegativeResponse(hcan, RxData[1], 0x13);
		return 1;
	}
	if (RxData[2] != 0x01 && RxData[3] != 0x23)
	{
		CANNegativeResponse(hcan, RxData[1], 0x31);
		return 1;
	}

	uint8_t TxBuffer[CAN_DATA_LENGTH];

	TxHeader.StdId 	= 0x7a2;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	TxBuffer[0]= 0x6e;
	TxBuffer[1]= RxData[2];
	TxBuffer[2]= RxData[3];
	TxBuffer[3]= 0x00;
	TxBuffer[4]= 0x00;
	TxBuffer[5]= 0x00;
	TxBuffer[6]= 0x00;
	TxBuffer[7]= 0x00;

	HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes);
	HAL_GPIO_TogglePin(GPIOB, LEDR_Pin);


	return 1;
}

uint8_t securityAccessSeedGen(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *RxHeader, uint8_t RxData[])
{

	uint8_t TxBuffer[8];
	TxHeader.StdId 	= 0x7a2;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	TxBuffer[0] = RxData[1];
	TxBuffer[1] = 0x01; //seed level
	/* Generate SEED */
	TxBuffer[2] = rand() % 128;
	TxBuffer[3] = rand() % 128;
	TxBuffer[4] = rand() % 128;
	TxBuffer[5] = rand() % 128;
	TxBuffer[6] = 0x00;
	TxBuffer[7] = 0x00;

	HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes);
	seed[0] = TxBuffer[2];
	seed[1] = TxBuffer[3];
	seed[2] = TxBuffer[4];
	seed[3] = TxBuffer[5];

	key[0] 	= seed[0] ^ seed[1];
	key[1] 	= seed[1] + seed[2];
	key[2] 	= seed[2] ^ seed[3];
	key[3] 	= seed[3] + seed[0];

	key[4] 	= seed[0] | seed[1];
	key[5] 	= seed[1] + seed[2];
	key[6] 	= seed[2] | seed[3];
	key[7] 	= seed[3] + seed[1];

	key[8] 	= seed[0] & seed[1];
	key[9] 	= seed[1] + seed[2];
	key[10] = seed[2] & seed[3];
	key[11] = seed[3] + seed[0];

	key[12] = seed[0] - seed[1];
	key[13] = seed[1] + seed[2];
	key[14] = seed[2] - seed[3];
	key[15] = seed[3] + seed[0];

	return 1;
}

uint8_t SecurityAccessKeyResponse(CAN_HandleTypeDef *hcan)
{
		receivekey[4] = RxBuffer[1];
		receivekey[5] = RxBuffer[2];
		receivekey[6] = RxBuffer[3];
		receivekey[7] = RxBuffer[4];
		receivekey[8] = RxBuffer[5];
		receivekey[9] = RxBuffer[6];
		receivekey[10] = RxBuffer[7];

		while (DiagnosticFlag != 1);
		DiagnosticFlag = 0;

		receivekey[11] = RxBuffer[1];
		receivekey[12] = RxBuffer[2];
		receivekey[13] = RxBuffer[3];
		receivekey[14] = RxBuffer[4];
		receivekey[15] = RxBuffer[5];

		for (int i = 0; i < 16; i++)
			{
				if (receivekey[i] != key[i])
					CANNegativeResponse(hcan, 0x27, 0x35);
			}

}

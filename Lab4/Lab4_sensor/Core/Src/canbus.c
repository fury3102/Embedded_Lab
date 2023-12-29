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

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxBuffer) != HAL_OK)
	{
		Error_Handler();
	}

	if (RxHeader.StdId == 0x7a2)
	{
		DiagnosticFlag = 1;
	}
}


void ReadDataByIdentifierRequest(CAN_HandleTypeDef *hcan){
	uint8_t TxBuffer[8];

	TxBuffer[0] = 0x02;
	TxBuffer[1] = 0x22;
	TxBuffer[2] = 0x01;
	TxBuffer[3] = 0x23;
	TxBuffer[4] = 0x00;
	TxBuffer[5] = 0x00;
	TxBuffer[6] = 0x00;
	TxBuffer[7] = 0x00;

	TxHeader.StdId 	= 0x712;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes) == HAL_OK)
	{
		HAL_GPIO_TogglePin(GPIOB, LEDB_Pin);
	}
}

void ReadDataByIdentifierResponse(uint8_t RxBuffer[])
{
	if(RxBuffer[0] == 0x62)
		if(RxBuffer[1]== 0x01 && RxBuffer[2] == 0x23)
		{
			HAL_GPIO_TogglePin(GPIOB, LEDR_Pin);
		}
}

void WriteDataByIdentifierRequest(CAN_HandleTypeDef *hcan)
{
	uint8_t TxBuffer[8];

	TxBuffer[0] 	= 0x03;
	TxBuffer[1] 	= 0x2e;
	TxBuffer[2]		= 0x01;
	TxBuffer[3]		= 0x23;
	TxBuffer[4]		= 0x00;
	TxBuffer[5]		= 0x00;
	TxBuffer[6]		= 0x00;
	TxBuffer[7]		= 0x00;

	TxHeader.StdId 	= 0x712;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes);

	HAL_GPIO_TogglePin(GPIOB, LEDG_Pin);
}

void WriteDataByIdenfierResponse(uint8_t RxBuffer[])
{
	if (RxBuffer[0] == 0x6e)
	{
		HAL_GPIO_TogglePin(GPIOB, LEDB_Pin);
	}
}

void SecurityAccessRequest(CAN_HandleTypeDef *hcan)
{
	uint8_t TxBuffer[8];

	TxBuffer[0]= 0x00;
	TxBuffer[1]= 0x27;
	TxBuffer[2]= 0x01;
	TxBuffer[3]= 0x00;
	TxBuffer[4]= 0x00;
	TxBuffer[5]= 0x00;
	TxBuffer[6]= 0x00;
	TxBuffer[7]= 0x00;

	TxHeader.StdId 	= 0x712;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes) == HAL_OK)
	{
		HAL_GPIO_TogglePin(GPIOB, LEDR_Pin);
	}

}

uint8_t SecurityAccessSeedResponse(uint8_t RxBuffer[])
{
	if (RxBuffer[0] == 0x67 && RxBuffer[1] == 0x01)
	{

			seed[0] = RxBuffer[2];
			seed[1] = RxBuffer[3];
			seed[2] = RxBuffer[4];
			seed[3] = RxBuffer[5];

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
}

void UnlockRequest(CAN_HandleTypeDef *hcan)
{
	uint8_t TxBuffer[8];
	uint8_t TxBuffer2[8];
	uint8_t TxBuffer3[8];

	TxBuffer[0] 	= 0x10;
	TxBuffer[1] 	= 0x12;
	TxBuffer[2] 	= 0x27;
	TxBuffer[3]		= 0x02;
	TxBuffer[4]		= key[0];
	TxBuffer[5]		= key[1];
	TxBuffer[6]		= key[2];
	TxBuffer[7]		= key[3];

	TxHeader.StdId 	= 0x712;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;
	TxHeader.DLC 	= 8;

	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes) == HAL_OK)
	{
		HAL_GPIO_TogglePin(GPIOB, LEDG_Pin);
	}

	TxBuffer2[0] 		= 0x21;
	TxBuffer2[1] 		= key[4];
	TxBuffer2[2] 		= key[5];
	TxBuffer2[3]		= key[6];
	TxBuffer2[4]		= key[7];
	TxBuffer2[5]		= key[8];
	TxBuffer2[6]		= key[9];
	TxBuffer2[7]		= key[10];
	HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer2, &CANTxMailboxes);

	TxBuffer3[0] 		= 0x22;
	TxBuffer3[1] 		= key[11];
	TxBuffer3[2] 		= key[12];
	TxBuffer3[3]		= key[13];
	TxBuffer3[4]		= key[14];
	TxBuffer3[5]		= key[15];
	TxBuffer3[6]		= 0x00;
	TxBuffer3[7]		= 0x00;
	HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer3, &CANTxMailboxes);

}


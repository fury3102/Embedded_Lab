#include "canbus.h"
#include "gpio.h"

volatile uint8_t ReceiveFlag = 0;
uint8_t RxBuffer[CAN_DATA_LENGTH];

CAN_RxHeaderTypeDef CANRxHeader;
CAN_TxHeaderTypeDef TxHeader;

uint32_t CANTxMailboxes = CAN_TX_MAILBOX1;
uint8_t TxBuffer[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


int calc_SAE_J1850(uint8_t data[], int crc_len)
{
	int crc, temp1, temp2, idx, idy;
	crc 	= 0;
	temp1 	= 0;
	temp2 	= 0;
	idx 	= 0;
	idy 	= 0;
	for ( idx = 0; idx < crc_len; idx ++)
	{
		if (idx == 0)
		{
			temp1 = 0;
		}
		else
		{
			temp1 = data[crc_len - idx];
		}
		crc = crc ^ temp1;
		for (idy = 8; idy > 0;idy --)
		{
			temp2 = crc;
			crc = crc << 1;
			if (0 != (temp2 & 128))
			{
				crc = crc ^ 0x1d;
			}
		}
	}
	return crc;
}


void calc_Response()
{
	for (int i = 0; i < 8; i ++)
		{
			TxBuffer[i] = RxBuffer[i];
		}
		TxBuffer[2] = RxBuffer[0] + RxBuffer[1];
		TxBuffer[7] = calc_SAE_J1850(TxBuffer, 7);

		TxHeader.StdId 	= CAN_TX_ID;
		TxHeader.IDE 	= CAN_ID_STD;
		TxHeader.RTR 	= CAN_RTR_DATA;
		TxHeader.DLC 	= CAN_DATA_LENGTH;
}

void CAN_Transmit(CAN_HandleTypeDef *hcan)
{
	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, &CANTxMailboxes) == HAL_OK)
	{
		HAL_GPIO_TogglePin(GPIOB, LEDB_Pin);
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &CANRxHeader, RxBuffer) != HAL_OK)
	{
		Error_Handler();
	}

	// Check true Transmitter
	if (CANRxHeader.StdId == CAN_RX_ID)
	{
		ReceiveFlag = 1;
		HAL_GPIO_TogglePin(GPIOB, LEDR_Pin);
	}
}

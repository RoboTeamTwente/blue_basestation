/*
 * basestationNRF.c
 *
 *  Created on: Mar 16, 2018
 *      Author: Ulf Stottmeister
 *
 * Description:
 *   Basestation specific code for using the nRF24 wireless module.
 */


#include "basestationNRF24.h"
#include "stm32f1xx_hal.h"
#include "packing.h"
#include "TextOut.h"
#include "main.h"

void initBase(SPI_HandleTypeDef* spiHandle24, uint8_t freqChannel){

	if(NRFinit(spiHandle24, nrf24nssHigh, nrf24nssLow, nrf24ceHigh, nrf24ceLow, nrf24irqRead ) == -1){
		Error_Handler();
	}

	//set interrupts
	uint8_t config_reg = readReg(CONFIG);
	config_reg &= ~MASK_RX_DR;   //enable for RX_DR
	config_reg &= ~MASK_TX_DS;  //enable for TX_DS
	config_reg &= ~MASK_MAX_RT; //enable for MAX_RT
	writeReg(CONFIG, config_reg);


	if(setFreqChannel(freqChannel) == -1){
		HAL_GPIO_WritePin(LD0_GPIO_Port,LD0_Pin, 0);
	}
	//setLowSpeed();
	//the default value of RF_SETUP sets the module to 2 Mbps



	//setting up ACKs (with payload)
	setDataPipes(ERX_P0); //enable pipe(s). Pipe 0: ACK packets.

	//auto-ack settings
	uint8_t arc=0; //auto-retransmit count
	uint8_t ard=0b010; //auto-retransmit delay
	writeReg(SETUP_RETR, (ard<<4)|(arc&0b1111));

	//enable dynamic packet length, ack payload, dynamic acks
	/*
	 * EN_DYN_ACK enables to use the SPI command W_TX_PAYLOAD_NOACK.
	 * That means: sending a packet which does not need to be answered with an ACK
	 * when the module is otherwise configured to wait for ACKs.
	 */
	writeReg(FEATURE, EN_DPL | EN_ACK_PAY | EN_DYN_ACK);

	writeReg(DYNPD, DPL_P0); //enable dynamic packet length for data pipe x


	//go to TX mode and be ready to listen
	powerUpTX();
}


uint8_t sendPacket(uint8_t packet[ROBOPKTLEN+1]){


	uint8_t roboID = (packet[0] >> 3);

	//uint8_t addressLong[5] = {0b11010000 + (packet[0] >> 4), 0x12, 0x34, 0x56, 0x78};
	//uint8_t addressLong[5] = {0, 0, 0, 0, 0x90 + (packet[0] >> 4)};
	uint8_t addressLong[5] = {0x99, 0xB0 + roboID, 0x34, 0x56, 0x99};

	uint8_t checksum=0;
	for(uint8_t i=0; i<ROBOPKTLEN; i++) {
		checksum ^= packet[i]; //checksum with XOR over all bytes
	}

	packet[ROBOPKTLEN] = checksum; //appending checksum

	setTXaddress(addressLong);

	sendData(packet, ROBOPKTLEN+1);
	return 0;
}

uint8_t getAndProcessAck(uint8_t idOfLastCalledRobot) {
	uint8_t ack_payload[32];
	uint8_t payload_length;
	int8_t returncode = 0;
	roboAckData receivedRoboAck;

	uint8_t bits_2_pc = packing_pc_empty;
	//sprintf(smallStrBuffer, "0\n");

//	uint8_t nonack[24] = {0};
//	nonack[1] = (uint8_t) ((idOfLastCalledRobot)<<3); //a

	do {
		returncode = getAck(ack_payload, &payload_length);
	} while(returncode == -1); //-1 means: no interrupt yet (no received packet yet)

	if(returncode == -2) {
		//packet loss. Send a non-ack to the PC
		//I'm not sure if this is the format we agreed on..
		//sprintf(smallStrBuffer, "%i\n", idOfLastCalledRobot);
		//TextOut(smallStrBuffer);

/*		for(uint8_t i=0; i < 24; i++) {
			sprintf(smallStrBuffer, "%02x", nonack[i]);
			TextOut(smallStrBuffer);
		}
		TextOut("\n");*/


	} else if(returncode == 1) {
		//we got a regular ACK packet! Let's see..
		//it it's the expected length, then unpack it to a struct
		bits_2_pc |= packing_pc_ack;
		//char * ptr = smallStrBuffer;
		if(payload_length >= SHORTACKPKTLEN) {

			ackPacketToRoboAckData(ack_payload, payload_length, &receivedRoboAck);

			//writing ACK payload to PC by printing it as HEX values.
			//Is that the right format?
			if(receivedRoboAck.batteryState){
				bits_2_pc |= packing_pc_batt;
			}
			if(receivedRoboAck.ballSensor){
				bits_2_pc |= packing_pc_ball;
			}
//			ptr += sprintf(ptr, "%02x", 1);
//			for(uint8_t i=1; i < payload_length+1; i++) {
//				ptr += sprintf(ptr, "%02x", ack_payload[i-1]);
//			}
//			if(payload_length == SHORTACKPKTLEN) {
//				for(uint8_t i=SHORTACKPKTLEN+1; i < 24; i++) {
//					ptr += sprintf(ptr, "%02x", 0);
//				}
//			}
		} else {
			//if the packet wasn't the right length, then ignore it
		}
		//ptr += sprintf(ptr, "1\n");

	} else if(returncode == 0) {
		//delivered, but got an empty ack.
		//send to the PC that there is no ackpayload.. so: send a non-ack.
		//sprintf(smallStrBuffer, "%i\n", idOfLastCalledRobot);
		//TextOut(smallStrBuffer);
/*		for(uint8_t i=0; i < 24; i++) {
			sprintf(smallStrBuffer, "%02x", nonack[i]);
			TextOut(smallStrBuffer);
		}*/
	} else if(returncode == -3) {
		//received a regular (non-ack) packet.
		//just ignore it and don't let the PC know.
	}
//	memcpy(smallStrBuffer, &bits_2_pc, 1);
//	char * EOT = "\n";
//	memcpy(smallStrBuffer + 1, EOT, 2);
//	TextOut(smallStrBuffer);

	clearInterrupts();

	return 0;
}


/*
 * Pin setters
 */



//put the nss pin corresponding to the SPI used high
void nrf24nssHigh(){
	HAL_GPIO_WritePin(CSN_SPI_NRF_GPIO_Port, CSN_SPI_NRF_Pin, GPIO_PIN_SET);
}

//put the nss pin corresponding to the SPI used low
void nrf24nssLow(){
	HAL_GPIO_WritePin(CSN_SPI_NRF_GPIO_Port, CSN_SPI_NRF_Pin, GPIO_PIN_RESET);
}

//put the ce pin corresponding to the SPI used high
void nrf24ceHigh(){
	//CE: chip enable
	HAL_GPIO_WritePin(CE_SPI_NRF_GPIO_Port, CE_SPI_NRF_Pin, GPIO_PIN_SET);
}

//put the ce pin corresponding to the SPI used low
void nrf24ceLow(){
	HAL_GPIO_WritePin(CE_SPI_NRF_GPIO_Port, CE_SPI_NRF_Pin, GPIO_PIN_RESET);
}

//reading if an interrupt happened
uint8_t nrf24irqRead(){
	return !HAL_GPIO_ReadPin(IRQ_SPI_NRF_GPIO_Port, IRQ_SPI_NRF_Pin);
}

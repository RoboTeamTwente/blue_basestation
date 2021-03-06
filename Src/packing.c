/*
 * packing.c
 *
 *  Created on: Mar 27, 2017
 *      Author: gebruiker
 */

#include "packing.h"
#include <stdio.h>

/*
 * Convert a struct with roboData to a Bytearray, which can be transmitted by the nRF module.
 * You will only use this function for creating packets for debugging purposes on the basestation.
 * In the final version, the computer already sends a read-to-transmit byte array.
 *
 */
void robotDataToPacket(roboData *input, uint8_t output[ROBOPKTLEN]) {

	output[0] = (uint8_t) (  							// aaaaabbb
		(0b11111000 & (input->id << 3))                  // aaaaa000   5 bits; bits  4-0 to 7-3
	  | (0b00000111 & (input->rho >> 8))                 // 00000bbb  11 bits; bits 10-8 to 2-0
	);

	output[1] = (uint8_t) (  							// bbbbbbbb
		input->rho                                       // bbbbbbbb  11 bits; bits  7-0 to 7-0
	);

	output[2] = (uint8_t) (  							// cccccccc
		input->theta >> 3                                // cccccccc 11 bits; bits 10-8 to 7-0
	);

	output[3] = (uint8_t) (  							// cccde00g

		(0b11100000 & (input->theta << 5)) |             // ccc00000 11 bits; bits  2-0 to 7-5
		(0b00010000 & (input->driving_reference << 4)) | // 000d0000  1 bit ; bit     0 to   4
		(0b00001000 & (input->use_cam_info) << 3) |      // 0000e000  1 bit ; bit     0 to   3
		(0b00000001 & (input->velocity_angular >> 8))    // 0000000g  9 bits; bit     8 to   0
	);

	output[4] = (uint8_t) (  							// gggggggg
		input->velocity_angular                          // gggggggg  8 bits; bits  7-0 to 7-0
	);

	output[5] = (uint8_t) (								// 0000hijk
		(0b00001000 & (input->debug_info << 3)) |        // 0000h000  1 bit ; bit     0 to   3
		(0b00000100 & (input->do_kick << 2)) |           // 00000i00  1 bit ; bit     0 to   2
		(0b00000010 & (input->do_chip << 1)) |           // 000000j0  1 bit ; bit     0 to   1
		(0b00000001 & (input->kick_chip_forced))    // 0000000k  1 bit ; bit     0 to   0
	);

	output[6] = (uint8_t) (  							// mmmmmmmm
		input->kick_chip_power                           // mmmmmmmm  8 bits; bits  7-0 to 7-0
	);

	output[7] = (uint8_t) (  							// nnnnnnnn
		input->velocity_dribbler                         // nnnnnnnn  8 bits; bits  7-0 to 7-0
	);

	output[8] = (uint8_t) ( 							// pppqqqqq
		(0b11100000 & (input->geneva_drive_state << 5)) |// ppp00000  3 bits; bits  2-0 to 7-5
		(0b00011111 & (input->cam_position_x >> 8 ))     // 000qqqqq 13 bits; bits 12-8 to 4-0
	);

	output[9] = (uint8_t) (  							// qqqqqqqq
		input->cam_position_x                            // qqqqqqqq 13 bits; bits  7-0 to 7-0
	);

	output[10] = (uint8_t) ( 							// rrrrrrrr
		input->cam_position_y >> 5                       // rrrrrrrr 13 bits; bits 12-5 to 7-0
	);

	output[11] = (uint8_t) (							// rrrrrsss
		(0b11111000 & (input->cam_position_y << 3)) |    // rrrrr000 13 bits; bits  4-0 to 7-3
		(0b00000111 & (input->cam_rotation >> 8))        // 00000sss 11 bits; bits 10-8 to 2-0
	);

	output[12] = (uint8_t) ( 							// ssssssss
		input->cam_rotation                              // ssssssss 11 bits; bits  7-0 to 7-0
	);
}

/*
 * Create a roboData structure from a given Bytearray.
 * This is used by the robot to convert a received nRF packet into a struct with named variables.
 */
void packetToRoboData(uint8_t input[ROBOPKTLEN], roboData *output) {
	/*
	output[0] aaaaabbb
	output[1] bbbbbbbb
	output[2] cccccccc
	output[3] cccde00g
	output[4] gggggggg
	output[5] 0000hijk
	output[6] mmmmmmmm
	output[7] nnnnnnnn
	output[8] pppqqqqq
	output[9] qqqqqqqq
	output[10] rrrrrrrr
	output[11] rrrrrsss
	output[12] ssssssss
	 */
	output->id = input[0]>>3; //a
	output->rho = (input[0]&0b111)<<8; //b
	output->rho |= input[1]; //b
	output->theta = input[2]<<3; //c
	output->theta |= (input[3]>>5)&0b111; //c
	output->driving_reference = (input[3]>>4)&1; //d
	output->use_cam_info = (input[3]>>3)&1; //e
	output->velocity_angular = (input[3]&1) << 8; //g
	output->velocity_angular |= input[4]; //g
	output->debug_info = (input[5]>>3)&1; //h
	output->do_kick = (input[5]>>2)&1; //i
	output->do_chip = (input[5]>>1)&1; //j
	output->kick_chip_forced = input[5]&1; //k
	output->kick_chip_power = input[6]; //m
	output->velocity_dribbler = input[7]; //n
	output->geneva_drive_state = (input[8]>>5)&0b111; //p
	output->cam_position_x = (input[8]&0b11111)<<8; //q
	output->cam_position_x |= input[9]; //q
	output->cam_position_y = input[10] << 5; //r
	output->cam_position_y |= (input[11]>>3)&0b11111; //r
	output->cam_rotation = (input[11]&0b111) << 8; //s
	output->cam_rotation |= input[12]; //s

}


/*
 * For the Robot ACKs we use the following packet definition
 *
 *skipping some characters for better readability

		Character   Description                 Values          Represented values    Units       Bits    Comment
		a           Robot ID                    [0,15]          [0,31]                -              5    -
		b           Left front wheel state      [0,1]           {true,false}          -              1    Indicates whether the left front wheel functions
		c           Right front wheel state     [0,1]           {true,false}          -              1    Indicates whether the right front wheel functions
		d           Left back wheel state       [0,1]           {true,false}          -              1    Indicates whether the left back wheel functions
		e           Right back wheel state      [0,1]           {true,false}          -              1    Indicates whether the right back wheel functions
		f           Geneva drive state          [0,1]           {true,false}          -              1    Indicates whether the Geneva drive functions
		g           Battery state               [0,1]           {true,false}          -              1    States whether the battery is nearing depletion
		h           x position robot            [-4096,4095]    [-1024,1023]          0.25cm        13    -
		k           y position robot            [-4096,4095]    [-1024,1023]          0.25cm        13    -
		m           rho            				[-1024,1023]    [?,?]		          	            11    Magnitude of the robot velocity vector
		o           theta           			[-1024,1023]    [?,?]                               11    Angle of the robot velocity vector
		p           Orientation                 [-1024,1023]    [-pi,pi>              0.00307rad    11    Angle of the facing direction. 2048 possible angles. Intervals of ~0.00307 rad
		q           Angular velocity            [-1024,1023]    [?,?]                 0.049rad/s?   11
		s           Ball sensor                 [0,128]         {?}			          -              7    Can be used to specify where on the dribbler the ball is located. For now a non-zero value represents the presence of the ball

		Extra
		t           Acceleration x              [0, 4294967295]    [0, 32 bit float]       m/s/s         32    -
		u           Acceleration y              [0, 4294967295]    [0, 32 bit float]       m/s/s         32    -
		v           Angular rate                [0, 4294967295]    [0, 32 bit float]       m/s/s         32    raw angular velocity from xsense


	===== Packet received from the robot =====
		Byte      Config
		 0        aaaaabcd
		 1        efghhhhh
		 2        hhhhhhhh
		 3        kkkkkkkk
		 4        kkkkkmmm
		 5        mmmmmmmm
		 6        oooooooo
		 7        oooppppp
		 8        ppppppqq
		 9        qqqqqqqq
		10        qsssssss

		Extra
		11        tttttttt
		12        tttttttt
		13        tttttttt
		14        tttttttt
		15        uuuuuuuu
		16        uuuuuuuu
		17        uuuuuuuu
		18        uuuuuuuu
		19        vvvvvvvv
		20        vvvvvvvv
		21        vvvvvvvv
		22        vvvvvvvv

 */




/*
 * First, fill the fields on a roboAckData struct.
 * Then convert that struct into a Bytearray by using this function.
 * The result can be used as an ACK payload to transmit it over air.
 */

void roboAckDataToPacket(roboAckData *input, uint8_t output[FULLACKPKTLEN]) {
	output[0]  = (uint8_t) ((input->roboID)<<3); //a
	output[0] |= (uint8_t) ((input->wheelLeftFront)<<2); //b
	output[0] |= (uint8_t) ((input->wheelRightFront)<<1); //c
	output[0] |= (uint8_t) ((input->wheelLeftBack)); //d


	output[1]  = (uint8_t) ((input->wheelRightBack)<<7); //e
	output[1] |= (uint8_t) ((input->genevaDriveState)<<6); //f
	output[1] |= (uint8_t) ((input->batteryState)<<5); //g
	output[1] |= (uint8_t) ((input->xPosRobot>>8))&0b11111; //h

	output[2]  = (uint8_t) (input->xPosRobot&0xff); //h

	output[3]  = (uint8_t) (input->yPosRobot>>5)&0xff; //k

	output[4]  = (uint8_t) ((input->yPosRobot&0b11111)<<3); //k
	output[4] |= (uint8_t) (input->rho>>8)&0b111; //m

	output[5]  = (uint8_t) (input->rho&0xff); //m

	output[6]  = (uint8_t) ((input->theta>>3)&0xff); //o

	output[7]   = (uint8_t) ((input->theta&0xff)<<5); //o
	output[7]  |= (uint8_t) (input->orientation>>6)&0b11111; //p

	output[8]   = (uint8_t) (input->orientation&0b111111)<<2; //p
	output[8]  |= (uint8_t) (input->angularVelocity>>9)&0b11; //q

	output[9]  = (uint8_t) ((input->angularVelocity>>1)&0xff); //q

	output[10]  = (uint8_t) ((input->angularVelocity&1)<<7); //q

	output[10] |= (uint8_t) (input->ballSensor)&0x7f; //s

	output[11] = (uint8_t) (input->xAcceleration >> 24)&0xff; //t
	output[12] = (uint8_t) (input->xAcceleration >> 16)&0xff; //t
	output[13] = (uint8_t) (input->xAcceleration >> 8)&0xff; //t
	output[14] = (uint8_t) (input->xAcceleration)&0xff; //t

	output[15] = (uint8_t) (input->yAcceleration >> 24)&0xff; //u
	output[16] = (uint8_t) (input->yAcceleration >> 16)&0xff; //u
	output[17] = (uint8_t) (input->yAcceleration >> 8)&0xff; //u
	output[18] = (uint8_t) (input->yAcceleration)&0xff; //u

	output[19] = (uint8_t) (input->angularRate >> 24)&0xff; //v
	output[20] = (uint8_t) (input->angularRate >> 16)&0xff; //v
	output[21] = (uint8_t) (input->angularRate >> 8)&0xff; //v
	output[22] = (uint8_t) (input->angularRate)&0xff; //v

}

/*
 * We would actually just pass the raw ack packet to the computer and let the computer handle the unwrapping.
 * But for completeness and for debug purposes it is nice to unwrap the ACK packet on the Basestation board
 * itself. In that way we can test and debug with just the Basestation and a serial monitor.
 *
 * ACK packets can be either 11 Bytes or 23 Bytes long. That depends on whether the Basestation requested
 * additional fields buy setting the debug_info flag in an earlier robo packet.
 */
void ackPacketToRoboAckData(uint8_t input[23], uint8_t packetlength, roboAckData *output) {
	//input is now specified as an array of size 23. Note that there are also ACK packets of the length 11.
	//You need to use packetlength to know which range of the array contains useful information.
	//The attempt of accessing input[11] to input[22] for a short ACK packet will yield garbage data.

	output->roboID = input[0]>>3; //a
	output->wheelLeftFront = (input[0]>>2)&1; //b
	output->wheelRightFront = (input[0]>>1)&1; //c
	output->wheelLeftBack = (input[0])&1; //d

	output->wheelRightBack = (input[1]>>7)&1; //e
	output->genevaDriveState = (input[1]>>6)&1; //f
	output->batteryState = (input[1]>>5)&1;  //g
	output->xPosRobot = ((input[1]&0b11111)<<8 | input[2]); //h

	output->yPosRobot = ((input[3]<<5) | (input[4]>>3)); //k

	output->rho = (input[4]&0b111)<<8; //m

	output->rho |= input[5]; //m

	output->theta = (input[6]<<3) | (input[7]>>5); //o

	output->orientation = ((input[7]&0b11111)<<6) | (input[8]>>2); //p

	output->angularVelocity = ((input[8]&0b11)<<9) | (input[9]<<1) | ((input[10]>>7)&1); //q

	output->ballSensor = input[10]&0x7f; //s

	if(packetlength < FULLACKPKTLEN)
		return;

	//extra data
	output->xAcceleration = (input[11]<<24) | (input[12]<<16) | (input[13]<<8) | input[14]; //t
	output->yAcceleration = (input[15]<<24) | (input[16]<<16) | (input[17]<<8) | input[18]; //u
	output->angularRate = (input[19]<<24) | (input[20]<<16) | (input[21]<<8) | input[22]; //v

}

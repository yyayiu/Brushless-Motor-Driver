# CAN Analysis
By Rex Cheng

Test Platform:
2 F4 board with 1 sender and 1 receiver, each with 120 Ohm termination resistor, TJA1050

Method of testing:
The sender would generate continuous data stream with an increasing integer. That number is copied twice to a 8-btye CAN message packet. Those packets would be sent with a constant data speed.

The receiver would check whether the incoming data are in correct order (by checking the integer), and whether the two copies of the integer number is the same. The receiver would also display the data rate.

If the data are not in the correct order, there will be a **TIME ERROR**. If the two copies are not the same, there will be a **CONSISTENCY ERROR**.

## Inital version 
A modified version (to adapt F4) of Kenneth's version.

### Performance
Time error does not exist below 50 KB/s. When tuned >60 KB/s, time error happens ~10 times every second. THe sender board sometimes die.

## Change #1
Disabled CAN_ABOM, enabled CAN_NART. Changed TX/RX GPIO to High Speed. 
Error still occurs at ~50KB/s, but less. I guess its just because of not sending the error data. This means error data exist.

## Change of method
Because error data is discarded, TIME_ERROR was changed to data<=last_data, added LOST_ERROR to be data!=(last_data+1).

## Change #2

	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 2;

### Performance
Stable until 90KB/s. I think that's the limit given 1Mbps.
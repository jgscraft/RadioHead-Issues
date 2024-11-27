// JGS_Alarm.h
//
// 241108 - (JGS) Alarm system addresses and messages
//

//-- Central receiving station
#ifdef CENTRAL
	const uint8_t 	MYADDRESS = 100;
	char RFLabel[12] = "Central    ";
#endif

//-- Remote sensor #1
#ifdef UNIT1
	const uint8_t 	TARGETADDR = 100;
	const uint8_t 	MYADDRESS = 101;
	uint8_t RFLabel[12] = "Front Door ";
#endif

//-- Remote sensor #2
#ifdef UNIT2
	const uint8_t 	TARGETADDR = 100;
	const uint8_t 	MYADDRESS =	102;
	char RFLabel[12] = "West Walk  ";
#endif

//-- Remote sensor #3
#ifdef UNIT3
	const uint8_t 	TARGETADDR	= 100;
	const uint8_t 	MYADDRESS =	103;
	char RFLabel[12] = "Lake Path  ";
#endif

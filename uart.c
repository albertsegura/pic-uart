#include "cbuffer.h"
#include "uart.h"

#define SIZEBUFFER_RC1 128
#define SIZEBUFFER_TX1 128

#define SIZEBUFFER_RC2 128
#define SIZEBUFFER_TX2 128

/*********	 	PRIVATE UART FUNCTIONS	 	*********/

char dataXAvailable(Circular_Buffer *cbRC);
char sendXByte(unsigned char data, unsigned char waitToTx, Circular_Buffer *cbTX, unsigned char id);
char sendXData(unsigned char *data, unsigned short size, unsigned char waitToTx, Circular_Buffer *cbTX, unsigned char id);
char readXByte(unsigned char *data, Circular_Buffer *cbRC);
char readXData(unsigned char *data, unsigned short size, Circular_Buffer *cbRC);

char dataXAvailable(Circular_Buffer *cbRC) {
	return circularbNumElements(cbRC);
}	

char sendXByte(unsigned char data, unsigned char waitToTx, Circular_Buffer *cbTX, unsigned char id){
	char ret = 0;
	
	while (circularbIsFull(cbTX)) ;
	ret = circularbWrite(cbTX,(char *)&data);
	
	if (id == 1) {
		PIE1bits.TX1IE = 0;
		if (TXSTA1bits.TXEN == 0) TXSTA1bits.TXEN = 1;
		PIE1bits.TX1IE = 1;
		
		if (waitToTx) while(TXSTA1bits.TRMT == 0 && TXSTA1bits.TXEN == 1) ;
	}
	else {
		PIE3bits.TX2IE = 0;
		if (TXSTA2bits.TXEN == 0) TXSTA2bits.TXEN = 1;
		PIE3bits.TX2IE = 1;
		
		if (waitToTx) while(TXSTA2bits.TRMT == 0 && TXSTA2bits.TXEN == 1) ;
	}
	return ret;	
}

char sendXData(unsigned char *data, unsigned short size, unsigned char waitToTx, Circular_Buffer *cbTX, unsigned char id){
	char ret = 0;
	unsigned short index = 0;
	unsigned short sizegiven;
	
	if (id == 1) sizegiven = (SIZEBUFFER_TX1-circularbNumElements(cbTX)) < size ? (SIZEBUFFER_TX1-circularbNumElements(cbTX)):size;
	else sizegiven = (SIZEBUFFER_TX2-circularbNumElements(cbTX)) < size ? (SIZEBUFFER_TX2-circularbNumElements(cbTX)):size;
	
	for (index = 0; index < sizegiven; index++) {
		circularbWrite(cbTX,(char *)data);

		if (id == 1) {
			PIE1bits.TX1IE = 0;
			if (TXSTA1bits.TXEN == 0) TXSTA1bits.TXEN = 1;
			PIE1bits.TX1IE = 1;
		}
		else {
			PIE3bits.TX2IE = 0;
			if (TXSTA2bits.TXEN == 0) TXSTA2bits.TXEN = 1;
			PIE3bits.TX2IE = 1;
		}
		data++;
	}
	for (;index<size; index++) {
		while (circularbIsFull(cbTX)) ;
		circularbWrite(cbTX,(char *)data);
		
		if (id == 1) {
			PIE1bits.TX1IE = 0;
			if (TXSTA1bits.TXEN == 0) TXSTA1bits.TXEN = 1;
			PIE1bits.TX1IE = 1;
		}
		else {
			PIE3bits.TX2IE = 0;
			if (TXSTA2bits.TXEN == 0) TXSTA2bits.TXEN = 1;
			PIE3bits.TX2IE = 1;
		}
		data++;
	}
	
	if (id == 1) {
		if (waitToTx) while(TXSTA1bits.TRMT == 0 && TXSTA1bits.TXEN == 1) ;
	}
	else {
		if (waitToTx) while(TXSTA2bits.TRMT == 0 && TXSTA2bits.TXEN == 1) ;
	}
	return ret;
}

char readXByte(unsigned char *data, Circular_Buffer *cbRC){
	char ret = 0;
	while (circularbIsEmpty(cbRC)) ;
	ret = circularbRead(cbRC,(char *)data);
	return ret;	
}

char readXData(unsigned char *data, unsigned short size, Circular_Buffer *cbRC){
	char ret = 0;
	unsigned short index = 0;
	unsigned short sizegiven;
	
	sizegiven = circularbNumElements(cbRC) < size ? circularbNumElements(cbRC):size;
	for (index = 0; index < sizegiven; index++) {
		circularbRead(cbRC,(char *)data);
		data++;
	}	
	for (;index<size; index++) {
		while (circularbIsEmpty(cbRC));
		circularbRead(cbRC,(char *)data);
		data++;
	}
	
	return ret;
}

/*********	 	UART1	 	*********/

Circular_Buffer cbRC1;
Circular_Buffer cbTX1;
char buff_rc1[SIZEBUFFER_RC1];
char buff_tx1[SIZEBUFFER_TX1];

void initUart1(void){
	
	/* UART1 CONFIGURATION */
	
	TXSTA1bits.TX9 = 	0; 
	TXSTA1bits.TXEN = 	0;
	TXSTA1bits.SYNC = 	0;
	TXSTA1bits.SENDB = 	0;
	TXSTA1bits.BRGH =	1;
	
	RCSTA1bits.SPEN = 	1;
	RCSTA1bits.RX9 = 	0;
	RCSTA1bits.CREN = 	1;
	RCSTA1bits.ADDEN = 	0;
	
	BAUDCON1bits.RXDTP = 	0;
	BAUDCON1bits.TXCKP = 	0;
	BAUDCON1bits.BRG16 = 	1;
	BAUDCON1bits.WUE = 		0;
	BAUDCON1bits.ABDEN = 	0;
	
	/* DATASHEET
	 * 9600 =  FOSC/(4 * (spbrg+1))
	 * spbrg = FOSC/(9600*4) - 1
	 * SPBRGH1 = 0;
	 * SPBRG1 = 77; */
	SPBRGH1 = 4;
	SPBRG1 = 225;

	TRISCbits.TRISC7 = 1; // Input RX
	TRISCbits.TRISC6 = 0; // Output TX
	
	/* BLUETOOTH CONFIGURATION */
	
	circularbInit(&cbRC1, buff_rc1, SIZEBUFFER_RC1);
	circularbInit(&cbTX1, buff_tx1, SIZEBUFFER_TX1);
	
	/* INTERRUPTION ENABLE */
	
	PIE1bits.TX1IE = 1;
	PIE1bits.RC1IE = 1;
	IPR1bits.RC1IP = 1;
	IPR1bits.TX1IP = 1;
	
	RCONbits.IPEN = 1;
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
}

char data1Available(void) {
	return dataXAvailable(&cbRC1);
}	

char send1Byte(unsigned char data, unsigned char waitToTx){
	return sendXByte(data, waitToTx, &cbTX1, 1);
}

char send1Data(unsigned char *data, unsigned short size, unsigned char waitToTx){
	return sendXData(data, size, waitToTx, &cbTX1, 1);
}

char read1Byte(unsigned char *data){
	return readXByte(data, &cbRC1);
}

char read1Data(unsigned char *data, unsigned short size){
	return readXData(data, size, &cbRC1);
}

void handler_uart1_int(void){
	unsigned char toTransmit, readed;
	if (PIR1bits.TX1IF == 1 && PIE1bits.TX1IE == 1) {
		if (!circularbIsEmpty(&cbTX1)) {
			circularbRead(&cbTX1,(char *)&toTransmit);
			TXREG1 = toTransmit;
		}
		else {
			if (TXSTA1bits.TRMT) TXSTA1bits.TXEN = 0;
		}
	}
	
	if (PIR1bits.RC1IF == 1 && PIE1bits.RC1IE == 1) {
		readed = RCREG1;
		if (!circularbIsFull(&cbRC1)) {			
			circularbWrite(&cbRC1, (char *)&readed);
		}
		PIR1bits.RC1IF = 0;
	}
}

/*********	 	UART2	 	*********/

Circular_Buffer cbRC2;
Circular_Buffer cbTX2;
char buff_rc2[SIZEBUFFER_RC2];
char buff_tx2[SIZEBUFFER_TX2];

void initUart2(void){
	
	/* UART1 CONFIGURATION */
	
	TXSTA2bits.TX9 = 	0; 
	TXSTA2bits.TXEN = 	0;
	TXSTA2bits.SYNC = 	0;
	TXSTA2bits.SENDB = 	0;
	TXSTA2bits.BRGH =	1;
	
	RCSTA2bits.SPEN = 	1;
	RCSTA2bits.RX9 = 	0;
	RCSTA2bits.CREN = 	1;
	RCSTA2bits.ADDEN = 	0;
	
	BAUDCON2bits.RXDTP = 	0;
	BAUDCON2bits.TXCKP = 	0;
	BAUDCON2bits.BRG16 = 	1;
	BAUDCON2bits.WUE = 		0;
	BAUDCON2bits.ABDEN = 	0;
	
	/* DATASHEET
	 * 9600 =  FOSC/(4 * (spbrg+1))
	 * spbrg = FOSC/(9600*4) - 1
	 * SPBRGH1 = 0;
	 * SPBRG1 = 77; */
	SPBRGH2 = 4;
	SPBRG2 = 225;

	/* REMAPEIG PORT */
	// Mapeig de TX2 a RP3/RB0
	RPOR3 = 0x05;
	// Mapeig de RX2 a RP4/RB1
	RPINR16 = 0x04;
	
	/* TX2 & RX2 CONFIGURATION */
	ANCON1bits.PCFG10 = 1;
	ANCON1bits.PCFG12 = 1;
	TRISBbits.TRISB0 = 0; // TX2 output
	TRISBbits.TRISB1 = 1; // RX2 input
	
	/* BLUETOOTH CONFIGURATION */
	
	circularbInit(&cbRC2, buff_rc2, SIZEBUFFER_RC2);
	circularbInit(&cbTX2, buff_tx2, SIZEBUFFER_TX2);
	
	/* INTERRUPTION ENABLE */
	
	PIE3bits.TX2IE = 1;
	PIE3bits.RC2IE = 1;
	IPR3bits.RC2IP = 1;
	IPR3bits.TX2IP = 1;
	
	RCONbits.IPEN = 1;
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
}

char data2Available(void) {
	return dataXAvailable(&cbRC2);
}	

char send2Byte(unsigned char data, unsigned char waitToTx){
	return sendXByte(data, waitToTx, &cbTX2, 2);
}

char send2Data(unsigned char *data, unsigned short size, unsigned char waitToTx){
	return sendXData(data, size, waitToTx, &cbTX2, 2);
}

char read2Byte(unsigned char *data){
	return readXByte(data, &cbRC2);
}

char read2Data(unsigned char *data, unsigned short size){
	return readXData(data, size, &cbRC2);
}

void handler_uart2_int(void){
	unsigned char toTransmit, readed;
	if (PIR3bits.TX2IF == 1 && PIE3bits.TX2IE == 1) {
		if (!circularbIsEmpty(&cbTX2)) {
			circularbRead(&cbTX2,(char *)&toTransmit);
			TXREG2 = toTransmit;
		}
		else {
			if (TXSTA2bits.TRMT) TXSTA2bits.TXEN = 0;
		}
	}
	
	if (PIR3bits.RC2IF == 1 && PIE3bits.RC2IE == 1) {
		readed = RCREG2;
		if (!circularbIsFull(&cbRC2)) {			
			circularbWrite(&cbRC2, (char *)&readed);
		}
		PIR3bits.RC2IF = 0;
	}
}

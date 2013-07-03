#ifndef UARTINTERRUPTION
#define UARTINTERRUPTION

/************************************************************************************
 * ==================================================================================
 *								CARACTERISTIQUES FUNCIONS
 * ==================================================================================
 *
 * 	dataXAvailable(Circular_Buffer *cbRC);
 *
 * Retorna el nombre de dades en el buffer de lectura
 * ==================================================================================
 *
 * 	sendXByte(unsigned char data, unsigned char waitToTx);
 *
 * Funció d'enviament bloquejant si el buffer d'enviament es ple.
 * Si waitToTx == 1 bloqueja fins l'acabament efectiu de la transmisió.
 * ==================================================================================
 * 
 * 	char sendXData(unsigned char *data, unsigned short size, unsigned char waitToTx);
 *
 * Funció d'enviament bloquejant si el buffer d'enviament es ple.
 * Si waitToTx == 1 bloqueja fins l'acabament efectiu de la transmisió.
 * Màxima llargaria de les dades d'escriptura 2^16-1 bytes.
 * ==================================================================================
 * 
 * 	char readXByte(unsigned char *data);
 *
 * Funció de recepció bloquejant si no hi han dades al buffer de recepció.
 * ==================================================================================
 * 
 * 	char readXData(unsigned char *data, unsigned short size);
 *
 * Funció de recepció bloquejant si no hi han dades al buffer de recepció. 
 * Màxima llargaria de les dades de lectura de 2^16-1 bytes.
 * ==================================================================================
 * ----------------------------------------------------------------------------------
 * ==================================================================================
 *	  - UART Library for PIC18F46J50 using Interruptions by Albert Segura - v1.2 -
 * ==================================================================================
 * ----------------------------------------------------------------------------------
 ************************************************************************************/ 

void initUart1(void);
char data1Available(void);
char send1Byte(unsigned char data, unsigned char waitToTx);
char send1Data(unsigned char *data, unsigned short size, unsigned char waitToTx);
char read1Byte(unsigned char *data);
char read1Data(unsigned char *data, unsigned short size);
void handler_uart1_int(void);

void initUart2(void);
char data2Available(void);
char send2Byte(unsigned char data, unsigned char waitToTx);
char send2Data(unsigned char *data, unsigned short size, unsigned char waitToTx);
char read2Byte(unsigned char *data);
char read2Data(unsigned char *data, unsigned short size);
void handler_uart2_int(void);

#endif

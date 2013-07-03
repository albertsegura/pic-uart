UART library for PIC18F46J50
============================

**2 UART's with interruptions**

**Functions:**
* dataXAvailable(Circular_Buffer *cbRC);
* sendXByte(unsigned char data, unsigned char waitToTx);
* char sendXData(unsigned char *data, unsigned short size, unsigned char waitToTx);
* char readXByte(unsigned char *data);
* char readXData(unsigned char *data, unsigned short size);

**Other:**
* The majority of the comments are in catalan, but the code it's quite self-explanatory.
* Don't doubt to ask/tell me if there is any doubt or contribution!


**Author:**
* Albert Segura Salvador

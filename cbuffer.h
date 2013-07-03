#ifndef _CIRCULAR_BUFFER
#define _CIRCULAR_BUFFER

typedef struct {
		unsigned int start, end, size, numelem;
		char *buffer;
}Circular_Buffer;


void circularbInit(Circular_Buffer* cb, char *buf, int size) {
	cb->buffer = buf;
	cb->start = 0;
	cb->end = 0;
	cb->size = size;
	cb->numelem = 0;
}

void circularbFree(Circular_Buffer *cb) {
	cb->start = 0;
	cb->end = 0;
}

int circularbIsFull(Circular_Buffer *cb) {
	unsigned int aux = cb->end + 1;
	if (aux == cb->size) aux = 0;
	return (aux == cb->start);
}

int circularbNumElements(Circular_Buffer *cb) {
	return cb->numelem;
}

int circularbIsEmpty(Circular_Buffer *cb) {
	return cb->end == cb->start;
}

int circularbWrite(Circular_Buffer *cb, char *element) {
	if (circularbIsFull(cb)) return -1;
	else {
		cb->buffer[cb->end] = *element;
		cb->end = cb->end + 1;
		if (cb->end == cb->size) cb->end = 0;
		cb->numelem++;
		return 0;
	}
}

int circularbRead(Circular_Buffer *cb, char *element) {
	*element = cb->buffer[cb->start];
	cb->start = cb->start + 1;
	if (cb->start == cb->size) cb->start = 0;
	cb->numelem--;
	return 0;
}

#endif /* _CIRCULAR_BUFFER */

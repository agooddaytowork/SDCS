/*
 Ring/Circular Buffer is done by having two pointers to the array one points to the Head
 and the other points to the Tail of the buffer. As data is added to the buffer, the head
 pointer moves up and as the data is being removed (read) the tail pointer moves up. There
 are two critical cases that have to be considered while using/implementing a circular buffer:
 1.  Head pointer == Tail Pointer -> the buffer is empty
 2. (Head pointer + 1) == Tail Pointer -> the buffer is full

 https://en.wikipedia.org/wiki/Circular_buffer

 Solution : allway keep one slot open

 */

#include "RingBuffer.h"

RingBuffStatus ringbuffer_init(RingBuffer *rbuff, uint8_t *pdata, uint32_t size)
{
	rbuff->head = 0;
	rbuff->tail = 0;
	if (pdata == (void*) 0)
		return BUFFER_EMPTY;
	if (size == 0)
		return BUFFER_EMPTY;
	rbuff->ringbuff = pdata;
	rbuff->buffsize = size;
	return BUFFER_OK;
}

RingBuffStatus ringbuffer_write_byte(RingBuffer *rbuff, uint8_t wdata)
{

	/*check if buffer is full or not*/
	uint32_t next = (uint32_t) (rbuff->head + 1) % (rbuff->buffsize); /*next = head +1*/
	if (next != rbuff->tail) /* tail != head+1  -->*/
	{
		rbuff->ringbuff[rbuff->head] = wdata;
		rbuff->head = next;
		return BUFFER_OK;
	}
	else
	{
		rbuff->ringbuff[rbuff->head]=wdata;
		return BUFFER_FULL;
	}
}

RingBuffStatus ringbuffer_read_byte(RingBuffer *rbuff, uint8_t *rdata)
{
	/*check if buffer is empty or not*/
	if (rbuff->head == rbuff->tail)
	{
		return BUFFER_EMPTY;
	}
	else
	{
		*rdata = rbuff->ringbuff[rbuff->tail];
		rbuff->tail = (uint32_t) (rbuff->tail + 1) % (rbuff->buffsize);
		return BUFFER_OK;
	}
}

RingBuffStatus ringbuffer_empty(RingBuffer *rbuff)
{
	rbuff->head = 0;
	rbuff->tail = 0;
	return BUFFER_OK;
}

RingBuffStatus ringbuffer_read_history_elements(RingBuffer *rbuff,
		uint8_t *rdata, uint8_t numdata)
{
	int32_t cur_pos = (rbuff->tail) - numdata;

	while (numdata--)
	{
		if (cur_pos < 0)
		{
			cur_pos = (rbuff->buffsize - 1);
		}
		*rdata++ = rbuff->ringbuff[cur_pos++];
	}
	return BUFFER_OK;
}

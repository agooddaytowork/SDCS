/*
Ring/Circular Buffer is done by having two pointers to the array one points to the Head 
and the other points to the Tail of the buffer. As data is added to the buffer, the head 
pointer moves up and as the data is being removed (read) the tail pointer moves up. There 
are two critical cases that have to be considered while using/implementing a circular buffer:
    1.  Head pointer == Tail Pointer -> the buffer is empty
    2. (Head pointer + 1) == Tail Pointer -> the buffer is full


*/

/*

https://en.wikipedia.org/wiki/Circular_buffer

Solution : allway keep one slot open

*/
#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
	 

#include "stdint.h"  /*for variable type*/
//#define RING_BUFFER_SIZE 256

typedef struct {
	uint32_t head;/*point to next written data pos*/
	uint32_t tail;/*point to current read data pos*/
	uint32_t buffsize;
	uint8_t *ringbuff;	
}RingBuffer;

typedef enum{
	BUFFER_OK=0,
	BUFFER_EMPTY=1, /*head = tail*/
	BUFFER_FULL=2 /*(head + 1)= tail*/
}RingBuffStatus;



RingBuffStatus ringbuffer_init(RingBuffer *rbuff,uint8_t *pdata,uint32_t size);
RingBuffStatus ringbuffer_write_byte(RingBuffer *rbuff,uint8_t wdata);
RingBuffStatus ringbuffer_read_byte(RingBuffer *rbuff,uint8_t *rdata);

RingBuffStatus ringbuffer_empty(RingBuffer *rbuff);
RingBuffStatus ringbuffer_read_history_elements(RingBuffer *rbuff,uint8_t *rdata,uint8_t numdata);
#ifdef __cplusplus
 {
#endif /* __cplusplus */

#endif

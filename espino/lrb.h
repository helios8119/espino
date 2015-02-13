#ifndef _ULW_RING_BUFF_H_
#define _ULW_RING_BUFF_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp8266_peri.h"

#define BUFFER_SIZE         128
#define BUFFER_NEARLY_FULL	96
#define RingBuff_Data_t     uint8_t

#if (BUFFER_SIZE <= 0xFF)
	#define RingBuff_Count_t   uint8_t
#else
	#define RingBuff_Count_t   uint16_t
#endif

typedef struct {
	RingBuff_Data_t  Buffer[BUFFER_SIZE]; /**< Internal ring buffer data, referenced by the buffer pointers. */
	RingBuff_Data_t* In; /**< Current storage location in the circular buffer */
	RingBuff_Data_t* Out; /**< Current retrieval location in the circular buffer */
	RingBuff_Count_t Count;
} RingBuff_t;

static inline void RingBuffer_InitBuffer(RingBuff_t* const Buffer){
	uint32_t irq = irq_save();
	Buffer->In  = Buffer->Buffer;
	Buffer->Out = Buffer->Buffer;
	irq_restore(irq);
}

static inline RingBuff_Count_t RingBuffer_GetCount(RingBuff_t* const Buffer){
	RingBuff_Count_t Count;
	uint32_t irq = irq_save();
	Count = Buffer->Count;
	irq_restore(irq);
	return Count;
}

static inline bool RingBuffer_IsFull(RingBuff_t* const Buffer){
	return (RingBuffer_GetCount(Buffer) == BUFFER_SIZE);
}

static inline bool RingBuffer_IsEmpty(RingBuff_t* const Buffer){
	return (RingBuffer_GetCount(Buffer) == 0);
}

static inline void RingBuffer_Insert(RingBuff_t* const Buffer, const RingBuff_Data_t Data){
	*Buffer->In = Data;
	if (++Buffer->In == &Buffer->Buffer[BUFFER_SIZE]) Buffer->In = Buffer->Buffer;
	uint32_t irq = irq_save();
	Buffer->Count++;
	irq_restore(irq);
}

static inline RingBuff_Data_t RingBuffer_Remove(RingBuff_t* const Buffer){
	RingBuff_Data_t Data = *Buffer->Out;
	if (++Buffer->Out == &Buffer->Buffer[BUFFER_SIZE]) Buffer->Out = Buffer->Buffer;
	uint32_t irq = irq_save();
	Buffer->Count--;
	irq_restore(irq);
	return Data;
}

#endif

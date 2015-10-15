#pragma once
#include "RingBuffer.h"

class RingBufferConsumer
{
public:
	RingBufferConsumer(RingBuffer *pRingBuffer);
	~RingBufferConsumer();

	void process();


	RingBuffer *m_pRingBuffer;
};


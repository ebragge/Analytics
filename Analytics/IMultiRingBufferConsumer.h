#pragma once
#include "RingBuffer.h"

class IMultiRingBufferConsumer
{
public:
	virtual void ConsumeNewWindow(std::vector<RingBuffer::Window> &windows) = 0;
	virtual UINT WindowSize() = 0;
};

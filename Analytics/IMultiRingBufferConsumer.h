#pragma once
#include "RingBuffer.h"

class IMultiRingBufferConsumer
{
public:

	enum State { Started, ReStarted, Stopped };
//Setters
	virtual UINT ConsumeNewWindow(std::vector<RingBuffer::Window> &windows) = 0;
	virtual void SampleInformation(UINT cChannels, UINT wBitsPerSample, UINT nSamplesPerSec) = 0;
	virtual void StateChanged(State newState) = 0;

//Getter
	virtual UINT WindowSize() = 0;
	virtual bool Ready() = 0;
};

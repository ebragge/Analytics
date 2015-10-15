#pragma once
#include "MultiRingBuffer.h"
#include "IMultiRingBufferConsumer.h"
#include "AudioControlData.h"

class StreamAnalytics :
	public IMultiRingBufferConsumer
{
public:
	StreamAnalytics();
	~StreamAnalytics();
	void SetObserver(AudioControlData *pObserver);
	
	//From IRingBufferConsumer
	virtual void ConsumeNewWindow(std::vector<RingBuffer::Window> &windows);
	virtual UINT WindowSize();

protected:

	std::vector<INT16> m_vBuffer;
	UINT32 m_write;
	UINT32 m_nWindowSize;
	UINT32 m_counter;

protected:
	AudioControlData *m_pOBserver;
};

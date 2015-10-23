#pragma once
#include "MultiRingBuffer.h"
#include "IMultiRingBufferConsumer.h"
#include "AudioControlData.h"
#include "AudioActivityDetector.h"

class StreamAnalytics :
	public IMultiRingBufferConsumer
{
public:
	StreamAnalytics();
	~StreamAnalytics();
	void SetObserver(AudioControlData *pObserver);
	
	//From IRingBufferConsumer
	virtual UINT ConsumeNewWindow(std::vector<RingBuffer::Window> &windows);
	virtual void SampleInformation(UINT cChannels, UINT wBitsPerSample, UINT nSamplesPerSec);
	virtual void StateChanged(State newState);

	virtual UINT WindowSize();
	virtual bool Ready();

protected:

	std::vector<INT16> m_vBuffer;
	UINT32 m_write;
	UINT32 m_nWindowSize;
	UINT32 m_counter;
	UINT32 m_cAmplitudeSampleSize;
	UINT32 m_cDetections = 0;

protected:
	AudioControlData		*m_pObserver;
	AudioActivityDetector	*m_pAudioActivityDetector;

};

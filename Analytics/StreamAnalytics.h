#pragma once
#include "MultiRingBuffer.h"
#include "IMultiRingBufferConsumer.h"
#include "AudioControlData.h"
#include "AudioActivityDetector.h"
#include "ChannelDeltaEstimator.h"

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

	std::vector<double> m_vBuffer;
	std::vector<double> m_vBuffer2;
	UINT32 m_write;
	UINT32 m_nWindowSize;
	UINT32 m_counter;
	UINT32 m_cAmplitudeSampleSize;
	UINT32 m_cDetections = 0;

protected:
	AudioControlData		*m_pObserver;
	AudioActivityDetector	*m_pAudioActivityDetector;
	ChannelDeltaEstimator	*m_pChannelDeltaEstimator;
};

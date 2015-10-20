#pragma once
#include "IMultiRingBufferProducer.h"
#include "IMultiRingBufferConsumer.h"
#include "IRingBufferConcurrency.h"
#include "RingBuffer.h"

class MultiRingBuffer:
	public IMultiRingBufferProducer
{
public:
	MultiRingBuffer();
	virtual ~MultiRingBuffer();
	void SetConsumer(IMultiRingBufferConsumer *pEventHandler);
	
//From IMultiRingBufferConsumer
	virtual void AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels);
	virtual void AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels, UINT64 u64DevicePosition, UINT64 u64QPCPosition, bool bDiscontinuity = false);
	virtual void AddChannels(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample,UINT cBufferSize, std::vector<UINT> &rChannels);

protected:
	UINT GetMaxWindowSize();
	virtual void AddDataInternal(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels);
	virtual void AddChannelsInternal(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels);

	std::thread ConsumerThread();
	std::vector<RingBuffer> m_vBuffer;	//Holds data 

	UINT m_cBufferSize;
	UINT m_cBytesPerSample;
	IMultiRingBufferConsumer *m_pEventHandler;
	std::condition_variable m_cvNewDataSignal;
	std::mutex mMutex;
	std::thread m_hConsumerThread;
	volatile std::atomic<bool> m_bRunThread;
	UINT m_samplesPerSecond;
	bool m_bSampleInformationCalled = false;
};


#pragma once
#include "MultiRingBuffer.h"

private enum class SyncRingBufferState
{
	InSync,
	OutOfSync
};

//SyncRingBuffer stores data before it is synced and passed to the ring buffer
class SyncMultiRingBuffer :
	public MultiRingBuffer
{
public:
	SyncMultiRingBuffer();
	virtual ~SyncMultiRingBuffer();
	virtual void AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels, UINT64 u64DevicePosition, UINT64 u64QPCPosition, bool bDiscontinuity = false);
	virtual void AddChannels(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels);

private:
	std::vector<UINT64> m_vInitQPCPositionPerChannel;
	std::vector<UINT64>	m_vInitPositionPerChannel;
	SyncRingBufferState m_eState;
	
};


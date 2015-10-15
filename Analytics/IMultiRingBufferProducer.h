#pragma once
class IMultiRingBufferProducer
{
public:
	virtual void AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels) = 0;
	virtual void AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels, UINT64 u64DevicePosition, UINT64 u64QPCPosition, bool bDiscontinuity = false) = 0;
	virtual void AddChannels(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels) = 0;
};


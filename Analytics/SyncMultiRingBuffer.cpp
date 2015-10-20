#include "pch.h"
#include "SyncMultiRingBuffer.h"


SyncMultiRingBuffer::SyncMultiRingBuffer() :
	m_eState(SyncRingBufferState::OutOfSync)
{
	m_vInitQPCPositionPerChannel = std::vector<UINT64>(0);
	m_vInitPositionPerChannel = std::vector<UINT64>(0);
}

SyncMultiRingBuffer::~SyncMultiRingBuffer()
{

}

void SyncMultiRingBuffer::AddChannels(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels)
{
	std::unique_lock<std::mutex> lock(mMutex);

	for (size_t i = 0; i < cChannels; i++)
	{
		m_vInitQPCPositionPerChannel.push_back(0);
		m_vInitPositionPerChannel.push_back(0);
	}
	
	AddChannelsInternal(cChannels, cSamplesPerSecond, cBytesPerSample, cBufferSize, rChannels);
}

void SyncMultiRingBuffer::AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels, UINT64 u64Position, UINT64 u64QPCPosition, bool bDiscontinuity)
{
	std::unique_lock<std::mutex> lock(mMutex);
	
	if (bDiscontinuity && m_eState == SyncRingBufferState::InSync)
	{
		//We clean ring buffers when discontinuity happens
		for (size_t i = 0; i < m_vBuffer.size(); i++)
		{
			m_vBuffer[i].ClearBuffer();
			m_vInitQPCPositionPerChannel[i] = 0;
			m_vInitPositionPerChannel[i] = 0;
		}
		m_eState = SyncRingBufferState::OutOfSync;
	} 

	//Calls parent AddData
	MultiRingBuffer::AddDataInternal(pData, cbBytes, cChannels, rChannels);

	if (m_eState == SyncRingBufferState::OutOfSync)
	{
		for (size_t i = 0; i < rChannels.size(); i++)
		{
			if (m_vInitQPCPositionPerChannel[rChannels[i]] == 0)
			{
				m_vInitQPCPositionPerChannel[rChannels[i]] = u64QPCPosition;
				m_vInitPositionPerChannel[rChannels[i]] = u64Position;
			}
		}

		bool doSync = true;

		for (size_t i = 0; i < m_vInitQPCPositionPerChannel.size() && doSync; i++)
		{
			if (m_vInitPositionPerChannel[i] == 0)
				doSync = false;
		}

		if (doSync)
		{
			UINT iLastChannel = 0;
			UINT64 last = 0;

			//First we find the channel which received data as last.
			for (size_t i = 0; i < m_vInitQPCPositionPerChannel.size(); i++)
			{
				if (m_vInitQPCPositionPerChannel[i] > last)
				{
					last = m_vInitQPCPositionPerChannel[i];
					iLastChannel = i;
				}
			}

			std::vector<INT64> posDeltas = std::vector<INT64>(m_vBuffer.size());
			std::vector<INT> rDeltas = std::vector<INT>(m_vBuffer.size());

			for (size_t i = 0; i < m_vInitQPCPositionPerChannel.size(); i++)
			{
				if (iLastChannel != i)
				{
					INT64 QPCDelta = static_cast<INT64>(m_vInitQPCPositionPerChannel[i] - m_vInitQPCPositionPerChannel[iLastChannel]);
					double QCPDeltaD = QPCDelta / 1000;
					rDeltas[i] = (QCPDeltaD * (double)m_samplesPerSecond) / 10000;
				}
				else
				{
					rDeltas[i] = 0;
				}
			}

			for (size_t i = 0; i < rDeltas.size(); i++)
			{
				m_vBuffer[i].MoveWindow(rDeltas[i]);
			}

			m_eState = SyncRingBufferState::InSync;
		}
	}
	else
	{

	}

}
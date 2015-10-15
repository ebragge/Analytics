#include "pch.h"
#include "SyncMultiRingBuffer.h"


SyncMultiRingBuffer::SyncMultiRingBuffer()
{
	m_state = SyncRingBufferState::OutOfSync;
	//m_samplesPerSecond = 44100;
	m_InitQPCPositionPerChannel = std::vector<UINT64>(0);
	m_InitPositionPerChannel = std::vector<UINT64>(0);

}

SyncMultiRingBuffer::~SyncMultiRingBuffer()
{

}

void SyncMultiRingBuffer::AddChannels(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels)
{
	std::unique_lock<std::mutex> lock(mMutex);

	for (size_t i = 0; i < cChannels; i++)
	{
		m_InitQPCPositionPerChannel.push_back(0);
		m_InitPositionPerChannel.push_back(0);
	}
	
	AddChannelsInternal(cChannels, cSamplesPerSecond, cBytesPerSample, cBufferSize, rChannels);
}

void SyncMultiRingBuffer::AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels, UINT64 u64Position, UINT64 u64QPCPosition, bool bDiscontinuity)
{
	std::unique_lock<std::mutex> lock(mMutex);
	
	if (bDiscontinuity && m_state == SyncRingBufferState::InSync)
	{
		//We clean ring buffers when discontinuity happens
		for (size_t i = 0; i < m_vBuffer.size(); i++)
		{
			m_vBuffer[i].ClearBuffer();
			m_InitQPCPositionPerChannel[i] = 0;
			m_InitPositionPerChannel[i] = 0;
		}
		m_state = SyncRingBufferState::OutOfSync;
	} 

	//Calls parent AddData
	MultiRingBuffer::AddDataInternal(pData, cbBytes, cChannels, rChannels);

	if (m_state == SyncRingBufferState::OutOfSync)
	{
		for (size_t i = 0; i < rChannels.size(); i++)
		{
			if (m_InitQPCPositionPerChannel[rChannels[i]] == 0)
			{
				m_InitQPCPositionPerChannel[rChannels[i]] = u64QPCPosition;
				m_InitPositionPerChannel[rChannels[i]] = u64Position;
			}
		}

		bool doSync = true;

		for (size_t i = 0; i < m_InitQPCPositionPerChannel.size() && doSync; i++)
		{
			if (m_InitPositionPerChannel[i] == 0)
				doSync = false;
		}

		if (doSync)
		{
			UINT iLastChannel = 0;
			UINT64 last = 0;

			//First we find the channel which received data as last.
			for (size_t i = 0; i < m_InitQPCPositionPerChannel.size(); i++)
			{
				if (m_InitQPCPositionPerChannel[i] > last)
				{
					last = m_InitQPCPositionPerChannel[i];
					iLastChannel = i;
				}
			}

			std::vector<INT64> posDeltas = std::vector<INT64>(m_vBuffer.size());
			std::vector<INT> rDeltas = std::vector<INT>(m_vBuffer.size());

			for (size_t i = 0; i < m_InitQPCPositionPerChannel.size(); i++)
			{
				if (iLastChannel != i)
				{
					INT64 QPCDelta = static_cast<INT64>(m_InitQPCPositionPerChannel[i] - m_InitQPCPositionPerChannel[iLastChannel]);
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

			m_state = SyncRingBufferState::InSync;
		}
	}
	else
	{

	}

	


	


	if (m_state != SyncRingBufferState::InSync)
	{
		/*
		//Streams are not in sync. Let's try to sync them.
		//This means that we remove / shift items from channels that are ahead.
		UINT iBiggestChannel = 0;
		UINT64 biggest = 0;

		//First we find an item that came as last.
		for (size_t i = 0; i < m_LastuQPCPositionPerChannel.size(); i++)
		{
			if (m_LastuQPCPositionPerChannel[i] > biggest)
			{
				biggest = m_LastuQPCPositionPerChannel[i];
				iBiggestChannel = i;
			}
		}

		std::vector<int> rDeltas = std::vector<int>(m_vBuffer.size());

		for (size_t i = 0; i < m_LastuQPCPositionPerChannel.size(); i++)
		{
			if (iBiggestChannel != i)
			{
				INT64 QPCDelta = static_cast<INT64>(m_LastuQPCPositionPerChannel[i] - m_LastuQPCPositionPerChannel[iBiggestChannel]);
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
			//ShiftChannel(rDeltas[i], i);
		}
		*/
	}

}
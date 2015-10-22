#include "pch.h"
#include "MultiRingBuffer.h"
#include <mutex>

using namespace Platform;


MultiRingBuffer::MultiRingBuffer()
{
	m_vBuffer = std::vector<RingBuffer>(0);
	m_samplesPerSecond = 0;
}

void MultiRingBuffer::AddChannels(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels)
{
	std::unique_lock<std::mutex> lock(mMutex);
	AddChannelsInternal(cChannels, cSamplesPerSecond, cBytesPerSample, cBufferSize, rChannels);
}

void MultiRingBuffer::AddChannelsInternal(UINT cChannels, UINT cSamplesPerSecond, UINT cBytesPerSample, UINT cBufferSize, std::vector<UINT> &rChannels)
{

	if (m_samplesPerSecond != 0 && cSamplesPerSecond != m_samplesPerSecond ||
		m_cBytesPerSample != 0 && m_cBytesPerSample != cBytesPerSample ||
		m_cBufferSize != 0 && cBufferSize != m_cBufferSize)
	{
		throw "Sample configurations are not same";
	}
	else
	{
		m_samplesPerSecond = cSamplesPerSecond;
		m_cBytesPerSample = cBytesPerSample;
		m_cBufferSize = cBufferSize;
	}

	size_t oldSize = m_vBuffer.size();

	for (size_t i = 0; i < cChannels; i++)
	{
		m_vBuffer.push_back(RingBuffer(cBufferSize));
		rChannels.push_back(oldSize + i);
	}
}

MultiRingBuffer::~MultiRingBuffer()
{
	m_bRunThread = false;
	m_cvNewDataSignal.notify_all();
	m_hConsumerThread.join();
}


void MultiRingBuffer::SetConsumer(IMultiRingBufferConsumer *pEventHandler)
{
	m_pEventHandler = pEventHandler;
	m_bRunThread = true;
	m_hConsumerThread = ConsumerThread();
}


void MultiRingBuffer::AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels)
{
	std::unique_lock<std::mutex> lock(mMutex);
	AddDataInternal(pData, cbBytes, cChannels, rChannels);
}

void MultiRingBuffer::AddDataInternal(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels)
{
	if (cbBytes > 0)
	{
		DWORD numPoints = cbBytes / cChannels / m_cBytesPerSample;
		INT16 *pi16 = (INT16*)pData;

		for (DWORD i = 0; i < numPoints; i++)
		{
			for (int j = 0; j < m_vBuffer.size(); j++)
			{
				m_vBuffer[j].WriteNext(*pi16);
				pi16++;
			}
		}
		m_cvNewDataSignal.notify_all();
	}
}


void MultiRingBuffer::AddData(BYTE* pData, DWORD cbBytes, UINT cChannels, std::vector<UINT> rChannels, UINT64 u64DevicePosition, UINT64 u64QPCPosition, bool bDiscontinuity)
{
	throw "AddData is not implemented for MultiRingBuffer";
}


UINT MultiRingBuffer::GetMaxWindowSize()
{
	UINT smallest = m_cBufferSize;

	for (size_t i = 0; i < m_vBuffer.size() && smallest > 0; i++)
	{
		if (smallest > m_vBuffer[i].GetMaxWindowSize())
		{
			smallest = m_vBuffer[i].GetMaxWindowSize();
		}
	}

	return smallest;
}

std::thread MultiRingBuffer::ConsumerThread()
{
	return std::thread([=] 
	{ 
		while (m_bRunThread)
		{
			std::unique_lock<std::mutex> lock(mMutex);
			m_cvNewDataSignal.wait(lock);
			
			if (m_pEventHandler != nullptr)
			{
				if (m_bRunThread)
				{
					if (!m_bSampleInformationCalled)
					{
						m_pEventHandler->SampleInformation(m_vBuffer.size(), m_cBytesPerSample * 8, m_samplesPerSecond);
						m_pEventHandler->StateChanged(IMultiRingBufferConsumer::State::Started);
						m_bSampleInformationCalled = true;
					}

					if (m_pEventHandler->Ready())
					{
						while (GetMaxWindowSize() > m_pEventHandler->WindowSize())
						{
							std::vector<RingBuffer::Window> windows = std::vector<RingBuffer::Window>();
							for (size_t i = 0; i < m_vBuffer.size(); i++)
							{
								windows.push_back(m_vBuffer[i].GetWindow(m_pEventHandler->WindowSize()));
							}

							UINT consumed = m_pEventHandler->ConsumeNewWindow(windows);

							for (size_t i = 0; i < m_vBuffer.size(); i++)
							{
								m_vBuffer[i].MoveWindow(consumed);
							}
						}
					}
				}
			}
		}
		m_pEventHandler->StateChanged(IMultiRingBufferConsumer::State::Stopped);
		OutputDebugString(L"ConsumerThread killed\n");
	});
}
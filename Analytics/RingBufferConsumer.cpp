#include "pch.h"
#include "RingBufferConsumer.h"
#include "AudioLevel.h"

using namespace Platform;


RingBufferConsumer::RingBufferConsumer(RingBuffer *pRingBuffer)
{
	m_pRingBuffer = pRingBuffer;
}


RingBufferConsumer::~RingBufferConsumer()
{

}
void RingBufferConsumer::process()
{
	{
		while (true)
		{
			m_pRingBuffer->Wait();

			AudioLevel level(2,200);
			m_pRingBuffer->RunAlgorithm(level);

			String^ str1 = String::Concat(double(level.ChannelLevels[0]).ToString(), "-ChannelLevels\n");
			OutputDebugString(str1->Data());

			String^ str2 = String::Concat(double(level.Amplitude[0]).ToString(), "-AMP\n");
			OutputDebugString(str2->Data());

			if (level.ReduceWindow() > 0)
			{
				OutputDebugString(L"deleted\n");
			}

		}
	}
}
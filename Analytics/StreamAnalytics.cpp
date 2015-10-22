#include "pch.h"
#include "StreamAnalytics.h"
#include "AudioControlData.h"

using namespace Platform;

StreamAnalytics::StreamAnalytics()
{
	m_vBuffer = std::vector<INT16>(10,0);
	m_counter = 0;
}

StreamAnalytics::~StreamAnalytics()
{

}

void StreamAnalytics::SetObserver(AudioControlData *pObserver)
{
	m_pObserver = pObserver;
}

UINT StreamAnalytics::WindowSize()
{
	return 200;
}

//RingBuffer calls this function when it has data. 
//This function should finish relatively quickly.
UINT StreamAnalytics::ConsumeNewWindow(std::vector<RingBuffer::Window> &windows)
{
	m_counter++;
	double amplitude = 0;

	for (size_t i = 0; i < WindowSize() ; i++)
	{
		amplitude += ((double)std::abs(windows[0][i])) / 32000;
	}

	m_write = ++m_write < m_vBuffer.size() ?
		m_write :
		0;

	if (m_write == 0)
	{
		m_pObserver->AddData(m_vBuffer);
	}

	m_vBuffer[m_write] = amplitude;
	
	if (amplitude > 10 && m_counter > 100)
	{
		m_counter = 0;
		for (size_t i = 0; i < windows.size(); i++)
		{
			m_pObserver->SetGraph(i, windows[i].getWindow());
		}
	}
	return WindowSize();
}

void StreamAnalytics::SampleInformation(UINT cChannels, UINT wBitsPerSample, UINT nSamplesPerSec)
{

}
void StreamAnalytics::StateChanged(State newState)
{

}

bool StreamAnalytics::Ready()
{
	return true;
}

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
	m_pOBserver = pObserver;
}

UINT StreamAnalytics::WindowSize()
{
	return 200;
}

//RingBuffer calls this function when it has data. 
//This function has to finish quickly.
void StreamAnalytics::ConsumeNewWindow(std::vector<RingBuffer::Window> &windows)
{
	m_counter++;
	double Amplitude = 0;
	double Levels = 0;

	for (size_t i = 0; i < WindowSize() ; i++)
	{
		Amplitude += ((double)std::abs(windows[0][i])) / 32000;
	}

	m_write = ++m_write < m_vBuffer.size() ?
		m_write :
		0;

	if (m_write == 0)
	{
		m_pOBserver->AddData(m_vBuffer);
	}

	m_vBuffer[m_write] = Amplitude;
	
	if (Amplitude > 10 && m_counter > 100)
	{
		m_counter = 0;
		for (size_t i = 0; i < windows.size(); i++)
		{
			m_pOBserver->SetGraph(i, windows[i].getWindow());
		}
	}
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

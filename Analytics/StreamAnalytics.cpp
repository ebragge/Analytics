#include "pch.h"
#include "StreamAnalytics.h"
#include "AudioControlData.h"

using namespace Platform;

StreamAnalytics::StreamAnalytics()
{
	m_vBuffer = std::vector<double>(10,0);
	m_counter = 0;
	m_pAudioActivityDetector = new AudioActivityDetector();
}

StreamAnalytics::~StreamAnalytics()
{
	delete m_pAudioActivityDetector;
}

void StreamAnalytics::SetObserver(AudioControlData *pObserver)
{
	m_pObserver = pObserver;
}

UINT StreamAnalytics::WindowSize()
{
	return m_cAmplitudeSampleSize;
}

//RingBuffer calls this function when it has data. 
//This function should finish relatively quickly.
UINT StreamAnalytics::ConsumeNewWindow(std::vector<RingBuffer::Window> &windows)
{
	m_counter++;
	
	double amplitudeAvg = 0; //amplitude average for 10ms period

	for (size_t i = 0; i < WindowSize() ; i++)
	{
		amplitudeAvg += ((double)std::abs(windows[0][i])) / 32000;
	}

	amplitudeAvg /= m_cAmplitudeSampleSize;

	bool changed = m_pAudioActivityDetector->DoDetection(amplitudeAvg);

	m_write = ++m_write < m_vBuffer.size() ?
		m_write :
		0;

	if (m_pObserver->Lock())
	{
		if (m_write == 0)
		{
			m_pObserver->AddMovingGraphData(0,m_vBuffer);
		}

		m_vBuffer[m_write] = amplitudeAvg;

		if (changed && m_pAudioActivityDetector->IsActive())
		{
			m_cDetections++;
		}
		
		if (m_cDetections %10 == 1)
		{
			std::map<std::wstring, UINT> map = std::map<std::wstring, UINT>();
			map[L"Detections"] = m_cDetections;
			m_pObserver->SetMap(map);
		}

		if (changed && m_counter > 100)
		{
			m_counter = 0;
			for (size_t i = 0; i < windows.size(); i++)
			{
				m_pObserver->SetStaticGraph(i, windows[i].getWindow());
			}
		}
		m_pObserver->Unlock();
	}
	return m_cAmplitudeSampleSize;
}

void StreamAnalytics::SampleInformation(UINT cChannels, UINT wBitsPerSample, UINT nSamplesPerSec)
{
	m_cAmplitudeSampleSize = nSamplesPerSec / 100; // 10ms 
}

void StreamAnalytics::StateChanged(State newState)
{

}

bool StreamAnalytics::Ready()
{
	return true;
}

#include "pch.h"
#include "AudioControlData.h"


AudioControlData::AudioControlData()
{
	m_vMovingGraphs = std::vector<std::vector<double>>();
	m_vMovingGraphsMax = std::vector<double>();
	m_vMovingGraphsCounter = std::vector<UINT>();

	m_vStaticGraphs = std::vector<std::vector<INT16>>();
	m_arrMap = std::map<std::wstring, UINT>();
}


AudioControlData::~AudioControlData()
{

}

void AudioControlData::AddMovingGraphData(UINT ch, std::vector<double> &vData)
{
	if (m_vMovingGraphs.size() <= ch)
	{
		m_vMovingGraphs.push_back(std::vector<double>(1000));
		m_vMovingGraphsCounter.push_back(0);
		m_vMovingGraphsMax.push_back(0.01);
	}

	for (size_t i = 0; i < vData.size(); i++)
	{
		m_vMovingGraphsCounter[ch] = ++m_vMovingGraphsCounter[ch] < m_vMovingGraphs[ch].size() ?
			m_vMovingGraphsCounter[ch] :
			0;

		m_vMovingGraphs[ch][m_vMovingGraphsCounter[ch]] = vData[i];

		if (vData[i] > m_vMovingGraphsMax[ch])
		{
			m_vMovingGraphsMax[ch] = vData[i];
		}
	}
}

std::vector<double> const & AudioControlData::GetMovingGraphData(UINT ch, UINT &startPos, double &dMax)
{
	if (ch >= m_vMovingGraphs.size())
	{
		m_vMovingGraphs.push_back(std::vector<double>(1000));
		m_vMovingGraphsCounter.push_back(0);
		m_vMovingGraphsMax.push_back(0.01);
		return m_vMovingGraphs[ch];
	}
	else
	{
		dMax = m_vMovingGraphsMax[ch];
		startPos = (UINT)m_vMovingGraphsCounter[ch];
		return m_vMovingGraphs[ch];
	}
}

void AudioControlData::SetMap(std::map<std::wstring, UINT> &arrCounters)
{
	m_arrMap = std::map<std::wstring, UINT>(arrCounters);
}

std::map<std::wstring, UINT> const & AudioControlData::GetMap() const
{
	return m_arrMap;
}

void AudioControlData::SetStaticGraph(UINT ch, const std::vector<INT16> &data)
{
	if (m_vStaticGraphs.size() > ch)
	{
		m_vStaticGraphs[ch] = data;
	}
	else
	{
		while (m_vStaticGraphs.size() <= ch)
		{
			m_vStaticGraphs.push_back(std::vector<INT16>());
		}
		m_vStaticGraphs[ch] = data;
	}
}

std::vector<std::vector<INT16>> const & AudioControlData::GetStaticGraphs() const
{
	return m_vStaticGraphs;
}

bool AudioControlData::Lock()
{
	using Ms = std::chrono::milliseconds;
	return m_mMutex.try_lock_for(Ms(100));
}

void AudioControlData::Unlock()
{
	m_mMutex.unlock();
}

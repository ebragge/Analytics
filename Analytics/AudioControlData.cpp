#include "pch.h"
#include "AudioControlData.h"


AudioControlData::AudioControlData()
{
	m_vBuffer = std::vector<int16>(1000, 0);
	m_vGraphs = std::vector<std::vector<INT16>>();

	m_write = 0;
}


AudioControlData::~AudioControlData()
{

}

void AudioControlData::AddData(std::vector<INT16> &data)
{
	std::unique_lock<std::mutex> lock(mMutex);

	for (size_t i = 0; i < data.size(); i++)
	{
		AddData(data[i]);
	}
}

void AudioControlData::AddData(INT16 data)
{
	m_write = ++m_write < 1000 ?
		m_write :
		0;

	if (m_write == 0)
	{
		m_vBuffer = std::vector<int16>(1000, 0);
	}
	m_vBuffer[m_write] = data;
}

std::vector<INT16> AudioControlData::GetData()
{
	std::unique_lock<std::mutex> lock(mMutex);
	std::vector<INT16> vect(m_vBuffer);

	return vect;
}

void AudioControlData::SetGraph(UINT ch, const std::vector<INT16> &data)
{
	std::unique_lock<std::mutex> lock(mMutex);
	if (m_vGraphs.size() > ch)
	{
		m_vGraphs[ch] = data;
	}
	else
	{
		while (m_vGraphs.size() <= ch)
		{
			m_vGraphs.push_back(std::vector<INT16>());
		}
		m_vGraphs[ch] = data;
	}
}

std::vector<INT16> AudioControlData::GetGraph(UINT ch)
{
	std::unique_lock<std::mutex> lock(mMutex);
	if (m_vGraphs.size() > ch)
	{
		std::vector<INT16> vect(m_vGraphs[ch]);
		return vect;
	}
}

UINT AudioControlData::GetNumberOfGraphs()
{
	std::unique_lock<std::mutex> lock(mMutex);
	return m_vGraphs.size();
}
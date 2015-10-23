#include "pch.h"
#include "AudioControlData.h"


AudioControlData::AudioControlData()
{
	m_vBuffer = std::vector<int16>(1000, 0);
	m_vGraphs = std::vector<std::vector<INT16>>();
	m_arrMap = std::map<std::wstring, UINT>();
	m_write = 0;
}


AudioControlData::~AudioControlData()
{

}

void AudioControlData::AddData(std::vector<INT16> &vData)
{
	std::unique_lock<std::mutex> lock(mMutex);

	for (size_t i = 0; i < vData.size(); i++)
	{
		m_write = ++m_write < m_vBuffer.size() ?
			m_write :
			0;

		m_vBuffer[m_write] = vData[i];
	}
}

std::vector<INT16> AudioControlData::GetData(UINT &startPos)
{
	std::unique_lock<std::mutex> lock(mMutex);
	std::vector<INT16> vect(m_vBuffer);
	startPos = (UINT)m_write;
	return vect;
}

void AudioControlData::SetMapVal(std::wstring name, UINT val)
{
	std::unique_lock<std::mutex> lock(mMutex);
	//Not yet implemented
}

void AudioControlData::SetMap(std::map<std::wstring, UINT> &arrCounters)
{
	std::unique_lock<std::mutex> lock(mMutex);
	m_arrMap = std::map<std::wstring, UINT>(arrCounters);
}

std::map<std::wstring, UINT> AudioControlData::GetMap()
{
	std::unique_lock<std::mutex> lock(mMutex);
	return m_arrMap;
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
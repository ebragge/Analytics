#pragma once
#include <mutex>
#include <thread>
#include <vector>

class AudioControlData
{
public:
	AudioControlData();
	~AudioControlData();
	
	void AddMovingGraphData(UINT ch, std::vector<double> &vData);
	std::vector<double> const & GetMovingGraphData(UINT ch, UINT &startPos, double &dMax);
	
	void SetStaticGraph(UINT ch, const std::vector<INT16> &data);
	std::vector<std::vector<INT16>> const & GetStaticGraphs() const;

	void SetMap(std::map<std::wstring, UINT> &arrCounters);
	std::map<std::wstring, UINT> const & GetMap() const;

	bool Lock();
	void Unlock();
	
private:
	void AddData(INT16 iData);

	std::vector<UINT> m_vMovingGraphsCounter;
	std::vector<std::vector<double>> m_vMovingGraphs;
	std::vector<double> m_vMovingGraphsMax;
	std::vector<std::vector<INT16>> m_vStaticGraphs;
	std::map<std::wstring, UINT> m_arrMap;

private:
	std::timed_mutex m_mMutex;
};


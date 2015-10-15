#pragma once
class AudioControlData
{
public:
	AudioControlData();
	~AudioControlData();
	
	void AddData(std::vector<INT16> &data);
	void AddData(INT16);
	std::vector<INT16> GetData();

	void SetGraph(UINT ch, const std::vector<INT16> &data);
	std::vector<INT16> GetGraph(UINT ch);
	UINT GetNumberOfGraphs();
	
	uint32 m_write;
	std::vector<INT16> m_vBuffer;
	std::vector<std::vector<INT16>> m_vGraphs;

private:
	std::mutex mMutex;
};


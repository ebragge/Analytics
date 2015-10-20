#pragma once
class AudioControlData
{
public:
	AudioControlData();
	~AudioControlData();
	
	void AddData(std::vector<INT16> &vData);
	void AddData(INT16 iData);
	std::vector<INT16> GetData(UINT &startPos);

	void SetGraph(UINT ch, const std::vector<INT16> &data);
	std::vector<INT16> GetGraph(UINT ch);
	UINT GetNumberOfGraphs();
	
	uint32 m_write;
	std::vector<INT16> m_vBuffer;
	std::vector<std::vector<INT16>> m_vGraphs;

private:
	std::mutex mMutex;
};


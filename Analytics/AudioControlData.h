#pragma once
class AudioControlData
{
public:
	AudioControlData();
	~AudioControlData();
	
	void AddData(std::vector<INT16> &vData);
	std::vector<INT16> GetData(UINT &startPos);

	void SetMap(std::map<std::wstring, UINT> &arrCounters);
	void SetMapVal(std::wstring name, UINT val);
	std::map<std::wstring, UINT> GetMap();

	void SetGraph(UINT ch, const std::vector<INT16> &data);
	std::vector<INT16> GetGraph(UINT ch);
	UINT GetNumberOfGraphs();
	
private:
	void AddData(INT16 iData);

	UINT m_write;
	std::vector<INT16> m_vBuffer;
	std::vector<std::vector<INT16>> m_vGraphs;
	std::map<std::wstring, UINT> m_arrMap;

private:
	std::mutex mMutex;
};


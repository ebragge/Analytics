#include "pch.h"
#include "ChannelDeltaEstimator.h"


ChannelDeltaEstimator::ChannelDeltaEstimator()
{
}


ChannelDeltaEstimator::~ChannelDeltaEstimator()
{
}

INT16 ChannelDeltaEstimator::CalculateCrossCorrelation(std::vector<RingBuffer::Window> &windows, UINT uiWindowSize)
{
	const UINT uMaxDelay = 15;

	std::vector<INT32> answer = std::vector<INT32>(uMaxDelay*2);
	UINT ch1 = 0; //f
	UINT ch2 = windows.size() - 1; //g
	
	for (size_t i = 0; i < answer.size(); i++)
	{
		for (size_t j = 0; j < uiWindowSize - uMaxDelay * 2; j++)
		{
			answer[i] += windows[ch1][j + uMaxDelay] * windows[ch2][i + j];
		}
	}

	size_t largest = 0;

	for (size_t i = 0; i < answer.size(); i++)
	{
		if (answer[i] > answer[largest]) 
		{
			largest = i;
		}
	}
	
	return largest - uMaxDelay;
}
#pragma once
#include "RingBuffer.h"

class ChannelDeltaEstimator
{
public:
	ChannelDeltaEstimator();
	~ChannelDeltaEstimator();
	void CalculateCrossCorrelation(std::vector<INT16> &a, std::vector<INT16> &b, std::vector<INT32> &answer);

	INT16 CalculateCrossCorrelation(std::vector<RingBuffer::Window> &windows, UINT uiWindowSize);


	
};


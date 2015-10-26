#pragma once
#include <deque>

private enum class ActivityState
{
	NoActivity,
	Activity
};

class AudioActivityDetector
{
public:
	AudioActivityDetector();
	~AudioActivityDetector();

	bool DoDetection(double dAmplitude);
	bool IsActive();

private:
	std::deque<double> m_vAmplitudeHistory;
	const UINT m_cMovingAvgWindow = 10;
	double m_dMovingAvg = 0.0;
	double m_dStdDev = 0.0;
	double m_dVariance = 0.0;
	double m_dLowLimit = 0.0;
	ActivityState m_eState;
};


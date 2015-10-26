#include "pch.h"
#include "AudioActivityDetector.h"


AudioActivityDetector::AudioActivityDetector() :
	m_eState(ActivityState::NoActivity),
	m_dMovingAvg(0.0),
	m_dStdDev(0.0),
	m_dVariance(0.0)
{

}


AudioActivityDetector::~AudioActivityDetector()
{

}

bool AudioActivityDetector::IsActive()
{
	return (m_eState == ActivityState::Activity);
}

bool AudioActivityDetector::DoDetection(double dAmplitude)
{
	double dMovingAvg = 0.0, dStdDev = 0.0, dVariance = 0.0;

	m_vAmplitudeHistory.push_front(dAmplitude);
	
	if (m_vAmplitudeHistory.size() > m_cMovingAvgWindow)
		m_vAmplitudeHistory.pop_back();

	for (size_t i = 0; i < m_vAmplitudeHistory.size(); i++)
	{
		dMovingAvg += m_vAmplitudeHistory[i];
	}

	dMovingAvg /= m_vAmplitudeHistory.size();

	for (size_t i = 0; i < m_vAmplitudeHistory.size(); ++i)
	{
		dVariance += (m_vAmplitudeHistory[i] - dMovingAvg)*(m_vAmplitudeHistory[i] - dMovingAvg);
	}
	
	dVariance /= m_vAmplitudeHistory.size();

	dStdDev = sqrt(dVariance);
	
	bool bActivityChanged = false;

	if (m_eState == ActivityState::NoActivity && 
		m_vAmplitudeHistory.size() == m_cMovingAvgWindow &&
		dAmplitude > (m_dMovingAvg * 3 * m_dStdDev))
	{
		m_eState = ActivityState::Activity;
		bActivityChanged = true;
		m_dLowLimit = (m_dMovingAvg);
	}
	else if (m_eState == ActivityState::Activity && 
		dAmplitude < m_dLowLimit) 
	{
		m_eState = ActivityState::NoActivity;
		bActivityChanged = true;
	}

	m_dMovingAvg = dMovingAvg;
	m_dStdDev = dStdDev;
	m_dVariance = dVariance;
	return bActivityChanged;

}

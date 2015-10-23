#include "pch.h"
#include "AudioActivityDetector.h"


AudioActivityDetector::AudioActivityDetector()
{

}


AudioActivityDetector::~AudioActivityDetector()
{

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

	//If new amplitude is 3*std(t-1) then we have detected audio activity
	if (dAmplitude > m_dMovingAvg*3*dStdDev && m_vAmplitudeHistory.size() == m_cMovingAvgWindow)
	{
		m_dMovingAvg = dMovingAvg;
		m_dStdDev = dStdDev;
		m_dVariance = dVariance;
		return true;
	}
	else
	{
		m_dMovingAvg = dMovingAvg;
		m_dStdDev = dStdDev;
		m_dVariance = dVariance;
		return false;
	}
}

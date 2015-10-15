#pragma once
#include "WASAPIRenderer.h"
#include "StreamAnalytics.h"
#include "AudioControlData.h"
#include "EngineParent.h"

using namespace WASAPIAudio;

ref class RecordingAnalyticsEngine sealed
	: EngineParent
{
public:
	RecordingAnalyticsEngine();
	void AnalyzeFile(IRandomAccessStream^ contentStream);
	virtual void StopAsync() override;

private:
	void OnDeviceStateChange(Object^ sender, DeviceStateChangedEventArgs^ e);
	void InitializeStreamAnalyser();
	
private:
internal:
	Windows::Foundation::EventRegistrationToken  m_deviceStateChangeToken;
	DeviceStateChangedEvent^ m_StateChangedEvent;
	DEVICEPROPS m_deviceProperties;
	ComPtr<WASAPIRenderer> m_spRenderer;
	StreamAnalytics *m_streamAnalytics;
	MultiRingBuffer *m_pRingBuffer;
};


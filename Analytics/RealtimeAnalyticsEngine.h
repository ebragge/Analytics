#pragma once
#include "EngineParent.h"
#include <ppltasks.h>
#include "WASAPICapture.h"
#include "SyncMultiRingBuffer.h"
#include "StreamAnalytics.h"


using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Media::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Platform::Collections;
using namespace WASAPIAudio;

static Platform::String^ PKEY_AudioEndpoint_Supports_EventDriven_Mode = "{1da5d803-d492-4edd-8c23-e0c0ffee7f0e} 7";

ref class RealtimeAnalyticsEngine sealed
	: EngineParent
{
public:
	RealtimeAnalyticsEngine();
	IAsyncAction^ InitializeAsync();
	virtual void StopAsync() override;

protected:
	void OnDeviceStateChange(Object^ sender, DeviceStateChangedEventArgs^ e);

private:
	std::vector< ComPtr<WASAPICapture>> m_vDevices;
	SyncMultiRingBuffer *m_pRingBuffer;
};


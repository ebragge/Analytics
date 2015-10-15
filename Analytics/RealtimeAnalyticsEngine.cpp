#include "pch.h"
#include "RealtimeAnalyticsEngine.h"
#include <ppltasks.h>

using namespace concurrency;

RealtimeAnalyticsEngine::RealtimeAnalyticsEngine()
{
	HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);
	m_devices = std::vector< ComPtr<WASAPICapture>>();
}

IAsyncAction^ RealtimeAnalyticsEngine::InitializeAsync()
{
	return create_async([this]
	{
		// Get the string identifier of the audio renderer
		String^ AudioSelector = MediaDevice::GetAudioCaptureSelector();

		// Add custom properties to the query
		auto PropertyList = ref new Platform::Collections::Vector<String^>();
		PropertyList->Append(PKEY_AudioEndpoint_Supports_EventDriven_Mode);

		// Setup the asynchronous callback
		Concurrency::task<DeviceInformationCollection^> enumOperation(DeviceInformation::FindAllAsync(AudioSelector, PropertyList));
		
		enumOperation.then([this](DeviceInformationCollection^ DeviceInfoCollection)
		{
			if ((DeviceInfoCollection == nullptr) || (DeviceInfoCollection->Size == 0))
			{
				OutputDebugString(L"No Devices Found.\n");
			}
			else
			{
				try
				{
					// Enumerate through the devices and the custom properties
					for (unsigned int i = 0; i < DeviceInfoCollection->Size; i++)
					{
						DeviceInformation^ deviceInfo = DeviceInfoCollection->GetAt(i);
						String^ DeviceInfoString = deviceInfo->Name;
						String^ DeviceIdString = deviceInfo->Id;

						if (deviceInfo->Properties->Size > 0)
						{
							ComPtr<WASAPICapture> capture = Make<WASAPICapture>();
							capture->Name = DeviceInfoString;
							capture->ID = DeviceIdString;

							capture->DeviceStateChangedToken = capture->DeviceStateChangedEv->StateChangedEvent +=
								ref new DeviceStateChangedHandler(this, &RealtimeAnalyticsEngine::OnDeviceStateChange);
							
							m_devices.push_back(capture);

							OutputDebugString(capture->Name->Data());
							OutputDebugString(capture->ID->Data());
							OutputDebugString(L"\n");
						}
					}
				}
				catch (Platform::Exception^ e)
				{

				}
			}
		})
			.then([this]()
		{
			if (m_devices.size() > 0)
			{
				
				m_RingBuffer = new SyncMultiRingBuffer();
				m_RingBuffer->SetConsumer(m_pRingBufferConsumer);

				for (int i = 0; i < m_devices.size(); i++)
				{
					m_devices[i]->InitializeAudioDeviceAsync(i, m_RingBuffer);
				}
			}
		});
	});
}

void RealtimeAnalyticsEngine::StopAsync()
{
	if (m_devices.size() > 0)
	{
		for (int i = 0; i < m_devices.size(); i++)
		{
			m_devices[i]->StopCaptureAsync();
		}
	}
}

void RealtimeAnalyticsEngine::OnDeviceStateChange(Object^ sender, DeviceStateChangedEventArgs^ e)
{
	bool readyToClean = true;

	// Handle state specific messages
	switch (e->State)
	{
	case DeviceState::DeviceStateInitialized:
		OutputDebugString(L"DeviceStateInitialized\n");
		m_devices.at(e->DeviceID)->StartCaptureAsync();
		break;

	case DeviceState::DeviceStatePlaying:
		OutputDebugString(L"DeviceStatePlaying\n");
		break;
	case DeviceState::DeviceStatePaused:
		OutputDebugString(L"DeviceStatePaused\n");
		break;
	case DeviceState::DeviceStateStopped:
		OutputDebugString(L"DeviceStateStopped\n");
		if (m_devices[e->DeviceID] != nullptr)
		{
			m_devices[e->DeviceID]->DeviceStateChangedEv->StateChangedEvent -= m_devices[e->DeviceID]->DeviceStateChangedToken;
			m_devices[e->DeviceID] = nullptr;
		}

		for (int i = 0; i < m_devices.size(); i++)
		{
			if (m_devices[e->DeviceID] != nullptr)
			{
				readyToClean = false;
			}
		}

		if (readyToClean)
		{
			m_devices.clear();
			delete m_RingBuffer;
			m_RingBuffer = nullptr;
		}

		break;

	case DeviceState::DeviceStateInError:
		OutputDebugString(L"DeviceStateInError\n");
		HRESULT hr = e->hr;
		/*
		if (m_deviceStateChangeToken.Value != 0)
		{
			m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
			m_StateChangedEvent = nullptr;
			m_deviceStateChangeToken.Value = 0;
		}
		*/
	}
}
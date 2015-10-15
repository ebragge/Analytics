#include "pch.h"
#include "RecordingAnalyticsEngine.h"

RecordingAnalyticsEngine::RecordingAnalyticsEngine() :
	m_spRenderer(nullptr),
	m_streamAnalytics(nullptr),
	m_pRingBuffer(nullptr),
	EngineParent()
{

}

void RecordingAnalyticsEngine::AnalyzeFile(IRandomAccessStream^ contentStream)
{
	if (!m_spRenderer)
	{
		HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);
		m_deviceProperties.ContentStream = contentStream;
		m_deviceProperties.IsTonePlayback = false;
		m_deviceProperties.IsHWOffload = false;
		m_deviceProperties.IsRawChosen = true;

		// Create a new WASAPI instance
		m_spRenderer = Make<WASAPIRenderer>();

		if (nullptr == m_spRenderer)
		{
			OnDeviceStateChange(this, ref new DeviceStateChangedEventArgs(DeviceState::DeviceStateInError, E_OUTOFMEMORY));
			return;
		}

		m_spRenderer->SetProperties(m_deviceProperties);

		// Get a pointer to the device event interface
		m_StateChangedEvent = m_spRenderer->GetDeviceStateEvent();

		if (nullptr == m_StateChangedEvent)
		{
			OnDeviceStateChange(this, ref new DeviceStateChangedEventArgs(DeviceState::DeviceStateInError, E_FAIL));
			return;
		}

		// Register for events
		m_deviceStateChangeToken = m_StateChangedEvent->StateChangedEvent += ref new DeviceStateChangedHandler(this, &RecordingAnalyticsEngine::OnDeviceStateChange);

		m_pRingBuffer = new MultiRingBuffer();
		m_pRingBuffer->SetConsumer(m_pRingBufferConsumer);
		m_spRenderer->m_DeviceProps.RingBuffer = m_pRingBuffer;

		// Selects the Default Audio Device
		m_spRenderer->InitializeAudioDeviceAsync();
	}
}

void RecordingAnalyticsEngine::StopAsync()
{
	if (m_spRenderer != nullptr)
	{
		m_spRenderer->StopPlaybackAsync();
	}
}

void RecordingAnalyticsEngine::OnDeviceStateChange(Object^ sender, DeviceStateChangedEventArgs^ e)
{
	// Handle state specific messages
	switch (e->State)
	{
	case DeviceState::DeviceStateInitialized:
		m_spRenderer->StartPlaybackAsync();
		//m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Closed;
		break;

	case DeviceState::DeviceStatePlaying:
		//ShowStatusMessage("Playback Started", NotifyType::StatusMessage);
		//m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Playing;
		break;

	case DeviceState::DeviceStatePaused:
		//ShowStatusMessage("Playback Paused", NotifyType::StatusMessage);
		//m_SystemMediaControls->PlaybackStatus = MediaPlaybackStatus::Paused;
		break;

	case DeviceState::DeviceStateStopped:
		m_spRenderer = nullptr;
		delete m_pRingBuffer;
		m_pRingBuffer = nullptr;

		if (m_deviceStateChangeToken.Value != 0)
		{
			m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
			m_StateChangedEvent = nullptr;
			m_deviceStateChangeToken.Value = 0;
		}
		break;

	case DeviceState::DeviceStateInError:
		HRESULT hr = e->hr;

		if (m_deviceStateChangeToken.Value != 0)
		{
			m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
			m_StateChangedEvent = nullptr;
			m_deviceStateChangeToken.Value = 0;
		}

	}
}
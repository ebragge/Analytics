//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// WASAPICapture.h
//

#include <Windows.h>
#include <wrl\implements.h>
#include <mfapi.h>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include "MainPage.xaml.h"
#include "DeviceState.h"
#include "MultiRingBuffer.h"

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

#pragma once
namespace WASAPIAudio
{
    // Primary WASAPI Capture Class
    class WASAPICapture :
        public RuntimeClass< RuntimeClassFlags< ClassicCom >, FtmBase, IActivateAudioInterfaceCompletionHandler > 
    {
    public:

        WASAPICapture();

        HRESULT InitializeAudioDeviceAsync(int i, IMultiRingBufferProducer *buffer);
        HRESULT StartCaptureAsync();
        HRESULT StopCaptureAsync();
        HRESULT FinishCaptureAsync();

        METHODASYNCCALLBACK( WASAPICapture, StartCapture, OnStartCapture );
        METHODASYNCCALLBACK( WASAPICapture, StopCapture, OnStopCapture );
        METHODASYNCCALLBACK( WASAPICapture, SampleReady, OnSampleReady );
        METHODASYNCCALLBACK( WASAPICapture, FinishCapture, OnFinishCapture );


        // IActivateAudioInterfaceCompletionHandler
        STDMETHOD(ActivateCompleted)( IActivateAudioInterfaceAsyncOperation *operation );

    private:
        ~WASAPICapture();

        HRESULT OnStartCapture( IMFAsyncResult* pResult );
        HRESULT OnStopCapture( IMFAsyncResult* pResult );
        HRESULT OnFinishCapture( IMFAsyncResult* pResult );
        HRESULT OnSampleReady( IMFAsyncResult* pResult );
        HRESULT OnSendScopeData( IMFAsyncResult* pResult );


        HRESULT OnAudioSampleRequested( Platform::Boolean IsSilence = false );
        
	public:
		String^ ID;
		String^ Name;
		DeviceStateChangedEvent^       DeviceStateChangedEv;
		EventRegistrationToken		   DeviceStateChangedToken;

	private:
        UINT32              m_BufferFrames;
        HANDLE              m_SampleReadyEvent;
        MFWORKITEM_KEY      m_SampleReadyKey;
		CRITICAL_SECTION    m_CritSec;
        DWORD               m_dwQueueID;

        DWORD               m_cbDataSize;
        BOOL                m_fWriting;
		DWORD				m_counter;
		WAVEFORMATEX        *m_MixFormat;

        IAudioClient3           *m_AudioClient;
        UINT32                  m_DefaultPeriodInFrames;
        UINT32                  m_FundamentalPeriodInFrames;
        UINT32                  m_MaxPeriodInFrames;
        UINT32                  m_MinPeriodInFrames;
        IAudioCaptureClient     *m_AudioCaptureClient;
        IMFAsyncResult          *m_SampleReadyAsyncResult;


		int StartCounter = 0;
		int m_CaptureDeviceID;
		IMultiRingBufferProducer *m_inputBuffer;
		std::vector<UINT> m_channels = std::vector<UINT>();
    };
}


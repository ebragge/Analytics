#pragma once
#include <Windows.h>
#include <wrl\implements.h>
#include <mfapi.h>
#include <AudioClient.h>
#include "IMultiRingBufferConsumer.h"


#define AUDIO_FILE_NAME "WASAPIAudioCapture.wav"
#define FLUSH_INTERVAL_SEC 3

using namespace Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Storage::Streams;

namespace WASAPIAudio
{
	class WavWriter : 
		public IMultiRingBufferConsumer
	{
	public:
		WavWriter();
		~WavWriter();

		void CreateWAVFile();
		HRESULT FixWAVHeader();
		

		//From Setters IMultiRingBufferConsumer
		virtual void ConsumeNewWindow(std::vector<RingBuffer::Window> &windows);
		virtual void SampleInformation(UINT cChannels, UINT wBitsPerSample, UINT nSamplesPerSec);
		virtual void StateChanged(IMultiRingBufferConsumer::State newState);

		virtual UINT WindowSize();
		virtual bool Ready();
	
	private:
		void Write(const Platform::Array<BYTE, 1> ^bytes);
		
		const UINT m_cWindowSize = 1000;

		DWORD	m_cbHeaderSize;
		DWORD	m_cbDataSize;
		DWORD	m_cbFlushCounter;
		BOOL	m_fWriting;
		UINT	m_nChannels;
		UINT	m_nBitsPerSample;
		UINT	m_nSamplesPerSec;
		bool	m_bReady;

		IRandomAccessStream^    m_ContentStream;
		IOutputStream^          m_OutputStream;
		DataWriter^             m_WAVDataWriter;
		WAVEFORMATEXTENSIBLE	m_MixFormat;
		//WAVEFORMAT				m_MixFormat;
	};
}

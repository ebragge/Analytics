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

namespace Wasapi
{
	class WavWriter : 
		public IMultiRingBufferConsumer
	{
	public:
		WavWriter(int bufferSizes, int bitsPerSample, int numberOfDevices, int channelsPerDevice);
		HRESULT CreateWAVFile(WAVEFORMATEX *mixFormat);
		HRESULT FixWAVHeader();
		~WavWriter();

	private:
		void Write(const Platform::Array<BYTE, 1> ^bytes);
		
	private:

		DWORD	m_cbHeaderSize;
		DWORD	m_cbDataSize;
		DWORD	m_cbFlushCounter;
		BOOL	m_fWriting;
		int		m_bitsPerSample;


		IRandomAccessStream^     m_ContentStream;
		IOutputStream^           m_OutputStream;
		DataWriter^              m_WAVDataWriter;
		WAVEFORMATEXTENSIBLE	 m_MixFormat;

	};
}

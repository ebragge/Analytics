#include "pch.h"
#include "WavWriter.h"
#include <Ksmedia.h>


using namespace Windows::Storage;
using namespace Windows::System::Threading;
using namespace WASAPIAudio;
using namespace Platform;

WavWriter::WavWriter() :
	
	m_ContentStream(nullptr),
	m_OutputStream(nullptr),
	m_WAVDataWriter(nullptr),
	m_nChannels(0),
	m_nBitsPerSample(0),
	m_nSamplesPerSec(0),
	m_bReady(false),
	m_cbDataSize(0),
	m_cbFlushCounter(0)
{

}

WavWriter::~WavWriter()
{
	m_ContentStream = nullptr;
	m_OutputStream = nullptr;
	m_WAVDataWriter = nullptr;
}

//
//  CreateWAVFile()
//
//  Creates a WAV file in KnownFolders::MusicLibrary
//
void WavWriter::CreateWAVFile()
{
	m_MixFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	m_MixFormat.Format.cbSize = 22;
	m_MixFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	m_MixFormat.Format.wBitsPerSample = m_nBitsPerSample;
	m_MixFormat.Format.nChannels = m_nChannels;
	m_MixFormat.Format.nSamplesPerSec = m_nSamplesPerSec;
	m_MixFormat.Format.nBlockAlign = m_nChannels*(m_nBitsPerSample / 8);
	m_MixFormat.Format.nAvgBytesPerSec = m_nSamplesPerSec*m_nChannels*(m_nBitsPerSample / 8);
	m_MixFormat.Samples.wValidBitsPerSample = m_MixFormat.Format.wBitsPerSample;
	m_MixFormat.dwChannelMask = 3;

	// Create the WAV file, appending a number if file already exists
	concurrency::task<StorageFile^>(KnownFolders::MusicLibrary->CreateFileAsync(AUDIO_FILE_NAME, CreationCollisionOption::GenerateUniqueName)).then(
		[this](StorageFile^ file)
	{
		if (nullptr == file)
		{
			ThrowIfFailed(E_INVALIDARG);
		}

		return file->OpenAsync(FileAccessMode::ReadWrite);
	})
	
	// Then create a RandomAccessStream
	.then([this](IRandomAccessStream^ stream)
	{
		if (nullptr == stream)
		{
			ThrowIfFailed(E_INVALIDARG);
		}

		// Get the OutputStream for the file
		m_ContentStream = stream;
		m_OutputStream = m_ContentStream->GetOutputStreamAt(0);

		// Create the DataWriter
		m_WAVDataWriter = ref new DataWriter(m_OutputStream);
		
		if (nullptr == m_WAVDataWriter)
		{
			ThrowIfFailed(E_OUTOFMEMORY);
		}

		// Create the WAV header
		DWORD header[] = {
			FCC('RIFF'),        // RIFF header
			0,                  // Total size of WAV (will be filled in later)
			FCC('WAVE'),        // WAVE FourCC
			FCC('fmt '),        // Start of 'fmt ' chunk
			sizeof(WAVEFORMATEX) + m_MixFormat.Format.cbSize     // Size of fmt chunk
		};

		//WAVEFORMATEXTENSIBLE
		DWORD data[] = { FCC('data'), 0 };  // Start of 'data' chunk

		auto headerBytes = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(header), sizeof(header));
		auto formatBytes = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(&m_MixFormat), sizeof(WAVEFORMATEX) + m_MixFormat.Format.cbSize);
		auto dataBytes = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(data), sizeof(data));
		int tmp = sizeof(WAVEFORMATEXTENSIBLE);

		if ((nullptr == headerBytes) || (nullptr == formatBytes) || (nullptr == dataBytes))
		{
			ThrowIfFailed(E_OUTOFMEMORY);
		}

		// Write the header
		m_WAVDataWriter->WriteBytes(headerBytes);
		m_WAVDataWriter->WriteBytes(formatBytes);
		m_WAVDataWriter->WriteBytes(dataBytes);

		return m_WAVDataWriter->StoreAsync();
	})

	// Wait for file data to be written to file
	.then([this](unsigned int BytesWritten)
	{
		m_cbHeaderSize = BytesWritten;
		return m_WAVDataWriter->FlushAsync();
	})

	// Our file is ready so we can signal that initialization is finished
	.then([this](bool f)
	{
		m_bReady = true;
		OutputDebugString(L"CreateWAVFile - Done");
	});
}

HRESULT WavWriter::FixWAVHeader()
{
	concurrency::task<unsigned int>(m_WAVDataWriter->StoreAsync()).then(
		[this](unsigned int BytesWritten)
	{
		BytesWritten;
		return m_WAVDataWriter->FlushAsync();
	})
	.then([this](bool flushed)
	{
		auto DataSizeByte = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(&m_cbDataSize), sizeof(DWORD));

		// Write the size of the 'data' chunk first
		IOutputStream^ OutputStream = m_ContentStream->GetOutputStreamAt(m_cbHeaderSize - sizeof(DWORD));
		m_WAVDataWriter = ref new DataWriter(OutputStream);
		m_WAVDataWriter->WriteBytes(DataSizeByte);

		concurrency::task<unsigned int>(m_WAVDataWriter->StoreAsync()).then(
			[this](unsigned int BytesWritten)
			{
			DWORD cbTotalSize = m_cbDataSize + m_cbHeaderSize - 8;
			auto TotalSizeByte = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(&cbTotalSize), sizeof(DWORD));

			// Write the total file size, minus RIFF chunk and size
			IOutputStream^ OutputStream = m_ContentStream->GetOutputStreamAt(sizeof(DWORD));  // sizeof(DWORD) == sizeof(FOURCC)
			m_WAVDataWriter = ref new DataWriter(OutputStream);
			m_WAVDataWriter->WriteBytes(TotalSizeByte);

			concurrency::task<unsigned int>(m_WAVDataWriter->StoreAsync()).then(
				[this](unsigned int BytesWritten)
			{
				return m_WAVDataWriter->FlushAsync();
			})

		.then(
			[this](bool f)
			{
				
				//Done
				OutputDebugString(L"FixWAVHeader - Done");
			});
		});
	});

	return S_OK;
}

//From Setters IMultiRingBufferConsumer
void WavWriter::ConsumeNewWindow(std::vector<RingBuffer::Window> &windows)
{
	Platform::Array<BYTE, 1> ^dataByte = ref new Platform::Array<BYTE, 1>(m_cWindowSize*m_nChannels*m_nBitsPerSample / 8);
	auto p = (INT16*)dataByte->Data;

	for (size_t step = 0; step < m_cWindowSize; step++)
	{
		for (size_t channel = 0; channel < m_nChannels; channel++)
		{
			*p = windows[channel][step];
			p++;
		}
	}

	Write(dataByte);
}

void WavWriter::SampleInformation(UINT nChannels, UINT wBitsPerSample, UINT nSamplesPerSec)
{
	m_nChannels = nChannels;
	m_nBitsPerSample = wBitsPerSample;
	m_nSamplesPerSec = nSamplesPerSec;
	CreateWAVFile();
}

void WavWriter::StateChanged(IMultiRingBufferConsumer::State newState)
{
	if (newState == IMultiRingBufferConsumer::State::Stopped)
	{
		FixWAVHeader();
	}
}

bool WavWriter::Ready()
{
	return m_bReady;
}

UINT WavWriter::WindowSize()
{
	return m_cWindowSize;
}

void WavWriter::Write(const Platform::Array<BYTE, 1> ^bytes)
{
	// Write File and async store
	m_WAVDataWriter->WriteBytes(bytes);

	// Increase the size of our 'data' chunk and flush counter.  m_cbDataSize needs to be accurate
	// Its OK if m_cbFlushCounter is an approximation
	m_cbDataSize += bytes->Length;
	m_cbFlushCounter += bytes->Length;

	if ((m_cbFlushCounter > (m_MixFormat.Format.nAvgBytesPerSec * FLUSH_INTERVAL_SEC)))
	{
		// Reset the counter now since we can process more samples during the async callback
		m_cbFlushCounter = 0;

		concurrency::task<unsigned int>(m_WAVDataWriter->StoreAsync()).then(
			[this](unsigned int BytesWritten)
		{
			return m_WAVDataWriter->FlushAsync();
		});
	}
}
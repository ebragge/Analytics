#include "pch.h"
#include "WavWriter.h"
#include <Ksmedia.h>


using namespace Windows::Storage;
using namespace Windows::System::Threading;
using namespace Wasapi;
using namespace Platform;

WavWriter::WavWriter(int bufferSizes, int bitsPerSample, int numberOfDevices, int channelsPerDevice) :
	
	m_ContentStream(nullptr),
	m_OutputStream(nullptr),
	m_WAVDataWriter(nullptr)
{
	m_bitsPerSample = bitsPerSample;
	//m_numberOfDevices = numberOfDevices;
	//m_channelsPerDevice = channelsPerDevice;

}

WavWriter::~WavWriter()
{

}
//
//  CreateWAVFile()
//
//  Creates a WAV file in KnownFolders::MusicLibrary
//
HRESULT WavWriter::CreateWAVFile(WAVEFORMATEX *mixFormat)
{
	memcpy(&m_MixFormat, mixFormat, sizeof(WAVEFORMATEXTENSIBLE));

	m_MixFormat.Format.cbSize = 22;
	m_MixFormat.Format.nChannels = 2;
	m_MixFormat.Format.nBlockAlign *= 2;
	m_MixFormat.Format.nAvgBytesPerSec *= 2;

	
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
			sizeof(WAVEFORMATEXTENSIBLE)     // Size of fmt chunk
		};

		DWORD data[] = { FCC('data'), 0 };  // Start of 'data' chunk

		auto headerBytes = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(header), sizeof(header));
		auto formatBytes = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(&m_MixFormat), sizeof(WAVEFORMATEXTENSIBLE));
		auto dataBytes = ref new Platform::Array<BYTE>(reinterpret_cast<BYTE*>(data), sizeof(data));

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

		// Our file is ready to go, so we can now signal that initialization is finished
		.then([this](bool f)
	{

	});

	return S_OK;
}

HRESULT WavWriter::FixWAVHeader()
{
	concurrency::task<unsigned int>(m_WAVDataWriter->StoreAsync()).then(
		[this](unsigned int BytesWritten)
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
				OutputDebugString(L"Done");
			});
		});
	});

	return S_OK;
}

/*
void WavWriter::Process(const std::vector<std::vector<INT16>> &buffer, const std::vector<int> &nextBufferItemToWrite)
{
	int smallest = GetSmallestDeltaBetweenWavAndNextBuffer(nextBufferItemToWrite);

	if (smallest > 100) //Store only if there are more than 100 frames
	{
	Platform::Array<BYTE, 1> ^dataByte = ref new Platform::Array<BYTE, 1>(smallest*m_numberOfDevices*m_channelsPerDevice*m_bitsPerSample / 8);
		auto p = (INT16*)dataByte->Data;

		for (size_t count = 0; count < smallest; count++)
		{
			for (size_t i = 0; i < m_numberOfDevices; i++)
			{
				int next = GetNextItem(i);

				for (size_t j = 0; j < m_channelsPerDevice; j++)
				{
					*p = buffer[i*m_channelsPerDevice + j][next];
					p++;
				}
			}
		}
		Write(dataByte);
	}
}
*/

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

		});
	}
}

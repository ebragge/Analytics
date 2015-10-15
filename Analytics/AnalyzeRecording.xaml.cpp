//
// AnalyzeRecording.xaml.cpp
// Implementation of the AnalyzeRecording class
//

#include "pch.h"
#include "AnalyzeRecording.xaml.h"
#include "AudioControl.xaml.h"
#include "AudioControlData.h"

using namespace Analytics;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace concurrency;



// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

AnalyzeRecording::AnalyzeRecording()
{
	InitializeComponent();
	m_analyticsEngine = ref new RecordingAnalyticsEngine();
}

AnalyzeRecording::~AnalyzeRecording()
{

}

void AnalyzeRecording::btnFilePicker_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	OnPickFileAsync();
}

void AnalyzeRecording::OnPickFileAsync()
{
	Pickers::FileOpenPicker^ filePicker = ref new Pickers::FileOpenPicker();
	filePicker->ViewMode = Pickers::PickerViewMode::List;
	filePicker->SuggestedStartLocation = Pickers::PickerLocationId::MusicLibrary;
	filePicker->FileTypeFilter->Append(".wav");
	filePicker->FileTypeFilter->Append(".mp3");
	filePicker->FileTypeFilter->Append(".wma");

	concurrency::create_task(filePicker->PickSingleFileAsync()).then(
		[this](Windows::Storage::StorageFile^ file)
	{
		if (nullptr != file)
		{
			// Open the stream
			concurrency::create_task(file->OpenAsync(FileAccessMode::Read)).then(
				[this, file](IRandomAccessStream^ stream)
			{
				if (stream != nullptr)
				{
					m_ContentStream = stream;
					UpdateContentProps(file->Path);
				}
			});
		}
	});
}

void AnalyzeRecording::UpdateContentProps(String^ str)
{
	String^ text = str;

	if (nullptr != txtContentProps)
	{
		// The event handler may be invoked on a background thread, so use the Dispatcher to invoke the UI-related code on the UI thread.
		txtContentProps->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
			[this, text]()
		{
			Windows::UI::Xaml::Media::SolidColorBrush ^brush;
			txtContentProps->Text = text;

			txtContentProps->Background = brush;
		}));
	}
}

void Analytics::AnalyzeRecording::btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_pStreamAnalytics = new StreamAnalytics();
	m_pStreamAnalytics->SetObserver(audioCtrl->m_pData);
	m_analyticsEngine->SetConsumer(m_pStreamAnalytics);
	m_analyticsEngine->AnalyzeFile(m_ContentStream);	
}



void Analytics::AnalyzeRecording::btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_analyticsEngine->StopAsync();
}

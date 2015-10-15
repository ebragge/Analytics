//
// AnalyzeRecording.xaml.h
// Declaration of the AnalyzeRecording class
//

#pragma once
#include "pch.h"
#include "AnalyzeRecording.g.h"
#include "MainPage.xaml.h"
#include "RecordingAnalyticsEngine.h"

using namespace Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Storage::Streams;
using namespace Platform;

namespace Analytics
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AnalyzeRecording sealed
	{

	public:
		AnalyzeRecording();
		virtual ~AnalyzeRecording();
	private:
		void btnFilePicker_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPickFileAsync();
		void UpdateContentProps(String^ str);

		void btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPlayPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	private:
		IRandomAccessStream^		m_ContentStream;
		RecordingAnalyticsEngine^	m_analyticsEngine;
		StreamAnalytics*			m_pStreamAnalytics;
	};
}

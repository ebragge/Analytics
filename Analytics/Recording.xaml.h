//
// Recording.xaml.h
// Declaration of the Recording class
//

#pragma once

#include "Recording.g.h"
#include "RealtimeAnalyticsEngine.h"
#include "WavWriter.h"

namespace Analytics
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Recording sealed
	{
	public:
		Recording();

	private:
		void btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		RealtimeAnalyticsEngine^ m_realtimeEngine;
		WavWriter *m_pWavWriter;
	};
}

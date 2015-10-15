//
// RealtimeAnalysis.xaml.h
// Declaration of the RealtimeAnalysis class
//

#pragma once

#include "RealtimeAnalysis.g.h"
#include "RealtimeAnalyticsEngine.h"

namespace Analytics
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class RealtimeAnalysis sealed
	{

	public:
		RealtimeAnalysis();

	private:
		void btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	private:
		RealtimeAnalyticsEngine^ m_realtimeEngine;
		StreamAnalytics* m_pStreamAnalytics;

	};
}

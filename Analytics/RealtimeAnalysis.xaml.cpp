//
// RealtimeAnalysis.xaml.cpp
// Implementation of the RealtimeAnalysis class
//

#include "pch.h"
#include "RealtimeAnalysis.xaml.h"
#include "AudioControlData.h"
#include "AudioControl.xaml.h"
#
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

RealtimeAnalysis::RealtimeAnalysis()
{
	InitializeComponent();
	m_realtimeEngine = ref new RealtimeAnalyticsEngine();
}

void Analytics::RealtimeAnalysis::btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_pStreamAnalytics = new StreamAnalytics();
	m_pStreamAnalytics->SetObserver(audioCtrl->m_pData);
	m_realtimeEngine->SetConsumer(m_pStreamAnalytics);
	m_realtimeEngine->InitializeAsync();
}

void Analytics::RealtimeAnalysis::btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_realtimeEngine->StopAsync();
}
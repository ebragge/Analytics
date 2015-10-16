//
// Recording.xaml.cpp
// Implementation of the Recording class
//

#include "pch.h"
#include "Recording.xaml.h"

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

Recording::Recording()
{
	InitializeComponent();
	m_realtimeEngine = ref new RealtimeAnalyticsEngine();
}

void Recording::btnPlay_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_pWavWriter = new WavWriter();
	//m_pWavWriter->SetObserver(audioCtrl->m_pData);
	m_realtimeEngine->SetConsumer(m_pWavWriter);
	m_realtimeEngine->InitializeAsync();
}


void Recording::btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_realtimeEngine->StopAsync();
}
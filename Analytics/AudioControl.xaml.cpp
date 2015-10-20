//
// AudioControl.xaml.cpp
// Implementation of the AudioControl class
//

#include "pch.h"
#include "AudioControl.xaml.h"

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
using namespace Windows::UI;
using namespace Microsoft::Graphics::Canvas::Brushes;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236


AudioControl::AudioControl()
{
	m_pData = new AudioControlData();

	InitializeComponent();

}

void AudioControl::OnDraw(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ canvasControl, ::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	auto brush = ref new CanvasSolidColorBrush(canvasControl->Device, Colors::Red);
	args->DrawingSession->DrawLine(0, DataHeight / 2, m_width, DataHeight / 2, brush);
	
	float step = m_width / m_pData->m_vBuffer.size();
	float step_y = DataHeight / 32767;
	float half = DataHeight / 2;
	
	UINT pos = 0;

	std::vector<INT16> data = m_pData->GetData(pos);

	for (size_t i = 0; i < m_pData->m_vBuffer.size(); i++)
	{
		args->DrawingSession->DrawRectangle(step*i, half, step, data[pos], brush);
		pos = ++pos < data.size() ? pos : 0;
	}

	//We draw graphs
	UINT size = m_pData->GetNumberOfGraphs();
	
	for (size_t i = 0; i < size; i++)
	{
		auto brush = ref new CanvasSolidColorBrush(canvasControl->Device, Colors::Green);
		float Middle = DataHeight + ((m_height - DataHeight) / (size + 1)*i+1);
		float Height = (m_height - DataHeight) / (size + 1);
		args->DrawingSession->DrawLine(0, Middle, m_width, Middle, brush);

		std::vector<INT16> vector = m_pData->GetGraph(i);
		float pointLength = m_width / vector.size();
		INT16 max = *std::max_element(vector.begin(), vector.end());

		float x0 = 0;
		float y0 = Middle + ((Height/max)/2)*vector[0];

		for (size_t j = 1; j < vector.size(); j++)
		{
			float x1 = j*pointLength;
			float y1 = Middle + ((Height / max) / 2)*vector[j];

			args->DrawingSession->DrawLine(x0, y0, x1, y1, brush);

			x0 = x1;
			y0 = y1;
		}

	}

	
}

void AudioControl::OnCreateResources(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl ^sender, ::Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs ^args)
{

}



void AudioControl::OnCanvasSizeChanged(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl ^sender, ::Windows::UI::Xaml::SizeChangedEventArgs ^args)
{
	m_height = args->NewSize.Height;
	m_width = args->NewSize.Width;
}
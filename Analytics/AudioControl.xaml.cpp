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
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Microsoft::Graphics::Canvas;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236


AudioControl::AudioControl()
{
	m_pData = new AudioControlData();

	InitializeComponent();

}

void AudioControl::OnDraw(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ canvasControl, ::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	if (m_pData->Lock())
	{

		auto brush = ref new CanvasSolidColorBrush(canvasControl->Device, Colors::Red);
		args->DrawingSession->DrawLine(0, DataHeight / 2, m_width, DataHeight / 2, brush);
	
		UINT pos = 0;
		double dMax = 0.01;

		std::vector<double> data = m_pData->GetMovingGraphData(0,pos, dMax);

		float step = m_width / data.size();
		float step_y = DataHeight / dMax;
		float half = DataHeight / 2;
	
		for (size_t i = 0; i < data.size(); i++)
		{
			args->DrawingSession->DrawRectangle(step*i, half, step, data[pos] * step_y, brush);
			pos = ++pos < data.size() ? pos : 0;
		}

		//Draw static graphs
		auto vStaticGraphs = m_pData->GetStaticGraphs();

		for (size_t i = 0; i < vStaticGraphs.size(); i++)
		{
			auto brush = ref new CanvasSolidColorBrush(canvasControl->Device, Colors::Green);
			float Middle = DataHeight + ((m_height - DataHeight) / (vStaticGraphs.size() + 1)*i + 1);
			float Height = (m_height - DataHeight) / (vStaticGraphs.size() + 1);
			args->DrawingSession->DrawLine(0, Middle, m_width, Middle, brush);

			float pointLength = m_width / vStaticGraphs[i].size();
			INT16 max = *std::max_element(vStaticGraphs[i].begin(), vStaticGraphs[i].end());

			float x0 = 0;
			float y0 = Middle + ((Height / max) / 2)*vStaticGraphs[i][0];

			for (size_t j = 1; j < vStaticGraphs[i].size(); j++)
			{
				float x1 = j*pointLength;
				float y1 = Middle + ((Height / max) / 2)*vStaticGraphs[i][j];

				args->DrawingSession->DrawLine(x0, y0, x1, y1, brush);

				x0 = x1;
				y0 = y1;
			}
		}

		//Draw counters
		auto map = m_pData->GetMap();

		for (auto const &ent1 : map)
		{
			args->DrawingSession->DrawText(ref new String(ent1.first.c_str()), 100, 400, Colors::Yellow);
			args->DrawingSession->DrawText(ent1.second.ToString(), 100, 450, Colors::Yellow);
		}
		m_pData->Unlock();
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
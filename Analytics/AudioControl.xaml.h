//
// AudioControl.xaml.h
// Declaration of the AudioControl class
//

#pragma once

#include "AudioControl.g.h"
#include <deque>
#include "AudioControlData.h"

namespace Analytics
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AudioControl sealed
	{
	public:
		AudioControl();
	internal:
		AudioControlData *m_pData;

	private:
		void OnDraw(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ canvasControl, ::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^);
		void OnCreateResources(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl ^sender, ::Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs ^args);
		void OnCanvasSizeChanged(::Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl ^sender, ::Windows::UI::Xaml::SizeChangedEventArgs ^args);
		
		float m_height;
		float m_width;

		const UINT32 DataHeight = 100;
	};
}

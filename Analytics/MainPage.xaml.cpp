//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

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
using namespace Windows::UI::Xaml::Interop;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

}
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	// Populate the ListBox with the scenarios as defined in SampleConfiguration.cpp.
	auto itemCollection = ref new Platform::Collections::Vector<Object^>();
	int i = 1;
	for (auto const& s : MainPage::Current->scenarios)
	{
		// Create a textBlock to hold the content and apply the ListItemTextStyle from Styles.xaml
		TextBlock^ textBlock = ref new TextBlock();
		ListBoxItem^ item = ref new ListBoxItem();
		//auto style = App::Current->Resources->Lookup("ListItemTextStyle");

		textBlock->Text = (i++).ToString() + ") " + s.Title;
		//textBlock->Style = safe_cast<Windows::UI::Xaml::Style ^>(style);

		item->Name = s.ClassName;
		item->Content = textBlock;
		itemCollection->Append(item);
	}

	// Set the newly created itemCollection as the ListBox ItemSource.
	ScenarioControl->ItemsSource = itemCollection;
	int startingScenarioIndex;

	if (Window::Current->Bounds.Width < 640)
	{
		startingScenarioIndex = -1;
	}
	else
	{
		startingScenarioIndex = 0;
	}

	ScenarioControl->SelectedIndex = startingScenarioIndex;
	ScenarioControl->ScrollIntoView(ScenarioControl->SelectedItem);
}

void MainPage::ScenarioControl_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
	ListBox^ scenarioListBox = safe_cast<ListBox^>(sender); //as ListBox;
	ListBoxItem^ item = dynamic_cast<ListBoxItem^>(scenarioListBox->SelectedItem);
	if (item != nullptr)
	{
		// Navigate to the selected scenario.
		TypeName scenarioType = { item->Name, TypeKind::Custom };
		ScenarioFrame->Navigate(scenarioType, this);
	}
}

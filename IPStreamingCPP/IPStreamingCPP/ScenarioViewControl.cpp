#include "pch.h"
#include "ScenarioViewControl.h"


using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;


ScenarioItem::ScenarioItem(Platform::String^ IDName,  Windows::UI::Xaml::Interop::TypeName TypeName, Windows::UI::Xaml::Controls::Symbol symbol, Platform::Object^ object)
{
	//m_callBackHandler = ref new Windows::Foundation::Collections:: <Platform::String ^, ScenarioEventSource^> ();

	m_callBackHandler = ref new Platform::Collections::Map<Platform::String ^, ScenarioEventSource^>();
	m_Symbol = symbol;
	m_ScenarioName = IDName;
	m_Object = object;
//	m_ClassType = type;
	m_TypeName = TypeName;



}

ScenarioItem::~ScenarioItem()
{
	m_callBackHandler->Clear();
}

String^ ScenarioItem::SymbolAsChar::get()
{
	wchar_t c[] = { (wchar_t)((int)(m_Symbol)), '\0' };
	return ref new String(c);
}


Platform::Object^  ScenarioItem::Object::get() {
	return this->m_Object; 
}



void ScenarioItem::Object::set(Platform::Object^ value)
{
	this->m_Object = value;

}




Platform::String^ ScenarioItem::ScenarioName::get()
{
	return this->m_ScenarioName;

}

void ScenarioItem::ScenarioName::set(Platform::String^ value)
{
	this->m_ScenarioName = value;

}

Windows::UI::Xaml::Interop::TypeName ScenarioItem::TypeClassName::get()
{
	return this->m_TypeName;

}

void ScenarioItem::TypeClassName::set(Windows::UI::Xaml::Interop::TypeName value)
{
	this->m_TypeName = value;

}
/*
Platform::Type^ ScenarioItem::ClassType::get()
{
	return this->m_ClassType;

}

void ScenarioItem::ClassType::set(Platform::Type^ value)
{
	this->m_ClassType = value;

}
*/


ScenarioViewControl::ScenarioViewControl()
{
	this->m_Items = ref new Platform::Collections::Vector<ScenarioItem^>();
	m_SelectedIndex = -1;
}


void ScenarioViewControl::SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	Windows::UI::Xaml::Controls::ListView^ inputsourceurl = safe_cast<ListView^>(sender);
	if (inputsourceurl->SelectedIndex == -1) return;

	auto selected = inputsourceurl->Items->GetAt(inputsourceurl->SelectedIndex);
	this->SelectedIndex = inputsourceurl->SelectedIndex;



}



int ScenarioViewControl::SelectedIndex::get() {
	return this->m_SelectedIndex;
}
void ScenarioViewControl::SelectedIndex::set(int value) {
	this->m_SelectedIndex = value;
}



ScenarioItem^ ScenarioViewControl::getSelectedItem()
{
	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	return(ScenarioItem^)Items->GetAt((unsigned int)SelectedIndex);

}

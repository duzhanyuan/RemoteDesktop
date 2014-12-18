#include "stdafx.h"
#include "Desktop_Monitor.h"
#include <thread>
#include "..\RemoteDesktop_Library\Handle_Wrapper.h"
#include "Wtsapi32.h"

RemoteDesktop::DesktopMonitor::DesktopMonitor(){

}


RemoteDesktop::DesktopMonitor::~DesktopMonitor(){
	if (m_hDesk != NULL)CloseDesktop(m_hDesk);
}

//
//RemoteDesktop::Desktops RemoteDesktop::DesktopMonitor::GetDesktop(HDESK s){
//	if (s == NULL)
//		return Default;
//	DWORD needed = 0;
//	wchar_t new_name[256];
//	auto result = GetUserObjectInformation(s, UOI_NAME, &new_name, 256, &needed);
//	std::wstring dname = new_name;
//	std::transform(dname.begin(), dname.end(), dname.begin(), ::tolower);
//	if (!result)
//		return Default;
//	if (std::wstring(L"default") == dname)
//		return Default;
//	else if (std::wstring(L"screensaver") == dname)
//		return ScreenSaver;
//	else
//		return Winlogon;
//	
//}

bool RemoteDesktop::DesktopMonitor::Is_InputDesktopSelected() const{

	// Get the input and thread desktops
	HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());

	RAIIHDESKTOP inputdesktop(OpenInputDesktop(0, FALSE,
		DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP));

	if (inputdesktop.get_Handle() == NULL) return true;


	DWORD dummy;
	wchar_t threadname[256];
	wchar_t inputname[256];

	if (!GetUserObjectInformation(threaddesktop, UOI_NAME, &threadname, 256, &dummy)) return false;
	if (!GetUserObjectInformation(inputdesktop.get_Handle(), UOI_NAME, &inputname, 256, &dummy)) return false;
	return wcscmp(threadname, inputname) == 0;
}
std::string RemoteDesktop::DesktopMonitor::get_ActiveUser(){
	char* ptr = NULL;
	DWORD size = 0;
	if (WTSQuerySessionInformationA(WTS_CURRENT_SERVER_HANDLE, WTSGetActiveConsoleSessionId(), WTS_INFO_CLASS::WTSUserName, &ptr, &size)){
		auto name = std::string(ptr);
		WTSFreeMemory(ptr);
		return name;
	}
	return "";
}
void RemoteDesktop::DesktopMonitor::Switch_to_ActiveDesktop(){
	auto threadsk = GetThreadDesktop(GetCurrentThreadId());
	HDESK desktop = OpenInputDesktop(0, FALSE,
		DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);

	if (desktop == NULL) return;
	if (!SetThreadDesktop(desktop))
	{
		CloseDesktop(desktop);
		return;
	}
	if (m_hDesk) {
		CloseDesktop(m_hDesk);
	}
	m_hDesk = desktop;
}
// MsKm.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MsKm.h"
#include <map>
#include <tchar.h>

using namespace std;

//装载NT驱动程序
HANDLE g_DriverHandle = INVALID_HANDLE_VALUE;
BOOL g_Is64BitOS = FALSE;

typedef UINT(WINAPI* GETSYSTEMWOW64DIRECTORY)(LPTSTR, UINT);

map<char, char> GKeyMapLst;
map<char, USHORT> GKeyMapCode;

void DebugInf(const PTCHAR szFormat, ...)
{
//#ifdef _DEBUG
	va_list arg_ptr;
	va_start(arg_ptr, szFormat);
	TCHAR szOutStr[4096] = { 0 };
	//wsprintf(szOutStr, szStr, arg_ptr); 
#ifdef UNICODE
	vswprintf_s(szOutStr, 4096, szFormat, arg_ptr);
#else
	vsprintf_s(szOutStr, 4096, szFormat, arg_ptr);
#endif
	OutputDebugString(szOutStr);
	va_end(arg_ptr);
//#endif
}

BOOL EnableDebugPrivilege(BOOL bEnable)
{
	//Enabling the debug privilege allows the application to see
	//information about service application
	BOOL fOK = FALSE;     //Assume function fails
	HANDLE hToken;
	//Try to open this process's acess token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		//Attempt to modify the "Debug" privilege
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOK = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOK;
}

void InitKeyMapLst()
{
	GKeyMapLst['~'] = '`';
	GKeyMapLst['!'] = '1';
	GKeyMapLst['@'] = '2';
	GKeyMapLst['#'] = '3';
	GKeyMapLst['$'] = '4';
	GKeyMapLst['%'] = '5';
	GKeyMapLst['^'] = '6';
	GKeyMapLst['&'] = '7';
	GKeyMapLst['*'] = '8';
	GKeyMapLst['('] = '9';
	GKeyMapLst[')'] = '0';
	GKeyMapLst['_'] = '-';
	GKeyMapLst['+'] = '=';
	GKeyMapLst['{'] = '[';
	GKeyMapLst['}'] = ']';
	GKeyMapLst['|'] = '\\';
	GKeyMapLst[':'] = ';';
	GKeyMapLst['"'] = '\'';
	GKeyMapLst['<'] = ',';
	GKeyMapLst['>'] = '.';
	GKeyMapLst['?'] = '/';
}

void InitKeyMapCode()
{
	GKeyMapCode['`'] = 192;
	GKeyMapCode['-'] = 189;
	GKeyMapCode['='] = 187;
	GKeyMapCode['['] = 219;
	GKeyMapCode[']'] = 221;
	GKeyMapCode['\\'] = 220;
	GKeyMapCode[';'] = 186;
	GKeyMapCode['\''] = 222;
	GKeyMapCode[','] = 188;
	GKeyMapCode['.'] = 190;
	GKeyMapCode['/'] = 191;

	GKeyMapCode['a'] = 65;
	GKeyMapCode['b'] = 66;
	GKeyMapCode['c'] = 67;
	GKeyMapCode['d'] = 68;
	GKeyMapCode['e'] = 69;
	GKeyMapCode['f'] = 70;
	GKeyMapCode['g'] = 71;
	GKeyMapCode['h'] = 72;
	GKeyMapCode['i'] = 73;
	GKeyMapCode['j'] = 74;
	GKeyMapCode['k'] = 75;
	GKeyMapCode['l'] = 76;
	GKeyMapCode['m'] = 77;
	GKeyMapCode['n'] = 78;
	GKeyMapCode['o'] = 79;
	GKeyMapCode['p'] = 80;
	GKeyMapCode['q'] = 81;
	GKeyMapCode['r'] = 82;
	GKeyMapCode['s'] = 83;
	GKeyMapCode['t'] = 84;
	GKeyMapCode['u'] = 85;
	GKeyMapCode['v'] = 86;
	GKeyMapCode['w'] = 87;
	GKeyMapCode['x'] = 88;
	GKeyMapCode['y'] = 89;
	GKeyMapCode['z'] = 90;
}

BOOL Is64BitOS()
{
#ifdef _WIN64
	return TRUE;
#else
	GETSYSTEMWOW64DIRECTORY getSystemWow64Directory;
	HMODULE hKernel32;
	TCHAR Wow64Directory[32767];

	hKernel32 = GetModuleHandle(TEXT("kernel32.dll"));
	if (hKernel32 == NULL)
	{
		//
		// This shouldn't happen, but if we can't get 
		// kernel32's module handle then assume we are 
		// on x86. We won't ever install 32-bit drivers
		// on 64-bit machines, we just want to catch it 
		// up front to give users a better error message.
		//
		return FALSE;
	}

	getSystemWow64Directory = (GETSYSTEMWOW64DIRECTORY)GetProcAddress(hKernel32, "GetSystemWow64DirectoryW");

	if (getSystemWow64Directory == NULL)
	{
		//
		// This most likely means we are running 
		// on Windows 2000, which didn't have this API 
		// and didn't have a 64-bit counterpart.
		//
		return FALSE;
	}

	if ((getSystemWow64Directory(Wow64Directory, _countof(Wow64Directory)) == 0) &&
		(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)) {
		return FALSE;
	}

	//
	// GetSystemWow64Directory succeeded 
	// so we are on a 64-bit OS.
	//
	return TRUE;
#endif
}


bool __stdcall KeyDown(USHORT VirtualKey)
{
	KEYBOARD_INPUT_DATA  kid;
	DWORD dwOutput;
	memset(&kid, 0, sizeof(KEYBOARD_INPUT_DATA));
	kid.Flags = KEY_DOWN;
	kid.MakeCode = (USHORT)MapVirtualKey(VirtualKey, 0);

	if (!DeviceIoControl(g_DriverHandle, IOCTL_KEYBOARD, &kid, sizeof(KEYBOARD_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}	
	return true;
}


bool __stdcall KeyUp(USHORT VirtualKey)
{
	KEYBOARD_INPUT_DATA  kid;
	DWORD dwOutput;
	memset(&kid, 0, sizeof(KEYBOARD_INPUT_DATA));
	kid.Flags = KEY_UP;
	kid.MakeCode = (USHORT)MapVirtualKey(VirtualKey, 0);

	if(!DeviceIoControl(g_DriverHandle, IOCTL_KEYBOARD, &kid, sizeof(KEYBOARD_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;

}


bool __stdcall MouseLeftButtonDown()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.ButtonFlags = MOUSE_LEFT_BUTTON_DOWN;
	if(!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseLeftButtonUp()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.ButtonFlags = MOUSE_LEFT_BUTTON_UP;
	if( !DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL) )
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseRightButtonDown()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.ButtonFlags = MOUSE_RIGHT_BUTTON_DOWN;
	if (!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseRightButtonUp()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.ButtonFlags = MOUSE_RIGHT_BUTTON_UP;
	if (!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseMiddleButtonDown()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.ButtonFlags = MOUSE_MIDDLE_BUTTON_DOWN;
	if (!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseMiddleButtonUp()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.ButtonFlags = MOUSE_MIDDLE_BUTTON_UP;
	if (!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseMoveRELATIVE(LONG dx, LONG dy)
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.Flags = MOUSE_MOVE_RELATIVE;
	mid.LastX = dx;
	mid.LastY = dy;
	if (!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MouseMoveABSOLUTE(LONG dx, LONG dy)
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.Flags = MOUSE_MOVE_ABSOLUTE;
	mid.LastX = dx * 0xffff / GetSystemMetrics(SM_CXSCREEN);
	mid.LastY = dy * 0xffff / GetSystemMetrics(SM_CYSCREEN);
	if (!DeviceIoControl(g_DriverHandle, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL))
	{
		DebugInf(_T("MS - DeviceIoControl:%d"), GetLastError());
		return false;
	}
	return true;
}

bool __stdcall MsKmKeyPress(USHORT VirtualKey, bool bShift, DWORD dwDelay)
{
	bool bRet = false;
	if (bShift)
	{
		bRet = KeyDown(VK_SHIFT);
		if (!bRet)
		{
			return false;
		}
		::Sleep(dwDelay);
	}

	bRet = KeyDown(VirtualKey);
	if (!bRet)
	{
		return false;
	}
	::Sleep(dwDelay);
	bRet = KeyUp(VirtualKey);
	if (!bRet)
	{
		return false;
	}

	if (bShift)
	{
		::Sleep(dwDelay);
		bRet = KeyUp(VK_SHIFT);
		if (!bRet)
		{
			return false;
		}
	}
	return true;
}

bool __stdcall MsKmMouseLeftClick(DWORD dwDelay)
{	
	bool bRet = MouseLeftButtonDown();
	if (!bRet)
	{
		return false;
	}
	::Sleep(dwDelay);
	bRet = MouseLeftButtonUp();
	if (!bRet)
	{
		return false;
	}
	return true;
}

bool __stdcall MsKmMouseLeftDbClick(DWORD dwDelay)
{
	bool bRet = MsKmMouseLeftClick(dwDelay);
	if (!bRet)
	{
		return false;
	}
	::Sleep(100);
	bRet = MsKmMouseLeftClick(dwDelay);
	if (!bRet)
	{
		return false;
	}
	return true;
}

bool __stdcall MsKmMouseRightClick(DWORD dwDelay)
{
	bool bRet = MouseRightButtonDown();
	if (!bRet)
	{
		return false;
	}
	::Sleep(dwDelay);
	bRet = MouseRightButtonUp();
	if (!bRet)
	{
		return false;
	}
	return true;
}

bool __stdcall MsKmMouseMiddleClick(DWORD dwDelay)
{
	bool bRet = MouseMiddleButtonDown();
	if (!bRet)
	{
		return false;
	}
	::Sleep(dwDelay);
	bRet = MouseMiddleButtonUp();
	if (!bRet)
	{
		return false;
	}
	return true;
}

bool __stdcall MsKmMouseMove(LONG dx, LONG dy)
{
	bool bRet = MouseMoveABSOLUTE(dx, dy);
	if (!bRet)
	{
		return false;
	}
	return true;
}

bool __stdcall MsKmPressPassword(HWND hGame, LPTSTR lpszText, DWORD dwDelay)
{
	char szText[1024] = { 0 };
	sprintf_s(szText, "%s", lpszText);
	DebugInf(_T("MS - %s"), szText);

	for (size_t i = 0; i < strlen(szText); ++i)
	{
		::Sleep(dwDelay);
		bool bShift = false;
		char crKey = char(szText[i]);
		USHORT wKeyCode = (USHORT)char(szText[i]);
		DebugInf(_T("MS - %c %d"), crKey, wKeyCode);
		if (::IsWindow(hGame))
		{
			if (::GetForegroundWindow() != hGame) continue;
		}
		
		if ((wKeyCode >= 65) && (wKeyCode <= 90))
		{
			bShift = true;
		}
		if (GKeyMapLst.find(crKey) != GKeyMapLst.end())
		{
			wKeyCode = (USHORT)GKeyMapLst[crKey];
			bShift = true;
		}
		if (GKeyMapCode.find(crKey) != GKeyMapCode.end())
		{
			wKeyCode = GKeyMapCode[crKey];
		}
		DebugInf(_T("MS - %c %d"), crKey, wKeyCode);
		//KeyDown(wKeyCode);
		//Sleep(100);
		//KeyUp(wKeyCode);
		MsKmKeyPress(wKeyCode, bShift, dwDelay);
	}
	return true;
}

BOOL __stdcall LoadNTDriver(bool IsDemandLoaded = true)
{
	char szDriverImagePath[256];
	char* pszSlash;
	//得到完整的驱动路径
	GetModuleFileNameA(GetModuleHandle(NULL), szDriverImagePath, sizeof(szDriverImagePath));
	pszSlash = _tcsrchr(szDriverImagePath, '\\');
	if (pszSlash)
		pszSlash[1] = 0;
	else
	{
		DebugInf(_T("MS - Get DriverImagePath() Faild %d !"), GetLastError());
		return false;
	}

	if (g_Is64BitOS)
		_tcscat_s(szDriverImagePath, "MsKm64.sys");
	//GetFullPathNameA("MsKm64.sys", 256, szWinIoDriverPath, NULL);
	else
		_tcscat_s(szDriverImagePath, "MsKm32.sys");
	//GetFullPathNameA("MsKm32.sys", 256, szWinIoDriverPath, NULL);

	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄

	UnloadNTDriver(KEYMOUSE_DRIVER_NAME);

	//打开服务控制管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		//OpenSCManager失败
		DebugInf(_T("MS - OpenSCManager() Faild %d !"), GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		////OpenSCManager成功
		DebugInf(_T("MS - OpenSCManager() ok !"));
	}

	//创建驱动所对应的服务
	hServiceDDK = CreateServiceA(hServiceMgr,
		KEYMOUSE_DRIVER_NAME, //驱动程序的在注册表中的名字  
		KEYMOUSE_DRIVER_NAME, // 注册表驱动程序的 DisplayName 值  
		SERVICE_ALL_ACCESS, // 加载驱动程序的访问权限  
		SERVICE_KERNEL_DRIVER,// 表示加载的服务是驱动程序  
		//SERVICE_FILE_SYSTEM_DRIVER,
		(IsDemandLoaded = true) ? SERVICE_DEMAND_START : SERVICE_SYSTEM_START, // 注册表驱动程序的 Start 值  
		SERVICE_ERROR_NORMAL, // 注册表驱动程序的 ErrorControl 值  
		szDriverImagePath, // 注册表驱动程序的 ImagePath 值  
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	DWORD dwRtn;
	//判断服务是否失败
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			//由于其他原因创建服务失败
			DebugInf(_T("MS - CrateService() Faild %d !"), dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			//服务创建失败，是由于服务已经创立过
			DebugInf(_T("MS - CrateService() Faild Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS!"));
		}

		// 驱动程序已经加载，只需要打开  
		hServiceDDK = OpenServiceA(hServiceMgr, KEYMOUSE_DRIVER_NAME, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			//如果打开服务也失败，则意味错误
			dwRtn = GetLastError();
			DebugInf(_T("MS - OpenService() Faild %d !"), dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			DebugInf(_T("MS - OpenService() ok !"));
		}
	}
	else
	{
		DebugInf(_T("MS - CrateService() ok !"));
	}

	//开启此项服务
	if (!StartService(hServiceDDK, 0, NULL))
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn == ERROR_ALREADY_EXISTS)
		{
			if (StartService(hServiceDDK, 0, NULL))
			{
				bRet = TRUE;
				goto BeforeLeave;
			}
			dwRtn = GetLastError();
		}
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			DebugInf(_T("MS - StartService() Faild %d %s!"), dwRtn, szDriverImagePath);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				//设备被挂住
				DebugInf(_T("MS - StartService() Faild ERROR_IO_PENDING !"));
				bRet = FALSE;
				goto BeforeLeave;
			}
			else
			{
				//服务已经开启
				DebugInf(_T("MS - StartService() Faild ERROR_SERVICE_ALREADY_RUNNING !"));
				bRet = TRUE;
				goto BeforeLeave;
			}
		}
	}
	bRet = TRUE;
	//离开前关闭句柄
BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}
/*
//卸载驱动程序  
BOOL __stdcall UnloadNTDriver(char * szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄
	SERVICE_STATUS SvrSta;
	//打开SCM管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		//带开SCM管理器失败
		DebugInf(_T("MS - OpenSCManager() Faild %d !"), GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		//带开SCM管理器失败成功
		DebugInf(_T("MS - OpenSCManager() ok !"));
	}
	//打开驱动所对应的服务
	hServiceDDK = OpenServiceA(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		//打开驱动所对应的服务失败
		DebugInf(_T("MS - OpenService() Faild %d !"), GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		DebugInf(_T("MS - OpenService() ok !"));
	}	
	////动态卸载驱动程序。  
	//if (!DeleteService(hServiceDDK))
	//{
	//	//卸载失败
	//	printf("DeleteSrevice() Faild %d !\n", GetLastError());
	//}
	//else
	//{
	//	//卸载成功
	//	printf("DelServer:eleteSrevice() ok !\n");
	//}

	DWORD dwBytesNeeded;
	DWORD cbBufSize;
	LPQUERY_SERVICE_CONFIGA pServiceConfig;

	bRet = QueryServiceConfigA(hServiceDDK, NULL, 0, &dwBytesNeeded);

	if (!bRet)
	{
		DebugInf(_T("MS - QueryServiceConfigA() Faild %d %d!"), GetLastError(), ERROR_INSUFFICIENT_BUFFER);
	}

	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		cbBufSize = dwBytesNeeded;
		pServiceConfig = (LPQUERY_SERVICE_CONFIGA)malloc(cbBufSize);
		if (!QueryServiceConfigA(hServiceDDK, pServiceConfig, cbBufSize, &dwBytesNeeded))
		{
			free(pServiceConfig);
			DebugInf(_T("MS - QueryServiceConfig() Faild %d !"), GetLastError());
		}
		DebugInf(_T("MS - QueryServiceConfig() ok %d %d!"), pServiceConfig->dwStartType, SERVICE_DEMAND_START);
		//If service is set to load automatically, don't delete it!
		if (pServiceConfig->dwStartType == SERVICE_DEMAND_START)
		{
			//停止驱动程序，如果停止失败，只有重新启动才能，再动态加载。  
			if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta))
			{
				DebugInf(_T("MS - ControlService() Faild %d !"), GetLastError());
			}
			else
			{
				//打开驱动所对应的失败
				DebugInf(_T("MS - ControlService() ok !"));
			}

			if (!DeleteService(hServiceDDK))
			{
				//卸载失败
				DebugInf(_T("MS - DeleteSrevice() Faild %d !"), GetLastError());
			}
			else
			{
				//卸载成功
				DebugInf(_T("MS - DeleteSrevice() ok !"));
			}
		}
	}
	bRet = TRUE;
BeforeLeave:
	//离开前关闭打开的句柄
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}
*/

//卸载驱动程序  
BOOL __stdcall UnloadNTDriver(char * szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄
	SERVICE_STATUS SvrSta;

	CloseHandle(g_DriverHandle);

	//打开SCM管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		//带开SCM管理器失败
		DebugInf(_T("MS - OpenSCManager() Faild %d ! \n"), GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		//带开SCM管理器失败成功
		//printf("OpenSCManager() ok ! \n");
	}
	//打开驱动所对应的服务
	hServiceDDK = OpenServiceA(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		//打开驱动所对应的服务失败
		DebugInf(_T("MS - OpenService() Faild %d ! \n"), GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		//printf("OpenService() ok ! \n");
	}
	//停止驱动程序，如果停止失败，只有重新启动才能，再动态加载。  
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta))
	{
		DebugInf(_T("MS - ControlService() Faild %d !\n"), GetLastError());
	}
	else
	{
		//打开驱动所对应的失败
		DebugInf(_T("MS - ControlService() ok !\n"));
	}
	//动态卸载驱动程序。  
	if (!DeleteService(hServiceDDK))
	{
		//卸载失败
		DebugInf(_T("MS - DeleteSrevice() Faild %d !\n"), GetLastError());
	}
	else
	{
		//卸载成功
		DebugInf(_T("MS - DeleteSrevice() ok !\n"));
	}
	bRet = TRUE;
BeforeLeave:
	//离开前关闭打开的句柄
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

BOOL __stdcall InitMsKm()
{
	g_Is64BitOS = Is64BitOS();
	g_DriverHandle = CreateFileA(KEYMOUSE_WIN32_DEVICE_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (g_DriverHandle == INVALID_HANDLE_VALUE)
	{
		DebugInf("MS - 打开驱动文件失败, 重新加载...");
		BOOL bRet = LoadNTDriver();
		if (!bRet)
		{
			return FALSE;
		}	
		g_DriverHandle = CreateFileA(KEYMOUSE_WIN32_DEVICE_NAME,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if (g_DriverHandle == INVALID_HANDLE_VALUE)
		{
			DebugInf("MS - 打开驱动文件失败, GameOver...");
			return FALSE;
		}
		DebugInf("MS - 驱动文件句柄 %.8X", g_DriverHandle);
	}
	return TRUE;
}

BOOL __stdcall UnInitMsKm()
{	
	UnloadNTDriver(KEYMOUSE_DRIVER_NAME);
	UnloadNTDriver("kmclass");
	UnloadNTDriver("kmclass64");
	UnloadNTDriver("MsKm64");
	UnloadNTDriver("MsKm");
	return TRUE;
}
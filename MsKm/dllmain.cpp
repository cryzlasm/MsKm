// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "MsKm.h"
#include <tchar.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DebugInf("MS - 加载驱动");
		InitKeyMapLst();
		InitKeyMapCode();
		//EnableDebugPrivilege(TRUE);
		//加载前先卸载一下， 防止重复加载， 引起蓝屏
		//UnInitMsKm();
		//UnloadNTDriver("kmclass");
		if (!InitMsKm())
		{
			MessageBoxA(NULL, "驱动加载失败", "提示", MB_ICONINFORMATION);
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	{
		break;
	}
	case DLL_THREAD_DETACH:
	{
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		DebugInf("MS - 卸载驱动");
		UnInitMsKm();
		break;
	}
		
	}
	return TRUE;
}


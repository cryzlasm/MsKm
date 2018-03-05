// MsKmTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\MsKm\MsKm.h"
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	DebugInf(_T("MS - 加载驱动"));
	InitKeyMapLst();
	InitKeyMapCode();
	if (!InitMsKm())
	{
		MessageBoxA(NULL, "驱动加载失败", "提示", MB_ICONINFORMATION);
	}
	Sleep(2000);
	if ( argc > 1 )
	{
		TCHAR* szPassWord = argv[1];
		MsKmPressPassword(szPassWord);
	}
	else
	{
		MsKmPressPassword("abcdA!2$&*6");
	}	
	UnInitMsKm();
	//EnableDebugPrivilege(TRUE);
	//加载前先卸载一下， 防止重复加载， 引起蓝屏
	//UnInitMsKm();
	//if (!InitMsKm())
	//{
	//	MessageBoxA(NULL, "驱动加载失败", "提示", MB_ICONINFORMATION);
	//}
	//DebugInf("MS - 卸载驱动");
	//UnInitMsKm();
	getchar();
	return 0;
}


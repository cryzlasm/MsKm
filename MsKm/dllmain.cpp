// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
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
		DebugInf("MS - ��������");
		InitKeyMapLst();
		InitKeyMapCode();
		//EnableDebugPrivilege(TRUE);
		//����ǰ��ж��һ�£� ��ֹ�ظ����أ� ��������
		//UnInitMsKm();
		//UnloadNTDriver("kmclass");
		if (!InitMsKm())
		{
			MessageBoxA(NULL, "��������ʧ��", "��ʾ", MB_ICONINFORMATION);
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
		DebugInf("MS - ж������");
		UnInitMsKm();
		break;
	}
		
	}
	return TRUE;
}


// MsKmTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "..\MsKm\MsKm.h"
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	DebugInf(_T("MS - ��������"));
	InitKeyMapLst();
	InitKeyMapCode();
	if (!InitMsKm())
	{
		MessageBoxA(NULL, "��������ʧ��", "��ʾ", MB_ICONINFORMATION);
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
	//����ǰ��ж��һ�£� ��ֹ�ظ����أ� ��������
	//UnInitMsKm();
	//if (!InitMsKm())
	//{
	//	MessageBoxA(NULL, "��������ʧ��", "��ʾ", MB_ICONINFORMATION);
	//}
	//DebugInf("MS - ж������");
	//UnInitMsKm();
	getchar();
	return 0;
}


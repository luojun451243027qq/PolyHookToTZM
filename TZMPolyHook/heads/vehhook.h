#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <tlhelp32.h>
#include <vector>
//�������״��������ļ�ʱ���ж�����STR�Ƿ񱻶����������ǣ�ֱ��ִ��#endif�������䣻������ǣ�ִ��#ifndef��#endif֮������
#ifndef STR
//��������ж�STRδ�����������#define����STR
#define STR
//#ifndef��#endif֮������,�����д����
//�����������
#endif
//�������������������ط�����
extern char str[1024];
using namespace std;
//����Ӳ���ϵ�ṹ
struct CPUSINGLE
{
	//Ӳ���ϵ��ַ,hook��ַ
	uintptr_t Dr;
	//Ӳ���ϵ����λ����
	uintptr_t Dr7;
	//hook���Լ���asm��ʵ�ֵķ���
	uintptr_t hkFun;
	//��Ҫ�������ֽ��볤��
	uintptr_t size;
	//Ӳ���ϵ��Ƿ�����
	bool of;
};

//����GUARD_PAGE�ϵ�ṹ
struct GUARD_PAGES
{
	//��־λ�ϵ�hook��ַ
	uintptr_t gpAdress;
	//hook���Լ���asm��ʵ�ֵķ���
	uintptr_t hkFun;
	//��Ҫ�������ֽ��볤��
	uintptr_t size;
	//�Ƿ����öϵ�
	bool ofGUARD_PAGE = true;
	//�ɱ�־λ
	DWORD oldProtection;
};

//��������veh�ϵ�������
extern vector<PVOID> veh_Handle;

extern VOID SetThreadHook(HANDLE hThread, CPUSINGLE cpus);
EXTERN_C void moneyfunc();
EXTERN_C void moneyfuncCPU();
//����Ӳ���ϵ�����
extern vector<CPUSINGLE> exceptionInfo;

//����GUARD_PAGE�ϵ�����
extern vector<GUARD_PAGES> guardInfo;
//������ͨ�ϵ�HOOK
extern bool  Hook();
//����Ӳ���ϵ�HOOK
extern VOID SetHook();
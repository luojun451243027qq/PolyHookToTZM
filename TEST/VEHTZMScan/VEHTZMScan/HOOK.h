#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include<tlhelp32.h>
#include<vector>

using namespace std;
//���ݳ�����64λ����32λ��ʼ������
#ifdef _WIN64
#define XAX Rax
#define XBX Rbx
#define XCX Rcx
#define XDX Rdx
#define XSI Rsi
#define XDI Rdi
#define XBP Rbp
#define XSP Rsp
#define XIP Rip
#else
#define XAX Eax
#define XBX Ebx
#define XCX Ecx
#define XDX Edx
#define XSI Esi
#define XDI Edi
#define XBP Ebp
#define XSP Esp
#define XIP Eip
#endif


//��������
PVOID VEH_Handle = nullptr;
char str[1024]{};

//��ԭʼ������ַ�ṩ��asm�ļ�ʹ��
EXTERN_C uintptr_t ogAdress = 0;

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

//����Ӳ���ϵ�����
vector<CPUSINGLE> exceptionInfo;

//����GUARD_PAGE�ϵ�����
vector<GUARD_PAGES> guardInfo;

//��������veh�ϵ�������
vector<PVOID> veh_Handle;

//��������
bool Hook();
bool AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2);
LONG WINAPI Handler(EXCEPTION_POINTERS* pExceptionInfo);



// _WIN64
#ifdef _WIN64

//���ԭʼ����ִ�еĺ���(���ڻ�ඨ��Ϊ����Ĵ���->����NewFunc->��ԭ�Ĵ���->������һ����)
EXTERN_C void HookFunc();

//���ԭʼ����ִ�еĺ���(���ڻ�ඨ��Ϊ����Ĵ���->����NewFunc->��ԭ�Ĵ���->������һ����)(Ӳ���ϵ�ʹ��)
EXTERN_C void HookFuncCpu();

//�Զ��庯��,���Խ���һЩɧ����,Ҳ�����滻��asm����ʵ�ֹ���
EXTERN_C void NewFunc() {
	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
}

//�Զ��庯��,���Խ���һЩɧ����,Ҳ�����滻��asm����ʵ�ֹ���(Ӳ���ϵ�)
EXTERN_C void NewFuncCpu() {
	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
}
#else

//�Զ��庯��,���Խ���һЩɧ����,Ҳ�����滻��asm����ʵ�ֹ���
//void NewFunc() {
//	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
//}
EXTERN_C void NewFunc() {
	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
}

//�Զ��庯��,���Խ���һЩɧ����,Ҳ�����滻��asm����ʵ�ֹ���
void NewFuncCpu() {
	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
}

//���ԭʼ����ִ�еĺ���(���ڻ�ඨ��Ϊ����Ĵ���->����NewFunc->��ԭ�Ĵ���->������һ����)
EXTERN_C void HookFunc();

//���ԭʼ����ִ�еĺ���(���ڻ�ඨ��Ϊ����Ĵ���->����NewFunc->��ԭ�Ĵ���->������һ����)
EXTERN_C void HookFuncCpu();


EXTERN_C void HOOKjinbi();
EXTERN_C void HOOKmaxhp();

#endif 

//����GUARD_PAGEVEH�쳣����	ExceptionRecord-> �쳣��Ϣ��¼ ContextRecord-> �Ĵ�����Ϣ  ����-1���쳣�Ѵ�������ִ�У�����0����������VEH��������������
LONG WINAPI  Handler(EXCEPTION_POINTERS* pExceptionInfo)
{
	//�ж��쳣���� STATUS_GUARD_PAGE_VIOLATION(GUARD_PAGE�쳣)  EXCEPTION_SINGLE_STEP(Ӳ���ϵ��쳣)
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		//�������жϵ�
		for (size_t i = 0; i < guardInfo.size(); i++)
		{
			//�ж��Ƿ�Ϊ���õ��쳣λ��
			if (pExceptionInfo->ContextRecord->XIP == (uintptr_t)guardInfo[i].gpAdress)
			{
				//���ݳ�����64λ����32λ��ʼ������
#ifdef _WIN64

			//�Զ������(Hook����,���Բ����Ĵ���)
				MessageBox(0, "����VEH", "VEH Hook", MB_SYSTEMMODAL);
				MessageBox(0, "��ʼ��ȡ�Ĵ�������", "VEH Hook", MB_SYSTEMMODAL);


				sprintf_s(str, 1024, "RAX=%016I64X\nRBX=%016I64X\nRCX=%016I64X\nRDX=%016I64X\nRSI=%016I64X\nRDI=%016I64X\nRBP=%016I64X\nRSP=%016I64X\nR8=%016I64X\nR9=%016I64X\nR10=%016I64X\nR11=%016I64X\nR12=%016I64X\n",
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->R8, pExceptionInfo->ContextRecord->R9,
					pExceptionInfo->ContextRecord->R10, pExceptionInfo->ContextRecord->R11,
					pExceptionInfo->ContextRecord->R12);

				MessageBox(0, str, "��VEH��ȡ�ļĴ�������:", MB_SYSTEMMODAL);

				//ģ���޸ļĴ�����ֵ
				//ֱ���������Ĵ�����ֵ���
				//�����dllע����Ҫmemcpy���������ڴ�
				//pExceptionInfo->ContextRecord->XCX = pExceptionInfo->ContextRecord->XDX;

				//�ֶ���ֵ(��Ҫ��ԭֵ����ƥ��,���Գ����Ǵ��ݵ�ָ��,�ֶ�ģ���˸�ָ��)
				//pExceptionInfo->ContextRecord->Rcx = (DWORD64)0x7FF7DDA01360;

				/*sprintf_s(str, 1024, "RAX=%016I64X\nRBX=%016I64X\nRCX=%016I64X\nRDX=%016I64X\nRSI=%016I64X\nRDI=%016I64X\nRBP=%016I64X\nRSP=%016I64X\nR8=%016I64X\nR9=%016I64X\nR10=%016I64X\nR11=%016I64X\nR12=%016I64X\n",
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->R8, pExceptionInfo->ContextRecord->R9,
					pExceptionInfo->ContextRecord->R10, pExceptionInfo->ContextRecord->R11,
					pExceptionInfo->ContextRecord->R12);

				MessageBox(0, str, "�޸ĺ��ȡ�ļĴ�������:", MB_SYSTEMMODAL);*/

				//��������ַƫ��X���ֽڵ���һ�������,Xȡ����hook�Ļ����볤��,�º���Ҫ��asm���滹ԭԭʼ������
				ogAdress = guardInfo[i].gpAdress + guardInfo[i].size;

				//���hook������ᵼ��asm�޷����÷���,ֻ�����������ȡ��hook
				DWORD old;
				VirtualProtect((LPVOID)guardInfo[i].gpAdress, 1, guardInfo[i].oldProtection, &old);

				//����Ҫ��STATUS_SINGLE_STEP���治�������,��Ȼ����ѭ��
				guardInfo[i].ofGUARD_PAGE = false;

				//��ת���Լ��ĺ���,��ԭ�����Ļ�����,Ȼ��jmp��������ַ+�������ֽڼ���ִ��
				pExceptionInfo->ContextRecord->XIP = (uintptr_t)guardInfo[i].hkFun;

#else
//�Զ����޸���Ϣ���е�����
//��֪����HOOK�ĺ�����MessageBoxA������������ĸ�����������ԭ������:/*int WINAPI MessageBoxA(  _In _opt_ HWND hWnd,_In_opt_ LPCSTR lpText,_In_opt_ LPCSTR lpCaption��_In_ UINT uType) ;*/
//��ô�ڽ��뺯��ʱ�����ĸ������ֱ�λ��ESP+Ox4��ESP+Ox8��ESP+0xC��ESP+0x10��λ����
//��֪����HOOK�ĺ�����MessageBoxA������������ĸ�����������ԭ������:/*int WINAPI MessageBoxA(  _In _opt_ HWND hWnd,_In_opt_ LPCSTR lpText,_In_opt_ LPCSTR lpCaption��_In_ UINT uType) ;*/
//��ô�ڽ��뺯��ʱ�����ĸ������ֱ�λ��ESP+Ox4��ESP+Ox8��ESP+0xC��ESP+0x10��λ����
//����������Ҫ�޸����е�lpText��������ô���������λ��ESP+8��λ���ϣ���������ַ����ĵ�ַ
//��Ҫ�޸ģ�����������Ҫ׼��һ���滻�õ��ַ���
// 

			//�Զ������(Hook����,���Բ����Ĵ���)
				MessageBox(0, "����VEH", "VEH Hook", MB_SYSTEMMODAL);
				MessageBox(0, "��ʼ��ȡ�Ĵ�������", "VEH Hook", MB_SYSTEMMODAL);

				//����Ĵ�������
				sprintf_s(str, 1024, "XSP+4=%016I32X\nXSP+8=%016I32X\nEAX=%016I32X\nEBX=%016I32X\nECX=%016I32X\nEDX=%016I32X\nESI=%016I32X\nEDI=%016I32X\nEBP=%016I32X\nESP=%016I32X\nEIP=%016I32X\n",
					*(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x4), *(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x8),
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->XIP);

				MessageBox(0, str, "��VEH��ȡ�ļĴ�������:", MB_SYSTEMMODAL);

				//ģ���޸ļĴ�����ֵ
				//ֱ���������Ĵ�����ֵ���
				//*(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x4) = *(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x8);


				//����Ĵ�������
				/*sprintf_s(str, 1024, "XSP+4=%016I32X\nXSP+8=%016I32X\nEAX=%016I32X\nEBX=%016I32X\nECX=%016I32X\nEDX=%016I32X\nESI=%016I32X\nEDI=%016I32X\nEBP=%016I32X\nESP=%016I32X\nEIP=%016I32X\n",
					*(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x4), *(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x8),
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->XIP);

				MessageBox(0, str, "�޸ĺ�ļĴ�������:", MB_SYSTEMMODAL);*/

				//const char* szStr = "zxbly";
				//// ��ESP��8ǿת��DWORD*��Ȼ���ֵȡ�������滻��szStr��szStrҲ��Ҫǿת
				//*(DWORD*)(pExceptionInfo->ContextRecord->Esp + 0x8) = (DWORD)szStr;



				///��������ַƫ��X���ֽڵ���һ�������,Xȡ����hook�Ļ����볤��,�º���Ҫ��asm���滹ԭԭʼ������
				ogAdress = guardInfo[i].gpAdress + guardInfo[i].size;

				//���hook������ᵼ��asm�޷����÷���,ֻ�����������ȡ��hook
				DWORD old;
				VirtualProtect((LPVOID)guardInfo[i].gpAdress, 1, guardInfo[i].oldProtection, &old);

				//����Ҫ��STATUS_SINGLE_STEP���治�������,��Ȼ����ѭ��
				guardInfo[i].ofGUARD_PAGE = false;

				//��ת���Լ��ĺ���,��ԭ�����Ļ�����,Ȼ��jmp��������ַ+�������ֽڼ���ִ��
				pExceptionInfo->ContextRecord->XIP = (uintptr_t)guardInfo[i].hkFun;
#endif
			}
			//TFλ���ó�1��cpu���뵥������ģʽ��ִ����һ��ָ��ʱ��ͬ���ᴥ��STATUS_SINGLE_STEP�쳣��������������ڵ����if����
			pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed. In short, we come right back into this exception handler 1 instruction later

		//������һ��ָ��
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	//��������STATUS_SINGLE_STEP������ζ�Ÿոշ�����PAGE_GUARD�쳣,��Ҫ������ӱ�־λ
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
	{
		DWORD dwOld;
		for (size_t i = 0; i < guardInfo.size(); i++)
		{
			//�ж��Ƿ�Ϊ���õ��쳣λ�ò�����������
			if (guardInfo[i].ofGUARD_PAGE)
			{
				//����Ӧ��PAGE_GUARD��־����Ϊÿ�δ�����ʱ�������ᱻɾ��
				VirtualProtect((LPVOID)guardInfo[i].gpAdress, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag because every_time it is triggered, it get removes
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}

		//������һ��ָ��
		return EXCEPTION_CONTINUE_EXECUTION;

	}
	//�������PAGE_GUARD��SINGLE_STEP���������������쳣�����б����ҵ���ȷ�Ĵ������
	return EXCEPTION_CONTINUE_SEARCH;
}
//����Ӳ���ϵ�VEH�쳣����	ExceptionRecord-> �쳣��Ϣ��¼ ContextRecord-> �Ĵ�����Ϣ  ����-1���쳣�Ѵ�������ִ�У�����0����������VEH��������������
LONG WINAPI  CpuHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
	//�ж��쳣���� STATUS_GUARD_PAGE_VIOLATION(GUARD_PAGE�쳣)  EXCEPTION_SINGLE_STEP(Ӳ���ϵ��쳣)
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		//�ж��Ƿ�Ϊ���õ��쳣λ��
		for (size_t i = 0; i < exceptionInfo.size(); i++)
		{
			if (exceptionInfo[i].of && exceptionInfo[i].Dr == pExceptionInfo->ContextRecord->XIP)
			{
				//���ݳ�����64λ����32λ��ʼ������
#ifdef _WIN64

			//�Զ������(Hook����,���Բ����Ĵ���)
				MessageBox(0, "����VEH", "VEH Hook", MB_SYSTEMMODAL);
				MessageBox(0, "��ʼ��ȡ�Ĵ�������", "VEH Hook", MB_SYSTEMMODAL);


				sprintf_s(str, 1024, "RAX=%016I64X\nRBX=%016I64X\nRCX=%016I64X\nRDX=%016I64X\nRSI=%016I64X\nRDI=%016I64X\nRBP=%016I64X\nRSP=%016I64X\nR8=%016I64X\nR9=%016I64X\nR10=%016I64X\nR11=%016I64X\nR12=%016I64X\n",
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->R8, pExceptionInfo->ContextRecord->R9,
					pExceptionInfo->ContextRecord->R10, pExceptionInfo->ContextRecord->R11,
					pExceptionInfo->ContextRecord->R12);
				MessageBox(0, str, "��VEH��ȡ�ļĴ�������:", MB_SYSTEMMODAL);

				//ģ���޸ļĴ�����ֵ
				//ֱ���������Ĵ�����ֵ���
				//�����dllע����Ҫmemcpy���������ڴ�
				//pExceptionInfo->ContextRecord->XCX = pExceptionInfo->ContextRecord->XDX;

				//�ֶ���ֵ(��Ҫ��ԭֵ����ƥ��,���Գ����Ǵ��ݵ�ָ��,�ֶ�ģ���˸�ָ��)
				//pExceptionInfo->ContextRecord->Rcx = (DWORD64)0x7FF7DDA01360;

				/*sprintf_s(str, 1024, "RAX=%016I64X\nRBX=%016I64X\nRCX=%016I64X\nRDX=%016I64X\nRSI=%016I64X\nRDI=%016I64X\nRBP=%016I64X\nRSP=%016I64X\nR8=%016I64X\nR9=%016I64X\nR10=%016I64X\nR11=%016I64X\nR12=%016I64X\n",
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->R8, pExceptionInfo->ContextRecord->R9,
					pExceptionInfo->ContextRecord->R10, pExceptionInfo->ContextRecord->R11,
					pExceptionInfo->ContextRecord->R12);

				MessageBox(0, str, "�޸ĺ��ȡ�ļĴ�������:", MB_SYSTEMMODAL);*/

				//��������ַƫ��X���ֽڵ���һ�������,Xȡ����hook�Ļ����볤��,�º���Ҫ��asm���滹ԭԭʼ������
				ogAdress = exceptionInfo[i].Dr + exceptionInfo[i].size;
				//���hook������ᵼ��asm�޷����÷���,ֻ�����������ȡ��hook
				switch (i)
				{
				case 0:
					pExceptionInfo->ContextRecord->Dr0 = 0;
					break;
				case 1:
					pExceptionInfo->ContextRecord->Dr1 = 0;
					break;
				case 2:
					pExceptionInfo->ContextRecord->Dr2 = 0;
					break;;
				case 3:
					pExceptionInfo->ContextRecord->Dr3 = 0;
					break;
				default:
					break;
			}
				pExceptionInfo->ContextRecord->Dr7 = 0;
				exceptionInfo[i].of = 0;
				//TFλ���ó�1��cpu���뵥������ģʽ��ִ����һ��ָ��ʱ��ͬ���ᴥ��STATUS_SINGLE_STEP�쳣��������������ڵ����if�����������ȡ������Ӳ���ϵ㣬������ﲻ���õ���ģʽ��������Ӳ���ϵ㶼��ʧЧ
				//pExceptionInfo->ContextRecord->EFlags |= 0x100;

				//��ת���Լ��ĺ���,��ԭ�����Ļ�����,Ȼ��jmp��������ַ+�������ֽڼ���ִ��
				pExceptionInfo->ContextRecord->XIP = (uintptr_t)exceptionInfo[i].hkFun;
#else
//�Զ����޸���Ϣ���е�����
//��֪����HOOK�ĺ�����MessageBoxA������������ĸ�����������ԭ������:/*int WINAPI MessageBoxA(  _In _opt_ HWND hWnd,_In_opt_ LPCSTR lpText,_In_opt_ LPCSTR lpCaption��_In_ UINT uType) ;*/
//��ô�ڽ��뺯��ʱ�����ĸ������ֱ�λ��ESP+Ox4��ESP+Ox8��ESP+0xC��ESP+0x10��λ����
//��֪����HOOK�ĺ�����MessageBoxA������������ĸ�����������ԭ������:/*int WINAPI MessageBoxA(  _In _opt_ HWND hWnd,_In_opt_ LPCSTR lpText,_In_opt_ LPCSTR lpCaption��_In_ UINT uType) ;*/
//��ô�ڽ��뺯��ʱ�����ĸ������ֱ�λ��ESP+Ox4��ESP+Ox8��ESP+0xC��ESP+0x10��λ����
//����������Ҫ�޸����е�lpText��������ô���������λ��ESP+8��λ���ϣ���������ַ����ĵ�ַ
//��Ҫ�޸ģ�����������Ҫ׼��һ���滻�õ��ַ���
// 

			//�Զ������(Hook����,���Բ����Ĵ���)
				//MessageBox(0, "����VEH", "VEH Hook", MB_SYSTEMMODAL);
				//MessageBox(0, "��ʼ��ȡ�Ĵ�������", "VEH Hook", MB_SYSTEMMODAL);

				////����Ĵ�������
				//sprintf_s(str, 1024, "XSP+4=%016I32X\nXSP+8=%016I32X\nEAX=%016I32X\nEBX=%016I32X\nECX=%016I32X\nEDX=%016I32X\nESI=%016I32X\nEDI=%016I32X\nEBP=%016I32X\nESP=%016I32X\nEIP=%016I32X\n",
				//	*(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x4), *(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x8),
				//	pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
				//	pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
				//	pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
				//	pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
				//	pExceptionInfo->ContextRecord->XIP);

				//MessageBox(0, str, "��VEH��ȡ�ļĴ�������:", MB_SYSTEMMODAL);

				//ģ���޸ļĴ�����ֵ
				//ֱ���������Ĵ�����ֵ���
				//*(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x4) = *(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x8);


				//����Ĵ�������
				/*sprintf_s(str, 1024, "XSP+4=%016I32X\nXSP+8=%016I32X\nEAX=%016I32X\nEBX=%016I32X\nECX=%016I32X\nEDX=%016I32X\nESI=%016I32X\nEDI=%016I32X\nEBP=%016I32X\nESP=%016I32X\nEIP=%016I32X\n",
					*(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x4), *(DWORD*)(pExceptionInfo->ContextRecord->XSP + 0x8),
					pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
					pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
					pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
					pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
					pExceptionInfo->ContextRecord->XIP);

				MessageBox(0, str, "�޸ĺ�ļĴ�������:", MB_SYSTEMMODAL);*/

				//const char* szStr = "zxbly";
				//// ��ESP��8ǿת��DWORD*��Ȼ���ֵȡ�������滻��szStr��szStrҲ��Ҫǿת
				//*(DWORD*)(pExceptionInfo->ContextRecord->Esp + 0x8) = (DWORD)szStr;


				//��������ַƫ��X���ֽڵ���һ�������,Xȡ����hook�Ļ����볤��,�º���Ҫ��asm���滹ԭԭʼ������
				ogAdress = exceptionInfo[i].Dr + exceptionInfo[i].size;
				//���hook������ᵼ��asm�޷����÷���,ֻ�����������ȡ��hook
				//���ֻϣ��ִ��һ������ȡ�����е�Ӳ���ϵ㣬�������ǵ�ǰ��
				switch (i)
				{
				case 0:
					pExceptionInfo->ContextRecord->Dr0 = 0;
					break;
				case 1:
					pExceptionInfo->ContextRecord->Dr1 = 0;
					break;
				case 2:
					pExceptionInfo->ContextRecord->Dr2 = 0;
					break;;
				case 3:
					pExceptionInfo->ContextRecord->Dr3 = 0;
					break;
				default:
					break;
				}
				pExceptionInfo->ContextRecord->Dr7 = 0;
				exceptionInfo[i].of = 0;
				//TFλ���ó�1��cpu���뵥������ģʽ��ִ����һ��ָ��ʱ��ͬ���ᴥ��STATUS_SINGLE_STEP�쳣��������������ڵ����if�����������ȡ������Ӳ���ϵ㣬������ﲻ���õ���ģʽ��������Ӳ���ϵ㶼��ʧЧ
				//pExceptionInfo->ContextRecord->EFlags |= 0x100;
				//��ת���Լ��ĺ���,��ԭ�����Ļ�����,Ȼ��jmp��������ַ+�������ֽڼ���ִ��
				pExceptionInfo->ContextRecord->XIP = (uintptr_t)exceptionInfo[i].hkFun;
#endif
		}
			else
			{
				//����Ӳ���ϵ�ʧЧ������һ��
				switch (i)
				{
				case 0:
					pExceptionInfo->ContextRecord->Dr0 = exceptionInfo[i].Dr;
					break;
				case 1:
					pExceptionInfo->ContextRecord->Dr1 = exceptionInfo[i].Dr;
					break;
				case 2:
					pExceptionInfo->ContextRecord->Dr2 = exceptionInfo[i].Dr;
					break;;
				case 3:
					pExceptionInfo->ContextRecord->Dr3 = exceptionInfo[i].Dr;
					break;
				default:
					break;
				}

				pExceptionInfo->ContextRecord->Dr7 = exceptionInfo[i].Dr7;
			}
	}
		//������һ��ָ��
		return EXCEPTION_CONTINUE_EXECUTION;
}

	//�������Ӳ���ϵ��쳣���������������쳣�����б����ҵ���ȷ�Ĵ������
	return EXCEPTION_CONTINUE_SEARCH;
}

//�ж��Ƿ���ͬһ��ҳ��ķ���
bool  AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2)
{
	MEMORY_BASIC_INFORMATION mbi1;
	//��ȡAddr1��ҳ����Ϣ
	if (!VirtualQuery(Addr1, &mbi1, sizeof(mbi1)))
		return true;

	MEMORY_BASIC_INFORMATION mbi2;
	//��ȡAddr2��ҳ����Ϣ
	if (!VirtualQuery(Addr2, &mbi2, sizeof(mbi2)))
		return true;
	//����Ƿ�Ϊͬһ��ҳ��
	if (mbi1.BaseAddress == mbi2.BaseAddress)
		//������ַ����ͬһҳ�У���ֹ�ҹ���
		return true;
	return false;
}

//����HOOK,��һ������ΪHOOK��,�ڶ���Ϊ�Զ��巽��
bool  Hook()
{
	for (size_t i = 0; i < guardInfo.size(); i++)
	{
		//���ǲ�����ͬһҳ�й�ס������������Ϊ���ǽ��������޻ص�
		if (AreInSamePage((const uint8_t*)guardInfo[i].gpAdress, (const uint8_t*)guardInfo[i].hkFun))
			return false;
		//ע���Զ����쳣�������	First���Ƿ����VEH��ͷ����Handler���쳣��������
		VEH_Handle = AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)Handler);
		veh_Handle.push_back(VEH_Handle);
		//�л�ҳ���ϵ�PAGE_GUARD��־
		if (VEH_Handle && VirtualProtect((LPVOID)guardInfo[i].gpAdress, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &(guardInfo[i].oldProtection)))
			return true;
	}

	return false;
}


//�����̵߳�Ӳ���ϵ�
VOID SetThreadHook(HANDLE hThread) {
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);
	for (size_t i = 0; i < exceptionInfo.size(); i++)
	{
		if (exceptionInfo[i].of)
		{
			switch (i)
			{
			case 0:
				ctx.Dr0 = exceptionInfo[i].Dr;
				break;
			case 1:
				ctx.Dr1 = exceptionInfo[i].Dr;
				break;
			case 2:
				ctx.Dr2 = exceptionInfo[i].Dr;
				break;
			case 3:
				ctx.Dr3 = exceptionInfo[i].Dr;
				break;
			default:
				break;
			}
		}
	}
	ctx.Dr7 = 0x405;
	SetThreadContext(hThread, &ctx);
}

//����Ӳ���ϵ�
VOID SetHook() {

	//�����쳣�ϵ�
	VEH_Handle = AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)CpuHandler);
	veh_Handle.push_back(VEH_Handle);
	//�����߳� ͨ��openthread��ȡ���̻߳���������Ӳ���ϵ�
	HANDLE hThreadShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (hThreadShot != INVALID_HANDLE_VALUE) {
		THREADENTRY32* ThreadInfo = new THREADENTRY32;
		ThreadInfo->dwSize = sizeof(THREADENTRY32);
		HANDLE hThread = NULL;
		//�����߳�
		while (Thread32Next(hThreadShot, ThreadInfo)) {
			//����̸߳�����IDΪ��ǰ����ID
			if (GetCurrentProcessId() == ThreadInfo->th32OwnerProcessID) {
				hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadInfo->th32ThreadID);
				//��ͣ�߳�
				SuspendThread(hThread);
				//����Ӳ���ϵ�
				SetThreadHook(hThread);
				//�ָ��߳�
				ResumeThread(hThread);
				CloseHandle(hThread);
			}

		}

		CloseHandle(hThread);
	}
}



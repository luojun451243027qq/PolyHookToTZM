#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>

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
//����hookԭʼ��ַ
uintptr_t og_fun;
//�����Զ��巽����ַ
uintptr_t hk_fun;
PVOID VEH_Handle;
DWORD oldProtection;
char str[1024]{};

//��������
bool Hook(uintptr_t og_fun, uintptr_t hk_fun);
//bool Unhook();
bool AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2);
LONG WINAPI Handler(EXCEPTION_POINTERS* pExceptionInfo);

//ȡ��HOOK(EXTERN_C��asm�ļ�����)
EXTERN_C bool  Unhook()
{
	MessageBox(0, "ȡ��Hook", "VEH Hook", MB_SYSTEMMODAL);
	//����ɱ�־
	DWORD old;
	if (VEH_Handle && //ȷ������ӵ��ע��VEH����Ч���
		VirtualProtect((LPVOID)og_fun, 1, oldProtection, &old) && //��ԭ�ɱ�־
		RemoveVectoredExceptionHandler(VEH_Handle)) //ж��VEH
		return true;

	return false;
}

// _WIN64
#ifdef _WIN64

//��ԭʼ������ַ�ṩ��asm�ļ�ʹ��
EXTERN_C INT64 ogAdress = 0;

//���ԭʼ����ִ�еĺ���(���ڻ�ඨ��Ϊ����Ĵ���->����Unhook->����NewFunc->��ԭ�Ĵ���->����ԭʼ����)
EXTERN_C void HookFunc();

//�Զ��庯��,���Խ���һЩɧ����,Ҳ�����滻��asm����ʵ�ֹ���
EXTERN_C void NewFunc() {
	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
}
#else

//�Զ��庯��,���Խ���һЩɧ����,Ҳ�����滻��asm����ʵ�ֹ���
void NewFunc() {
	MessageBox(0, "����asm�ļ����Զ��庯��!", "VEH Hook", MB_SYSTEMMODAL);
}

//__declspec(naked)��ֹ�����Զ���Ӷ�ջƽ��,���ӻᱻ�ƻ���ջ,�����Լ�ʹ�� RET �� RET n ָ��� (���ǲ����أ�����JMP��ԭ����); 
void __declspec(naked) HookFunc() {
	__asm {

		push eax;
		push ebx;
		push ecx;
		push edx;
		push esi;
		push edi;
		push ebp;
		push esp;
		call Unhook;
		call NewFunc;
		pop esp;
		pop ebp;
		pop edi;
		pop esi;
		pop edx;
		pop ecx;
		pop ebx;
		pop eax;
		jmp og_fun;

	}
}
#endif 






//����VEH�쳣����
LONG WINAPI  Handler(EXCEPTION_POINTERS* pExceptionInfo)
{
	//�ж��쳣����
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //We will catch PAGE_GUARD Violation
	{

		//�ж��Ƿ�Ϊ���õ��쳣λ��
		if (pExceptionInfo->ContextRecord->XIP == (uintptr_t)og_fun) //Make sure we are at the address we want within the page
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
			//pExceptionInfo->ContextRecord->Rcx = pExceptionInfo->ContextRecord->Rdx;

			//�ֶ���ֵ(��Ҫ��ԭֵ����ƥ��,���Գ����Ǵ��ݵ�ָ��,�ֶ�ģ���˸�ָ��)
			//pExceptionInfo->ContextRecord->Rcx = (DWORD64)0x7FF7DDA01360;

			sprintf_s(str, 1024, "RAX=%016I64X\nRBX=%016I64X\nRCX=%016I64X\nRDX=%016I64X\nRSI=%016I64X\nRDI=%016I64X\nRBP=%016I64X\nRSP=%016I64X\nR8=%016I64X\nR9=%016I64X\nR10=%016I64X\nR11=%016I64X\nR12=%016I64X\n",
				pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
				pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
				pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
				pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
				pExceptionInfo->ContextRecord->R8, pExceptionInfo->ContextRecord->R9,
				pExceptionInfo->ContextRecord->R10, pExceptionInfo->ContextRecord->R11,
				pExceptionInfo->ContextRecord->R12);

			MessageBox(0, str, "�޸ĺ��ȡ�ļĴ�������:", MB_SYSTEMMODAL);


			//ִ���Զ��巽��(hk_fun),����ִ��֮ǰ�ķ���(og_fun)
			//pExceptionInfo->ContextRecord->XIP = (uintptr_t)og_fun; //����ִ��ԭ����
			pExceptionInfo->ContextRecord->XIP = (uintptr_t)hk_fun; //��ת���Լ��ĺ���,ȡ��HOOKȻ��ִ��NewFunc,Ȼ��ص�֮ǰ�ĺ�����ַ����ִ��,�����ڳ�ʼ����ȡ�Ĵ�����ַ

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
			sprintf_s(str, 1024, "EAX=%016I32X\nEBX=%016I32X\nECX=%016I32X\nEDX=%016I32X\nESI=%016I32X\nEDI=%016I32X\nEBP=%016I32X\nESP=%016I32X\nEIP=%016I32X\n",
				pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
				pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
				pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
				pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
				pExceptionInfo->ContextRecord->XIP);

			MessageBox(0, str, "��VEH��ȡ�ļĴ�������:", MB_SYSTEMMODAL);

			//�޸ļĴ�������
			//pExceptionInfo->ContextRecord->XAX = (DWORD)0x270f;


			//����Ĵ�������
			sprintf_s(str, 1024, "EAX=%016I32X\nEBX=%016I32X\nECX=%016I32X\nEDX=%016I32X\nESI=%016I32X\nEDI=%016I32X\nEBP=%016I32X\nESP=%016I32X\nEIP=%016I32X\n",
				pExceptionInfo->ContextRecord->XAX, pExceptionInfo->ContextRecord->XBX,
				pExceptionInfo->ContextRecord->XCX, pExceptionInfo->ContextRecord->XDX,
				pExceptionInfo->ContextRecord->XSI, pExceptionInfo->ContextRecord->XDI,
				pExceptionInfo->ContextRecord->XBP, pExceptionInfo->ContextRecord->XSP,
				pExceptionInfo->ContextRecord->XIP);

			MessageBox(0, str, "�޸ĺ�ļĴ�������:", MB_SYSTEMMODAL);

			//const char* szStr = "zxbly";
			//// ��ESP��8ǿת��DWORD*��Ȼ���ֵȡ�������滻��szStr��szStrҲ��Ҫǿת
			//*(DWORD*)(pExceptionInfo->ContextRecord->Esp + 0x8) = (DWORD)szStr;

			//���ضϵ�λ�ü���ִ��
			//pExceptionInfo->ContextRecord->XIP = (uintptr_t)og_fun;
			//�����ϵ�������ִ��
			//pExceptionInfo->ContextRecord->XIP += 2;
			pExceptionInfo->ContextRecord->XIP = (uintptr_t)hk_fun;//��ת���Լ��ĺ���,ȡ��HOOKȻ��ִ��NewFunc,Ȼ��ص�֮ǰ�ĺ�����ַ����ִ��,�����ڳ�ʼ����ȡ�Ĵ�����ַ

#endif
		}
		//�Զ��彫��ִ����һ��ָ�����������STATUS_SINGLE_STEP�쳣�������֮�������Ժ󽫷��ص�����쳣�������1ָ��
		pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed. In short, we come right back into this exception handler 1 instruction later
		//������һ��ָ��
		return EXCEPTION_CONTINUE_EXECUTION; //Continue to next instruction
	}

	//���ǻ�������STATUS_SINGLE_STEP������ζ�����Ǹոշ�����PAGE_GUARD��ͻ
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) //We will also catch STATUS_SINGLE_STEP, meaning we just had a PAGE_GUARD violation
	{
		DWORD dwOld;
		//����Ӧ��PAGE_GUARD��־����Ϊÿ�δ�����ʱ�������ᱻɾ��
		VirtualProtect((LPVOID)og_fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag because every_time it is triggered, it get removes
		//������һ��ָ��
		return EXCEPTION_CONTINUE_EXECUTION; //Continue the next instruction
	}
	//�������PAGE_GUARD��SINGLE_STEP����������������쳣�����б����ҵ���ȷ�Ĵ������
	return EXCEPTION_CONTINUE_SEARCH; //Keep going down the exception handling list to find the right handler IF it is not PAGE_GUARD nor SINGLE_STEP
}

//�ж��Ƿ���ͬһ��ҳ��ķ���
bool  AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2)
{
	MEMORY_BASIC_INFORMATION mbi1;
	//��ȡAddr1��ҳ����Ϣ
	if (!VirtualQuery(Addr1, &mbi1, sizeof(mbi1))) //Get Page information for Addr1
		return true;

	MEMORY_BASIC_INFORMATION mbi2;
	//��ȡAddr2��ҳ����Ϣ
	if (!VirtualQuery(Addr2, &mbi2, sizeof(mbi2))) //Get Page information for Addr1
		return true;
	//����Ƿ�Ϊͬһ��ҳ��
	if (mbi1.BaseAddress == mbi2.BaseAddress) //See if the two pages start at the same Base Address
		//������ַ����ͬһҳ�У���ֹ�ҹ���
		return true; //Both addresses are in the same page, abort hooking!

	return false;
}

//����HOOK,��һ������ΪHOOK��,�ڶ���Ϊ�Զ��巽��
bool  Hook(uintptr_t original_fun, uintptr_t hooked_fun)
{
	//��ʼ������
	og_fun = 0;
	hk_fun = 0;
	VEH_Handle = nullptr;
	oldProtection = 0;


	//��ֵ����
	og_fun = original_fun;
	hk_fun = hooked_fun;

#ifdef _WIN64
	//����һ��ԭʼ������ַ��asm�ļ�����
	ogAdress = original_fun;
#endif // _WIN64



	//���ǲ�����ͬһҳ�й�ס������������Ϊ���ǽ��������޻ص�
	if (AreInSamePage((const uint8_t*)og_fun, (const uint8_t*)hk_fun))
		return false;

	//ע���Զ����쳣�������
	VEH_Handle = AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)Handler);

	//�л�ҳ���ϵ�PAGE_GUARD��־
	if (VEH_Handle && VirtualProtect((LPVOID)og_fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &oldProtection))
		return true;

	return false;
}



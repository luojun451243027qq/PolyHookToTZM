;����ģ�� ΢��ٷ�����https://learn.microsoft.com/zh-cn/cpp/assembler/masm/dot-386?view=msvc-150
;��Ҫ����Ŀ����->������->��������� /SAFESEH:NO ��Ȼ�ᱨ�� ģ����� SAFESEH ӳ���ǲ���ȫ��
;ָ��ָ�386,��ʾҪд386�ĳ���(Ҳ����32λ)������
;�� VS2019 �У��������������������� .asm �ļ���������Ҫ��ʽ������ʹ�� SSE ָ�����ֻ��Ҫֱ��д�� SSE ָ�����Ĵ�����ʹ�� xmm �Ĵ������ɡ�
;.386
;������ģʽ,flatΪWindows����ʹ�õ�ģʽ(���������ʹ��ͬһ��4GB��),cΪAPI����ʱ�ұߵĲ�������ջ(�ڴ�����Ϊƽ̹ģʽ,Ĭ�ϵ���Լ��c)
.model flat,c
;��ջ��С
.stack 4096
;ָ����ʶ����Сд����
option casemap:none
;����̶��������������й����в����޸�
.const
;��������Ҫ�õ��ĺ������ڳ���Ŀ�ʼ���ֱ���Ԥ������ �������� PROTO[���ù���]:[��һ����������][,:������������] ����MessageBoxA PROTO :dword , :dword, :dword, :dword

;����һ��������Ϊ��name��������Ϊ��type�����ⲿ��������ǩ����š�
;EXTERNDEF [language-type] name:type [, [language-type] name:type ...]
;�����ģ���ж��������ƣ���������Ϊ PUBLIC�� �����ģ�������������� ����������Ϊ EXTERN�� ���δ�������ƣ���������� ��type�������� ABS��������name����Ϊ�������롣 ͨ�����ڰ����ļ���
;;���������ͺ���
EXTERNDEF ogAdress:DWORD
EXTERNDEF HookFunc:PROTO
EXTERNDEF HookFuncCpu:PROTO

;�������� ������code segment
;��������
;[������] ���Ƿ� ���ʽ,[,���ʽ]
;���ʽ�� ? �򲻽��г�ʼ��
;��������������
;32λû����xmm�Ĵ������ݽ���,ֻ���Լ��������滻
.data
	;4�ֽ�����
	sss dword 999
	jinbi dword 999999999
	maxhpp dq  999999999.00
	;˫����
	;maxhpp dq  999999999.00
;����Σ�����end����ν���
.code
;���庯��
HookFunc PROC
	    ;����Ĵ���
		push eax;
		push ebx;
		push ecx;
		push edx;
		push esi;
		push edi;
		push ebp;
		push esp;
		;�����ⲿ����
		;call NewFunc;
		;��ԭ�Ĵ���
		pop esp;
		pop ebp;
		pop edi;
		pop esi;
		pop edx;
		pop ecx;
		pop ebx;
		pop eax;
		;�д��Լ���˽��
		mov eax, sss;
		;��ԭ������(������[edx]����ָ���ֽ���dword ptr����qword ptr)
		mov dword ptr[edx], eax;
		;��ת��ogAdress��ַ����ִ��
		jmp ogAdress;
HookFunc ENDP
HookFuncCpu PROC
	    ;����Ĵ���
		push eax;
		push ebx;
		push ecx;
		push edx;
		push esi;
		push edi;
		push ebp;
		push esp;
		;�����ⲿ����
		;call NewFunc;
		;��ԭ�Ĵ���
		pop esp;
		pop ebp;
		pop edi;
		pop esi;
		pop edx;
		pop ecx;
		pop ebx;
		pop eax;
		;�д��Լ���˽��
		mov eax, sss;
		;��ԭ������(������[edx]����ָ���ֽ���dword ptr����qword ptr)
		;movsd xmm0,maxhpp;
		;movsd qword ptr [esi+50h],xmm0;
		mov dword ptr[edx], eax;
		;��ת��ogAdress��ַ����ִ��
		jmp ogAdress;
HookFuncCpu ENDP
HOOKjinbi PROC
	    ;����Ĵ���
		push eax;
		push ebx;
		push ecx;
		push edx;
		push esi;
		push edi;
		push ebp;
		push esp;
		;�����ⲿ����
		;call NewFunc;
		;��ԭ�Ĵ���
		pop esp;
		pop ebp;
		pop edi;
		pop esi;
		pop edx;
		pop ecx;
		pop ebx;
		pop eax;
		;��ԭ������(������[edx]����ָ���ֽ���dword ptr����qword ptr)
		;mov dword ptr[esi+48h],jinbi;
		mov eax,jinbi;
		mov [esi+48h],eax;
		;��ת��ogAdress��ַ����ִ��
		jmp ogAdress;
HOOKjinbi ENDP
HOOKmaxhp PROC
	    ;����Ĵ���
		push eax;
		push ebx;
		push ecx;
		push edx;
		push esi;
		push edi;
		push ebp;
		push esp;
		;�����ⲿ����
		;call NewFunc;
		;��ԭ�Ĵ���
		pop esp;
		pop ebp;
		pop edi;
		pop esi;
		pop edx;
		pop ecx;
		pop ebx;
		pop eax;
		;��ԭ������(������[edx]����ָ���ֽ���dword ptr����qword ptr)
		movsd xmm0,maxhpp;
		movsd qword ptr [esi+50h],xmm0;
		;��ת��ogAdress��ַ����ִ��
		jmp ogAdress;
HOOKmaxhp ENDP
END 

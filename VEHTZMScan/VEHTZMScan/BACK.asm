;ע��:
;������Ҫ������ߵ����ı������ߺ���,dq��ʾqdword=64λ
extern ogAdress:dq;
;farΪ�������
extern NewFunc:far;
extern NewFuncCpu:far;

;��������ʼ
.code
;������ʼ
HookFunc PROC
;����Ĵ���
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
;������Ĵ��������������ⲿ����(��hook����ʱ���Ե���,hook����λᵼ��δ֪�쳣)
;call NewFunc;
;��ԭ�Ĵ���
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
;������NewFunc�н�og_funƫ�Ƶ�ogAdress,��Ҫ��ԭog_fun��ogAdress֮����ֽڱ���ʧ��
;����ͷmov qword ptr [rsp+10H],rdx
;�д��Լ���˽��
mov eax,666
;��ԭ������(������[rcx]����ָ���ֽ���dword ptr����qword ptr)
mov dword ptr [rcx],eax
;��ת��ogAdress��ַ����ִ��
jmp ogAdress;
;��������
HookFunc ENDP
;������ʼ
HookFuncCpu PROC
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push rsp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
pushf
;������Ĵ��������������ⲿ����(��hook����ʱ���Ե���,hook����λᵼ��δ֪�쳣)
;call NewFuncCpu;
;��ԭ�Ĵ���
popf
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rsp
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
;������NewFuncCpu�н�og_funƫ�Ƶ�ogAdress,��Ҫ��ԭog_fun��ogAdress֮����ֽڱ���ʧ��
;����ͷmov qword ptr [rsp+10H],rdx
;�д��Լ���˽��
mov eax,666
;��ԭ������(������[rcx]����ָ���ֽ���dword ptr����qword ptr)
mov dword ptr [rcx],eax
;��ת��ogAdress��ַ����ִ��
jmp ogAdress;
;��������
HookFuncCpu ENDP
;����������
end
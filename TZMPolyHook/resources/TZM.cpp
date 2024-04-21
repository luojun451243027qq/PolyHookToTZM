#include<Windows.h>
#include <atlstr.h>
#include<vector>
#include<time.h>
#include<iostream>

//ÿ�ζ�ȡ�ڴ������С
#define BLOCKMAXSIZE 409600

//ÿ�ν���ȡ���ڴ��������
BYTE* MemoryData;

SHORT Next[260];

//������ת�ֽڼ�
WORD HecToDec(char* Tzm, WORD* tzmArray)
{
	int len = 0;
	WORD tzmLength = strlen(Tzm) / 3 + 1;
	//��ʮ������������תΪʮ����
	for (int i = 0; i < strlen(Tzm); )
	{
		char num[2];
		num[0] = Tzm[i++];
		num[1] = Tzm[i++];
		i++;
		if (num[0] != '?' && num[1] != '?')
		{
			int sum = 0;
			WORD a[2];
			for (int i = 0; i < 2; i++)
			{
				if (num[i] >= '0' && num[i] <= '9')
				{
					a[i] = num[i] - '0';
				}
				else if (num[i] >= 'a' && num[i] <= 'z')
				{
					a[i] = num[i] - 87;
				}
				else if (num[i] >= 'A' && num[i] <= 'Z')
				{
					a[i] = num[i] - 55;
				}

			}
			sum = a[0] * 16 + a[1];
			tzmArray[len++] = sum;
		}
		else
		{
			tzmArray[len++] = 256;
		}
	}
	return tzmLength;
}

//ɨ��StartAddress��ʼ��size���ڴ�
void SearchMemoryBlock(HANDLE hProcess, WORD* Tzm, WORD tzmLength, ULONG64 StartAddress, ULONG size, std::vector<ULONG64>& ResultArray)
{
	if (!ReadProcessMemory(hProcess, (LPCVOID)StartAddress, MemoryData, size, NULL))
	{
		return;
	}

	for (int i = 0, j, k; i < size;)
	{
		j = i; k = 0;
		//�����루�ֽڼ�����ÿ���ֽڵķ�Χ��0-255��0-FF��֮�䣬256������ʾ�ʺţ���260��Ϊ�˷�ֹԽ��
		for (; k < tzmLength && j < size && (Tzm[k] == MemoryData[j] || Tzm[k] == 256); k++, j++);

		if (k == tzmLength)
		{
			ResultArray.push_back(StartAddress + i);
		}

		if ((i + tzmLength) >= size)
		{
			return;
		}

		int num = Next[MemoryData[i + tzmLength]];
		if (num == -1)
			//������������ʺţ��ʹ��ʺŴ���ʼƥ�䣬���û�о�i+=-1
			i += (tzmLength - Next[256]);
		else
			i += (tzmLength - num);
	}
}

//������������
int SearchMemory(HANDLE hProcess, char* Tzm, ULONG64 StartAddress, ULONG64 EndAddress, int InitSize, std::vector<ULONG64>& ResultArray)
{
	int i = 0;
	unsigned long BlockSize;
	MEMORY_BASIC_INFORMATION mbi;

	WORD tzmLength = strlen(Tzm) / 3 + 1;
	WORD* tzmArray = new WORD[tzmLength];

	HecToDec(Tzm, tzmArray);
	for (int i = 0; i < 260; i++)
		Next[i] = -1;
	for (int i = 0; i < tzmLength; i++)
		Next[tzmArray[i]] = i;
	//��ʼ���������
	ResultArray.clear();
	ResultArray.reserve(InitSize);

	while (VirtualQueryEx(hProcess, (LPCVOID)StartAddress, &mbi, sizeof(mbi)) != 0)
	{
		//ҳ�汣������
		if (//ֻ��
			mbi.Protect == PAGE_READONLY ||
			//�ɶ�д
			mbi.Protect == PAGE_READWRITE ||
			//��ִ��
			mbi.Protect == PAGE_EXECUTE ||
			//�ɶ���ִ��
			mbi.Protect == PAGE_EXECUTE_READ||
			//�ɶ���ִ�п�д
			mbi.Protect == PAGE_EXECUTE_READWRITE)
		{
			i = 0;
			BlockSize = mbi.RegionSize;
			//��������ڴ�
			while (BlockSize >= BLOCKMAXSIZE)
			{
				SearchMemoryBlock(hProcess, tzmArray, tzmLength, StartAddress + (BLOCKMAXSIZE * i), BLOCKMAXSIZE, ResultArray);
				BlockSize -= BLOCKMAXSIZE; i++;
			}
			SearchMemoryBlock(hProcess, tzmArray, tzmLength, StartAddress + (BLOCKMAXSIZE * i), BlockSize, ResultArray);

		}
		StartAddress += mbi.RegionSize;

		if (EndAddress != 0 && StartAddress > EndAddress)
		{
			delete[] tzmArray;
			return ResultArray.size();
		}
	}
	delete[] tzmArray;
	return ResultArray.size();
}

//Ѱ��������
SIZE_T FindMemoryTZM(CONST DWORD pid, ULONG64* buffer, CONST ULONG bufferCount, CONST PCHAR tzm, ULONG64 startAddr, ULONG64 endAddr) {
	MemoryData = new BYTE[BLOCKMAXSIZE];
	std::vector<ULONG64> ResultArray;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	SearchMemory(hProcess, tzm, startAddr, endAddr, bufferCount, ResultArray);

	for (std::vector<ULONG64>::iterator it = ResultArray.begin(); it != ResultArray.end(); it++)
	{
		memcpy(buffer++, &*it, sizeof LPVOID);
	}
	delete[] MemoryData;
	return ResultArray.size();
}
/*
VOID Test()
{
	MemoryData = new BYTE[BLOCKMAXSIZE];

	DWORD pid = 0;
	std::vector<ULONG64> ResultArray;

	std::cout << "���������ID��" << std::endl;//25156
	std::cin >> pid;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	int start = clock();
	SearchMemory(hProcess, (char*)"48 8D 0D 7F 13 ?? 00 E8 3A FF", 0x410000, 0x7FFFFFFFFFFF, 30, ResultArray);
	int end = clock();

	std::cout << "��ʱ��" << end - start << "����" << std::endl;
	std::cout << "������" << ResultArray.size() << "�����" << std::endl;

	for (std::vector<ULONG64>::iterator it = ResultArray.begin(); it != ResultArray.end(); it++)
	{
		printf("%x\n", *it);
	}

	return;
}
*/
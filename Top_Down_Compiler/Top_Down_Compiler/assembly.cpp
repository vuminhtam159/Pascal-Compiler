#include <iostream>
using namespace std;
char DataSegment[65536];
int *look;
int main()
{
	look = (int*)DataSegment;
	_asm{
		push eax         // Assembler header for all programs (Top)
		push ebx
		push ecx
		push edx
		push ebp
		push edi
		push esi
		push esp

		lea edi, DataSegment
		jmp kmain       // Assembler header for all programs (Bottom)


		kmain:

		mov [edi+0],8

		mov eax,1
		sub eax, 1		// zero base the index
		imul eax, 4		// index * size of data
		add eax, 12
		mov [edi + eax], 3
		mov eax,[edi+0]
		sub eax, 1		// zero base the index
		imul eax, 4		// index * size of data
		add eax, 12
		mov [edi + eax], 99
		pop esp    // Assembler footer for all programs (Top)
		pop esi
		pop edi
		pop ebp
		pop edx
		pop ecx
		pop ebx
		pop eax    // Assembler footer for all programs (Bottom)
	}
return 0;
}

#include "TopDownParser.h"

int main()
{
	//TopDownParser parser("programabc.txt");
	TopDownParser parser("test.txt");
	parser.parseCode();

//	Clex LEX;
//LEX.Init("programabc.txt");
//CToken TOKEN;
//while (LEX.GetToken(TOKEN))
//{
//	cout << TOKEN.tokenType << " " << TOKEN.tokenValue << endl;
//}

	system("pause");
	return 0;
}

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <vector>

using namespace std;


class CToken
{
public:
	string tokenValue;
	string tokenType;
};

class Clex
{
private:
	fstream inputFile1;
	int dfa[128][15];
	int size = 20;
	string rWord[20]; // begin, end, while, do, if, then, else, or, and, var
public:

	Clex()
	{

	}


	Clex(string file)
	{
		rWord[0] = "begin";
		rWord[1] = "end";
		rWord[2] = "while";
		rWord[3] = "do";
		rWord[4] = "if";
		rWord[5] = "then";
		rWord[6] = "else";
		rWord[7] = "or";
		rWord[8] = "and";
		rWord[9] = "var";
		rWord[10] = "boolean";
		rWord[11] = "procedure";
		rWord[12] = "function";
		rWord[13] = "integer";
		rWord[14] = "char";
		rWord[15] = "real";
		rWord[16] = "true";
		rWord[17] = "false";
		rWord[18] = "array";
		rWord[19] = "of";
	
		

		for (int i = 0; i < 128; i++) {
			for (int j = 0; j < 15; j++) {
				dfa[i][j] = 0;
			}
		}

		inputFile1.open(file);
		if (!inputFile1.is_open()) {
			cout << "Failed to open the file." << endl;
		}

		fstream inputFile("dfa1.txt");
		if (!inputFile.is_open()) {
			cout << "Failed to open the file." << endl;
		}
		else
		{
			for (int i = 0; i < 128; i++) {
				for (int j = 0; j < 15; j++) {
					int number;
					inputFile >> number;
					dfa[i][j] = number;
				}
			}

			inputFile.close();
		}

	}

	bool GetToken(CToken& theToken)
	{
		int currentState = 0;
		int prevState = 0;
		int num;

		theToken.tokenType = "";
		theToken.tokenValue = "";

		while (inputFile1.peek() != -1)
		{
			string firstDot = ".";

			char character = inputFile1.get();
			prevState = currentState;
			currentState = dfa[character][currentState];

			//if (firstDot == "." && character == '.')
			//{
			//	theToken.tokenValue = firstDot + firstDot;
			//	theToken.tokenType = "special";
			//	firstDot = "n";
			//	return true;
			//}

			//if (character == '.')
			//{

			//		character = inputFile1.get();
			//		if (character == '.' and theToken.tokenValue == "")
			//		{
			//			theToken.tokenValue = firstDot + firstDot;
			//			theToken.tokenType = "special";
			//			return true;
			//		}
			//		else
			//		{

			//			inputFile1.unget();
			//			inputFile1.unget();
			//			theToken.tokenType = "integer";
			//			return true;
			//		}
			//	//inputFile1.unget();

			//}

			if (prevState == 3 && character == '.')
			{
					theToken.tokenValue.pop_back();
					inputFile1.unget();
					inputFile1.unget();
					theToken.tokenType = "integer";
					return true;

			}



			if (currentState != 0 && currentState != 55 && currentState != 99)
			{
				theToken.tokenValue += character;	// append char to tokenValue
			}
			else if (currentState == 55 && character != 9 && character != 13 && character != 32 && character != 10)
			{
				inputFile1.unget();	// move back one position in your file
			}

			if (currentState == 55)
			{
				// set token type by looking at prevState
				if (prevState == 1 )
				{
					theToken.tokenType = "word";
					for (int i = 0; i < size; i++)
					{
						if (theToken.tokenValue == rWord[i])
						{
							theToken.tokenType = "rword";
						}
					}
				}
				else if (prevState == 2)
				{
					theToken.tokenType = "integer";
				}
				else if (prevState == 4 || prevState == 9)
				{
					theToken.tokenType = "real";
				}
				else if (prevState == 10)
				{
					theToken.tokenType = "special";
				}
				else if (prevState == 11 || prevState == 12 || prevState == 13 || prevState == 14)
				{
					theToken.tokenType = "special";
				}


				return true;
			}
			else if (currentState == 99)
			{
				cout << "Lexing error." << endl;
				return false;
			}

		}

		//if (currentState == 0)
		//{
		//	return false;
		//}

		if (currentState == 55 || currentState == 1 || currentState == 2 || currentState == 4 || currentState == 9 || currentState == 10 || currentState == 11 || currentState == 12 || currentState == 13	|| currentState == 14)
		{
			if (currentState == 1)
			{
				theToken.tokenType = "word";
				for (int i = 0; i < size; i++)
				{
					if (theToken.tokenValue == rWord[i])
					{
						theToken.tokenType = "rword";
					}
				}
			}
			else if (currentState == 2)
			{
				theToken.tokenType = "integer";
			}
			else if (currentState == 4 || currentState == 9)
			{
				theToken.tokenType = "real";
			}
			else if (currentState == 10 || currentState == 11 || currentState == 12 || currentState == 13 || currentState == 14)
			{
				theToken.tokenType = "special";
			}
		}
		else
		{
			if (currentState != 0)
			{
				cout << "Lexing error." << endl;
			}
			theToken.tokenType = "special";
			theToken.tokenValue = "$";
			return false;
		}



		inputFile1.close();
		return true;
	}

	void Init(string filename)
	{
		rWord[0] = "begin";
		rWord[1] = "end";
		rWord[2] = "while";
		rWord[3] = "do";
		rWord[4] = "if";
		rWord[5] = "then";
		rWord[6] = "else";
		rWord[7] = "or";
		rWord[8] = "and";
		rWord[9] = "var";
		rWord[10] = "boolean";
		rWord[11] = "procedure";
		rWord[12] = "function";
		rWord[13] = "integer";
		rWord[14] = "char";
		rWord[15] = "real";
		rWord[16] = "true";
		rWord[17] = "false";
		rWord[18] = "array";
		rWord[19] = "of";

		for (int i = 0; i < 128; i++) {
			for (int j = 0; j < 15; j++) {
				dfa[i][j] = 0;
			}
		}

		inputFile1.open(filename);
		if (!inputFile1.is_open()) {
			cout << "Failed to open the file." << endl;
		}

		fstream inputFile("dfa1.txt");
		if (!inputFile.is_open()) {
			cout << "Failed to open the file." << endl;
		}
		else
		{
			for (int i = 0; i < 128; i++) {
				for (int j = 0; j < 15; j++) {
					int number;
					inputFile >> number;
					dfa[i][j] = number;
				}
			}

			inputFile.close();
		}
	}


};


//int main()
//{
//	Clex LEX("programabc.txt");
//
//	CToken TOKEN;
//	while (LEX.GetToken(TOKEN))
//	{
//		cout << TOKEN.tokenType << " " << TOKEN.tokenValue << endl;
//	}
//	system("pause");
//	return 0;
//}



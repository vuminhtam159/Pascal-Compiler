#pragma once
#include "Lexer.h"
#include <map>
#include <stack>
#include "SymbolTable.cpp"

class TopDownParser
{
private:
	Clex lex;


	fstream inputFile;


	CToken DTOKEN;
	string nextToken;
	string currToken;

	bool getTokenCheck;

	SymbolTable s1;
	vector<string> varVector;
	stack<dataTypeStackObject> dataTypeStack;


	string topDataType = "";
	string bottomDataType = "";
	string dataType = "";
	int byteSize = 0;
	bool passByRef = false;
	string functionName = "";
	string procedureName = "";
	int offset;

	ofstream out;
	map<string, bool> registerMap;

	bool inParam = false;
	string paramAssembly = "";
	stack<string> paramInReverse;

	int orCount = 1;
	bool orUsed = false;
	string lastSign = "";
	int ifWhileCount = 1;
	stack<ifWhileObject> ifWhileStack;

	dataTypeStackObject arrayTemp;
	string arrayName = "";

	string location1 = "";
	string location2 = "";


public:

	TopDownParser(string fileName);

	bool parseCode();

	bool getToken();

	void program();

	void block();

	void statement();

	void statementPrime();

	void ifPrime();

	void expression();

	void sExpression();

	void sExpressionPrime();

	void sExpressionR();

	void T();

	void TPrime();

	void TR();

	void F();

	void FPrime();

	void FR();

	//char lookUp(string& p);

	void VPF();

	void VarList();

	void DataType();

	void mvar();

	void Param();

	void MParam();

	void GParam();

	void consumeParam();

	void printAssemblerHeader();

	void printAssemblerFooter();

	void assemblerAddSubMult(string which, dataTypeStackObject& left, dataTypeStackObject& right);

	void assemblerDivision(dataTypeStackObject& right, dataTypeStackObject& left);

	string findUnusedRegister();

	void resetFinishRegister(string name);

	void getRamLocation(dataTypeStackObject& object);

	void dumpProcedureHeader(string name);

	void dumpProcedureFooter(string name);

	void assemblerIf(dataTypeStackObject& right, dataTypeStackObject& left);

	void assemblerElse();

	void assemblerThen();

	void assemblerDo();

	void assemblerDoExpression();

	void assemblerAndExpression();

	void assemblerOrIf();

	void assemblerOrWhile();

	void assemblerElseIf();

	void assemblerEndIf();

	void assemblerWhile();

	void assemblerIfPushStack();

	void assemblerPopWhile();

	void twod();

	void assemblerArrayLeft(dataTypeStackObject temp);

	void assemblerArrayRight(dataTypeStackObject temp);

	void getArrayLocation(dataTypeStackObject temp, dataTypeStackObject temp2);
};
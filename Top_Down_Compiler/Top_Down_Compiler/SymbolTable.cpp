#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Scope;

struct ifWhileObject
{
	string ifOrWhile = "";
	int ifWhileCount = 0;
};

struct VarData
{
	string type = "";
	string returnType = "";
	string LFV = "";		//function, or variable
	int size = 0;
	int offset = 0;
	int paramOffset = 0;
	bool copyOrRef = false;
	Scope* NextScope = NULL;
	string LGL = "";	// local global literal

	string array = "";
	int startRow = 0;
	int endRow = 0;
	int startCol = 0;
	int endCol = 0;
	bool onedimension = true;
};

struct dataTypeStackObject
{
	string type = "";
	string returnType = "";
	string LFV = "";
	string ramOrRegister = "ram";
	string registerValue = "";
	string LGLP = "";	// local global literal param
	int offset = 0;
	string literal = "";
	bool copyOrRef = false;
	string address = "";

	bool array = false;
	int startRow = 0;
	int endRow = 0;
	int startCol = 0;
	int endCol = 0;
	bool onedimension = true;
};

struct Scope
{
	unordered_map<string, VarData> Table;
	int offset = 4;	// local variables
	int paramOffset = 8;	
	string ScopeName;
	Scope* PreviousScope = NULL;

	vector<string> paramOrder;

};

class SymbolTable
{
private:
	Scope* CurrentScope;

public:

	SymbolTable()
	{
		CurrentScope = new Scope;
		CurrentScope->offset = 0;
		CurrentScope->ScopeName = "";
		CurrentScope->PreviousScope = NULL;
	}

	bool checkPrevScopeExist()
	{
		if (CurrentScope->PreviousScope != NULL)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool AddVariable(string name, string type, int size, bool copyOrRef, string lglp)
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end())
		{
			return false;
		}
		CurrentScope->Table[name].type = type;
		CurrentScope->Table[name].LFV = "variable";
		CurrentScope->Table[name].size = size;
		CurrentScope->Table[name].offset = CurrentScope->offset;
		CurrentScope->Table[name].LGL = lglp;
		CurrentScope->Table[name].copyOrRef = copyOrRef;
		CurrentScope->Table[name].NextScope = NULL;
		CurrentScope->offset += size;
		return true;
	}

	bool AddArray(string name, int startRow, int endRow, int startCol, int endCol, bool onedimension, int size, string type, string LGLV)
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end())
		{
			return false;
		}
		CurrentScope->Table[name].array = true;
		CurrentScope->Table[name].startRow = startRow;
		CurrentScope->Table[name].endRow = endRow;
		CurrentScope->Table[name].startCol = startCol;
		CurrentScope->Table[name].endCol = endCol;
		CurrentScope->Table[name].offset = CurrentScope->offset;
		CurrentScope->Table[name].onedimension = onedimension;
		CurrentScope->Table[name].LFV = "array";
		CurrentScope->Table[name].type = type;
		CurrentScope->Table[name].LGL = LGLV;
		CurrentScope->offset += size;
		
		return true;

	}

	bool FindVariable(string name, int& offset)
	{
		Scope* TravScope = CurrentScope;
		while (TravScope != NULL)
		{
			if (TravScope->Table.find(name) == TravScope->Table.end())
			{
				TravScope = TravScope->PreviousScope;
			}
			else
			{
				offset = TravScope->Table[name].offset;
				return true;
			}
		}
		return false;
	}

	bool checkValidFuncReturn(string name)
	{
		return CurrentScope->ScopeName == name;
	}

	bool AddProcFunc(string name, string type)
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end())
		{
			return false;
		}
		CurrentScope->Table[name].LFV = type;
		CurrentScope->Table[name].size = 0;
		CurrentScope->Table[name].offset = 0;

		Scope* OldCurrent = CurrentScope;

		CurrentScope->Table[name].NextScope = new Scope;
		CurrentScope = CurrentScope->Table[name].NextScope;
		CurrentScope->ScopeName = name;
		CurrentScope->PreviousScope = OldCurrent;

		return true;

	}

	int getNumberOfParam(string name)
	{
		Scope* TravScope = CurrentScope->Table[name].NextScope;
		return TravScope->paramOrder.size();
	}

	bool AddParam(string name, string dataType, int size, bool copyOrRef)
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end())
		{
			return false;
		}

		CurrentScope->Table[name].type = dataType;
		CurrentScope->Table[name].size = size;
		CurrentScope->Table[name].copyOrRef = copyOrRef;
		CurrentScope->Table[name].NextScope = NULL;
		CurrentScope->Table[name].offset = CurrentScope->paramOffset;
		CurrentScope->Table[name].LGL = "param";
		CurrentScope->paramOffset += size;
		CurrentScope->Table[name].LFV = "variable";

		CurrentScope->paramOrder.push_back(name);

		return true;
	}

	bool checkIfParamVar(string name)
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end())
		{
			return false;
		}

		if (CurrentScope->Table[name].LGL == "param")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void getInfoFromTable(string name, int currentParam, bool& copyOrRef, string& dataType, int& offset, string& LFV, string& LGL)
	{
	

		Scope* TravScope = CurrentScope;
		while (TravScope != NULL)
		{
			if (TravScope->Table.find(name) == TravScope->Table.end())
			{
				TravScope = TravScope->PreviousScope;
			}
			else
			{
				copyOrRef = TravScope->Table[name].copyOrRef;
				dataType = TravScope->Table[name].type;
				offset = TravScope->Table[name].offset;
				LFV = TravScope->Table[name].LFV;
				LGL = TravScope->Table[name].LGL;
				break;
			}
		}

	}

	void getParamInfo(string name, int currentParam, bool& copyOrRef, string& dataType, int& offset, string& LFV, string& LGLV)
	{

		Scope* TravScope = CurrentScope;
		while (TravScope != NULL)
		{
			if (TravScope->Table.find(name) == TravScope->Table.end())
			{
				TravScope = TravScope->PreviousScope;
			}
			else
			{
				string paramName = TravScope->Table[name].NextScope->paramOrder[currentParam];
				copyOrRef = TravScope->Table[name].NextScope->Table[paramName].copyOrRef;
				dataType = TravScope->Table[name].NextScope->Table[paramName].type;
				offset = TravScope->Table[name].NextScope->Table[paramName].offset;
				LFV = TravScope->Table[name].NextScope->Table[paramName].LFV;
				LGLV = TravScope->Table[name].NextScope->Table[paramName].LGL;
				break;
			}
		}

	}

	void getParamVectorOrder(string name, int currentParam, bool& copyOrRef, string& dataType, int& offset, string& LFV, string& LGLV)
	{
		Scope* TravScope = CurrentScope;
		while (TravScope != NULL)
		{
			if (TravScope->Table.find(name) == TravScope->Table.end())
			{
				TravScope = TravScope->PreviousScope;
			}
			else
			{
				string paramName = TravScope->Table[name].NextScope->paramOrder[currentParam];
				dataType = TravScope->Table[name].NextScope->Table[paramName].type;
				offset = TravScope->Table[name].NextScope->Table[paramName].offset;
				LFV = TravScope->Table[name].NextScope->Table[paramName].LFV;
				LGLV = TravScope->Table[name].NextScope->Table[paramName].LGL;
				break;
			}
		}
	}

	void setFunctionReturnType(string name,string returnType)
	{
		if (CurrentScope->PreviousScope)
		{
			CurrentScope->PreviousScope->Table[name].returnType = returnType;
		}
	}

	string getFunctionReturnType(string name)
	{
		return	CurrentScope->Table[name].returnType;
	}

	string getUpperScopeFunctionReturnType(string name)
	{
		return	CurrentScope->PreviousScope->Table[name].returnType;
	}

	void BackOut()
	{
		if (CurrentScope->PreviousScope) CurrentScope = CurrentScope->PreviousScope;
	}

	int getCurrentScopeOffset()
	{
		return CurrentScope->offset - 4;
	}

	int getCurrentScopeParamNum()
	{
		return CurrentScope->paramOrder.size();
	}

};


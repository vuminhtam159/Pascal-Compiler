#include "TopDownParser.h"



TopDownParser::TopDownParser(string fileName)
{
	lex.Init(fileName);
}

bool TopDownParser::parseCode()
{
	getTokenCheck = getToken();



	out.open("assembly.cpp");
	if (!out.is_open())
	{
		cout << "Failed to open the assembly file." << endl;
	}

	program();


	
	if (DTOKEN.tokenValue.compare("$") != 0)
	{
		cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		system("pause");
		exit(99);
	}
	

	cout << "gooooooood code!!!" << endl;
	
	
	

	//system("pause");
	return false;
}

bool TopDownParser::getToken()
{
	if (!lex.GetToken(DTOKEN))
		return false;

	if (DTOKEN.tokenType.compare("special") == 0 || DTOKEN.tokenType.compare("rword") == 0)
	{
		nextToken = DTOKEN.tokenValue;
	}
	else
	{
		nextToken = DTOKEN.tokenType;
	}
	return true;
}

void TopDownParser::program()
{
	registerMap["eax"] = false;
	registerMap["ebx"] = false;
	registerMap["ecx"] = false;
	registerMap["edx"] = false;
	//registerMap["ebp"] = false;		// ebp-4 is the first local variable, ebp+8 is parameter #1 in a procedure
	//registerMap["edi"] = false;
	// registerMap["esi"] = false;		// esi is holding the address if things are passed by reference
	//registerMap["esp"] = false;

	printAssemblerHeader();	// (2)

	VPF();

	out << "		kmain:" << endl << endl;	// (1)
	block();

	if (nextToken == ".")
	{
		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		printAssemblerFooter();	// (3)
	}
	else
	{
		cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		system("pause");
		exit(99);
	}
}

void TopDownParser::block()
{
	if (nextToken == "begin")
	{
		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		statement();
		statementPrime();

		if (nextToken == "end")
		{
			getTokenCheck = getToken();
			//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		system("pause");
		exit(99);
	}
}


void TopDownParser::statementPrime()
{
	if (nextToken == ";")
	{
		getTokenCheck = getToken();
		statement();
		statementPrime();
	}
	else
	{
		//Epsilon
	}
}

void TopDownParser::statement()
{
	if (nextToken == "word")
	{
		procedureName = DTOKEN.tokenValue;	//to pass into the symbol table to get info out

		if (s1.FindVariable(DTOKEN.tokenValue, offset))	//(8)
		{
			dataTypeStackObject temp1;
			dataTypeStackObject temp2;

			s1.getInfoFromTable(DTOKEN.tokenValue, 0, temp1.copyOrRef, temp1.type, temp1.offset, temp1.LFV, temp1.LGLP);
			//dataTypeStack.push(temp.type);

			getTokenCheck = getToken();

			if (nextToken == ":=" && (temp1.LFV == "variable" || temp2.LFV == "function"))
			{
				if ((temp1.LFV == "function" && !s1.checkValidFuncReturn(procedureName)))
				{
					cout << "Invalid return type for function";
					system("pause");
					exit(99);
				}

				if(temp1.LFV == "function")
				{
					temp1.type = s1.getUpperScopeFunctionReturnType(procedureName);
				}

				
				dataTypeStack.push(temp1);
				getTokenCheck = getToken();
				expression();

				temp1 = dataTypeStack.top();		//(10)
				topDataType = temp1.type;
				dataTypeStack.pop();

				temp2 = dataTypeStack.top();	//left
				bottomDataType = temp2.type;
				dataTypeStack.pop();

				if (topDataType != bottomDataType)
				{
					cout << "DataType mismatch!!!!" << endl;
					cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
					system("pause");
					exit(99);
				}
				else
				{
			
						if (temp1.LGLP == "literal")
						{
							string register1;
							getRamLocation(temp1);
							getRamLocation(temp2);

							out << "		mov " << temp2.registerValue << "," << temp1.registerValue << endl << endl;
										
						}
						else
						{
							string register1 = temp1.registerValue;
							string temp1Result = temp1.registerValue;
							getRamLocation(temp1);
							getRamLocation(temp2);

							if (temp2.copyOrRef == false)
							{

								if (temp1.ramOrRegister == "ram")
								{
									register1 = findUnusedRegister();
									out << "		mov " + register1 + ", " + temp1.registerValue << endl;
								}

								out << "		mov " + temp2.registerValue + ", " + register1 << endl << endl;


							}
							else
							{

								out << "		mov esi, " + temp2.registerValue << endl;
								out << "		mov [esi]," + temp1Result << endl << endl;

							}
							resetFinishRegister(register1);
						}
					
				}
			}
			else if (nextToken == "(" && (temp1.LFV == "procedure" || temp1.LFV=="function"))
			{
				//dataTypeStack.push(temp.type);
				getTokenCheck = getToken();

				consumeParam();

				if (nextToken == ")")
				{
					getTokenCheck = getToken();
				}
				else
				{
					cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					system("pause");
					exit(99);
				}
			}
			else if (nextToken == "[" && temp1.LFV == "array")
			{
				dataTypeStack.push(temp1);
				getTokenCheck = getToken();

				expression();

				temp1 = dataTypeStack.top();		//(10)
				topDataType = temp1.type;
				dataTypeStack.pop();

				temp2 = dataTypeStack.top();	//left
				bottomDataType = temp2.type;
				dataTypeStack.pop();

				if (topDataType != bottomDataType)
				{
					cout << "DataType mismatch!!!!" << endl;
					cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
					system("pause");
					exit(99);
				}
				else
				{
					temp2.registerValue = temp1.registerValue;
			
					dataTypeStack.push(temp2);
				}

				if (nextToken == "]")
				{
					string register1 = temp1.registerValue;
					if (temp1.ramOrRegister == "ram")
					{
						register1 = findUnusedRegister();
						getRamLocation(temp1);

						out << "		mov " + register1 + "," + temp1.registerValue << endl;
						registerMap[register1] = true;
						temp2.registerValue = register1;
						location1 = register1;
					}
					assemblerArrayLeft(temp2);

					getTokenCheck = getToken();
					if (nextToken == ":=")
					{
						getTokenCheck = getToken();
						expression();

						temp1 = dataTypeStack.top();		//(10)
						topDataType = temp1.type;
						dataTypeStack.pop();

						temp2 = dataTypeStack.top();	//left
						bottomDataType = temp2.type;
						dataTypeStack.pop();

						if (topDataType != bottomDataType)
						{
							cout << "DataType mismatch!!!!" << endl;
							cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
							system("pause");
							exit(99);
						}
						else
						{
							location2 = "";

							if (temp2.LGLP == "global")
							{
								location2 = "edi";
							}
							else if (temp2.LGLP == "local")
							{
								location2 = "ebp";
							}
							else if (temp2.LGLP == "literal")
							{
								location2 = temp2.literal;
							}
							else if (temp2.LGLP == "param")
							{
								location2 = "ebp";
							}

							if (temp1.ramOrRegister == "ram")
							{
								getRamLocation(temp1);
							}
							

							out << "		mov [" + location2 + " + " + location1 + "], " + temp1.registerValue << endl;
							resetFinishRegister(location1);
						//	assemblerArrayLeft(temp2);

						}
					}
					else
					{			
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}

					
				}
				else
				{
					cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					system("pause");
					exit(99);
				}

				
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "Undeclare Identifier:" << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}

		
	}
	else if (nextToken == "begin")
	{
		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		statement();
		statementPrime();

		if (nextToken == "end")
		{
			getTokenCheck = getToken();
			//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else if (nextToken == "while")
	{
		assemblerWhile();

		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		expression();
		if (nextToken == "do")
		{
			assemblerDo();
			getTokenCheck = getToken();
			//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			statement();

			assemblerPopWhile();

		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else if (nextToken == "if")
	{
		assemblerIfPushStack();

		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		expression();
		if (nextToken == "then")
		{
			assemblerThen();
			dataTypeStackObject temp1 =  dataTypeStack.top();
			string topOfStack = temp1.type;
			dataTypeStack.pop();

			if (topOfStack != "boolean")
			{
				cout << "After \"If\", expression must be a boolean!!!!" << endl;
				system("pause");
				exit(99);
			}
			// pop the top off the stack, and check if it is a bool, if not then syntax error
			getTokenCheck = getToken();
			//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			statement();
			ifPrime();
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		// Epsilon
	}
}

void TopDownParser::ifPrime()
{
	if (nextToken == "else")
	{
		assemblerElse();	// (10)

		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		statement();
	}
	else
	{
		assemblerElseIf();
		// Epsilon
	}

	assemblerEndIf();

}

void TopDownParser::expression()
{
	sExpression();
}

void TopDownParser::sExpression()
{
	sExpressionR();
	sExpressionPrime();
}

void TopDownParser::sExpressionPrime()
{
	if (nextToken == "<" || nextToken == ">" || nextToken == "=")
	{
		lastSign = nextToken;	// (9) set last sign

		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		sExpressionR();
		dataTypeStackObject temp1;
		dataTypeStackObject temp2;

		temp1 = dataTypeStack.top();
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		if (topDataType == bottomDataType)
		{

			assemblerIf(temp1, temp2);
			temp2.type = "boolean";

			dataTypeStack.push(temp2);	//the new dataType should be the same as the old ones. I think lol...
		
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		sExpressionPrime();
	}
	else 
	{
		// Epsilon
	}
}

void TopDownParser::sExpressionR()
{
	T();
}



void TopDownParser::T()
{
	TR();
	TPrime();
}



void TopDownParser::TPrime()
{
	if (nextToken == "+")
	{	// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		TR();

		dataTypeStackObject temp1;
		dataTypeStackObject temp2;

		temp1 = dataTypeStack.top();	//right
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();	//left
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		if (topDataType == bottomDataType)
		{
			if (temp1.LGLP == "literal" && temp2.LGLP == "literal")
			{
				temp2.literal = std::to_string(stoi(temp2.literal) - stoi(temp1.literal));
			}
			else
			{
				assemblerAddSubMult("add", temp1, temp2);

				temp2.LGLP = "";
			}

	
			dataTypeStack.push(temp2);
			//the new dataType should be the same as the old ones. I think lol...
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		TPrime();
		
	}
	else if (nextToken == "-")
	{
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		TR();

		dataTypeStackObject temp1;
		dataTypeStackObject temp2;

		temp1 = dataTypeStack.top();
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		if (topDataType == bottomDataType)
		{
			if (temp1.LGLP == "literal" && temp2.LGLP == "literal")
			{
				temp2.literal = std::to_string(stoi(temp2.literal) - stoi(temp1.literal));
			}
			else
			{
				assemblerAddSubMult("sub", temp1, temp2);

				temp2.LGLP = "";
			}

			dataTypeStack.push(temp2);	//the new dataType should be the same as the old ones. I think lol...
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		TPrime();
	}
	else if (nextToken == "or")
	{
		// consumer token
		if (ifWhileStack.top().ifOrWhile == "w")
		{
			assemblerOrWhile();
		}
		else
		{
			assemblerOrIf();
		}
		

		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		TR();

		dataTypeStackObject temp1;
		dataTypeStackObject temp2;


		temp1 = dataTypeStack.top();
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		
		if (topDataType == bottomDataType)
		{
			dataTypeStack.push(temp2);	//the new dataType should be the same as the old ones. I think lol...
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		TPrime();
	}
	else
	{
	
	}
}

void TopDownParser::TR()
{
	F();
}

void TopDownParser::F()
{
	FR();
	FPrime();
}


void TopDownParser::FR()
{
	if (nextToken == "(")
	{
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			
		expression();
	
			if (nextToken == ")")
			{
				// consumer token
				getTokenCheck = getToken();	// moving to next token
				//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		
		
	}
	else if (nextToken == "real")
	{
		dataTypeStackObject temp1;
		temp1.LGLP = "literal";
		temp1.literal = DTOKEN.tokenValue;
		temp1.type = nextToken;

		dataTypeStack.push(temp1); //(11)
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
	}
	else if (nextToken == "integer")
	{
		dataTypeStackObject temp1;
		temp1.LGLP = "literal";
		temp1.literal = DTOKEN.tokenValue;
		temp1.type = nextToken;

		dataTypeStack.push(temp1); //(11)
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
	}
	else if (nextToken == "+")
	{
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		TR();
	}
	else if (nextToken == "-")
	{
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		TR();
	}
	else if (nextToken == "word")
	{
		if (s1.FindVariable(DTOKEN.tokenValue, offset))	//(8)
		{
			dataTypeStackObject temp;
			s1.getInfoFromTable(DTOKEN.tokenValue, 0, temp.copyOrRef, temp.type, temp.offset, temp.LFV, temp.LGLP);	//offset is currently always 4, needs fixing
			//getTokenCheck = getToken();

			if (temp.LFV == "variable")
			{
				dataTypeStack.push(temp);
			}
			else if (temp.LFV == "function")
			{
				procedureName = DTOKEN.tokenValue;
				getTokenCheck = getToken();
				if (nextToken == "(")
				{

					getTokenCheck = getToken();
					consumeParam();

					dataTypeStackObject temp1;
					temp1.type = s1.getFunctionReturnType(procedureName);

					dataTypeStack.push(temp1);	// push the return type of function onto the stack
					if (nextToken == ")")
					{
						//getTokenCheck = getToken();
					}
					else
					{
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}

				}
				else
				{
					cout << "Potential Errors" << endl;
					cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					system("pause");
					exit(99);
				}
			}
			else if (temp.LFV == "procedure")
			{
				if (nextToken == "(")
				{

					getTokenCheck = getToken();
					consumeParam();

					if (nextToken == ")")
					{
						//getTokenCheck = getToken();
					}
					else
					{
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}

				}
				else
				{
					cout << "Potential errors:" << endl;
					cout <<  DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					cout << "Procedure cannot be an expression" << endl;
					system("pause");
					exit(99);
				}
			}
		}
		else
		{
			cout << "Undeclare Identifier:" << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}

		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
	}
	else if (nextToken == "true")
	{
		dataTypeStackObject temp1;
		temp1.type = "boolean";
		dataTypeStack.push(temp1); //(11)
		getTokenCheck = getToken();
	}
	else if (nextToken == "false")
	{
		dataTypeStackObject temp1;
		temp1.type = "boolean";
		dataTypeStack.push(temp1); //(11)
		getTokenCheck = getToken();
	}
	else
	{
		cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		system("pause");
		exit(99);
	}
}

void TopDownParser::FPrime()
{
	if (nextToken == "*")
	{	// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		FR();

		dataTypeStackObject temp1;
		dataTypeStackObject temp2;

		temp1 = dataTypeStack.top();
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		if (topDataType == bottomDataType)
		{
			if (temp1.LGLP == "literal" && temp2.LGLP == "literal")
			{
				temp2.literal = std::to_string(stoi(temp2.literal) * stoi(temp1.literal));
			}
			else
			{
				assemblerAddSubMult("mult", temp1, temp2);
				temp2.LGLP = "";
			}



			dataTypeStack.push(temp2);	//the new dataType should be the same as the old ones. I think lol...
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		FPrime();

	}
	else if (nextToken == "/")
	{
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		FR();

		dataTypeStackObject temp1;
		dataTypeStackObject temp2;

		temp1 = dataTypeStack.top();
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		if (topDataType == bottomDataType)
		{
			if (temp1.LGLP == "literal" && temp2.LGLP == "literal")
			{
				temp2.literal = std::to_string(stoi(temp2.literal) / stoi(temp1.literal));
			}
			else
			{
				assemblerDivision( temp1, temp2);
				temp2.LGLP = "";
			}

			dataTypeStack.push(temp2);	//the new dataType should be the same as the old ones. I think lol...
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		FPrime();
	}
	else if (nextToken == "and")
	{
	/*	if (ifWhileStack.top().ifOrWhile == "while")
		{
			assemblerDoExpression();
		
		}
		else
		{*/
			assemblerAndExpression();
		/*}*/
		
		// consumer token
		getTokenCheck = getToken();	// moving to next token
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		FR();

		dataTypeStackObject temp1;
		dataTypeStackObject temp2;

		temp1 = dataTypeStack.top();
		topDataType = temp1.type;	//(9)
		dataTypeStack.pop();

		temp2 = dataTypeStack.top();
		bottomDataType = temp2.type;
		dataTypeStack.pop();

		if (topDataType == bottomDataType)
		{
	
			dataTypeStack.push(temp2);	//the new dataType should be the same as the old ones. I think lol...
		}
		else
		{
			cout << "DataType mismatch!!!!" << endl;
			cout << "Cannot do " << bottomDataType << " with " << topDataType << endl;
			system("pause");
			exit(99);
		}

		FPrime();
	}
	else
	{

	}
}

void TopDownParser::VPF()
{
	if (nextToken == "var")
	{
		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		if (nextToken == "word")
		{
			arrayName = DTOKEN.tokenValue;
			varVector.push_back(DTOKEN.tokenValue);	// (1)

			getTokenCheck = getToken();
			VarList();
			if (nextToken == ":")
			{
				getTokenCheck = getToken();
				if (nextToken == "array")
				{
					getTokenCheck = getToken();
					if (nextToken == "[")
					{
						getTokenCheck = getToken();

						if (nextToken == "integer")
						{

							arrayTemp.startRow = stoi(DTOKEN.tokenValue);
							getTokenCheck = getToken();
							if (nextToken == "..")
							{
								getTokenCheck = getToken();
								if (nextToken == "integer")
								{
									arrayTemp.endRow = stoi(DTOKEN.tokenValue);
									getTokenCheck = getToken();
									twod();
									if (nextToken == "]")
									{
										getTokenCheck = getToken();
										if (nextToken == "of")
										{
											getTokenCheck = getToken();
											DataType();
											arrayTemp.type = dataType;
											
											// add array into symbol table
											if (s1.checkPrevScopeExist())
											{
												s1.AddArray(arrayName, arrayTemp.startRow, arrayTemp.endRow, arrayTemp.startCol, arrayTemp.endCol, arrayTemp.onedimension, byteSize, arrayTemp.type, "local");
											}
											else
											{
												s1.AddArray(arrayName, arrayTemp.startRow, arrayTemp.endRow, arrayTemp.startCol, arrayTemp.endCol, arrayTemp.onedimension, byteSize, arrayTemp.type, "global");
											}


											if (nextToken == ";")
											{
												getTokenCheck = getToken();
												mvar();
												VPF();
											}
											else
											{
												cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
												system("pause");
												exit(99);
											}
										}
										else
										{
											cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
											system("pause");
											exit(99);
										}
									}
									else
									{
										cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
										system("pause");
										exit(99);
									}
								}
								else
								{
									cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
									system("pause");
									exit(99);
								}
							}
							else
							{
								cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
								system("pause");
								exit(99);
							}
						}
						else
						{
							cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
							system("pause");
							exit(99);
						}
					}
					else
					{
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}
				}
				else
				{
					DataType();
					for (int i = 0; i < varVector.size(); i++)	// (2)
					{
						if (s1.checkPrevScopeExist())
						{
							s1.AddVariable(varVector[i], dataType, byteSize, false, "local");
						}
						else
						{
							s1.AddVariable(varVector[i], dataType, byteSize, false, "global");
						}
					}
					varVector.clear();

					if (nextToken == ";")
					{
						getTokenCheck = getToken();
						mvar();
						VPF();
					}
					else
					{
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}
				}
				
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else if (nextToken == "procedure")
	{
		
		getTokenCheck = getToken();
		procedureName = DTOKEN.tokenValue;
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		if (nextToken == "word")
		{
			s1.AddProcFunc(DTOKEN.tokenValue, "procedure");	//(3)
		//	s1.AddProcFunc(DTOKEN.tokenValue, "");	//(3)
			getTokenCheck = getToken();
			//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			if (nextToken == "(")
			{
				getTokenCheck = getToken();

				Param();
				if (nextToken == ")")
				{
					getTokenCheck = getToken();
					if (nextToken == ";")
					{
						getTokenCheck = getToken();
						VPF();
						dumpProcedureHeader(procedureName);	//(3)
						block();
						if (nextToken == ";")
						{
							dumpProcedureFooter(procedureName); //(5)
							s1.BackOut();	//(6)

							getTokenCheck = getToken();
							VPF();
						}
						else
						{
							cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
							system("pause");
							exit(99);
						}
					}
					else
					{
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}
				}
				else
				{
					cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					system("pause");
					exit(99);
				}
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else if (nextToken == "function")
	{
		getTokenCheck = getToken();
		//cout << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;

		if (nextToken == "word")
		{
			s1.AddProcFunc(DTOKEN.tokenValue, "function");	//(3)
			functionName = DTOKEN.tokenValue;
			getTokenCheck = getToken();
			if (nextToken == "(")
			{
				getTokenCheck = getToken();
				Param();
				if (nextToken == ")")
				{
					getTokenCheck = getToken();
					if (nextToken == ":")
					{
						getTokenCheck = getToken();
						DataType();

						s1.setFunctionReturnType(functionName, dataType);	//(7)

						if (nextToken == ";")
						{
							getTokenCheck = getToken();
							VPF();

							dumpProcedureHeader(functionName);	//(3)

							block();
							if (nextToken == ";")
							{

								dumpProcedureFooter(functionName);	//(5)
								s1.BackOut();	//(6)

								getTokenCheck = getToken();
								VPF();
							}
							else
							{
								cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
								system("pause");
								exit(99);
							}
						}
						else
						{
							cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
							system("pause");
							exit(99);
						}
					}
					else
					{
						cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
						system("pause");
						exit(99);
					}
				}
				else
				{
					cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					system("pause");
					exit(99);
				}
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		//epsilon
	}
}

void TopDownParser::VarList()
{
	if (nextToken == ",")
	{
		getTokenCheck = getToken();
		if (nextToken == "word")
		{
			varVector.push_back(DTOKEN.tokenValue);	// (1)

			getTokenCheck = getToken();
			VarList();
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}

	}
	else
	{
		//epsilon
	}
}

void TopDownParser::DataType()
{
	if (DTOKEN.tokenValue == "boolean")
	{
		dataType = DTOKEN.tokenValue;
		byteSize = 2;
		getTokenCheck = getToken();
	}
	else if (DTOKEN.tokenValue == "integer")
	{
		dataType = DTOKEN.tokenValue;
		byteSize = 4;
		getTokenCheck = getToken();
	}
	else if (DTOKEN.tokenValue == "char")
	{
		dataType = DTOKEN.tokenValue;
		byteSize = 1;
		getTokenCheck = getToken();
	}
	else if (DTOKEN.tokenValue == "real")
	{
		dataType = DTOKEN.tokenValue;
		byteSize = 4;
		getTokenCheck = getToken();
	}
	else
	{
		cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		system("pause");
		exit(99);
	}

}

void TopDownParser::mvar()
{
	if (nextToken == "word")
	{
		varVector.push_back(DTOKEN.tokenValue);	// (1)

		getTokenCheck = getToken();
		VarList();
		if (nextToken == ":")
		{
			getTokenCheck = getToken();

			DataType();
			for (int i = 0; i < varVector.size(); i++)	// (2)
			{
				if (s1.checkPrevScopeExist())
				{
					s1.AddVariable(varVector[i], dataType, byteSize, "", "local");
				}
				else
				{
					s1.AddVariable(varVector[i], dataType, byteSize, "", "global");
				}
			}
			varVector.clear();

			if (nextToken == ";")
			{
				getTokenCheck = getToken();
				mvar();
				VPF();
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		//epsilon
	}
}

void TopDownParser::Param()
{
	if (nextToken == "var")
	{
		passByRef = true;	// (4)

		getTokenCheck = getToken();
		if (nextToken == "word")
		{
			varVector.push_back(DTOKEN.tokenValue);	// (1)

			getTokenCheck = getToken();
			VarList();
			if (nextToken == ":")
			{
				getTokenCheck = getToken();

				DataType();
				
				for (int i = 0; i < varVector.size(); i++)	// (5)
				{
					s1.AddParam(varVector[i], dataType, byteSize, passByRef);
				}
				varVector.clear();
				passByRef = false;

				MParam();
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else if (nextToken == "word")
	{
		varVector.push_back(DTOKEN.tokenValue);	// (1)

		getTokenCheck = getToken();
		VarList();
		if (nextToken == ":")
		{
			getTokenCheck = getToken();

			DataType();
			for (int i = 0; i < varVector.size(); i++)	// (5)
			{
				s1.AddParam(varVector[i], dataType, byteSize, passByRef);
			}
			varVector.clear();
			passByRef = false;

			MParam();
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		//epsilon
	}
}

void TopDownParser::MParam()
{
	if (nextToken == ";")
	{
		getTokenCheck = getToken();
		GParam();
	}
	else
	{
		//epsilon
	}
}

void TopDownParser::GParam()
{
	if (nextToken == "var")
	{
		passByRef = true;	//(4)

		getTokenCheck = getToken();
		if (nextToken == "word")
		{
			varVector.push_back(DTOKEN.tokenValue);	// (1)

			getTokenCheck = getToken();
			VarList();
			if (nextToken == ":")
			{
				getTokenCheck = getToken();

				DataType();
				for (int i = 0; i < varVector.size(); i++)	// (5)
				{
					s1.AddParam(varVector[i], dataType, byteSize, passByRef);
				}
				varVector.clear();
				passByRef = false;

				MParam();
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else if (nextToken == "word")
	{
		varVector.push_back(DTOKEN.tokenValue);	// (1)

		getTokenCheck = getToken();
		VarList();
		if (nextToken == ":")
		{
			getTokenCheck = getToken();

			DataType();
			for (int i = 0; i < varVector.size(); i++)	// (5)
			{
				s1.AddParam(varVector[i], dataType, byteSize, passByRef);
			}
			varVector.clear();
			passByRef = false;

			MParam();
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
		system("pause");
		exit(99);
	}
}

void TopDownParser::consumeParam()
{
	int totalParam = s1.getNumberOfParam(procedureName);
	int currentParam = 0;

	dataTypeStackObject param;
	inParam = true;
	string register1 = "";

	while (currentParam < totalParam)
	{
		s1.getParamInfo(procedureName, currentParam, param.copyOrRef, param.type, param.offset,param.LFV,param.LGLP);

		if (param.copyOrRef)
		{
			if (nextToken == "word")
			{
				bool copyOrRefVar = false;
				string paramDataTypeVar = "";
				int paramOffsetVar = 0;
				string LFVVar = "";
				string LGLV = "";

				s1.getInfoFromTable(DTOKEN.tokenValue, currentParam, copyOrRefVar, paramDataTypeVar, paramOffsetVar,LFVVar,LGLV);
				if (param.type != paramDataTypeVar)
				{
					cout << "Incompatible types!!!!" << param.type << " " << dataType << endl;
					system("pause");
					exit(99);
				}

				//
				register1 = findUnusedRegister();
				paramAssembly = paramAssembly + "		mov " + register1 + "," + to_string(paramOffsetVar) + "\n";
				paramAssembly = paramAssembly + "		add " + register1 + ",edi \n";
				paramAssembly = paramAssembly + "		push " + register1 + "\n\n";
				//registerMap[register1] = true;

				paramInReverse.push(paramAssembly);
				paramAssembly = "";


				getTokenCheck = getToken();
			}
			else
			{
				cout << "Must be a variable. " << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			expression();

			s1.getParamVectorOrder(procedureName, currentParam, param.copyOrRef, param.type, param.offset, param.LFV, param.LGLP);

			dataTypeStackObject temp1;
			

			temp1 = dataTypeStack.top();
		

			if (param.type != temp1.type)
			{
				cout << "Incompatible types!!!!" << param.type << " " << dataType << endl;
				system("pause");
				exit(99);
			}
			else
			{
				if (temp1.ramOrRegister == "ram")
				{
					register1 = findUnusedRegister();
					getRamLocation(temp1);
					paramAssembly = paramAssembly + "		mov " + register1 + "," + temp1.registerValue + "\n";
					paramAssembly = paramAssembly + "		push " + register1 + "\n\n";
				}
				else
				{
					paramAssembly = paramAssembly + "		push " + temp1.registerValue + "\n\n";
				}
				//registerMap[register1] = true;
				paramInReverse.push(paramAssembly);
				paramAssembly = "";

				dataTypeStack.pop();
			}
		}

		// create assembler code and put on stack

		currentParam++;

		if (currentParam < totalParam)
		{
			if (nextToken == ",")
			{
				getTokenCheck = getToken();
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}

		registerMap[register1] = false;
	}

	for (int i = 0; i < totalParam; i++)
	{
		paramAssembly = paramInReverse.top();
		out << paramAssembly;
		paramInReverse.pop();
	}
	inParam = false;

	out << "		call " + procedureName << endl << endl;
}

void TopDownParser::printAssemblerHeader()
{
	out << "#include <iostream>" << endl;
	out << "using namespace std;" << endl;
	out << "char DataSegment[65536];" << endl;
	out << "int *look;" << endl;
	out << "int main()" << endl;
	out << "{" << endl;

	out << "	look = (int*)DataSegment;" << endl;

	out << "	_asm{" << endl;
	out << "		push eax         // Assembler header for all programs (Top)" << endl;
	out << "		push ebx" << endl;
	out << "		push ecx" << endl;
	out << "		push edx" << endl;
	out << "		push ebp" << endl;
	out << "		push edi" << endl;
	out << "		push esi" << endl;
	out << "		push esp" << endl << endl;
	out << "		lea edi, DataSegment" << endl;
	out << "		jmp kmain       // Assembler header for all programs (Bottom)" << endl << endl << endl;

}

void TopDownParser::printAssemblerFooter()
{
	out << "		pop esp    // Assembler footer for all programs (Top)" << endl;
	out << "		pop esi" << endl;
	out << "		pop edi" << endl;
	out << "		pop ebp" << endl;
	out << "		pop edx" << endl;
	out << "		pop ecx" << endl;
	out << "		pop ebx" << endl;
	out << "		pop eax    // Assembler footer for all programs (Bottom)" << endl;
	out << "	}" << endl;
	out << "return 0;" << endl;
	out << "}" << endl;

	out.close();
}

void TopDownParser::assemblerAddSubMult(string which, dataTypeStackObject& right, dataTypeStackObject& left)
{	
	// computer cannot do ram and ram


	getRamLocation(left);
	getRamLocation(right);

	string register1 = left.registerValue;
	string register2 = right.registerValue;
	if (inParam == false)
	{
		if (left.ramOrRegister != "register")
		{
			register1 = findUnusedRegister();
			registerMap[register1] = true;
			if (left.copyOrRef == true)
			{
				out << "		mov esi, " + left.registerValue << endl;
				out << "		mov " + register1 + ", [esi]" << endl;

			}
			else
				out << "		mov " << register1 << "," << left.registerValue << endl;

		}

		if (right.copyOrRef == true)
		{
			out << "		mov esi, " + right.registerValue << endl;
			register2 = "[esi]";
		}




		if (which == "add")		// add register, register/literal/ram
		{
			out << "		add " << register1 << "," << register2 << endl << endl;
			left.registerValue = register1;
			right.registerValue = register2;
		}
		else if (which == "sub")
		{
			out << "		sub " << register1 << "," << register2 << endl << endl;
			left.registerValue = register1;
			right.registerValue = register2;
		}
		else if (which == "mult")
		{
			out << "		imul " << register1 << "," << register2 << endl << endl;
			left.registerValue = register1;
			right.registerValue = register2;
		}

		left.ramOrRegister = "register";

		resetFinishRegister(register2);
		//set right reg to false

	}
	else
	{
		if (left.ramOrRegister != "register")
		{
			register1 = findUnusedRegister();
			registerMap[register1] = true;
			if (left.copyOrRef == true)
			{
				paramAssembly = paramAssembly + "		mov esi, " + left.registerValue + "\n";
				paramAssembly = paramAssembly + "		mov " + register1 + ", [esi]" + "\n";

			}
			else
				paramAssembly = paramAssembly + "		mov " + register1 + "," + left.registerValue + "\n";

		}

		if (right.copyOrRef == true)
		{
			paramAssembly = paramAssembly + "		mov esi, " + right.registerValue + "\n";
			register2 = "[esi]";
		}




		if (which == "add")		// add register, register/literal/ram
		{
			paramAssembly = paramAssembly + "		add " + register1 + "," + register2 + "\n";
			left.registerValue = register1;
			right.registerValue = register2;
		}
		else if (which == "sub")
		{
			paramAssembly = paramAssembly + "		sub " + register1 + "," + register2 + "\n";
			left.registerValue = register1;
			right.registerValue = register2;
		}
		else if (which == "mult")
		{
			paramAssembly = paramAssembly + "		imul " + register1 + "," + register2 + "\n";
			left.registerValue = register1;
			right.registerValue = register2;
		}

		left.ramOrRegister = "register";

		resetFinishRegister(register2);
		//set right reg to false
	}
}

void TopDownParser::assemblerDivision(dataTypeStackObject& right, dataTypeStackObject& left)
{
	if (inParam == false)
	{
		// quotient eax, remainder edx
		out << "		push eax " << endl;
		out << "		push edx " << endl;


		getRamLocation(left);
		getRamLocation(right);

		string register1 = left.registerValue;
		string register2 = right.registerValue;


		register1 = findUnusedRegister();
		if (left.copyOrRef == true)
		{
			out << "		mov esi, " + left.registerValue << endl;
			out << "		mov " + register1 + ", [esi]" << endl;

		}
		else
		{
			out << "		mov " << register1 << "," << left.registerValue << endl;
			registerMap[register1] = true;
		}


		if (right.copyOrRef == true)
		{
			out << "		mov esi, " + right.registerValue << endl;
			register2 = "[esi]";
		}



		string register1Value = left.registerValue;
		string register2Value = right.registerValue;

		register2 = findUnusedRegister();
		out << "		mov " << register2 << "," << register2Value << endl;
		registerMap[register2] = true;
		out << "		cdq" << endl;
		out << "		idiv " << register2 << endl;
		out << "		mov " << register2 << ",eax" << endl;
		out << "		pop edx " << endl;
		out << "		pop eax " << endl << endl;

		resetFinishRegister("eax");

		registerMap[register2] = false;
		left.registerValue = register2;
	}
	else
	{
		paramAssembly = paramAssembly + "		push eax " + "\n";
		paramAssembly = paramAssembly + "		push edx " + "\n";


		getRamLocation(left);
		getRamLocation(right);

		string register1 = left.registerValue;
		string register2 = right.registerValue;


		register1 = findUnusedRegister();
		if (left.copyOrRef == true)
		{
			paramAssembly = paramAssembly + "		mov esi, " + left.registerValue + "\n";
			paramAssembly = paramAssembly + "		mov " + register1 + ", [esi]" + "\n";

		}
		else
		{
			paramAssembly = paramAssembly + "		mov " + register1 + "," + left.registerValue + "\n";
			registerMap[register1] = true;
		}


		if (right.copyOrRef == true)
		{
			paramAssembly = paramAssembly + "		mov esi, " + right.registerValue + "\n";
			register2 = "[esi]";
		}



		string register1Value = left.registerValue;
		string register2Value = right.registerValue;

		register2 = findUnusedRegister();
		paramAssembly = paramAssembly + "		mov " + register2 + "," + register2Value + "\n";
		registerMap[register2] = true;
		paramAssembly = paramAssembly + "		cdq" + "\n";
		paramAssembly = paramAssembly + "		idiv " + register2 + "\n";
		paramAssembly = paramAssembly + "		mov " + register2 + ",eax" + "\n";
		paramAssembly = paramAssembly + "		pop edx " + "\n";
		paramAssembly = paramAssembly + "		pop eax " + "\n\n";

		registerMap[register2] = false;
		left.registerValue = register2;
	}
	
}

string TopDownParser::findUnusedRegister()
{

	for (map<string, bool>::iterator x = registerMap.begin(); x != registerMap.end(); x++)
	{
		if (x->second == false)
			return x->first;
	}

}

void TopDownParser::resetFinishRegister(string name)
{
	for (map<string, bool>::iterator x = registerMap.begin(); x != registerMap.end(); x++)
	{
		if (x->first == name)
		{
			x->second = false;
			return;
		}
			
	}
}

void TopDownParser::getRamLocation(dataTypeStackObject& object)
{
	if (object.LGLP == "global")
	{
		object.registerValue = "[edi+" + std::to_string(object.offset) + "]";
	}
	else if (object.LGLP == "local")
	{
		object.registerValue = "[ebp-" + std::to_string(object.offset) + "]";
	}
	else if (object.LGLP == "literal")
	{
		object.registerValue = object.literal;
	}
	else if (object.LGLP == "param")
	{
		object.registerValue = "[ebp+" + std::to_string(object.offset) + "]";
	}

}

void TopDownParser::dumpProcedureHeader(string name)
{
	int localOffset = s1.getCurrentScopeOffset();


	out << "		" << name << ":		// top of the procedure" << endl;
	out << "		push ebp" << "		// Assembler procedure heading for all procedures (Top)" << endl;
	out << "		mov ebp,esp" <<	endl;
	out << "		sub esp," << localOffset << "	// Make room for local variables on stack" << endl << endl;
	out << "		push eax" << endl;
	out << "		push ebx" << endl;
	out << "		push ecx" << endl;
	out << "		push edx" << endl;
	out << "		push ebp" << endl;
	out << "		push edi" << endl;
	out << "		push esi" << "		// Assembler procedure heading for all procedures (Bottom)" << endl << endl;
}

void TopDownParser::dumpProcedureFooter(string name)
{
	int localOffset = s1.getCurrentScopeOffset();

	out << "		pop esi" << "		// Assembler procedure footer for all procedures (Top)" << endl;
	out << "		pop edi" << endl;
	out << "		pop ebp" << endl;
	out << "		pop edx" << endl;
	out << "		pop ecx" << endl;
	out << "		pop ebx" << endl;
	out << "		pop eax" << endl << endl;

	

	out << "		add esp," << localOffset << "		// remove local variables" << endl << endl;
	out << "		pop ebp" << "		// Restore old ebp" <<endl;

	localOffset = s1.getCurrentScopeParamNum() * 4;
	out << "		ret " << localOffset << "		// Assembler procedure footer for all procedures (Bottom)" << endl;
	out << "	// ret takes parameters off of the stack and returns to the calling procedure" << endl << endl;
}


void TopDownParser::assemblerIf(dataTypeStackObject& right, dataTypeStackObject& left)
{
	string register1 = "";
	string register2 = "";

	getRamLocation(left);
	getRamLocation(right);

	dataTypeStackObject temp;

	if (left.ramOrRegister == "ram")
	{
		register1 = findUnusedRegister();

		out << "		mov " + register1 + "," + left.registerValue << endl;
		out << "		cmp " + register1 + "," + right.registerValue << endl;
		registerMap[register1] = true;
	}
	else
	{
		out << "		cmp " + left.registerValue + "," + right.registerValue << endl;
	}

	resetFinishRegister(register1);


	temp.type = "boolean";

}

void TopDownParser::assemblerElse()
{
	out << "		JMP endif" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	out << "elseif" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;
}

void TopDownParser::assemblerThen()
{
	// then is also treated like an or
	string temp = lastSign;


	assemblerOrIf();


		
		
	out << "		JMP elseif" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	
	



	out << "IF" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;

}

void TopDownParser::assemblerDo()
{
	// then is also treated like an or
	string temp = lastSign;

	assemblerDoExpression();

	out << "		JMP endwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;




	out << "inwhile" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;

}

void TopDownParser::assemblerDoExpression()
{
	if (lastSign == "=")
	{
		out << "		JE inwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}
	else if (lastSign == "<")
	{
		out << "		JL inwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}
	else if (lastSign == ">")
	{
		out << "		JG inwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}


	if (orUsed)
	{
		out << "OR" + to_string(orCount) + ":" << endl;

		orCount++;
		orUsed = false;
	}
}

void TopDownParser::assemblerAndExpression()
{
	// if we find an and
	if (lastSign == "=")
	{
		out << "		JNE OR" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}
	else if (lastSign == "<")
	{
		out << "		JGE OR" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}
	else if (lastSign == ">")
	{
		out << "		JLE OR" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}

	orUsed = true;
	lastSign = "";

}
void TopDownParser::assemblerOrIf()
{
	if (lastSign == "=")
	{
		out << "		JE IF" + to_string(ifWhileStack.top().ifWhileCount) << endl;	// JE inside IF1
	}
	else if (lastSign == "<")
	{
		out << "		JL IF" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}
	else if (lastSign == ">")
	{
		out << "		JG IF" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}

	if (orUsed)
	{
		out << "OR" + to_string(orCount) + ":" << endl;

		orCount++;
		orUsed = false;
	}
}

void TopDownParser::assemblerOrWhile()
{
	if (lastSign == "=")
	{
		out << "		JE inwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;	// JE inside IF1
	}
	else if (lastSign == "<")
	{
		out << "		JL inwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}
	else if (lastSign == ">")
	{
		out << "		JG inwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	}

	if (orUsed)
	{
		out << "OR" + to_string(orCount) + ":" << endl;

		orCount++;
		orUsed = false;
	}
}

void TopDownParser::assemblerElseIf()
{
	out << "elseif" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;
}

void TopDownParser::assemblerEndIf()
{
	out << "endif" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;
	ifWhileStack.pop();
}

void TopDownParser::assemblerWhile()
{
	ifWhileObject whileObject;
	whileObject.ifOrWhile = "w";
	whileObject.ifWhileCount = ifWhileCount;
	ifWhileCount++;

	ifWhileStack.push(whileObject);

	out << "topwhile" + to_string(whileObject.ifWhileCount) + ":" << endl;
}

void TopDownParser::assemblerIfPushStack()
{
	ifWhileObject ifObject;
	ifObject.ifOrWhile = "i";
	ifObject.ifWhileCount = ifWhileCount;
	ifWhileCount++;

	ifWhileStack.push(ifObject);

	out << "topif" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;
}

void TopDownParser::assemblerPopWhile()
{
	out << "		JMP topwhile" + to_string(ifWhileStack.top().ifWhileCount) << endl;
	out << "endwhile" + to_string(ifWhileStack.top().ifWhileCount) + ":" << endl;
	ifWhileStack.pop();
}

void TopDownParser::twod()
{
	if (nextToken == ",")
	{
		arrayTemp.onedimension = false;
		getTokenCheck = getToken();
		
		if (nextToken == "integer")
		{
			getTokenCheck = getToken();
			if (nextToken == "..")
			{
				getTokenCheck = getToken();
				if (nextToken == "integer")
				{
					getTokenCheck = getToken();
				}
				else
				{
					cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
					system("pause");
					exit(99);
				}
			}
			else
			{
				cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
				system("pause");
				exit(99);
			}
		}
		else
		{
			cout << "baaaaaaaaaaaaaaaaaaaad code!!!!" << DTOKEN.tokenType << " " << DTOKEN.tokenValue << endl;
			system("pause");
			exit(99);
		}
	}
	else
	{
		// epsilon
	}
}

void TopDownParser::assemblerArrayLeft(dataTypeStackObject temp)
{
	int size = 0;
	if (temp.type == "integer")
	{
		size = 4;
	}
	else if (temp.type == "real")
	{
		size = 4;
	}
	else if (temp.type == "bool")
	{
		size = 1;
	}
	else if (temp.type == "char")
	{
		size = 1;
	}



	out << "		sub " + temp.registerValue + ", 1		// zero base the index" << endl;
	out << "		imul " + temp.registerValue + ", " + to_string(size) + "		// index * size of data" << endl;
	out << "		add " + temp.registerValue + ", " + to_string(temp.offset) << endl;
}

void TopDownParser::assemblerArrayRight(dataTypeStackObject temp)
{
	int size = 0;
	if (temp.type == "integer")
	{
		size == 4;
	}
	else if (temp.type == "real")
	{
		size = 4;
	}
	else if (temp.type == "bool")
	{
		size = 1;
	}
	else if (temp.type == "char")
	{
		size = 1;
	}

	out << "		sub " + temp.registerValue + ", 1		// zero base the index" << endl;
	out << "		imul " + temp.registerValue + ", " + to_string(size) + "		// index * size of data" << endl;
	out << "		add " + temp.registerValue + ", " + to_string(temp.offset);
}

void TopDownParser::getArrayLocation(dataTypeStackObject temp, dataTypeStackObject temp2)
{
	string location = "";

	if (temp.LGLP == "global")
	{
		location = "edi";
	}
	else if (temp.LGLP == "local")
	{
		location = "ebp";
	}
	else if (temp.LGLP == "literal")
	{
		location = temp.literal;
	}
	else if (temp.LGLP == "param")
	{
		location = "ebp";
	}

	out << "		mov [" + location + " + " + temp2.registerValue + "], ";

}

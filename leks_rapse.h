#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <regex>
using namespace std;

#pragma region read_file
string read_file(const string& fileName) {
	ifstream f(fileName);
	f.seekg(0, ios::end);
	size_t size = f.tellg();
	string s(size, ' ');
	f.seekg(0);
	f.read(&s[0], size);
	return s;
}
#pragma endregion

#pragma region keyWordsAndSymbols

vector<string> listOfSymbols{ "=", "+","-","<",">","<=",">=", "!=", "==", "*", "/" };
vector<string> listOfIfSymbols{ "<=",">=", "!=", "==", "<",">" };
vector<string> listOfKeyWords{ "var","if", "else", "print", "return", "while", "continue", "break" };
vector<string> listOfDelimeters{ ",", ";" };

//����� ���������� � �������� ��������� �� �����������
vector<string> list_of_vars = {}; // ���������� ( 'A' 'B' 'C' 'D' )
vector<string> list_of_numbers = {}; // ��������� ( '0' '16' '35' '12' )

// ����� ��������� ��� ���������� � �������� ���������
vector<string> list_of_vars_and_nums = {}; // ���������� � ��������� ( 'A' 'B' 'C' 'D' '0' '16' '35' '12' ) 

map<string, string> operators = {
	{ "+", "ADD" },
	{ "-", "SUB" },
	{ "*", "MUL" },
	{ "/", "DIV" }
};

map<string, string> comparison_operators = {
	{ "==", "EQ" },
	{ "!=", "NEQ" },
	{ "<=", "LE" },
	{ ">=", "GE" },
	{ "<", "LS" },
	{ ">", "GR" }
};



//map<string, string> vars_And_Vals = {}; // ?

#pragma endregion

#pragma region parse_string
vector<string> parse_string(string buff) {
	vector<string> wordsAndSymbols;

	vector<string> allVariables = {};
	vector<string> allNumbers = {};

	const regex txt_regexNotW("^\\W");     //������ ������� ��� ���� � ����.  ������ ( ')' '?'  )
	const regex txt_regexNotS("^\\S");     //��� ��� ��������
	const regex txt_regexW("\\w");         //������ ����� � �����
	const regex txt_regexNotD("\\D");      //������� ����
	const regex txt_regexD("\\d");         //only �����
	const regex txt_regexS("\\s");         //������ ���������� �������

	for (int i1 = 0; i1 < buff.size();) {
		string tempString = "";
		string symbol = "";
		tempString += buff[i1];

		if (regex_match(tempString, txt_regexNotW) && regex_match(tempString, txt_regexNotS)) { //������ ������� ��� ��������
			if (tempString == ">" || tempString == "<" || tempString == "=" || tempString == "!") {
				symbol += buff[i1 + 1];
				if (symbol == "=") {
					i1 += 1;
					tempString += buff[i1];
				}
			}
			if (tempString == "/") {
				symbol += buff[i1 + 1];
				if (symbol == "/") {
					while (symbol != "\n") { //����� � ����� ��� ��������
						i1 += 1;
						tempString += buff[i1];
						symbol = "";
						symbol += buff[i1 + 1];
					}
					i1 += 1;
					continue;
				}
			}
			symbol = "";
			i1 += 1;
			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexW) && regex_match(tempString, txt_regexNotD)) { //������ �����
			symbol += buff[i1 + 1];
			while (regex_match(symbol, txt_regexW) && regex_match(symbol, txt_regexNotS) && !regex_match(symbol, txt_regexNotW)) { //����� � ����� ��� ��������
				i1 += 1;
				tempString += buff[i1];
				symbol = "";
				symbol += buff[i1 + 1];
			}
			symbol = "";
			i1 += 1;

			//��������� ���� �� (���???) ����� �������� ���� � ���� ������� ��������� � ������ ����������
			for (int i = 0; i < listOfKeyWords.size(); ) {
				if (tempString == listOfKeyWords[i]) break;
				else i += 1;
				if (i == listOfKeyWords.size()) allVariables.push_back(tempString);
			}
			/////////////////////////////////////////////////////////////////////////////

			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexD)) { //�������� ���������
			symbol += buff[i1 + 1];
			while (regex_match(symbol, txt_regexD)) {
				i1 += 1;
				tempString += buff[i1];
				symbol = "";
				symbol += buff[i1 + 1];
			}
			symbol = "";
			i1 += 1;

			//������� �������� ��������� � �������� �� � ���� ������
			allNumbers.push_back(tempString);
			////////////////////////////////////////////////////////

			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexS)) { //���������� �������
			i1 += 1;
		}
	}

	//������� ��������� ����� ���������� � �����
	if (allVariables.size() != 0) {
		for (int i = 0; i < allVariables.size(); i++) {
			if (list_of_vars.size() == 0) list_of_vars.push_back(allVariables[i]);
			for (int j = 0; j < list_of_vars.size(); ) {
				if (allVariables[i] == list_of_vars[j]) break;
				j += 1;
				if (j == list_of_vars.size())list_of_vars.push_back(allVariables[i]);
			}
		}
	}

	if (allNumbers.size() != 0) {
		for (int i = 0; i < allNumbers.size(); i++) {
			if (list_of_numbers.size() == 0) list_of_numbers.push_back(allNumbers[i]);
			for (int j = 0; j < list_of_numbers.size(); ) {
				if (allNumbers[i] == list_of_numbers[j]) break;
				j += 1;
				if (j == list_of_numbers.size())list_of_numbers.push_back(allNumbers[i]);
			}
		}
	}

	return wordsAndSymbols;
}

#pragma endregion


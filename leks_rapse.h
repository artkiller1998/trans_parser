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

//здесь переменные и числовые константы по отдельности
vector<string> list_of_vars = {}; // ПЕРЕМЕННЫЕ ( 'A' 'B' 'C' 'D' )
vector<string> list_of_numbers = {}; // КОНСТАНТЫ ( '0' '16' '35' '12' )

// здесь находятся все переменные и числовые константы
vector<string> list_of_vars_and_nums = {}; // ПЕРЕМЕННЫЕ и КОНСТАНТЫ ( 'A' 'B' 'C' 'D' '0' '16' '35' '12' ) 

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

	const regex txt_regexNotW("^\\W");     //только символы без цифр и букв.  ПРИМЕР ( ')' '?'  )
	const regex txt_regexNotS("^\\S");     //все без пробелов
	const regex txt_regexW("\\w");         //только цифры и буквы
	const regex txt_regexNotD("\\D");      //никаких цифр
	const regex txt_regexD("\\d");         //only цифры
	const regex txt_regexS("\\s");         //только пробельные символы

	for (int i1 = 0; i1 < buff.size();) {
		string tempString = "";
		string symbol = "";
		tempString += buff[i1];

		if (regex_match(tempString, txt_regexNotW) && regex_match(tempString, txt_regexNotS)) { //только символы без пробелов
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
					while (symbol != "\n") { //буквы и цифры без пробелов
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

		if (regex_match(tempString, txt_regexW) && regex_match(tempString, txt_regexNotD)) { //только буквы
			symbol += buff[i1 + 1];
			while (regex_match(symbol, txt_regexW) && regex_match(symbol, txt_regexNotS) && !regex_match(symbol, txt_regexNotW)) { //буквы и цифры без пробелов
				i1 += 1;
				tempString += buff[i1];
				symbol = "";
				symbol += buff[i1 + 1];
			}
			symbol = "";
			i1 += 1;

			//проверяем есть ли (ЧТО???) среди ключевых слов и если повезет добавляем в массив переменных
			for (int i = 0; i < listOfKeyWords.size(); ) {
				if (tempString == listOfKeyWords[i]) break;
				else i += 1;
				if (i == listOfKeyWords.size()) allVariables.push_back(tempString);
			}
			/////////////////////////////////////////////////////////////////////////////

			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexD)) { //числовые константы
			symbol += buff[i1 + 1];
			while (regex_match(symbol, txt_regexD)) {
				i1 += 1;
				tempString += buff[i1];
				symbol = "";
				symbol += buff[i1 + 1];
			}
			symbol = "";
			i1 += 1;

			//находим числовые константы и собираем их в один массив
			allNumbers.push_back(tempString);
			////////////////////////////////////////////////////////

			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexS)) { //пробельные символы
			i1 += 1;
		}
	}

	//убираем дубликаты среди переменных и чисел
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


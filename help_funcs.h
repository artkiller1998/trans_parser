#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <regex>
using namespace std;


#pragma region HelpFULL_Functions
//напишем функцию которая проверяет наличие элемента в массиве
bool be_in(string str, vector<string>& vec) {
	for (string temp : vec) {
		if (str == temp) return true;
	}
	return false;
}
bool be_in(string str, map<string, string>& myMap) {
	for (auto temp : myMap) {
		if (str == temp.first) return true;
	}
	return false;
}

bool be_in(string str, map<int, string>& myMap) {
	for (auto temp : myMap) {
		if (str == temp.second) return true;
	}
	return false;
}

//напишем функцию которая возвращает номер позиции элемента в массиве
int findIn(string str, vector<string>& vec) {
	int pos = 0;
	for (string temp : vec) {
		if (str == temp) return pos;
		pos++;
	}
	return -1;
}

int findIn(string str, map<string, string>& myMap) {
	int pos = 0;
	for (auto temp : myMap) {
		if (str == temp.first) return pos;
		pos += 1;
	}
	return -1;
}

int findIn(string str, map<int, string>& myMap) {
	for (auto temp : myMap) {
		if (str == temp.second) return temp.first;
	}
	return -1;
}


#pragma endregion
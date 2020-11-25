#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include "leks_rapse.h"
using namespace std;

vector<string> listOfAriphmeticSymbols{ "=", "+","-", "*", "/" };
vector<string> not_init_vars;
vector<string> init_vars;


bool valid_expression(vector<string> exp);

string check_syntax(vector<string>expression)
{

	if (expression[0] == "var")  // обработка объявлений и инициализации переменных var
	{
		not_init_vars.push_back(expression[1]);

		if (find(list_of_vars.begin(), list_of_vars.end(), expression[1]) != list_of_vars.end())
		{
			if (expression[2] == ",")
			{
				int count = 3;
				while (count < expression.size())
				{
					if (find(list_of_vars.begin(), list_of_vars.end(), expression[count]) != list_of_vars.end())
					{
						not_init_vars.push_back(expression[count]);
						count++;
					}
					else if (expression[count] == ",")
						count++;
					else return "Invalid expression";
				}
			}
			else if (expression[2] == "=")
			{
				init_vars.push_back(expression[1]);
				vector<string>temp;
				temp.assign(++++expression.begin(), expression.end());
				if (!valid_expression(temp))
					return "Invalid expression";
				
			}
		}

	}

	else if (find(not_init_vars.begin(), not_init_vars.end(), expression[0]) != not_init_vars.end()) // обработка операции присвоения
	{
		if (expression[1] == "=")
		{
			vector<string>temp;
			temp.assign(++++expression.begin(), expression.end());
			if (!valid_expression(temp))
				return "Invalid expression with =";
			init_vars.push_back(expression[0]);
			
		}
		else return"Invalid expression";
	}

	else if (expression[0] == "while") // обработка циклов while 
	{

		if (expression[1] == "(" && expression.size() > 2)
		{
			int count_of_bracket = 1;
			int count = 2;
			vector<string> temp;
			while (count < expression.size() && count_of_bracket > 0)
			{
				if (expression[count] == "(")
				{
					count_of_bracket++;
					count++;
					continue;
				}
				else if (expression[count] == ")")
				{
					count_of_bracket--;
					count++;
					continue;
				}
				if (find(listOfIfSymbols.begin(), listOfIfSymbols.end(), expression[count]) != listOfIfSymbols.end())
				{
					if (!valid_expression(temp))
						return "Invalid while expression";
					temp.clear();
					count++;
					continue;
				}
				temp.push_back(expression[count]);
				count++;
			}
			if (!valid_expression(temp))
				return "Invalid while expression";
			if (count < expression.size())
			{
				vector<string>temp;
				temp.assign(expression.begin()+count, expression.end());
				if (!valid_expression(temp))
					return "Invalid expression";
			}
		}
		else
			return "Invalid while expression";

	}
	else if (expression[0] == "if")
	{
		if (expression[1] == "(" && expression.size() > 2)
		{
			int count_of_bracket = 1;
			int count = 2;
			vector<string> temp;
			while (count < expression.size() && count_of_bracket > 0)
			{
				if (expression[count] == "(")
				{
					count_of_bracket++;
					count++;
					continue;
				}
				else if (expression[count] == ")")
				{
					count_of_bracket--;
					count++;
					continue;
				}
				if (find(listOfIfSymbols.begin(), listOfIfSymbols.end(), expression[count]) != listOfIfSymbols.end())
				{
					if (!valid_expression(temp))
						return "Invalid if expression";
					temp.clear();
					count++;
					continue;
				}
				temp.push_back(expression[count]);
				count++;
			}
			if (!valid_expression(temp))
				return "Invalid if expression";
			if (count < expression.size())
			{
				vector<string>temp;
				temp.assign(expression.begin() + count, find(expression.begin(), expression.end(), "else"));
				if (check_syntax(temp)!="OK")
					return "Invalid if expression";
				temp.assign(++find(expression.begin(), expression.end(), "else"), expression.end());
				if (check_syntax(temp) != "OK")
					return "Invalid else expression";
			}
		}
		else
			return "Invalid if expression";
	}
	else if (expression[0] == "print" || expression[0] == "return") //обработка print и return выражений
	{
		vector<string>temp;
		temp.assign(++expression.begin(), expression.end());
		if (!valid_expression(temp))
			return "Invalid expression";
	}

	else return "Invalid expression";
	return "OK";



}

bool valid_expression(vector<string> exp) // проверяет правильность выражений 
{
	if (*exp.rbegin() == ";")
		exp.pop_back();
	if (exp.size() % 2 == 0)return false;

	if (find(init_vars.begin(), init_vars.end(), exp[0]) != init_vars.end()
		|| find(list_of_numbers.begin(), list_of_numbers.end(), exp[0]) != list_of_numbers.end())
	{
		int count = 1;

		while (count < exp.size())
		{

			if (find(listOfAriphmeticSymbols.begin(), listOfAriphmeticSymbols.end(), exp.at(count)) != listOfAriphmeticSymbols.end()
				&& (find(init_vars.begin(), init_vars.end(), exp.at(count + 1)) != init_vars.end()
					|| find(list_of_numbers.begin(), list_of_numbers.end(), exp.at(count + 1)) != list_of_numbers.end()))
				count += 2;
			else
			{
				cout << "Invalid expresssion:	" << exp[count] << exp[count + 1] << endl;
				return false;
			}
		}
	}
	else
	{
		cout << "Not init vars	" << exp[0] << endl;
		return false;
	}
	return true;
}
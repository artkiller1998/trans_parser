#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <regex>
#include "leks_rapse.h"
#include "help_funcs.h"
#include "syntax.h"

using namespace std;



class IfParse;
class WhileParse;
class AssignmentParse;
class PrintParse;
class ReturnParse;


//pair< string, int > command; //команда в прорамме представляет собой мэп. сама команда в виде строки и число (оно используется только для переходов, то есть оно только для if)

#pragma region expressions

#pragma region UnionForAllClasses
class AllExpClasses {
public:
	IfParse *ifparse;
	WhileParse *whileparse;
	AssignmentParse *assignmentparse;
	PrintParse *printparse;
	ReturnParse *returnparse;

	AllExpClasses() {
		ifparse = NULL;
		whileparse = NULL;
		assignmentparse = NULL;
	}
};

#pragma endregion

#pragma region BASIC_CLASS
class Basic {
public:
	vector<AllExpClasses *> childs;

	vector< pair< string, int > > commands;

};

#pragma endregion

#pragma region ASSIGNMENT_PARSE
class AssignmentParse {
public:

	int count_tokens = 0;
	vector <string> left;
	vector <string> right;

	vector< pair< string, int > > commands;


	AssignmentParse(vector<string> &expression, vector<AllExpClasses *> &childsParent, vector< pair< string, int > > &parentVectorOfCommands) {  //второй параметр для множественного присваивания типа "var A = 1, B = 14, C = A * B;" . они становятся отдельными присваиваниями

		string leftOrRight = "left";
		
		int i = 0;
		if (expression[i] == "var") {
			i++;
		}
		while (i < expression.size()){
			if (std::find(list_of_vars_and_nums.begin(), list_of_vars_and_nums.end(), expression[i]) != list_of_vars_and_nums.end()) {
				if (leftOrRight == "left")
					left.push_back(expression[i]);
				if (leftOrRight == "right")
					right.push_back(expression[i]);
				i++;
			}
			else if (expression[i] == "=") {
				leftOrRight = "right";
				i++;
			}
			else if (expression[i] == ";") {
				if (right.size() != 0) {
					AllExpClasses *newunion = new  AllExpClasses();
					newunion->assignmentparse = this;
					childsParent.push_back(newunion);
					//childsParent[childsParent.size() - 1]->assignmentparse->print_node();
				}
				i++;
				break;
			}

			else if (expression[i] == ",") {
				if (right.size() != 0) {
					AllExpClasses *newunion = new  AllExpClasses();
					newunion->assignmentparse = this;
					childsParent.push_back(newunion);
					childsParent[childsParent.size() - 1]->assignmentparse->print_node();
				}
				i++;
				vector<string> temp;
				temp.assign(&expression[i], &expression[expression.size() - 1]);
				AssignmentParse* expr = new AssignmentParse(temp, childsParent, parentVectorOfCommands);
				

				i += expr->count_tokens;
				break;

			}
			else {   //это если арифметические знаки
				right.push_back(expression[i]);
				i++;
			}
				
			

		} 

		this->count_tokens = i;
		print_node();
		translate(parentVectorOfCommands);
	}

	void translate(vector< pair< string, int > > &parentVectorOfCommands) {
		if (right.size() != 0) {
			Tree *root = new Tree(right);
			//здесь строим дерево из правой части
			build_tree(root);

			fout << "___Assignment___" << endl;
			//здесь обходим это дерево и транслируем
			pass_tree(root, parentVectorOfCommands);
			
			//здесь транслируем запись результата в левую часть
			fout	<< setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
					<< "POP "
					<< setfill('0') << setw(4) << uppercase << hex << findIn(left[0], list_of_vars_and_nums) << ends << endl;  //вооюще по логике слева всегда одна переменная поэтому можно было сделать left не вектором а строкой но оставим как есть
			GlobalCount += 1;

			oss.str("");
			oss << "POP " << setfill('0') << setw(4) << uppercase << hex << findIn(left[0], list_of_vars_and_nums) + 1 << ends;
			parentVectorOfCommands.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов

			fout << endl << endl;
		}

	}


	void print_node() {

		cout << "\n------------------ASSIGNMENT" << endl;
		cout << "\nleft" << endl;
		
		for (auto el : left)
			cout << "'" << el << "' ";
		if (right.size() != 0) {
			cout << "\nright" << endl;
			for (auto el : right)
				cout << "'" << el << "' ";
		}
		
		cout << "\ncount_tokens" << endl;
		cout << "'" << count_tokens << "' " << endl;

		cout << "\n----------------------------------------\n\n\n" << endl;

	}
};

#pragma endregion

#pragma region PRINT_PARSE
class PrintParse {
public:

	int count_tokens = 0;

	vector <string> right;

	vector< pair< string, int > > commands;


	PrintParse(vector<string> &expression, vector<AllExpClasses *> &childsParent, vector< pair< string, int > > &parentVectorOfCommands) {  //второй параметр для множественного присваивания типа "var A = 1, B = 14, C = A * B;" . они становятся отдельными присваиваниями

		int i = 0;
		if (expression[i] == "print") {
			i++;
		}
		while (i < expression.size()) {
			if (std::find(list_of_vars_and_nums.begin(), list_of_vars_and_nums.end(), expression[i]) != list_of_vars_and_nums.end()) {
				right.push_back(expression[i]);
				i++;
			}
			else if (expression[i] == ";") {
				//if (right.size() != 0) {
				//	AllExpClasses *newunion = new  AllExpClasses();
				//	newunion->printparse = this;
				//	childsParent.push_back(newunion);
				//	//childsParent[childsParent.size() - 1]->assignmentparse->print_node();
				//}
				i++;
				break;
			}
			else {   //это если арифметические знаки
				right.push_back(expression[i]);
				i++;
			}

		}

		this->count_tokens = i;
		print_node();
		translate(parentVectorOfCommands);
	}

	void translate(vector< pair< string, int > > &parentVectorOfCommands) {


		if (right.size() != 0) {
			Tree *root = new Tree(right);
			//здесь строим дерево из правой части
			build_tree(root);

			fout << "___Print___" << endl;
			//здесь обходим это дерево и транслируем
			pass_tree(root, parentVectorOfCommands);

			//здесь транслируем запись результата в левую часть
			fout << setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
				<< "PRINT" << endl;  //вооюще по логике слева всегда одна переменная поэтому можно было сделать left не вектором а строкой но оставим как есть
			GlobalCount += 1;

			oss.str("");
			oss << "PRINT";
			parentVectorOfCommands.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов

			fout << endl << endl;
		}

	}


	void print_node() {

		cout << "\n------------------PRINT" << endl;
		
		if (right.size() != 0) {
			cout << "\nright" << endl;
			for (auto el : right)
				cout << "'" << el << "' ";
		}

		cout << "\ncount_tokens" << endl;
		cout << "'" << count_tokens << "' " << endl;

		cout << "\n----------------------------------------\n\n\n" << endl;

	}
};

#pragma endregion


#pragma region RETURN_PARSE
class ReturnParse {
public:

	int count_tokens = 0;
	vector <string> right;

	vector< pair< string, int > > commands;


	ReturnParse(vector<string> &expression, vector<AllExpClasses *> &childsParent, vector< pair< string, int > > &parentVectorOfCommands) {  //второй параметр для множественного присваивания типа "var A = 1, B = 14, C = A * B;" . они становятся отдельными присваиваниями

		int i = 0;
		if (expression[i] == "return") {
			i++;
		}
		while (i < expression.size()) {
			if (std::find(list_of_vars_and_nums.begin(), list_of_vars_and_nums.end(), expression[i]) != list_of_vars_and_nums.end()) {
				right.push_back(expression[i]);
				i++;
			}
			else if (expression[i] == ";") {
				//if (right.size() != 0) {
				//	AllExpClasses *newunion = new  AllExpClasses();
				//	newunion->printparse = this;
				//	childsParent.push_back(newunion);
				//	//childsParent[childsParent.size() - 1]->assignmentparse->print_node();
				//}
				i++;
				break;
			}
			else {   //это если арифметические знаки
				right.push_back(expression[i]);
				i++;
			}

		}

		this->count_tokens = i;
		print_node();
		translate(parentVectorOfCommands);
	}

	void translate(vector< pair< string, int > > &parentVectorOfCommands) {


		if (right.size() != 0) {
			Tree *root = new Tree(right);
			//здесь строим дерево из правой части
			build_tree(root);

			fout << "___Return___" << endl;
			//здесь обходим это дерево и транслируем
			pass_tree(root, parentVectorOfCommands);

			//здесь транслируем запись результата в левую часть
			fout << setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
				<< "EXIT" << endl;  
			GlobalCount += 1;

			oss.str("");
			oss << "EXIT";
			parentVectorOfCommands.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов

			fout << endl << endl;
		}

	}


	void print_node() {

		cout << "\n------------------Return" << endl;
		
		if (right.size() != 0) {
			cout << "\nright" << endl;
			for (auto el : right)
				cout << "'" << el << "' ";
		}

		cout << "\ncount_tokens" << endl;
		cout << "'" << count_tokens << "' " << endl;

		cout << "\n----------------------------------------\n\n\n" << endl;

	}
};

#pragma endregion


//надо создать дополнительную переменную куда можно записывать временные данные. это для таких сравнений где и слева и справа больше одного символа
#pragma region CONDITION_PARSE
class ConditionParse {
public:

	int count_tokens = 0;
	vector <string> left;
	vector <string> right;
	string sign;

	vector< pair< string, int > > commands;


	ConditionParse(vector<string> &expression) {  //второй параметр для множественного присваивания типа "var A = 1, B = 14, C = A * B;" . они становятся отдельными присваиваниями

		string leftOrRight = "left";

		int i = 0;

		while (i < expression.size()) {
			if (	expression[i] == "<" || expression[i] == "<=" || expression[i] == "!=" || 
					expression[i] == ">" || expression[i] == ">=" || expression[i] == "=="		) 
			{

				sign = expression[i];
				leftOrRight = "right";
				i++;
				continue;
			}
			if (leftOrRight == "left") {
				left.push_back(expression[i]);
				i++;
				continue;
			}
				
			if (leftOrRight == "right") {
				right.push_back(expression[i]);
				i++;
				continue;
			}

		}


		this->count_tokens = i;
		print_node();
		//translate будет вызываться из ифа или вайла
		//translate();
	}


	void print_node() {

		cout << "\n------------------CONDITION" << endl;
		cout << "\nleft" << endl;

		for (auto el : left)
			cout << "'" << el << "' ";
		if (right.size() != 0) {
			cout << "\nright" << endl;
			for (auto el : right)
				cout << "'" << el << "' ";
		}
		cout << "\nsign" << endl;
		cout << "'" << sign << "' " << endl;

		cout << "\ncount_tokens" << endl;
		cout << "'" << count_tokens << "' " << endl;

		cout << "\n----------------------------------------\n\n\n" << endl;

	}

	void translate(vector< pair< string, int > > &parentcommands, string flag_WHILE_OR_IF, int count_of_false_commands = 0) {
		if (right.size() != 0) {

			vector< pair< string, int > > temp;

			Tree *rootleft = new Tree(left);
			//здесь строим дерево из левой части
			build_tree(rootleft);

			fout << "___Condition___" << endl;
			fout << "___left___" << endl;
			//здесь обходим это дерево и транслируем
			if (flag_WHILE_OR_IF == "while") {
				oss.str("");
				oss << "W ";  //здесь метка начала цикла (или)
				pass_tree(rootleft, temp, false);

			}
			else {
				oss.str("");
				oss << "IFN ";  //здесь метка начала цикла (или)
				pass_tree(rootleft, temp);


			}
			
			fout << "___temp___" << endl;
			//здесь заносим результат вычисления левой части во временную переменную
			fout << setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
				<< "POP "
				<< setfill('0') << setw(4) << uppercase << hex << 0 << ends << endl;
			GlobalCount += 1;

			oss.str("");
			oss << "POP " << setfill('0') << setw(4) << uppercase << hex << 0 << ends;
			temp.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов

			fout << "___right___" << endl;

			Tree *rootright = new Tree(right);
			//здесь строим дерево из правой части
			build_tree(rootleft);

			//здесь обходим это дерево и транслируем
			pass_tree(rootright, temp);

			fout << setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
				<< "PUSH "
				<< setfill('0') << setw(4) << uppercase << hex << 0 << ends << endl;  
			GlobalCount += 1;

			oss.str("");
			oss << "PUSH " << setfill('0') << setw(4) << uppercase << hex << 0 << ends;
			temp.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов
			
			//это операция IFN
			fout << setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
				<< "IFN "
				<< setfill('0') << setw(4) << uppercase << hex << 0 << ends << endl;  
			GlobalCount += 1;

			//трансляция самого оператора сравнения (GR LS GE EQ NEQ LE)
			oss.str("");
			oss << comparison_operators[sign];
			temp.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов

			if (flag_WHILE_OR_IF == "while") {
				oss.str("");
				oss << "SW IFN ";  //здесь метка начала цикла (или)  //SW = START WHILE
				temp.push_back(make_pair(oss.str(), parentcommands.size() + 2));


				//это jump для зацикливания
				oss.str("");
				oss << "JMP ";
				parentcommands.push_back(make_pair(oss.str(), 0 - (parentcommands.size() + 1))); //здесь переход не вперед а назад (к проверке условия) поэтому минус
			}
			else{
				oss.str("");
				oss << "IFN ";  //здесь метка начала цикла (или)
				temp.push_back(make_pair(oss.str(), parentcommands.size() + 1));

				if (count_of_false_commands > 0) {
					oss.str("");
					oss << "JMP ";
					parentcommands.push_back(make_pair(oss.str(), count_of_false_commands + 1));
				}


			}


			parentcommands.insert(parentcommands.begin(), temp.begin(), temp.end());


			fout << endl << endl;
		}

	}
};



#pragma endregion


#pragma region WHILE_PARSE
class WhileParse {
public:
	
	int count_tokens = 0;

	vector<AllExpClasses *> childsWhile;
	ConditionParse *condition;

	WhileParse(vector<string>& expression);

	vector< pair< string, int > > commands;



	void print_node() {

		cout << "\n----------------------------------------" << endl;
		cout << "\nWHILE  ID:" << endl;
		cout << "'" << this << "' ";
		
		cout << "\ncount_tokens" << endl;
		cout << "'" << this->count_tokens << "' " << endl;

		cout << "\n----------------------------------------\n\n\n" << endl;

	}

	void translate() {
		condition->translate(commands, "while");

	}
};
#pragma endregion

#pragma region IF_PARSE
class IfParse {
public:
	ConditionParse *condition;

	int count_tokens = 0;

	vector<AllExpClasses *> true_childs;
	vector<AllExpClasses *> false_childs;

	vector< pair< string, int > > commands;
	vector< pair< string, int > > truecommands;
	vector< pair< string, int > > falsecommands;

	IfParse(vector<string>& expression) {
		bool continue_read = true;

		int i = 0;
		do {
			int count_of_brackets = 0;

			if (expression[i] == "if") {
				i++;

#pragma region "if_parse condition"
				vector<string> conditiontemp;
				do
				{
					if (expression[i] == "(")
					{
						count_of_brackets++;
						i++;
					}
					if (expression[i] == ")")
					{
						count_of_brackets--;
					}

					if (count_of_brackets <= 0) {
						i++;
						break;
					}
					
					conditiontemp.push_back(expression[i]);
					i += 1;


				} while (count_of_brackets > 0);
				condition = new ConditionParse(conditiontemp);
				//cout << endl;
				//i++;
#pragma endregion "if_parse condition"
				if (expression[i] == "if") {
					vector<string> temp;
					temp.assign(&expression[i], &expression[expression.size() - 1]);

					AllExpClasses *newunion = new AllExpClasses();
					IfParse* expr = new IfParse(temp);
					newunion->ifparse = expr;
					true_childs.push_back(newunion);

					i += expr->count_tokens;
					//
					truecommands.insert(truecommands.end(), expr->commands.begin(), expr->commands.end());

					continue_read = false;
				}
				if (expression[i] == "while") {
					vector<string> temp;
					temp.assign(&expression[i], &expression[expression.size() - 1]);

					AllExpClasses *newunion = new AllExpClasses();
					WhileParse* expr = new WhileParse(temp);
					newunion->whileparse = expr;
					true_childs.push_back(newunion);

					i += expr->count_tokens;
					//
					truecommands.insert(truecommands.end(), expr->commands.begin(), expr->commands.end());

					continue_read = false;
				}
				if (expression[i] == "break") {

					truecommands.push_back(make_pair("break", 0));
					i += 2;

					continue_read = false;
				}
				if (expression[i] == "continue") {

					truecommands.push_back(make_pair("continue", 0));
					i += 2;

					continue_read = false;
				}
				if (expression[i] == "print") {
					vector<string> temp;
					temp.assign(&expression[i], &expression[expression.size() - 1]);

					AllExpClasses *newunion = new AllExpClasses();
					PrintParse* expr = new PrintParse(temp, true_childs, truecommands);
					newunion->printparse = expr;

					i += expr->count_tokens;

					continue_read = false;
				}
				if (expression[i] == "return") {
					vector<string> temp;
					temp.assign(&expression[i], &expression[expression.size() - 1]);

					AllExpClasses *newunion = new AllExpClasses();
					ReturnParse* expr = new ReturnParse(temp, true_childs, truecommands);
					newunion->returnparse = expr;
					
					i += expr->count_tokens;

					continue_read = false;
				}
				if ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
					vector<string> temp;
					temp.assign(&expression[i], &expression[expression.size() - 1]);

					AllExpClasses *newunion = new AllExpClasses();
					AssignmentParse* expr = new AssignmentParse(temp, true_childs, truecommands);
					newunion->assignmentparse = expr;

					i += expr->count_tokens;

					continue_read = false;
				}
				
#pragma region "if_parse {}"
				if (expression[i] == "{" ) {

					do
					{
						if (expression[i] == "{")
						{
							count_of_brackets++;
							i++;
						}

						
						while (expression[i] == "if") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							IfParse* expr = new IfParse(temp);
							newunion->ifparse = expr;
							true_childs.push_back(newunion);

							i += expr->count_tokens;
							//
							truecommands.insert(truecommands.end(), expr->commands.begin(), expr->commands.end());

							break;
						}
						while (expression[i] == "while") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							WhileParse* expr = new WhileParse(temp);
							newunion->whileparse = expr;
							true_childs.push_back(newunion);

							i += expr->count_tokens;
							//
							truecommands.insert(truecommands.end(), expr->commands.begin(), expr->commands.end());

							break;
						}
						while (expression[i] == "break") {

							truecommands.push_back(make_pair("break", 0));
							i += 2;
						}
						while (expression[i] == "continue") {

							truecommands.push_back(make_pair("continue", 0));
							i += 2;
						}
						while (expression[i] == "print") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							PrintParse* expr = new PrintParse(temp, true_childs, truecommands);
							newunion->printparse = expr;

							i += expr->count_tokens;

							break;
						}
						while (expression[i] == "return") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							ReturnParse* expr = new ReturnParse(temp, true_childs, truecommands);
							newunion->returnparse = expr;

							i += expr->count_tokens;

							break;
						}
						while ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							AssignmentParse* expr = new AssignmentParse(temp, true_childs, truecommands);
							newunion->assignmentparse = expr;
						
							i += expr->count_tokens;

							break;
						}
						

						if (expression[i] == "}")
						{
							count_of_brackets--;
						}

						if (count_of_brackets <= 0) {
							i++;
							break;
						}

						
					} while (count_of_brackets > 0);
#pragma endregion "if_parse {}"
				}
				else {
#pragma region 
					do {
						if (!continue_read)
						{
							this->count_tokens = i;
							break;
						}
						
						if (expression[i] == "if") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							IfParse* expr = new IfParse(temp);
							newunion->ifparse = expr;
							true_childs.push_back(newunion);

							i += expr->count_tokens;
							//
							truecommands.insert(truecommands.end(), expr->commands.begin(), expr->commands.end());


							continue_read = false;
							break;
						}
						if (expression[i] == "while") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							WhileParse* expr = new WhileParse(temp);
							newunion->whileparse = expr;
							true_childs.push_back(newunion);

							i += expr->count_tokens;
							//
							truecommands.insert(truecommands.end(), expr->commands.begin(), expr->commands.end());


							continue_read = false;
							break;
						}
						if (expression[i] == "break") {

							falsecommands.push_back(make_pair("break", 0));
							i += 2;

							continue_read = false;
						}
						if (expression[i] == "continue") {

							falsecommands.push_back(make_pair("continue", 0));
							i += 2;

							continue_read = false;
						}
						if (expression[i] == "print") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							PrintParse* expr = new PrintParse(temp, true_childs, truecommands);
							newunion->printparse = expr;
							//true_childs.push_back(newunion);

							i += expr->count_tokens;
							continue_read = false;
							break;
						}
						if (expression[i] == "return") {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							ReturnParse* expr = new ReturnParse(temp, true_childs, truecommands);
							newunion->returnparse = expr;
							//true_childs.push_back(newunion);

							i += expr->count_tokens;
							continue_read = false;
							break;
						}
						if ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
							vector<string> temp;
							temp.assign(&expression[i], &expression[expression.size() - 1]);

							AllExpClasses *newunion = new AllExpClasses();
							AssignmentParse* expr = new AssignmentParse(temp, true_childs, truecommands);
							newunion->assignmentparse = expr;
							//true_childs.push_back(newunion);

							i += expr->count_tokens;
							continue_read = false;
							break;
						}
						

						if (std::find(listOfKeyWords.begin(), listOfKeyWords.end(), 
							expression[i]) != listOfKeyWords.end()) {
							i--;
							this->count_tokens = i;
							break;
						}
						
						//true_leaf.push_back(expression[i]);
						//i += 1;
						
					} while (expression[i] != ";" && continue_read);

					if (expression[i] == ";")
					{

						//true_leaf.push_back(";");
						continue_read = false;
						i++;
					}
					
					if (i == expression.size()) {
						count_tokens = i;
						break;
					}
#pragma endregion "if_parse ;"					
				}

				if (expression[i] == "else") {
					i++;
					if (expression[i] == "{") {
							do
							{
								if (expression[i] == "{")
								{
									count_of_brackets++;
									i++;
								}

								
								while (expression[i] == "if") {
									vector<string> temp;
									temp.assign(&expression[i], &expression[expression.size() - 1]);

									AllExpClasses *newunion = new AllExpClasses();
									IfParse* expr = new IfParse(temp);
									newunion->ifparse = expr;
									false_childs.push_back(newunion);

									i += expr->count_tokens;
									//
									falsecommands.insert(falsecommands.end(), expr->commands.begin(), expr->commands.end());

									break;
								}
								while (expression[i] == "while") {
									vector<string> temp;
									temp.assign(&expression[i], &expression[expression.size() - 1]);

									AllExpClasses *newunion = new AllExpClasses();
									WhileParse* expr = new WhileParse(temp);
									newunion->whileparse = expr;
									false_childs.push_back(newunion);

									i += expr->count_tokens;
									//
									falsecommands.insert(falsecommands.end(), expr->commands.begin(), expr->commands.end());


									break;
								}
								while (expression[i] == "break") {

									falsecommands.push_back(make_pair("break", 0));
									i += 2;

								}
								while (expression[i] == "continue") {

									falsecommands.push_back(make_pair("continue", 0));
									i += 2;

								}
								while (expression[i] == "print") {
									vector<string> temp;
									temp.assign(&expression[i], &expression[expression.size() - 1]);

									AllExpClasses *newunion = new AllExpClasses();
									PrintParse* expr = new PrintParse(temp, false_childs, falsecommands);
									newunion->printparse = expr;
									//false_childs.push_back(newunion);

									i += expr->count_tokens;

									break;
								}
								while (expression[i] == "return") {
									vector<string> temp;
									temp.assign(&expression[i], &expression[expression.size() - 1]);

									AllExpClasses *newunion = new AllExpClasses();
									ReturnParse* expr = new ReturnParse(temp, false_childs, falsecommands);
									newunion->returnparse = expr;
									//false_childs.push_back(newunion);

									i += expr->count_tokens;

									break;
								}
								while ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end()) || expression[i] == "var") {
									vector<string> temp;
									temp.assign(&expression[i], &expression[expression.size() - 1]);

									AllExpClasses *newunion = new AllExpClasses();
									AssignmentParse* expr = new AssignmentParse(temp, false_childs, falsecommands);
									newunion->assignmentparse = expr;
									//false_childs.push_back(newunion);

									i += expr->count_tokens;

									break;
								}
								
								if (expression[i] == "}")
								{
									count_of_brackets--;
								}

								
								if (count_of_brackets <= 0) {
									i++;
									break;
								}

								/*if (!continue_read)
								{
									this->count_tokens = i;
									break;
								}*/
								

								//false_leaf.push_back(expression[i]);
								//i += 1;

							} while (count_of_brackets > 0);
					}
					else {
						do {

							
							if (expression[i] == "if") {
								vector<string> temp;
								temp.assign(&expression[i], &expression[expression.size() - 1]);

								AllExpClasses *newunion = new AllExpClasses();
								IfParse* expr = new IfParse(temp);
								newunion->ifparse = expr;
								false_childs.push_back(newunion);

								i += expr->count_tokens;
								//
								falsecommands.insert(falsecommands.end(), expr->commands.begin(), expr->commands.end());


								continue_read = false;
								break;
							}
							if (expression[i] == "while") {
								vector<string> temp;
								temp.assign(&expression[i], &expression[expression.size() - 1]);

								AllExpClasses *newunion = new AllExpClasses();
								WhileParse* expr = new WhileParse(temp);
								newunion->whileparse = expr;
								false_childs.push_back(newunion);

								i += expr->count_tokens;
								//
								falsecommands.insert(falsecommands.end(), expr->commands.begin(), expr->commands.end());


								continue_read = false;
								break;
							}
							if (expression[i] == "break") {

								falsecommands.push_back(make_pair("break", 0));
								i += 2;

								continue_read = false;
							}
							if (expression[i] == "continue") {

								falsecommands.push_back(make_pair("continue", 0));
								i += 2;

								continue_read = false;
							}
							if (expression[i] == "print") {
								vector<string> temp;
								temp.assign(&expression[i], &expression[expression.size() - 1]);

								AllExpClasses *newunion = new AllExpClasses();
								PrintParse* expr = new PrintParse(temp, false_childs, falsecommands);
								newunion->printparse = expr;
								//false_childs.push_back(newunion);

								i += expr->count_tokens;


								continue_read = false;
								break;
							}
							if (expression[i] == "return") {
								vector<string> temp;
								temp.assign(&expression[i], &expression[expression.size() - 1]);

								AllExpClasses *newunion = new AllExpClasses();
								ReturnParse* expr = new ReturnParse(temp, false_childs, falsecommands);
								newunion->returnparse = expr;
								//false_childs.push_back(newunion);

								i += expr->count_tokens;


								continue_read = false;
								break;
							}
							if ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
								vector<string> temp;
								temp.assign(&expression[i], &expression[expression.size() - 1]);

								AllExpClasses *newunion = new AllExpClasses();
								AssignmentParse* expr = new AssignmentParse(temp, false_childs, falsecommands);
								newunion->assignmentparse = expr;
								//false_childs.push_back(newunion);

								i += expr->count_tokens;
								

								continue_read = false;
								break;
							}
							
							
							//false_leaf.push_back(expression[i]);
							//i += 1;
							
						} while (expression[i] != ";");
						if (expression[i] == ";")
						{
							//false_leaf.push_back(";");
							continue_read = false;
							i++;
						}
						/*false_leaf.push_back(expression[i]);
						i += 1;*/
					}

				}
				if (!continue_read)
				{
					this->count_tokens = i;
					break;
				}
			}
			else
			{
				this->count_tokens = i;
				break;
			}
			
		} while (i < expression.size());

		//this->count_tokens = i;
		print_node();
		translate();
	}
	

	void print_node() {

		cout << "\n----------------------------------------" << endl;
		cout << "\nIF ID:" << endl;
		cout << "'" << this << "' ";
		
		cout << "\ncount_tokens" << endl;
		cout << "'" << this->count_tokens << "' " << endl;
		/*i += expr->count_tokens;*/

		cout << "\n----------------------------------------\n\n\n" << endl;

	}

	void translate() {
		condition->translate(truecommands, "if", falsecommands.size());
		commands.insert(commands.end(), truecommands.begin(), truecommands.end());
		commands.insert(commands.end(), falsecommands.begin(), falsecommands.end());

	}
};
#pragma endregion

#pragma region WhileParce_CONSTRUCTOR
WhileParse::WhileParse(vector<string>& expression) {
	bool continue_read = true;

	int i = 0;
	do {
		int count_of_brackets = 0;

		if (expression[i] == "while") {
			i++;

#pragma region "if_parse condition"
			vector<string> conditiontemp;
			do
			{
				if (expression[i] == "(")
				{
					count_of_brackets++;
					i++;
				}
				if (expression[i] == ")")
				{
					count_of_brackets--;
				}

				if (count_of_brackets <= 0) {
					i++;
					break;
				}

				conditiontemp.push_back(expression[i]);
				i += 1;


			} while (count_of_brackets > 0);
			condition = new ConditionParse(conditiontemp);

			//cout << endl;
			//i++;
#pragma endregion "if_parse condition"

			if (expression[i] == "while") {
				vector<string> temp;
				temp.assign(&expression[i], &expression[expression.size() - 1]);

				AllExpClasses *newunion = new AllExpClasses();
				WhileParse* expr = new WhileParse(temp);
				newunion->whileparse = expr;
				childsWhile.push_back(newunion);

				i += expr->count_tokens;
				//
				commands.insert(commands.end(), expr->commands.begin(), expr->commands.end());

				continue_read = false;
			}
			if (expression[i] == "if") {
				vector<string> temp;
				temp.assign(&expression[i], &expression[expression.size() - 1]);

				AllExpClasses *newunion = new AllExpClasses();
				IfParse* expr = new IfParse(temp);
				newunion->ifparse = expr;
				childsWhile.push_back(newunion);

				i += expr->count_tokens;
				//
				commands.insert(commands.end(), expr->commands.begin(), expr->commands.end());

				continue_read = false;
			}
			if (expression[i] == "print") {
				vector<string> temp;
				temp.assign(&expression[i], &expression[expression.size() - 1]);

				AllExpClasses *newunion = new AllExpClasses();
				PrintParse* expr = new PrintParse(temp, childsWhile, commands);
				newunion->printparse = expr;
				//childsWhile.push_back(newunion);

				i += expr->count_tokens;

				continue_read = false;
			}
			if (expression[i] == "return") {
				vector<string> temp;
				temp.assign(&expression[i], &expression[expression.size() - 1]);

				AllExpClasses *newunion = new AllExpClasses();
				ReturnParse* expr = new ReturnParse(temp, childsWhile, commands);
				newunion->returnparse = expr;
				//childsWhile.push_back(newunion);

				i += expr->count_tokens;

				continue_read = false;
			}

			if ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
				vector<string> temp;
				temp.assign(&expression[i], &expression[expression.size() - 1]);

				AllExpClasses *newunion = new AllExpClasses();
				AssignmentParse* expr = new AssignmentParse(temp, childsWhile, commands);
				newunion->assignmentparse = expr;
				//childsWhile.push_back(newunion);

				i += expr->count_tokens;

				continue_read = false;
			}
#pragma region "if_parse {}"
			if (expression[i] == "{") {

				do
				{
					if (expression[i] == "{")
					{
						count_of_brackets++;
						i++;
					}

					while (expression[i] == "while") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						WhileParse* expr = new WhileParse(temp);
						newunion->whileparse = expr;
						childsWhile.push_back(newunion);

						i += expr->count_tokens;
						//
						commands.insert(commands.end(), expr->commands.begin(), expr->commands.end());

						//i += 1;
						break;
					}
					while (expression[i] == "if") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						IfParse* expr = new IfParse(temp);
						newunion->ifparse = expr;
						childsWhile.push_back(newunion);

						i += expr->count_tokens;
						//
						commands.insert(commands.end(), expr->commands.begin(), expr->commands.end());

						//i += 1;
						break;
					}
					while (expression[i] == "print") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);
						temp.push_back(expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						PrintParse* expr = new PrintParse(temp, childsWhile, commands);
						newunion->printparse = expr;
						//childsWhile.push_back(newunion);

						i += expr->count_tokens;

						break;
					}
					while (expression[i] == "return") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);
						temp.push_back(expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						ReturnParse* expr = new ReturnParse(temp, childsWhile, commands);
						newunion->returnparse = expr;
						//childsWhile.push_back(newunion);

						i += expr->count_tokens;

						break;
					}
					while ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);
						temp.push_back(expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						AssignmentParse* expr = new AssignmentParse(temp, childsWhile, commands);
						newunion->assignmentparse = expr;
						//childsWhile.push_back(newunion);

						i += expr->count_tokens;

						break;
					}

					if (expression[i] == "}")
					{
						count_of_brackets--;
					}

					if (count_of_brackets <= 0) {
						i++;

						break;
					}

					/*if (!continue_read)
					{
						this->count_tokens = i;
						break;
					}*/


					//leaf.push_back(expression[i]);
					//i += 1;


				} while (count_of_brackets > 0);
#pragma endregion "if_parse {}"
			}
			else {
#pragma region 
				do {
					if (!continue_read)
					{
						this->count_tokens = i;
						break;
					}
					if (expression[i] == "while") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						WhileParse* expr = new WhileParse(temp);
						newunion->whileparse = expr;
						childsWhile.push_back(newunion);

						i += expr->count_tokens;
						//
						commands.insert(commands.end(), expr->commands.begin(), expr->commands.end());

						i += 1;
						break;
					}
					if (expression[i] == "if") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						IfParse* expr = new IfParse(temp);
						newunion->ifparse = expr;
						childsWhile.push_back(newunion);

						i += expr->count_tokens;
						//
						commands.insert(commands.end(), expr->commands.begin(), expr->commands.end());

						i += 1;
						break;
					}
					if (expression[i] == "print") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						PrintParse* expr = new PrintParse(temp, childsWhile, commands);
						newunion->printparse = expr;
						//childsWhile.push_back(newunion);

						i += expr->count_tokens;

						break;
					}
					if (expression[i] == "return") {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						ReturnParse* expr = new ReturnParse(temp, childsWhile, commands);
						newunion->returnparse = expr;
						//childsWhile.push_back(newunion);

						i += expr->count_tokens;

						break;
					}
					if ((std::find(list_of_vars.begin(), list_of_vars.end(), expression[i]) != list_of_vars.end() || expression[i] == "var")) {
						vector<string> temp;
						temp.assign(&expression[i], &expression[expression.size() - 1]);

						AllExpClasses *newunion = new AllExpClasses();
						AssignmentParse* expr = new AssignmentParse(temp, childsWhile, commands);
						newunion->assignmentparse = expr;
						//childsWhile.push_back(newunion);

						i += expr->count_tokens;

						break;
					}

					if (std::find(listOfKeyWords.begin(), listOfKeyWords.end(),
						expression[i]) != listOfKeyWords.end()) {
						i--;
						this->count_tokens = i;

						break;
					}

					//leaf.push_back(expression[i]);
					//i += 1;

				} while (expression[i] != ";" && continue_read);

				if (expression[i] == ";")
				{

					continue_read = false;
					i++;
				}

				if (i == expression.size()) {
					count_tokens = i;
					break;
				}
#pragma endregion "if_parse ;"					
			}

			if (!continue_read)
			{
				this->count_tokens = i;
				break;
			}
		}
		else
		{
			this->count_tokens = i;
			break;
		}

	} while (i < expression.size());

	this->count_tokens = i;
	print_node();
	translate();
}

#pragma endregion


void make_expressions(vector<string>& _list_of_tokens, Basic *basic) {

	//Basic *basic = new Basic();

	if (_list_of_tokens.size() != 0) {
		vector<string> expression = {};
		for (int i = 0; i < _list_of_tokens.size() - 1;) {
			if (i < _list_of_tokens.size() - 1) {
				while (_list_of_tokens[i] == "if") {
					cout << "\n\n---------------------------BEGINING IF---------------------\n";
					vector<string> temp;
					temp.assign(&_list_of_tokens[i], &_list_of_tokens[_list_of_tokens.size() - 1]);
					IfParse* expr = new IfParse(temp);
					AllExpClasses *newunion = new AllExpClasses();
					newunion->ifparse = expr;
					basic->childs.push_back(newunion);

					basic->commands.insert(basic->commands.end(), expr->commands.begin(), expr->commands.end());

					i += expr->count_tokens;
					cout << "\n\n---------------------------ENDING IF---------------------\n";
				}
			}
			if (i < _list_of_tokens.size() - 1) {
				while (_list_of_tokens[i] == "while") {
					cout << "\n\n---------------------------BEGINING WHILE---------------------\n";
					vector<string> temp;
					temp.assign(&_list_of_tokens[i], &_list_of_tokens[_list_of_tokens.size() - 1]);
					WhileParse* expr = new WhileParse(temp);
					AllExpClasses *newunion = new AllExpClasses();
					newunion->whileparse = expr;
					basic->childs.push_back(newunion);

					basic->commands.insert(basic->commands.end(), expr->commands.begin(), expr->commands.end());
					

					i += expr->count_tokens;
					cout << "\n\n---------------------------ENDING WHILE---------------------\n";
				}
			}
			if (i < _list_of_tokens.size() - 1) {

				if (_list_of_tokens[i] == "print") {
					cout << "\n\n---------------------------BEGINING PRINT---------------------\n";
					vector<string> temp;
					temp.assign(&_list_of_tokens[i], &_list_of_tokens[_list_of_tokens.size() - 1]);
					PrintParse *ass = new PrintParse(temp, basic->childs, basic->commands);
					i += ass->count_tokens;
					cout << "\n\n---------------------------ENDING PRINT---------------------\n";
				}

			}
			if (i < _list_of_tokens.size() - 1) {

				if (_list_of_tokens[i] == "return") {
					cout << "\n\n---------------------------BEGINING RETURN---------------------\n";
					vector<string> temp;
					temp.assign(&_list_of_tokens[i], &_list_of_tokens[_list_of_tokens.size() - 1]);
					ReturnParse *ass = new ReturnParse(temp, basic->childs, basic->commands);
					i += ass->count_tokens;
					cout << "\n\n---------------------------ENDING RETURN---------------------\n";
				}

			}
			if (i < _list_of_tokens.size() - 1) {
				
				if ((std::find(list_of_vars.begin(), list_of_vars.end(), _list_of_tokens[i]) != list_of_vars.end() || _list_of_tokens[i] == "var")) {
					cout << "\n\n---------------------------BEGINING ASSIGNMENT---------------------\n";
					vector<string> temp;
					temp.assign(&_list_of_tokens[i], &_list_of_tokens[_list_of_tokens.size() - 1]);
					AssignmentParse *ass = new AssignmentParse(temp, basic->childs, basic->commands);
					i += ass->count_tokens;
					cout << "\n\n---------------------------ENDING ASSIGNMENT---------------------\n";
				}
				
			}


		}
	}
}

#pragma region ERRORS

void while_expression_parsing(int& i, vector<string>& _list_of_tokens);

vector<string> list_of_if_content = {};

void if_expression_parsing(int& i, vector<string>& _list_of_tokens) {
	int count_of_brackets = 0;

	if (_list_of_tokens[i] == "if") {
		list_of_if_content.push_back(_list_of_tokens[i]);

		i++;
		do
		{
			if (_list_of_tokens[i] == "(")
			{
				count_of_brackets++;
			}
			if (_list_of_tokens[i] == ")")
			{
				count_of_brackets--;
			}

			list_of_if_content.push_back(_list_of_tokens[i]);
			i += 1;


		} while (count_of_brackets > 0);


		if (_list_of_tokens[i] == "{") {
			while (_list_of_tokens[i - 1] != "}") {
				if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
				if (_list_of_tokens[i] == "while") while_expression_parsing(i, _list_of_tokens);

				list_of_if_content.push_back(_list_of_tokens[i]);
				i += 1;
			}
		}
		else {
			while (_list_of_tokens[i - 1] != ";") {
				if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
				if (_list_of_tokens[i] == "while") while_expression_parsing(i, _list_of_tokens);

				list_of_if_content.push_back(_list_of_tokens[i]);
				i += 1;
			}
		}

		if (_list_of_tokens[i] == "else") {
			if (_list_of_tokens[i] == "{") {
				while (_list_of_tokens[i - 1] != "}") {
					if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
					if (_list_of_tokens[i] == "while") while_expression_parsing(i, _list_of_tokens);

					list_of_if_content.push_back(_list_of_tokens[i]);
					i += 1;
				}
			}
			else {
				while (_list_of_tokens[i - 1] != ";") {
					if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
					if (_list_of_tokens[i] == "while") while_expression_parsing(i, _list_of_tokens);

					list_of_if_content.push_back(_list_of_tokens[i]);
					i += 1;
				}
			}

		}
	}
	return;
}

vector<string> list_of_while_content = {};

void while_expression_parsing(int& i, vector<string>& _list_of_tokens) {
	int count_of_brackets = 0;

	if (_list_of_tokens[i] == "while") {
		list_of_while_content.push_back(_list_of_tokens[i]);
		i++;
		do
		{
			if (_list_of_tokens[i] == "(")
			{
				count_of_brackets++;
			}
			if (_list_of_tokens[i] == ")")
			{
				count_of_brackets--;
			}

			list_of_while_content.push_back(_list_of_tokens[i]);
			i += 1;


		} while (count_of_brackets > 0);

		if (_list_of_tokens[i] == "{") {
			while (_list_of_tokens[i - 1] != "}") {
				if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
				if (_list_of_tokens[i] == "while") while_expression_parsing(i, _list_of_tokens);
				list_of_while_content.push_back(_list_of_tokens[i]);
				i += 1;
			}
		}
		else {
			while (_list_of_tokens[i + 1] != ";") {
				list_of_while_content.push_back(_list_of_tokens[i]);
				i += 1;
			}
			return;
		}
	}
	return;
}



vector<vector<string>> list_of_expressions = {};

void make_expressions_for_CHECK_ERRORS(vector<string>& _list_of_tokens) {

	if (_list_of_tokens.size() != 0) {
		vector<string> expression = {};
		for (int i = 0; i < _list_of_tokens.size();) {
			while (_list_of_tokens[i] != ";") {
				if (_list_of_tokens[i] == "if") {
					if_expression_parsing(i, _list_of_tokens);
					expression.insert(expression.end(), list_of_if_content.begin(), list_of_if_content.end());
				}

				else if (_list_of_tokens[i] == "while") {
					while_expression_parsing(i, _list_of_tokens);
					expression.insert(expression.end(), list_of_while_content.begin(), list_of_while_content.end());
				}

				else if (i >= _list_of_tokens.size()) break;
				expression.push_back(_list_of_tokens[i]);
				i += 1;
			}
			i += 1;
			list_of_expressions.push_back(expression);
			expression = {};
		}
	}
}

#pragma endregion


void second_translate(Basic *basic){
	for (int i = 0; i < basic->commands.size(); i++) {
		
		if (basic->commands[i].first == "continue") {
			int temp = i;
			int temp2 = 0;
			while (basic->commands[i].first[0] != 'W') {
				i--; 
				temp2++;
			}
			basic->commands[temp].first = "JMP ";
			basic->commands[temp].second = 0 - temp2;
		}
		if (basic->commands[i].first == "break") {
			int temp = i;
			int temp2 = 0;
			while (basic->commands[i].first[0] != 'S' && basic->commands[i].first[1] != 'W') {
			
				i--;
				temp2++;
			}
			basic->commands[temp].first = "JMP ";
			basic->commands[temp].second = basic->commands[i].second - temp2;
		}
	}

}

void third_translate(Basic *basic) {
	for (int i = 0; i < basic->commands.size(); i++) {

		if (basic->commands[i].first[0] == 'W') {
			string temp = "";
			for (int j = 2; j < basic->commands[i].first.size(); j++ ) {
				temp = temp + basic->commands[i].first[j];
			}
			basic->commands[i].first = temp;
		}
		if (basic->commands[i].first[0] == 'S' && basic->commands[i].first[1] == 'W') {
			string temp = "";
			for (int j = 3; j < basic->commands[i].first.size(); j++) {
				temp = temp + basic->commands[i].first[j];
			}
			basic->commands[i].first = temp;
		}
	}

}


#pragma endregion

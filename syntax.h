#pragma once


#include <regex>
#include <iomanip>
#include "leks_rapse.h"
#include "exp_parse.h"
#include "help_funcs.h"


#include <sstream>
using namespace std;


ofstream fout;

//глобальный счетчик операций
int GlobalCount{0};

//временная строка для формирования команды с потоком для нее
string str{""};
ostringstream oss(str);

class Tree {
public:
	vector<string> data;
	Tree* left;
	Tree* right;

	Tree() {
		data = {};
		left = NULL, right = NULL;
	}

	Tree(vector<string> data) {
		this->data = data;
		left = NULL, right = NULL;
	}
};

// функция строит дерево  ЧТО ПЕРЕДАВАТЬ ЧТО ВЫВОДИТ
void build_tree(Tree* root) {
	//создадим флажок нахождения в скобках
	// точнее счетчик
	//когда "(" он +1 а когда ")" то -1

	int numOfScob = 0;

	if (root->data.size() == 1) return;

	//убираем лишние скобки в начале и в конце типа было (A + B) а стало A + B просто

	string firstSkob = "";
	if (root->data[0] == "(" && root->data[root->data.size() - 1] == ")") {
		//bool flag = true;
		//сначала надо убедиться что после первой ( не стоит )
		for (int i = 1; i < (root->data.size() - 1); i++) {
			if (root->data[i] == "(" || root->data[i] == ")") {
				firstSkob += root->data[i];
				break;
			}
		}
		//теперь проверим счетчик
		for (int j = 1; j < (root->data.size() - 1); ) {
			if (root->data[j] == "(") {
				numOfScob += 1;
				j += 1;
			}
			else if (root->data[j] == ")") {
				numOfScob -= 1;
				j += 1;
			}
			else j += 1;
		}
		if (firstSkob != ")" && numOfScob == 0) {
			root->data.erase(root->data.begin());
			root->data.pop_back();
		}
	}

	//строим вниз через рекурсию
	for (int i = 0; i < root->data.size(); ) {

		if (root->data[i] == "(") {
			numOfScob += 1;
			i += 1;
		}
		else if (root->data[i] == ")") {
			numOfScob -= 1;
			i += 1;
		}
		else if (numOfScob == 0) {
			if (root->data[i] == "+" || root->data[i] == "-") {

				vector<string> newVec1 = {};
				vector<string> newVec2 = {};

				vector<string> thisLeaf = { root->data[i] };


				newVec1.insert(newVec1.end(), &root->data[0], &root->data[i]);
				root->data.push_back("qwe"); //это чтобы можно обратиться к &root->data[root->data.size() - 1]
				newVec2.insert(newVec2.end(), &root->data[i + 1], &root->data[root->data.size() - 1]);
				root->data.pop_back();

				Tree* newSubExp1 = new Tree(newVec1);
				Tree* newSubExp2 = new Tree(newVec2);

				root->data = thisLeaf;
				root->left = newSubExp1;
				root->right = newSubExp2;

				build_tree(root->left);
				build_tree(root->right);

				return;
			}
			i += 1;

		}
		else i += 1;
	}

	for (int i = 0; i < root->data.size(); ) {

		if (root->data[i] == "(") {
			numOfScob += 1;
			i += 1;
		}
		else if (root->data[i] == ")") {
			numOfScob -= 1;
			i += 1;
		}
		else if (numOfScob == 0) {
			if (root->data[i] == "*" || root->data[i] == "/") {

				vector<string> newVec1 = {};
				vector<string> newVec2 = {};

				vector<string> thisLeaf = { root->data[i] };


				newVec1.insert(newVec1.end(), &root->data[0], &root->data[i]);
				root->data.push_back("qwe"); //это чтобы можно обратиться к &root->data[root->data.size() - 1]
				newVec2.insert(newVec2.end(), &root->data[i + 1], &root->data[root->data.size() - 1]);
				root->data.pop_back();

				Tree* newSubExp1 = new Tree(newVec1);
				Tree* newSubExp2 = new Tree(newVec2);

				root->data = thisLeaf;
				root->left = newSubExp1;
				root->right = newSubExp2;

				build_tree(root->left);
				build_tree(root->right);

				return;

			}
			i += 1;

		}
		else i += 1;

	}
}



//функция обхода дерева. сначала левое. потом правое поддерево. затем корень
void pass_tree(Tree *root, vector< pair< string, int > > &commands, bool reset = true) {
	vector<Tree *> stack = {};
	while (root != NULL || stack.size() != 0) {
		if (stack.size() != 0) {
			root = stack[stack.size() - 1];
			stack.pop_back();
			if (stack.size() != 0 && root->right == stack[stack.size() - 1]) {
				root = stack[stack.size() - 1];
				stack.pop_back();
			}
			else {
				//добавим обработку операторов
				if (be_in(root->data[0], list_of_vars_and_nums)) {
					//обнуляем временную строку (поток)
					if(reset)oss.str("");
					
					// запись в файл нужно производить уже в конце программы из одного массива. здесь выводится отладочная информация
					fout	<< setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
							<< "PUSH "
							<< setfill('0') << setw(4) << uppercase << hex << findIn(root->data[0], list_of_vars_and_nums) + 1 << ends << endl; //это будет типа "0012: PUSH 0001"
					//то же самое только без адреса команды заносим в массив команд. это будет типа "PUSH 0001"
					oss << "PUSH " << setfill('0') << setw(4) << uppercase << hex << findIn(root->data[0], list_of_vars_and_nums) + 1 << ends; //+1  из за временной переменной в 0000:

					commands.push_back(make_pair(oss.str(), 0)); //0 потому что здесь не будет никаких переходов

					//GlobalCount нужен только для отладки
					reset = true;

					GlobalCount += 1;
				}
				else if (be_in(root->data[0], operators)) {

					fout << setfill('0') << setw(4) << uppercase << hex << GlobalCount << ends << ": "
						<< operators[root->data[0]] << endl;

					commands.push_back(make_pair(operators[root->data[0]], 0)); //0 потому что здесь не будет никаких переходов

					GlobalCount += 1;

				}

				root = NULL;
			}
		}
		while (root != NULL) {
			stack.push_back(root);
			if (root->right != NULL) {
				stack.push_back(root->right);
				stack.push_back(root);
			}
			root = root->left;
		}
	}
}



// ПЕЧАТЬ ДЕРЕВА indent - ширина
void print_tree(Tree* p, int indent = 0)
{
	if (p != NULL) {
		if (p->right) {
			print_tree(p->right, indent + 4);
		}
		if (indent) {
			std::cout << std::setw(indent) << ' ';
		}
		if (p->right) std::cout << " /\n" << std::setw(indent) << ' ';
		for (string str : p->data) {
			cout << "'" << str << "'" << endl;
		}
		if (p->left) {
			std::cout << std::setw(indent) << ' ' << " \\\n";
			print_tree(p->left, indent + 4);
		}
	}
}


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <regex>
#include <iomanip>
using namespace std;

#pragma region readFile
string readFile(const string& fileName) {
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
vector<string> listOfKeyWords{ "var","if", "else", "print", "return" };
vector<string> listOfDelimeters{ ",", ";" };


vector<string> list_of_vars = {};
vector<string> list_of_numbers = {};
#pragma endregion

#pragma region parseFile
vector<string> parseFile(string& buff) {
	vector<string> wordsAndSymbols;

	vector<string> allVariables = {};
	vector<string> allNumbers = {};

	const regex txt_regexNotW("^\\W");     //òîëüêî ñèìâîëû áåç öèôð è áóêâ
	const regex txt_regexNotS("^\\S");     //âñå áåç ïðîáåëîâ
	const regex txt_regexW("\\w");         //òîëüêî öèôðû è áóêâû
	const regex txt_regexNotD("\\D");      //íèêàêèõ öèôð
	const regex txt_regexD("\\d");         //only öèôðû
	const regex txt_regexS("\\s");         //òîëüêî ïðîáåëüíûå ñèìâîëû

	for (int i1 = 0; i1 < buff.size();) {
		string tempString = "";
		string symbol = "";
		tempString += buff[i1];

		if (regex_match(tempString, txt_regexNotW) && regex_match(tempString, txt_regexNotS)) { //òîëüêî ñèìâîëû áåç ïðîáåëîâ
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
					while (symbol != "\n") { //áóêâû è öèôðû áåç ïðîáåëîâ
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

		if (regex_match(tempString, txt_regexW) && regex_match(tempString, txt_regexNotD)) { //òîëüêî áóêâû
			symbol += buff[i1 + 1];
			while (regex_match(symbol, txt_regexW) && regex_match(symbol, txt_regexNotS) && !regex_match(symbol, txt_regexNotW)) { //áóêâû è öèôðû áåç ïðîáåëîâ
				i1 += 1;
				tempString += buff[i1];
				symbol = "";
				symbol += buff[i1 + 1];
			}
			symbol = "";
			i1 += 1;

			//ïðîâåðÿåì åñòü ëè ñðåäè êëþ÷åâûõ ñëîâ è åñëè ïîâåçåò äîáàâëÿåì â ìàññèâ ïåðåìåííûõ
			for (int i = 0; i < listOfKeyWords.size(); ) {
				if (tempString == listOfKeyWords[i]) break;
				else i += 1;
				if (i == listOfKeyWords.size()) allVariables.push_back(tempString);
			}
			/////////////////////////////////////////////////////////////////////////////

			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexD)) { //÷èñëîâûå êîíñòàíòû
			symbol += buff[i1 + 1];
			while (regex_match(symbol, txt_regexD)) {
				i1 += 1;
				tempString += buff[i1];
				symbol = "";
				symbol += buff[i1 + 1];
			}
			symbol = "";
			i1 += 1;

			//íàõîäèì ÷èñëîâûå êîíñòàíòû è ñîáèðàåì èõ â îäèí ìàññèâ
			allNumbers.push_back(tempString);
			////////////////////////////////////////////////////////

			wordsAndSymbols.push_back(tempString);
		}

		if (regex_match(tempString, txt_regexS)) { //ïðîáåëüíûå ñèìâîëû
			i1 += 1;
		}
	}

	//óáèðàåì äóáëèêàòû ñðåäè ïåðåìåííûõ è ÷èñåë
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

#pragma region expressions
//ñ èôàìè âñå ñëîæíî ïèïåö
vector<string> list_of_if_content = {};

void if_expression_parsing(int& i, vector<string>& _list_of_tokens) {
	int count_of_brackets = 0;

	if (_list_of_tokens[i] == "if") {
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

		//! make_tree(list_of_if_content);
		


		/*while (_list_of_tokens[i - 1] != ")" && count_of_brackets > 0) {
			
			list_of_if_tokens.push_back(_list_of_tokens[i]);
			i += 1;
		}*/

		if (_list_of_tokens[i] == "{") {
			while (_list_of_tokens[i - 1] != "}") {
				if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
				list_of_if_content.push_back(_list_of_tokens[i]);
				i += 1;
			}
		}
		else {
			while (_list_of_tokens[i - 1] != ";") {
				if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
				list_of_if_content.push_back(_list_of_tokens[i]);
				i += 1;
			}
		}

		if (_list_of_tokens[i] == "else") {
			if (_list_of_tokens[i] == "{") {
				while (_list_of_tokens[i - 1] != "}") {
					if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
					list_of_if_content.push_back(_list_of_tokens[i]);
					i += 1;
				}
			}
			else {
				while (_list_of_tokens[i - 1] != ";") {
					if (_list_of_tokens[i] == "if") if_expression_parsing(i, _list_of_tokens);
					list_of_if_content.push_back(_list_of_tokens[i]);
					i += 1;
				}
			}

		}
	}
	return;
}

//ðàçáèâàåì íà âûðàæåíèÿ òî åñòü îò ; äî ; // îòäåëüíî ïîëíîñòüþ âûâîäèì èôû
vector<vector<string>> list_of_expressions = {};

void make_expressions(vector<string>& _list_of_tokens) {
	
	if (_list_of_tokens.size() != 0) {
		vector<string> expression = {};
		for (int i = 0; i < _list_of_tokens.size();) {
			while (_list_of_tokens[i] != ";") {
				if (_list_of_tokens[i] == "if") {
					if_expression_parsing(i, _list_of_tokens);
					expression.insert(expression.end(), list_of_if_content.begin(), list_of_if_content.end());
				}
				if (i >= _list_of_tokens.size()) break;
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

// òåïåðü äëÿ êàæäîãî âûðàæåíèÿ íàäî ïîñòðîèòü äåðåâöå
/* äëÿ ýòîãî:
1. ïðèîðèòåòû
2. ðàçáèòèå íà ïîäâûðàæåíèÿ
*/

//êëàññ äëÿ ðàçáîðà âûðàæåíèé íà ïîäâûðàæåíèÿ òî áèøü è äëÿ äåðåâüåâ
class tree {
public:
	vector<string> data;
	tree* left;
	tree* right;

	tree() {
		data = {};
		left = NULL, right = NULL;
	}

	tree(vector<string> &data) {
		this->data = data;
		left = NULL, right = NULL;
	}
};


// ôóíêöèÿ ñòðîèò äåðåâî
void treesProto(tree* root) {
	//ñîçäàäèì ôëàæîê íàõîæäåíèÿ â ñêîáêàõ
	// òî÷íåå ñ÷åò÷èê
	//êîãäà "(" îí +1 à êîãäà ")" òî -1

	int numOfScob = 0;

	if (root->data.size() == 1) return;

	//óáèðàåì ëèøíèå ñêîáêè â íà÷àëå è â êîíöå òèïà áûëî (A + B) à ñòàëî A + B ïðîñòî

	//bool flag = false;
	string firstSkob = "";
	if (root->data[0] == "(" && root->data[root->data.size() - 1] == ")") {
		//bool flag = true;
		//ñíà÷àëà íàäî óáåäèòüñÿ ÷òî ïîñëå ïåðâîé ( íå ñòîèò )
		for (int i = 1; i < (root->data.size() - 1); i++) {
			if (root->data[i] == "(" || root->data[i] == ")") {
				firstSkob += root->data[i];
				break;
			}
		}
		//òåïåðü ïðîâåðèì ñ÷åò÷èê
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

	for (string str : root->data) {
		cout << str;
	}
	cout << endl;

	//ñòðîèì âíèç ÷åðåç ðåêóðñèþ
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
				root->data.push_back("qwe"); //ýòî ÷òîáû ìîæíî îáðàòèòüñÿ ê &root->data[root->data.size() - 1]
				newVec2.insert(newVec2.end(), &root->data[i + 1], &root->data[root->data.size() - 1]);
				root->data.pop_back();

				tree* newSubExp1 = new tree(newVec1);
				tree* newSubExp2 = new tree(newVec2);

				root->data = thisLeaf;
				root->left = newSubExp1;
				root->right = newSubExp2;

				treesProto(root->left);
				treesProto(root->right);

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
				root->data.push_back("qwe"); //ýòî ÷òîáû ìîæíî îáðàòèòüñÿ ê &root->data[root->data.size() - 1]
				newVec2.insert(newVec2.end(), &root->data[i + 1], &root->data[root->data.size() - 1]);
				root->data.pop_back();

				tree* newSubExp1 = new tree(newVec1);
				tree* newSubExp2 = new tree(newVec2);

				root->data = thisLeaf;
				root->left = newSubExp1;
				root->right = newSubExp2;

				treesProto(root->left);
				treesProto(root->right);

				return;

			}
			i += 1;

		}
		else i += 1;

	}
}

//íàïèøåì ôóíêöèþ êîòîðàÿ ïðîâåðÿåò íàëè÷èå ýëåìåíòà â ìàññèâå
bool beIn(string str, vector<string>& vec) {
	for (string temp : vec) {
		if (str == temp) return true;
	}
	return false;
}

//íàïèøåì ôóíêöèþ êîòîðàÿ âîçâðàùàåò íîìåð ïîçèöèè ýëåìåíòà â ìàññèâå
int findIn(string str, vector<string>& vec) {
	int pos = 0;
	for (string temp : vec) {
		if (str == temp) return pos;
	}
	return -1;
}


//åñòü ìíîãî âèäîâ èíñòðóêöèé:
//1. îáúÿâëåíèÿ ïåðåìåííûõ
//2. ïðèñâîåíèÿ çíà÷åíèé
//3. ñïåöèàëüí èíñòðóêöèè òèïà print
//4. óñëîâíûå èíñòðóêöèè
//5. öèêëû
//6. èíñòðóêöèè ïîòîêà óïðàâëåíèÿ òèïà return, break
//7. áëîêè êîäà òèïà {...}
//òî áèøü íàäî 7 êëàññîâ åùå


#pragma region classes
//ýòî êëàññû äëÿ èíñòðóêöèé
//class setValInstr {
//	setValInstr(int pos, vector<string>& str) {
//		//ñíà÷àëà ïîñìîòðèì íàäî ëè âû÷èñëÿòü òî ÷òî ïîñëå "="
//		if (str.size() > 3) {
//
//		}
//		//åñëè íàäî òî...
//		else {
//			int i = 0;
//			for (string temp : allVarsAndVals) {
//				if (temp == str[pos]) {
//					cout << hex << i << endl;
//
//
//				}
//			}
//		}
//	}
//};

class varDefInstr {
	varDefInstr(vector<string>& exp) {
		int pos = 0;
		for (string tempExp : exp) {
			for (string tempVariables : list_of_vars) {
				if (tempExp == tempVariables) {
					if (exp[pos + 1] == ",") {
						//ïðîñòî ïåðåõîäèì ê ñëåäóþùåé ïåðåìåííîé
						pos += 1;
					}
					else if (exp[pos + 1] == "=") {
						//åñëè ïðîèñõîäèò íå òîëüêî îáúÿâëåíèå íî è îïðåäåëåíèå òî ñíà÷àëà ñ÷èòàåì âûðàæåíèå ñïðàâà è ïîòîì  äåëàåì push
						vector<string> tempVector = {};
						for (int i = pos + 1; i < exp.size() || exp[i] != ",";) {
							tempVector.push_back(exp[i]);
						}

					}
				}
			}
			pos += 1;
		}
	}

};

class srecInstr {

};
class ifInstr {

};
//class circleInstr {
//
//};
class threadControlInstr {

};
class blockInstr {

};

#pragma endregion



// çäåñü íàõîäÿòñÿ âñå ïåðåìåííûå è ÷èñëîâûå êîíñòàíòû
vector<string> list_of_vars_and_nums = {};


//ýòà ùòóêà áóäåò îïðåäåëÿòü òèï èíòñðóêöèè è íåïîñðåäñòâåííî îáðàáàòûâàòü
void parseExp(vector<string>& exp) {
	if (exp[0] == "if") {

	}
	/*else if (exp[0] == "while" || exp[0] == "for") {
	}*/
	else if (exp[0] == "{") {

	}
	else if (exp[0] == "return") {

	}
	else if (exp[0] == "print") {

	}
	else if (exp[0] == "var") {

	}
	else {
		for (string page : exp) {

		}
	}
	return;
}



void print_Tree(tree* p, int level) {
	if (p)
	{
		print_Tree(p->left, level + 1);
		for (int i = 0; i < level; i++) cout << "   ";
		for (string str : p->data) {
			cout << str << endl;
		}
		print_Tree(p->right, level + 1);
	}
}

int main() {
	string buffer = readFile("test.txt");

	cout << buffer;

	vector<string> list_of_tokens = parseFile(buffer);

	make_expressions(list_of_tokens);

	list_of_vars_and_nums.insert(list_of_vars_and_nums.end(), list_of_vars.begin(), list_of_vars.end());
	list_of_vars_and_nums.insert(list_of_vars_and_nums.end(), list_of_numbers.begin(), list_of_numbers.end());

	/*int i = 0;
	for (string temp : allVarsAndVals) {
	cout << setfill('0') << setw(4) << uppercase << hex << i << ends << " :" << allVarsAndVals[i] << endl;
	i++;
	}*/

	/*vector<BinaryTree *> trees = {};
	for (vector<string> exp : Expressions) {
	BinaryTree* newTree = new BinaryTree();
	newTree->trees(exp);
	trees.push_back(newTree);
	}
	for (BinaryTree* temp : trees) {
	print_Tree(temp->root, 0);
	}*/

	tree* operations = new tree(list_of_expressions[0]);
	treesProto(operations);

	print_Tree(operations, 0);

	return 0;

}

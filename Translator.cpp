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
vector<string> listOfKeyWords{ "var","if", "else", "print", "return" };
vector<string> listOfDelimeters{ ",", ";" };


vector<string> Variables = {};
vector<string> Numbers = {};
#pragma endregion

#pragma region parseFile
vector<string> parseFile(string& buff) {
    vector<string> wordsAndSymbols;

    vector<string> allVariables = {};
    vector<string> allNumbers = {};

    const regex txt_regexNotW("^\\W");     //только символы без цифр и букв
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

            //проверяем есть ли среди ключевых слов и если повезет добавляем в массив переменных
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
            if (Variables.size() == 0) Variables.push_back(allVariables[i]);
            for (int j = 0; j < Variables.size(); ) {
                if (allVariables[i] == Variables[j]) break;
                j += 1;
                if (j == Variables.size())Variables.push_back(allVariables[i]);
            }
        }
    }

    if (allNumbers.size() != 0) {
        for (int i = 0; i < allNumbers.size(); i++) {
            if (Numbers.size() == 0) Numbers.push_back(allNumbers[i]);
            for (int j = 0; j < Numbers.size(); ) {
                if (allNumbers[i] == Numbers[j]) break;
                j += 1;
                if (j == Numbers.size())Numbers.push_back(allNumbers[i]);
            }
        }
    }

    return wordsAndSymbols;
}

#pragma endregion

#pragma region expressions
//с ифами все сложно пипец
vector<string> expressionIF = {};

void IF(int& i, vector<string>& leksika) {
    if (leksika[i] == "if") {
        while (leksika[i - 1] != ")") {
            expressionIF.push_back(leksika[i]);
            i += 1;
        }

        if (leksika[i] == "{") {
            while (leksika[i - 1] != "}") {
                if (leksika[i] == "if") IF(i, leksika);
                expressionIF.push_back(leksika[i]);
                i += 1;
            }
        }
        else {
            while (leksika[i - 1] != ";") {
                if (leksika[i] == "if") IF(i, leksika);
                expressionIF.push_back(leksika[i]);
                i += 1;
            }
        }

        if (leksika[i] == "else") {
            if (leksika[i] == "{") {
                while (leksika[i - 1] != "}") {
                    if (leksika[i] == "if") IF(i, leksika);
                    expressionIF.push_back(leksika[i]);
                    i += 1;
                }
            }
            else {
                while (leksika[i - 1] != ";") {
                    if (leksika[i] == "if") IF(i, leksika);
                    expressionIF.push_back(leksika[i]);
                    i += 1;
                }
            }

        }
    }
    return;
}

//разбиваем на выражения то есть от ; до ; // отдельно полностью выводим ифы
vector<vector<string>> Expressions = {};

void FirstAge(vector<string>& leksika) {
    if (leksika.size() != 0) {
        vector<string> expression = {};
        for (int i = 0; i < leksika.size();) {
            while (leksika[i] != ";") {
                if (leksika[i] == "if") {
                    IF(i, leksika);
                    expression.insert(expression.end(), expressionIF.begin(), expressionIF.end());
                }
                if (i >= leksika.size()) break;
                expression.push_back(leksika[i]);
                i += 1;
            }
            i += 1;
            Expressions.push_back(expression);
            expression = {};
        }
    }
}
#pragma endregion

// теперь для каждого выражения надо построить деревце
/* для этого:
    1. приоритеты
    2. разбитие на подвыражения
*/

//это класс для лексем
class node {
public:
    string data;
    node* left;
    node* right;

    node() {
        data = "";
        left = NULL, right = NULL;
    }
};

//это класс который содержит подвыражение в виде дерева
class BinaryTree {
public:
    node* root;
    vector<node*> stack;

    BinaryTree() {
        this->root = new node();
        this->stack = {};
    }

    //надо обозначить приоритеты операций и ключевых слов
    /*map<string, int> priority = {
        {"(", 4},
        {")", 4},
        {"+", 2},
        {"-", 2},
        {"*", 1},
        {"/", 1}
    };*/

    //это код со страницы https://aliev.me/runestone/Trees/ParseTree.html
    void trees(vector<string>& exp) {
        stack.push_back(root);
        node* current_node = root;

        for (string tepmStr : exp) {
            if (tepmStr == "(") {
                node* newNode = new node();
                current_node->left = newNode;
                stack.push_back(current_node);
                current_node = current_node->left;
            }
            else if (   tepmStr != "+" &&
                        tepmStr != "-" &&
                        tepmStr != "*" &&
                        tepmStr != "/" && 
                        tepmStr != ")") {
                current_node->data = tepmStr;
                node* parent = stack[stack.size() - 1]; stack.pop_back();
                current_node = parent;

            }
            else if (   tepmStr == "+" ||
                        tepmStr == "-" ||
                        tepmStr == "*" ||
                        tepmStr == "/") {
                current_node->data = tepmStr;
                node* newNode = new node();
                current_node->right = newNode;
                stack.push_back(current_node);
                current_node = current_node->right;

            }
            else if (tepmStr == ")") {
                current_node = stack[stack.size() - 1]; stack.pop_back();
            }

        }
    }
};


//есть много видов инструкций:
//1. объявления переменных
//2. присвоения значений
//3. специальн инструкции типа print
//4. условные инструкции
//5. циклы
//6. инструкции потока управления типа return, break
//7. блоки кода типа {...}
//то бишь надо 7 классов еще

vector<string> allVarsAndVals = {};

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
    else {
        for (string page : exp) {

        }
    }
    return;
}

#pragma region classes
//это классы для выражений
class setValInstr {
    setValInstr(int pos, vector<string>& str) {
        //сначала посмотрим надо ли вычислять то что после "="
        if (str.size() > 3) {

        }
        //если надо то...
        else {
            int i = 0;
            for (string temp : allVarsAndVals) {
                if (temp == str[pos]) {
                    cout << hex << i << endl;


                }
            }
        }
    }
};

class varDefInstr {
    varDefInstr(vector<string>& exp) {
        int pos = 0;
        for (string tempExp : exp) {
            for (string tempVariables : Variables) {
                if (tempExp == tempVariables) {
                    if (exp[pos + 1] == ",") {
                        //просто переходим к следующей переменной
                        pos += 1;
                    }

                    if (exp[pos + 1] == "=") {
                        //если происходит не только объявление но и определение то сначала считаем выражение справа и потом  делаем push
                        vector<string> tempVector = {};
                        for (int i = pos + 1; i < exp.size() && exp[i] != ",";) {
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

void print_Tree(node* p, int level){
    if (p)
    {
        print_Tree(p->left, level + 1);
        for (int i = 0; i < level; i++) cout << "   ";
        cout << p->data << endl;
        print_Tree(p->right, level + 1);
    }
}

int main() {
    string buffer = readFile("test.txt");

    /*cout << setfill('0') << setw(4) << uppercase << hex << 0 << ends << endl;
    cout << setfill('0') << setw(4) << uppercase << hex << 1 << ends << endl;
    cout << setfill('0') << setw(4) << uppercase << hex << 10 << ends << endl;
    cout << setfill('0') << setw(4) << uppercase << hex << 12 << ends << endl;*/

    //cout << buffer;

    vector<string> qwe = parseFile(buffer);

    FirstAge(qwe);

    allVarsAndVals.insert(allVarsAndVals.end(), Variables.begin(), Variables.end());
    allVarsAndVals.insert(allVarsAndVals.end(), Numbers.begin(), Numbers.end());

    /*int i = 0;
    for (string temp : allVarsAndVals) {
        cout << setfill('0') << setw(4) << uppercase << hex << i << ends << " :" << allVarsAndVals[i] << endl;
        i++;
    }*/

    vector<BinaryTree *> trees = {};
    for (vector<string> exp : Expressions) {
        BinaryTree* newTree = new BinaryTree();
        newTree->trees(exp);
        trees.push_back(newTree);
    }


    for (BinaryTree* temp : trees) {
        print_Tree(temp->root, 0);
    }


    return 0;

}
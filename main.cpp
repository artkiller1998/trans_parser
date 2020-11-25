#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <regex>
#include <iomanip>
#include <string>
#include "leks_rapse.h" // ������ ����� ( ��������� ������� ����� �� ������ )
#include "exp_parse.h" // ������ IF/WHILE � ������ ( ��������� ������� ����� �� ������ )
#include "help_funcs.h" // ������� ��� ������� ( ���� �� ������� � ������� )
#include "syntax.h" // ������ � �������
#include "check_syntax.h"
using namespace std;


int main(int argc, char *argv[]) {


	string input_filename = "";
	string output_filename = "";

	bool continuing = true;

	//������ ��������� ���������� ������

	for (size_t i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i],"--input")) {
			input_filename = argv[i + 1];
		}
		else if (!strcmp(argv[i], "--output"))
		{
			output_filename = argv[i + 1];
		}
	}

	if (input_filename == "") {
		input_filename = "input.txt";
	}
	if (output_filename == "")
	{
		output_filename = "output.txt";
	}

	GlobalCount = 0;
	string buffer = "";
	try
	{
		buffer = read_file(input_filename); //������ ������� ���� -> leks_rapse.h ("test.txt" - �������� �����)
	}
	catch (const std::exception&)
	{
		cout << "You must add file with code 'input.txt' near excecuting file translator.exe.\n"
			<< "You can add custom files in params: translator.exe --input u_input.txt --output u_output.txt\n\n";
		return 1;
	}
	

	cout  << "+	----- INPUT FILE:-----\n" 
		  << "	----------------------\n" 
		  << buffer << endl
		  << "-	----- INPUT FILE:-----\n"
		  << "	----------------------\n\n";

	vector<string> list_of_tokens = parse_string(buffer); // ������ ���� �� ������ ('var' 'a' '=' '5'... )

	//�������� �� ������
	make_expressions_for_CHECK_ERRORS(list_of_tokens);
	for (auto it = list_of_expressions.begin(); it != list_of_expressions.end(); it++)
	{
		if (check_syntax(*it) != "OK") continuing = false;
		cout << "Expression :  " << check_syntax(*it) << endl;
	}
	//���� ��� ��������� - ����������
	



	if (continuing) {
		cout << "+	----- TOKENS LIST:-----\n"
			<< "	----------------------\n";
		for (auto el : list_of_tokens)
		{
			cout << "'" << el << "'  ";
		}
		cout << endl;
		cout << "-	----- TOKENS LIST:-----\n"
			<< "	----------------------\n\n";

		list_of_vars_and_nums.insert(list_of_vars_and_nums.end(), list_of_vars.begin(), list_of_vars.end()); //? leks_rapse.h ��������� ����������
		list_of_vars_and_nums.insert(list_of_vars_and_nums.end(), list_of_numbers.begin(), list_of_numbers.end()); //? leks_rapse.h ��������� ����� 
																												   //� ����� ��� � 1 ������			

		Basic *basic = new Basic(); //������� ����� ���������� ������ � ���������
		make_expressions(list_of_tokens, basic); // �������� ���������  -> exp_parse.h  ( if {...} ) � ��������������� ���������� (������ break � continue ������ �������)
		second_translate(basic);  //������ ������� ��������� ����������� �������
		third_translate(basic);   //������� �������������� ������� (���� ���������� ������ ����� while)


		//�������� ���������� � ����
		ofstream finalfout;
		finalfout.open(output_filename);
		finalfout << setfill('0') << setw(4) << uppercase << hex << 0 << ends << ": "
					<< "Temp" << endl;

		for (int i = 0; i < list_of_vars_and_nums.size(); i++) {

			finalfout << setfill('0') << setw(4) << uppercase << hex << i + 1 << ends << ": "
				<< list_of_vars_and_nums[i] << endl;
		}

		finalfout << endl << endl;

		for (int i = 0; i < basic->commands.size(); i++) {
			finalfout << setfill('0') << setw(4) << uppercase << hex << i << ends << ": "
				<< basic->commands[i].first;
			if (basic->commands[i].second != 0)
				finalfout << setfill('0') << setw(4) << uppercase << hex << i + basic->commands[i].second << ends << endl;
			else
				finalfout << endl;
		}


		cout << "+	----- list_of_vars_and_nums:-----\n"
			<< "	----------------------\n";
		for (auto el : list_of_vars_and_nums)
		{
			cout << "'" << el << "'  ";
		}
		cout << endl;
		cout << "-	----- list_of_vars_and_nums:-----\n"
			<< "	----------------------\n\n";




		cout << "+	----- TREE:-----\n"
			<< "	----------------\n";

		Tree* test_tree = new Tree
		(parse_string((string)"A + C * B - 66 / 2")); // ����� ������ �� ������ � ��������� ������ � ������
		build_tree(test_tree); // ������ ������
		print_tree(test_tree, 0); // �������� ������, 0 - � ������ ���� ��������
	}
		



	return 0;

}

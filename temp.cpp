#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <unordered_map>
using namespace std;


int main()
{
	std::string path = "D:\\codefile\\cpp_code\\homework\\os\\file\\chen";
	std::string name = "\\tt.txt";
	std::string str;
	string file_path = path + name;
	if(remove(file_path.c_str()) == 0)
		cout << "jfie";
	// std::string str = "hello world!";
	// fs << str << endl;
	// fs.close();
	// fs.open(file_path);
	// fs.seekp(0, ios::end);
	// fs << str << endl;
	// fs.close();
	// fs.open(file_path);
	// while(!fs.eof())
	// {
	// 	fs >> str;
	// 	cout << str << endl;
	// }
	// fs.close();
	// remove(file_path.c_str());
	// std::string command("mkdir -p D:\\codefile\\cpp_code\\homework\\os\\file\\chen");
	// system(command.c_str());
	system("pause");
	return 0;
}
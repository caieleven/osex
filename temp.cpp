#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <unordered_map>
using namespace std;


int main()
{
	// std::string path = "D:\\codefile\\cpp_code\\homework\\os\\file\\chen";
	// std::string name = "\\tt.txt";
	// std::string str;
	// string file_path = path + name;
	// if(remove(file_path.c_str()) == 0)
	// 	cout << "jfie";
	std::string path = "D:\\codefile\\cpp_code\\homework\\os\\file\\chen\\kk.txt";
	ofstream tempin(path.c_str(), ios::app);
	path = "a";
	tempin << path;

	tempin.close();
	system("pause");
	return 0;
}
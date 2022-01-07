#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unordered_map>
using namespace std;

int iii = 2;
void func(int*& point)
{
	point = &iii;
}

void func2(int * point)
{
	point = &iii;
}

int main()
{
	std::string path = "D:\\codefile\\cpp_code\\homework\\os\\file\\";
	std::string name = "jj.txt";
	string file_path = path + name;
	cout << file_path;
	FILE *fp = nullptr;
	fp = fopen(file_path.c_str(), "w+");
	if(fp == nullptr)
		printf("jfje");
	fclose(fp);
	system("pause");
	return 0;
}
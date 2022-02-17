/*
 * @Author: Baorun Chen
 * @Date: 2022-02-17 22:53:48
 * @LastEditTime: 2022-02-17 23:30:44
 * @Description: 
 */
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>


class Attributes
{
public:
    Attributes(std::string &username, std::string &filename, int permissions) : perm(permissions)
    {
        path = base + username + "\\" + filename;
        //datasize大小计算待补充
    }
    Attributes()
    {
    }
    Attributes(const Attributes &Att)
    {
        this->path = Att.path;
        this->perm = Att.perm;
    }

public:
    std::string path;
    //文件权限 与Linux相同---rwx=1-2-4
    //暂时只支持1, 2, 3
    int perm;
};

//mode + stream
typedef std::pair<std::string, std::fstream *> StreamAtt; //mode + stream


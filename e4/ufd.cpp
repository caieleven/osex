/*
 * @Author: Baorun Chen
 * @Date: 2022-02-17 23:06:16
 * @LastEditTime: 2022-02-17 23:31:20
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
#include "ufd.h"

bool UFD::GetStream(std::string &filename, std::string &mode, std::fstream *&stream)
{
    //初始化为空
    stream = nullptr;
    Attributes attb = file_map_[filename];
    if (mode == "open")
    {
        if (attb.perm == 1)
            mode = "r";
        else if (attb.perm == 2)
            mode = "w";
        else
            mode = "rw";
    }
    if (mode == "r" && attb.perm != 2)
        stream = new std::fstream(attb.path, std::ios::in);
    else if (mode == "w" && attb.perm >= 2)
        stream = new std::fstream(attb.path, std::ios::out);
    else if (mode == "rw" && attb.perm == 3)
        stream = new std::fstream(attb.path, std::ios::in | std::ios::out);
    else if (mode == "a" && attb.perm >= 2)
    {
        stream = new std::fstream(attb.path, std::ios::out);
        stream->seekp(0, std::ios::end);
    }
    if (stream != nullptr)
        return true;
    return false;
}

bool UFD::CreateNewFile(std::string &username, std::string &filename, std::string &mode, int permission, std::fstream *&stream)
{
    stream = nullptr;
    //加入到当前ufd中
    // Attributes att(username, filename, permission);
    // file_map_.insert({filename, att});
    this->Insert(username, filename, permission);
    //新建文件
    std::fstream newstream(file_map_[filename].path, std::ios::out);
    newstream.close();
    //将此更改写入对应用户的配置文件
    newstream.open(base + username + "\\" + "file.config", std::ios::app);
    newstream << filename << "\t" << permission << std::endl;
    newstream.close();
    //获取stream，并修改mode
    // mode = "open";
    // this->GetStream(filename, mode, stream);
    // //std::cout << att.path << std::endl;
    // // if (file_map_.find("aa.txt") != file_map_.end())
    // //     std::cout << "插入成功" << std::endl;

    if (stream != nullptr)
        return true;
    return false;
}

bool UFD::DeleteFile(std::string &filename)
{
    bool flag = (remove(file_map_[filename].path.c_str()) == 0);
    file_map_.erase(filename);
    return flag;
}

void UFD::Insert(std::string &username, std::string &filename, int permission)
{
    Attributes attb(username, filename, permission);
    file_map_.emplace(filename, attb);
    std::cout << "当前ufd大小=" << file_map_.size() << std::endl;
}

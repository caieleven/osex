/*
 * @Author: Baorun Chen
 * @Date: 2022-02-17 23:08:10
 * @LastEditTime: 2022-02-17 23:24:03
 * @Description: 
 */


#include "filemanager.h"

FileManager::FileManager()
{
    std::ifstream userstream("D:\\codefile\\cpp_code\\homework\\os\\file\\Users.config");
    std::ifstream one_user;
    std::string username, filepath, filename;
    int permission;
    while (!userstream.eof())
    {
        //读取User.config中的用户
        username.clear();
        filepath.clear();
        userstream >> username;
        filepath = base + username + "\\" + "file.config";
        one_user.open(filepath);
        UFD ufd;
        //对每一个用户，读取其config文件，创建ufd对象
        while (!one_user.eof())
        {
            filename.clear();
            one_user >> filename;
            if (filename.length() == 0)
                break;
            //std::cout << filename << std::endl;
            one_user >> permission;
            ufd.Insert(username, filename, permission);
        }
        MFD_.emplace(username, ufd);
        one_user.close();
    }
    userstream.close();
}

void FileManager::Main()
{
    std::string username;
    FileOperateMode mode;
    std::string filename;
    //输入并检测用户名
    do
    {
        printf("请输入用户名:");
        getline(std::cin, username);
        //scanf("%s", username.c_str());
    } while (!CheckUser(username));

    //执行操作
    while (1)
    {
        //读取并解析命令
        std::string command;
        do
        {
            command.clear();
            printf("请输入要执行的操作:");
            //getline(std::cin, command);
            std::cin >> command >> filename;
        } while (!ParseAndCheckCommand(command, mode));
        //执行命令
        Operator(username, filename, mode);
        printf("是否继续使用?(y or n)");
        char iscontinue;
        std::cin >> iscontinue;
        if (iscontinue == 'y')
            continue;
        else
            break;
    }
}

void FileManager::Operator(std::string &user, std::string &filename, FileOperateMode &mode)
{
    //提取UFD
    UFD &ufd = MFD_.at(user);
    bool isexist = ufd.IsExist(filename);
    int perm;
    std::fstream *stream = nullptr;
    std::string file_open_mode, str;
    switch (mode)
    {
        //创建新文件,默认用w+打开文件,得到文件句柄
    case FileOperateMode::Create:
    {
        if (isexist)
        {
            printf("同名文件已经存在!\n");
            return;
        }
        printf("请输入文件权限:");
        std::cin >> perm;
        file_open_mode = "w"; //创建一个文件只写
        ufd.CreateNewFile(user, filename, file_open_mode, perm, stream);
        //openfiles_.insert({filename, StreamAtt(file_open_mode, stream)});
        printf("创建文件成功\n");
        break;
    }
    case FileOperateMode::Delete:
    {
        if (!isexist)
        {
            printf("文件不存在!\n");
            return;
        }
        if (ufd.DeleteFile(filename))
            printf("删除成功!\n");
        else
            printf("删除失败\n");
        break;
    }
    case FileOperateMode::Open:
    {
        if (!isexist)
        {
            printf("文件不存在!\n");
            return;
        }
        file_open_mode = "open";
        if (ufd.GetStream(filename, file_open_mode, stream))
        {
            printf("打开成功\n");
            openfiles_.emplace(filename, StreamAtt(file_open_mode, stream));
        }
        else
        {
            printf("打开失败\n");
        }
        break;
    }
    case FileOperateMode::Close:
    {
        if (openfiles_.find(filename) != openfiles_.end())
        {
            openfiles_[filename].second->close();
            delete openfiles_[filename].second;
            printf("关闭成功\n");
            openfiles_.erase(filename);
        }
        break;
    }
    case FileOperateMode::Read:
    {
        if (!isexist)
        {
            printf("文件不存在!\n");
            return;
        }
        file_open_mode = "r";
        if (openfiles_.find(filename) == openfiles_.end())
        {
            ufd.GetStream(filename, file_open_mode, stream);
            openfiles_.emplace(filename, StreamAtt(file_open_mode, stream));
        }
        stream = openfiles_[filename].second;
        while (std::getline(*stream, str))
            std::cout << str << std::endl;
        break;
    }
    case FileOperateMode::Write:
    {
        if (!isexist)
        {
            printf("文件不存在!\n");
            return;
        }
        file_open_mode = "w";
        if (openfiles_.find(filename) == openfiles_.end())
        {
            ufd.GetStream(filename, file_open_mode, stream);
            openfiles_.emplace(filename, StreamAtt(file_open_mode, stream));
        }
        stream = openfiles_[filename].second;
        printf("请输入要写入的内容(换行符结尾)\n");
        getchar();
        std::getline(std::cin, str);
        (*stream) << str << std::endl;
        printf("写入成功\n");
        break;
    }
    case FileOperateMode::Append:
    {
        if (!isexist)
        {
            printf("文件不存在!\n");
            return;
        }
        file_open_mode = "a";
        if (openfiles_.find(filename) == openfiles_.end())
        {
            ufd.GetStream(filename, file_open_mode, stream);
            openfiles_.emplace(filename, StreamAtt(file_open_mode, stream));
        }
        stream = openfiles_[filename].second;
        printf("请输入要写入的内容（换行符结尾）:\n");
        std::getline(std::cin, str);
        (*stream) << str << std::endl;
        printf("写入成功\n");
        break;
    }
    default:
        break;
    }
}

bool FileManager::CheckUser(std::string &username)
{
    return MFD_.find(username) == MFD_.end() ? false : true;
}

bool FileManager::ParseAndCheckCommand(std::string &command, FileOperateMode &mode)
{
    int flag = true;
    if (command == "create")
        mode = FileOperateMode::Create;
    else if (command == "delete")
        mode = FileOperateMode::Delete;
    else if (command == "open")
        mode = FileOperateMode::Open;
    else if (command == "close")
        mode = FileOperateMode::Close;
    else if (command == "read")
        mode = FileOperateMode::Read;
    else if (command == "write")
        mode = FileOperateMode::Write;
    else if (command == "Append")
        mode = FileOperateMode::Append;
    else
    {
        printf("未识别的命令!\n");
        flag = false;
    }
    return flag;
}



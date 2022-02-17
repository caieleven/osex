/*
 * @Author: Baorun Chen
 * @Date: 2022-02-17 21:04:20
 * @LastEditTime: 2022-02-17 21:07:14
 * @Description: OS2  实验4 文件管理系统
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

std::string base = "D:\\codefile\\cpp_code\\homework\\os\\file\\";

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

class UFD
{
public:
    bool IsExist(std::string filename)
    {
        return !(file_map_.find(filename) == file_map_.end());
    }
    /**
     * @brief 通过检查mode(欲打开方式)与文件权限是否匹配,若匹配,得到文件读写句柄
     *       注意：前提是文件已经存在
     * @param filename 文件名
     * @param mode 打开文件的方式,可能被修改，修改后为当前返回的流对象支持的方式
     * @param stream 文件的读写句柄,是调用者想要得到的信息
     * @return 是否成功
     */
    bool GetStream(std::string &filename, std::string &mode, std::fstream *&stream);
    bool CreateNewFile(std::string &username, std::string &filename, std::string &mode, int permission, std::fstream *&stream);
    bool DeleteFile(std::string &filename);
    void Insert(std::string &username, std::string &filename, int permission);

private:
    std::unordered_map<std::string, Attributes> file_map_; //文件名+文件属性
};

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
    mode = "open";
    this->GetStream(filename, mode, stream);
    //std::cout << att.path << std::endl;
    // if (file_map_.find("aa.txt") != file_map_.end())
    //     std::cout << "插入成功" << std::endl;

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

enum class FileOperateMode : int
{
    Create = 0,
    Delete = 1,
    Open = 2,
    Close = 3,
    Read = 4,
    Write = 5,
    Append = 6
};

//一次只允许一个用户，每个用户退出时，清空打开文件目录
//在Main()中得到用户名，循环操作，直到退出。
//每次操作格式 command file_name
class FileManager
{
public:
    FileManager();
    void Main();
    void Operator(std::string &user, std::string &filename, FileOperateMode &mode);

private:
    bool end_;
    /**
     * @brief 查看用户是否存在
     * @param username 用户名-引用
     * @return 如果存在,返回true
     */
    bool CheckUser(std::string &username);

    /**
     * @brief 解析命令，将解析结果用引用的形式传递给mode；检查命令是否合法、
     * @param command 命令的string格式,即读取的原格式
     * @param mode 转换为FileOperateMode格式
     * @return 如果命令可识别,返回true
     */
    bool ParseAndCheckCommand(std::string &command, FileOperateMode &mode);
    std::unordered_map<std::string, UFD> MFD_;   //username + ufd
    std::map<std::string, StreamAtt> openfiles_; //filename + mode + stream
};

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
        openfiles_.insert({filename, StreamAtt(file_open_mode, stream)});
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

// 打开文件目录：打开文件名，打开保护码，读写指针
// MFD可以看到所有文件

int main()
{
    FileManager fm;
    fm.Main();
    system("pause");
    return 0;
}

/**
 * Users.config格式：username
 * {user}.config格式：filename, permission
 */


/*

* create 创建文件、给出权限


 */


/*
    in不创建 out与app创建

 */

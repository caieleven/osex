/*
 * @Author: Baorun Chen
 * @Date: 2022-01-04 20:36:44
 * @LastEditTime: 2022-01-07 09:00:21
 * @Description: OS2  实验4 文件管理系统
 */
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
    Attributes(std::string &filename, int permissions) : perm(permissions)
    {
        std::string base = "D:\\codefile\\cpp_code\\homework\\os\\file\\";
        path = base + filename;
        datasize = 0;
        //datasize大小计算待补充
    }
    Attributes(const Attributes &Att)
    {
        this->path = Att.path;
        this->perm = Att.perm;
        this->datasize = Att.datasize;
    }

public:
    std::string path;
    int datasize;
    //文件权限 与Linux相同---rwx=1-2-4
    int perm;
};

typedef std::pair<std::string, FILE *> StreamAtt;

class UFD
{
public:
    bool IsExist(std::string filename)
    {
        return !(file_map_.find(filename) == file_map_.end());
    }
    /**
     * @brief 通过检查mode(欲打开方式)与文件权限是否匹配,若匹配,得到文件读写句柄
     * @param filename 文件名
     * @param mode 打开文件的方式
     * @param stream 文件的读写句柄,是调用者想要得到的信息
     * @return 是否成功
     */
    bool GetStream(std::string &filename, std::string &mode, FILE *&stream);
    bool CreateNewFile(std::string &filename, std::string &mode, int permission, FILE *&stream);
    bool DeleteFile(std::string &filename);

private:
    //std::set<std::string> file_set_;
    std::unordered_map<std::string, Attributes> file_map_;
};

bool UFD::GetStream(std::string &filename, std::string &mode, FILE *&stream)
{
    stream = nullptr;
    stream = fopen(file_map_.at(filename).path.c_str(), mode.c_str());
    if (stream != nullptr)
        return true;
    return false;
}

bool UFD::DeleteFile(std::string &filename)
{
    return (remove(file_map_.at(filename).path.c_str()) == 0);
}

bool UFD::CreateNewFile(std::string &filename, std::string &mode, int permission, FILE *&stream)
{
    stream = nullptr;
    Attributes att(filename, permission);
    file_map_.insert({filename, att});
    stream = fopen(file_map_.at(filename).path.c_str(), mode.c_str());
    if (stream != nullptr)
        return true;
    return false;
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
     * @brief 解析命令,同时检查命令是否合法
     * @param command 命令的string格式,即读取的原格式
     * @param mode 转换为FileOperateMode格式
     * @return 如果命令可识别,返回true
     */
    bool ParseAndCheckCommand(std::string &command, FileOperateMode &mode);
    std::unordered_map<std::string, UFD> MFD_;
    std::map<std::string, StreamAtt> openfiles_;
};


FileManager::FileManager()
{

}

void FileManager::Main()
{
    std::string username;
    FileOperateMode mode;
    std::string filename;
    do
    {
        printf("请输入用户名:");
        getline(std::cin, username);
        //scanf("%s", username.c_str());
    } while (!CheckUser(username));
    while (1)
    {
        std::string command;
        do
        {
            command.clear();
            printf("请输入要执行的操作:");
            //getline(std::cin, command);
            std::cin >> command >> filename;
        } while (!ParseAndCheckCommand(command, mode));
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
    UFD &ufd = MFD_.at(user);
    bool isexist = ufd.IsExist(filename);
    int perm;
    FILE *fp = nullptr;
    std::string file_open_mode;
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
        ufd.CreateNewFile(filename, file_open_mode, perm, fp);
        openfiles_.insert({filename, StreamAtt(file_open_mode, fp)});
        printf("创建文件成功\n");
    }
    case FileOperateMode::Delete:
    {
        if (!isexist)
        {
            printf("文件不存在!\n");
            return;
        }
        ufd.DeleteFile(filename);
        printf("删除成功!\n");
    }
    case FileOperateMode::Open:
    {
    }
    case FileOperateMode::Close:
    {
    }
    case FileOperateMode::Read:
    {
    }
    case FileOperateMode::Write:
    {
    }
    case FileOperateMode::Append:
    {
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

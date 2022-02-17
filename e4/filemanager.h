/*
 * @Author: Baorun Chen
 * @Date: 2022-02-17 23:07:45
 * @LastEditTime: 2022-02-17 23:08:56
 * @Description: 
 */

#include "attributes.h"
#include "ufd.h"


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

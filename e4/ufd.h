/*
 * @Author: Baorun Chen
 * @Date: 2022-02-17 23:03:39
 * @LastEditTime: 2022-02-17 23:09:07
 * @Description: 
 */

#include "attributes.h"


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



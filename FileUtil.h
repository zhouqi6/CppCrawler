//
// Created by zhouqi on 2023/9/6.
//

#ifndef CPPCRAWLER_FILEUTIL_H
#define CPPCRAWLER_FILEUTIL_H
#include "string"

class FileUtil {
public:
    std::string getMD5(const std::wstring&);
    void deleteAllFileWithMD5(const std::wstring&,const std::string&);
};


#endif //CPPCRAWLER_FILEUTIL_H

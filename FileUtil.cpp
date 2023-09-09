//
// Created by zhouqi on 2023/9/6.
//

#include "FileUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>

using namespace std;
void FileUtil::deleteAllFileWithMD5(const std::wstring &, const std::string &) {

}

std::string FileUtil::getMD5(const std::wstring & filePath_W) {
//    wifstream file(filePath_W, ios::binary);
//    if (!file) {
//        wcerr << L"Failed to open file: " << filePath_W << endl;
//        return "";
//    }
//
//    // 读取文件内容到缓冲区
//    const int BUFFER_SIZE = 1024;
//    char buffer[BUFFER_SIZE];
//    MD5_CTX md5;
//    MD5_Init(&md5);
//    while (file.read(buffer, BUFFER_SIZE)) {
//        MD5_Update(&md5, buffer, file.gcount());
//    }
//    MD5_Final(reinterpret_cast<unsigned char*>(buffer), &md5);
//
//    // 将哈希值转换为字符串
//    stringstream ss;
//    for (unsigned char byte : buffer) {
//        ss << hex << (int)byte;
//    }
//    return ss.str();
    return "";
}

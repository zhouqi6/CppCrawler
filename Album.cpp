//
// Created by zhouqi on 2023/9/2.
//

#include "Album.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <codecvt>
#include <filesystem>
#include "CurlExcutor.h"

using namespace std;
Album::Album(const std::string &url) {
    albumCurrentPageUrl = std::move(url);
    albumFirstPageUrl = albumCurrentPageUrl;
    pageTotalNum = 0;
    currentPagePos = 0;
}
Album::Album(std::string &&url) {
    albumCurrentPageUrl = std::move(url);
    albumFirstPageUrl = albumCurrentPageUrl;
    pageTotalNum = 0;
    currentPagePos = 0;
}

void replaceIllegalPath(string& path){
    // 将非法字符替换为点号
    for (char& c : path) {
        if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '.';
        }
    }
}
CURLcode Album::downloadAlbumCurrentPage() {
    page.clear();
    CURLcode code = CurlExcutor::getRequest(albumCurrentPageUrl.c_str(), nullptr, &page);
    if(page.response == nullptr){
        albumCurrentPage = "";
    }else{
        albumCurrentPage = page.response;
        page.clear();
    }
    return code;
}

Album::~Album() {

}

const string invalidTitle = "invalid title";
bool Album::parseTitle() {

    std::string titlePattern = "<title>(.*)- E-Hentai Galleries</title>";
    regex regexStr(titlePattern);

    // 创建一个迭代器，用于迭代匹配的子字符串
    std::sregex_iterator it(albumCurrentPage.begin(), albumCurrentPage.end(), regexStr);
    std::sregex_iterator end;
    while (it != end) {
        string && title = it->str();

        if (title.length() < 37) {
            cout << "title is too short\n";
        }else{
            std::string&& subStr = title.substr(7, title.length() - 36);
//            std::cout << title << std::endl; // 输出匹配的子字符串
            std::cout << subStr << std::endl; // 输出匹配的子字符串
            albumTitle = subStr;
            replaceIllegalPath(albumTitle);
            return true;
        }

        ++it; // 移动到下一个匹配的子串
    }
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(now);
    tm utc_time = *gmtime(&t);

    // 格式化时间字符串
    std::ostringstream oss;
    oss << std::put_time(&utc_time, "%Y-%m-%d %H:%M:%S");
    albumTitle = invalidTitle+oss.str();
    return false;

}

bool Album::parsePicturePageUrls() {
    regex regexStr("https://e-hentai.org/s/[a-z0-9]+/[0-9-]+");
    // 创建一个迭代器，用于迭代匹配的子字符串
    std::sregex_iterator it(albumCurrentPage.begin(), albumCurrentPage.end(), regexStr);
    std::sregex_iterator end;

    while (it != end) {
        albumPicturePageUrls.emplace_back(it->str());
        //std::cout << it->str() << std::endl; // 输出匹配的子字符串

        ++it; // 移动到下一个匹配的子串
    }
    //std::cout << "found pictureUrls size:" << albumPicturePageUrls.size() << std::endl; // 输出匹配的子字符串
    return !albumPicturePageUrls.empty();
}

bool validPageCount(const string &  str){
    for(char c:str){
        if(c==';'){
            return false;
        }
    }
    return true;
}
bool Album::getPageCount() {
    std::string pageNumPattern = "onclick=\"return false\">[^<]*</a>";
    regex regexStr(pageNumPattern);

    // 创建一个迭代器，用于迭代匹配的子字符串
    std::sregex_iterator it(albumCurrentPage.begin(), albumCurrentPage.end(), regexStr);
    std::sregex_iterator end;
    while (it != end) {
        std::sregex_iterator last = it;
        auto&& temp = it->str();
        if(++last != end && validPageCount(last->str())){
            ++it; // 移动到下一个匹配的子串
        }else{
            break;
        }
    }
    if (it == end) {
        return false;
    }
    string && pageSumStr = it->str();

    if (pageSumStr.length() < 28) {
        std::cout << "pageSumStr is too short\n";
        return false;
    }else{
        std::string&& subStr = pageSumStr.substr(23, pageSumStr.length() - 27);
        //std::cout << "album page count" << subStr << std::endl; // 输出匹配的子字符串
        stringstream stream;
        stream<<subStr;
        stream >> pageTotalNum;
        return true;
    }
}

bool Album::setAndMakeAlbumDir(const string& localPath) {
    string && folderStr = (localPath+"\\"+albumTitle);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    albumFolderStr_W = converter.from_bytes(folderStr);
    std::filesystem::path dir(albumFolderStr_W);
    std::error_code ec;
    // 创建文件夹
    if (std::filesystem::create_directory(dir,ec)) {
        return true;
    } else {
        SetConsoleOutputCP(936);
        cout << "create_directory failed:" << ec.message() << endl;
        SetConsoleOutputCP(CP_UTF8);
        return false;
    }
}

bool Album::toNextPosUrl() {
    if(currentPagePos++<pageTotalNum){
        stringstream stream;
        stream<<albumFirstPageUrl<<"?p="<<currentPagePos;
        albumCurrentPageUrl = std::move(stream.str());
        return true;
    }else{
        return false;
    }
}

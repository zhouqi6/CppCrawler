#include <iostream>
#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include "CurlCallbacks.h"
#include "CurlExcutor.h"
#include <regex>
#include <sstream>
#include <windows.h>
#include <strsafe.h>
#include <filesystem>
#include <future>
#include "thread"
#include "Album.h"

using namespace std;
vector<string> parsePageAndGetAlbumUrls(html& pageWithAlbums){
    vector<string> albums;
    string response = pageWithAlbums.response;
    //regex regexStr("<a href=\"https://e-hentai.org/g/[0-9]+/[a-zA-Z0-9]+/\">");
    regex regexStr("https://e-hentai.org/g/[0-9]+/[a-zA-Z0-9]+/");
    // 创建一个迭代器，用于迭代匹配的子字符串
    std::sregex_iterator it(response.begin(), response.end(), regexStr);
    std::sregex_iterator end;

    while (it != end) {
        albums.emplace_back(it->str());
        //std::cout << it->str() << std::endl; // 输出匹配的子字符串

        ++it; // 移动到下一个匹配的子串
    }
    std::cout << "found albums size:" << albums.size() << std::endl; // 输出匹配的子字符串
    return albums;
}
vector<string> getPictureDownloadUrls(const std::string& albumPage){
    vector<string> pictureUrls;
    regex regexStr("<img id=\"img\" src=\"([^\"]*)\"");
    // 创建一个迭代器，用于迭代匹配的子字符串
    std::sregex_iterator it(albumPage.begin(), albumPage.end(), regexStr);
    std::sregex_iterator end;

    while (it != end) {
        if (it->str().length() < 22) {
            std::cout << "pictureUrl is too short\n";
        }else{
            std::string&& subStr = it->str().substr(19, it->str().length() - 20);
//            std::cout << subStr << std::endl; // 输出匹配的子字符串
            pictureUrls.emplace_back(subStr);
        }

        ++it; // 移动到下一个匹配的子串
    }
    //std::cout << "getPictureDownloadUrls size:" << pictureUrls.size() << std::endl; // 输出匹配的子字符串
    return pictureUrls;
}
bool downloadPicture(string& pictureUrl, const wstring& albumFolder, size_t count){
    html imagePage;
    memset(&imagePage, 0, sizeof(html));
    CurlExcutor::getRequest(pictureUrl.c_str(), nullptr, &imagePage);
    if(imagePage.response== nullptr){
        return false;
    }
    string &&imagePageStr = imagePage.response;
    vector<string> pictureDownloadUrls = getPictureDownloadUrls(imagePageStr);
    if(pictureDownloadUrls.size()>0){
        // 创建一个临时文件用于保存图片数据
        wstringstream wStream;
        wStream << albumFolder << L"\\" << count << L".jpg";
        FILE *temp_file = _wfopen(wStream.str().c_str(), L"wb");
        if (temp_file == nullptr) {
            cout<<"_wfopen failed :"<<strerror(errno)<<" in func:"<< __FUNCTION__ <<endl;
        }else{
            CurlExcutor::getRequest(pictureDownloadUrls[0].c_str(), nullptr, temp_file);
            fclose(temp_file);
        }
    }
    return true;
}
bool albumDownload(const string& albumUrl,const string& localRootPath){

    int maxConcurrentPicNum = 20;
    int pictureWorkWaitInterval = 4;
    Album album(albumUrl);
    do {
        album.downloadAlbumCurrentPage();
        album.parsePicturePageUrls();
        if(album.pageTotalNum <= 0){
            album.parseTitle();
            album.getPageCount();
            bool makeDirSucceeded = album.setAndMakeAlbumDir(localRootPath);
            if(!makeDirSucceeded){
                return false;
            }
        }
    }while(album.toNextPosUrl());

    vector<std::future<bool>> pictureDownloadResults;
    for(size_t i = 0;i<album.albumPicturePageUrls.size();){
        if(pictureDownloadResults.size() >= maxConcurrentPicNum){
            for(size_t j=0;j<pictureDownloadResults.size();++j){
                if (pictureDownloadResults[j].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    //std::cout << "pictureDownload finished execution, set work "<<i<< " now\n";
                    pictureDownloadResults[j] = std::async(std::launch::async, downloadPicture,ref(album.albumPicturePageUrls[i]),album.albumFolderStr_W, i);
                    ++i;
                    break;
                } else {
                    continue;
                }
            }
            // 并发的图片下载任务数达到maxConcurrentPicNum，睡眠图片集下载任务线程
            // 等待任务执行pictureWorkWaitInterval秒后再次查询任务状态
            std::this_thread::sleep_for(std::chrono::seconds(pictureWorkWaitInterval));
        }else{
            pictureDownloadResults.emplace_back(std::async(std::launch::async, downloadPicture,ref(album.albumPicturePageUrls[i]),album.albumFolderStr_W, i));
            ++i;
        }
    }

    return true;
}

int main1()
{
    //单线程下载相册，每个相册多线程下载
    SetConsoleOutputCP(CP_UTF8);
    string localPath = R"(D:\P\Image\eHentai)";
    html albumListPage;
    memset(&albumListPage, 0, sizeof(html));
    const char* eHentaiUrl = "https://e-hentai.org/popular";
    CurlExcutor::getRequest(eHentaiUrl, nullptr, &albumListPage);
    vector<string>&& albumUrls = parsePageAndGetAlbumUrls(albumListPage);
    vector<thread> albumThreads;
    for(size_t i=0;i<albumUrls.size();++i){
        string &albumUrl =albumUrls[i];
        Album album(albumUrl);
        do {
            album.downloadAlbumCurrentPage();
            album.parsePicturePageUrls();
            if(album.pageTotalNum <= 0){
                album.parseTitle();
                album.getPageCount();
                album.setAndMakeAlbumDir(localPath);
            }
        }while(album.toNextPosUrl());

        vector<thread> pictureThreads;
        for(size_t i = 0;i<album.albumPicturePageUrls.size();++i){
            if(i%5==0){
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
            pictureThreads.emplace_back(downloadPicture,album.albumPicturePageUrls[i],album.albumFolderStr_W, i);
        }
        for(auto& pictureThread:pictureThreads){
            pictureThread.join();
        }
    }

    return 0;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    string localPath = R"(D:\P\Image\eHentai)";
    html albumListPage;
    memset(&albumListPage, 0, sizeof(html));
    int maxConcurrentAlbumNum = 10;
    int albumNumWorkWaitInterval = 10;
    const char* eHentaiUrl = "https://e-hentai.org/popular";
    CurlExcutor::getRequest(eHentaiUrl, nullptr, &albumListPage);
    vector<string>&& albumUrls = parsePageAndGetAlbumUrls(albumListPage);
    vector<std::future<bool>> albumDownloadResults;
    for(size_t i=0;i<albumUrls.size();){
        string &albumUrl =albumUrls[i];
        if(albumDownloadResults.size() >= maxConcurrentAlbumNum){
            for(size_t j=0;j<albumDownloadResults.size();++j){
                if (albumDownloadResults[j].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    std::cout << "albumDownload finished execution, set work "<<i<< " now\n";
                    albumDownloadResults[j] = std::async(std::launch::async, albumDownload,albumUrls[i],localPath);
                    ++i;
                    break;
                } else {
                    continue;
                }
            }
            // 并发的图片集任务数达到maxConcurrentAlbumNum，睡眠主线程
            // 等待任务执行albumNumWorkWaitInterval秒后再次查询任务状态
            std::this_thread::sleep_for(std::chrono::seconds(albumNumWorkWaitInterval));
        }else{
            albumDownloadResults.emplace_back(std::async(std::launch::async, albumDownload,albumUrls[i],localPath));
            ++i;
        }
    }

    return 0;
}
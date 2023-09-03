//
// Created by zhouqi on 2023/9/2.
//

#ifndef CPPCRAWLER_ALBUM_H
#define CPPCRAWLER_ALBUM_H

#include <curl/curl.h>
#include <vector>
#include "string"
#include "CurlCallbacks.h"

class Album{
private:
    std::string albumCurrentPageUrl;
    std::string albumFirstPageUrl;
    html page;
public:
    explicit Album(const std::string&);
    explicit Album(std::string&&);
    virtual ~Album();
    bool parseTitle();
    bool getPageCount();
    CURLcode downloadAlbumCurrentPage();
    bool parsePicturePageUrls();
    bool toNextPosUrl();
    bool setAndMakeAlbumDir(const std::string& localPath);
    std::string albumCurrentPage;
    std::string albumTitle;
    std::vector<std::string> albumPicturePageUrls;
    std::wstring albumFolderStr_W;
    int pageTotalNum;
    int currentPagePos;
};


#endif //CPPCRAWLER_ALBUM_H

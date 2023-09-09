//
// Created by zhouqi on 2023/9/2.
//

#ifndef CPPCRAWLER_CURLEXCUTOR_H
#define CPPCRAWLER_CURLEXCUTOR_H

#include <curl/curl.h>
#include "CurlCallbacks.h"
#include "xstring"
class CurlExcutor{
public:
    static CURLcode getRequest(const char *url, char *cookie, html* content);
    static CURLcode getRequest(const char *url, char *cookie, const std::wstring& filePath);
    static void setProxy(CURL* curl);
};


#endif //CPPCRAWLER_CURLEXCUTOR_H

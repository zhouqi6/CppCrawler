//
// Created by zhouqi on 2023/9/2.
//

#include "CurlExcutor.h"
#include "CurlCallbacks.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdio>

using namespace std;
CURLcode CurlExcutor::getRequest(const char *url, char *cookie, FILE *file) {
    char strHeader[2048] = {0};

    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5); // 重定向次数
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//设置301、302跳转跟随location

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 120);// 超时10秒

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlCallbacks::fileWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CurlCallbacks::header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, strHeader);
    setProxy(curl);
    if (cookie)
    {
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie);
    }

    if (0 == strncmp(url, "https:", 6))
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }
    CURLcode code = CURLE_AGAIN;
    int retryCount = 0;
    int masRetry = 5;
    int retryInterval = 3;
    while(retryCount++ <masRetry && (code = curl_easy_perform(curl)) != CURLE_OK) {
        cout << "curl_easy_perform failed, retry time:" << retryCount << endl;
        // 清除文件内容
//        fseek(file, 0, SEEK_END);
//        ftruncate(fileno(file), 0);
        // 睡眠当前线程
        std::this_thread::sleep_for(std::chrono::seconds(retryInterval*retryCount));
    }
    if(retryCount>=masRetry&&code!=CURLE_OK){

        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(code));
        long http_code = 0;
        /* 获取HTTP状态码 */
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_code);
        printf("HTTP status code: %ld\n", http_code);
        cout << "headers:" << strHeader << endl;
    }
    if (curl)
        curl_easy_cleanup(curl);
    return code;
}

CURLcode CurlExcutor::getRequest(const char *url, char *cookie, html *content) {
    char strHeader[2048] = {0};

    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5); // 重定向次数
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//设置301、302跳转跟随location

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 120);// 超时 秒

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlCallbacks::htmlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, content);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CurlCallbacks::header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, strHeader);
    setProxy(curl);
    if (cookie)
    {
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie);
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie);
    }

    if (0 == strncmp(url, "https:", 6))
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }

    CURLcode code = CURLE_AGAIN;
    int retryCount = 0;
    int masRetry = 5;
    int retryInterval = 3;
    while(retryCount++ <masRetry && (code = curl_easy_perform(curl)) != CURLE_OK) {
        cout << "curl_easy_perform failed, retry time:" << retryCount << endl;
        // 睡眠当前线程
        std::this_thread::sleep_for(std::chrono::seconds(retryInterval*retryCount));
    }
    if(retryCount>=masRetry&&code!=CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(code));
        long http_code = 0;
        /* 获取HTTP状态码 */
        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_code);
        printf("HTTP status code: %ld\n", http_code);
        cout << "headers:" << strHeader << endl;
        if(content->response){
            cout<<"body:"<<content->response<<endl;
        }
    }
    if (curl)
        curl_easy_cleanup(curl);
    return code;
}

void CurlExcutor::setProxy(CURL *curl) {
    const char * clashProxy = "127.0.0.1:7890";
    curl_easy_setopt(curl, CURLOPT_PROXY, clashProxy);
    //设置http的代理类型：
    curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
    //若是https请求，设置通道：
    //curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
    //若需要代理用户密码：
    //curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, "user:password");
}

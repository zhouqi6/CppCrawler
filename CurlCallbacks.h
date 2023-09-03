//
// Created by zhouqi on 2023/9/1.
//

#ifndef CPPCRAWLER_CURLCALLBACKS_H
#define CPPCRAWLER_CURLCALLBACKS_H

#include <cstdlib>
#include <cstring>

struct html{
    html();
    ~html();
    char *response;
    size_t size;
    void clear();
};
class CurlCallbacks {
public:
    static size_t htmlWriteCallback(void *data, size_t size, size_t nmemb, void *clientp);
    static size_t fileWriteCallback(void *data, size_t size, size_t nmemb, void *clientp);
    static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata);
};


#endif //CPPCRAWLER_CURLCALLBACKS_H

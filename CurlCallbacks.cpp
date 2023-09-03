//
// Created by zhouqi on 2023/9/1.
//

#include <cstdio>
#include <iostream>
#include "CurlCallbacks.h"

using namespace std;
size_t CurlCallbacks::htmlWriteCallback(void *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = size * nmemb;
    struct html *mem = (struct html *)clientp;

    char *ptr = static_cast<char *>(realloc(mem->response, mem->size + realsize + 1));
    if(ptr == nullptr)
        return 0;  /* out of html! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

size_t CurlCallbacks::header_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
    /* 'userdata' is set with CURLOPT_HEADERDATA */
    memcpy(userdata,buffer,size*nitems);
    return nitems * size;
}

size_t CurlCallbacks::fileWriteCallback(void *data, size_t size, size_t nmemb, void *clientp) {
    size_t written;
    FILE *out = (FILE *)clientp;
    written = fwrite(data, size, nmemb, out);
    if (written != size * nmemb) {
        cout<<"file 处理写入错误"<<endl;
    }
    return nmemb * size;
}

void html::clear() {
    if(this->response){
        free(this->response);
        this->response = nullptr;
    }
    this->size = 0;
}

html::html() {
    this->response = nullptr;
    this->size = 0;
}

html::~html() {
    clear();
}

cmake_minimum_required(VERSION 3.23)
project(CppCrawler)
set(CMAKE_CXX_STANDARD 17)

add_executable(CppCrawler
        CppCrawler.cpp
        CurlCallbacks.cpp CurlCallbacks.h Album.cpp Album.h CurlExcutor.cpp CurlExcutor.h ThreadPool.cpp ThreadPool.h FileUtil.cpp FileUtil.h)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(CppCrawler PRIVATE CURL::libcurl)

find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(CppCrawler PRIVATE tinyxml2::tinyxml2)

find_package(OpenSSL REQUIRED)
target_link_libraries(CppCrawler PRIVATE OpenSSL::SSL OpenSSL::Crypto)
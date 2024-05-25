#pragma once
#include "ofd_demo.h"
using namespace std;
void CreateDirectoryR(const char* dir);
void CreateDirectoryR(const string& dir);
bool get_dir(char* file_path);
bool get_dir(string& file_path);
int unofd(const char* path);
int deofd();
bool is_relative(const string& path);
bool is_relative(const char* path);
string get_loc(const string& dst, const string& base = string());
int filecopy(const string& dstPath, const string& srcPath);
bool filetest(const char* path);
bool filetest(string path);
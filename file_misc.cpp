#include "ofd_demo.h"
using namespace std;
bool get_dir(char* file_path)
//若传入文件夹路径，使字符串变为上一级目录
//若传入文件路径，使字符串变为文件所在文件夹目录
//若成功返回上一级，返回true，若不能再返回上一级，则返回false
{
	char* c_end = file_path + strlen(file_path);
	char* cur = c_end;
	while (cur != file_path && *cur != '/')
	{
		cur--;
	}
	if (cur != file_path)
	{
		while (cur != c_end)
		{
			*cur = '\0';
			cur++;
		}
		return true;
	}
	else
		return false;
}

bool get_dir(string& file_path)
{
	size_t pos = file_path.rfind('/');
	if (pos < file_path.length())
		file_path.erase(pos, file_path.length() - pos);
	else
		file_path.clear();
	return pos == string::npos;
}
void CreateDirectoryR(const char* dir)//递归地创建多级文件夹
{
	int len = strlen(dir);
	char* copy = (char*)malloc(sizeof(char) * (len + 1));
	memcpy(copy, dir, len + 1);
	if (get_dir(copy))
		CreateDirectoryR(copy);
	free(copy);
	CreateDirectory(dir, NULL);
	return;
}

void CreateDirectoryR(const string& dir)//递归地创建多级文件夹
{
	CreateDirectoryR(dir.c_str());
	return;
}

int unofd(const char* path)//解压ofd
{
#ifdef LOGGING
	log_file.open(log_path, ios::out | ios::app);
	log_file.write("unofd:", 6);
	log_file.put('\n');
	log_file.close();
#endif
	unzFile zf = unzOpen(path);
	if (zf == NULL)
	{
		cout << "invalid path" << endl;
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("invalid path", 12);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
#ifdef LOGGING
	else
	{
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("success", 7);
		log_file.put('\n');
		log_file.close();
	}
#endif
	unzGoToFirstFile(zf);
	do
	{
		unz_global_info zf_ginfo;
		unzGetGlobalInfo(zf, &zf_ginfo);
		unz_file_info zf_finfo;
		char file_name[256] = { 0 };
		unzGetCurrentFileInfo(zf, &zf_finfo, file_name, 256, NULL, 0, NULL, 0);
		//cout << file_name << endl;
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write(file_name, strlen(file_name));
		log_file.put('\n');
		log_file.close();
#endif
		char* file_dir = (char*)malloc(sizeof(char) * (strlen(file_name) + 1));
		strcpy(file_dir, file_name);
		if (get_dir(file_dir))
			CreateDirectoryR(file_dir);
		ofstream ofs;
		ofs.open(file_name, ios::binary | ios::out | ios::ate);
		char buf = 0;
		unzOpenCurrentFile(zf);
		while (unzReadCurrentFile(zf, &buf, 1) > 0)
		{
			ofs.write(&buf, 1);
		}
		unzCloseCurrentFile(zf);
		free(file_dir);
	} while (unzGoToNextFile(zf) != UNZ_END_OF_LIST_OF_FILE);
	return 0;
}
int deofd()//删除解压所得的临时文件
{
	filesystem::remove_all("Doc_0");
	DeleteFile("OFD.xml");
	return 0;
}


bool is_relative(const string& path)
{
	if (path.empty()) return true;
	return path.front() != '/';
}
bool is_relative(const char* path)
{
	return *path != '/';
}
string get_loc(const string& dst, const string& base)//用于处理OFD下的相对路径和绝对路径
{
	if (dst.front() == '/')
		return dst.substr(1);
	else
		return base.empty() ? dst : base + '/' + dst;
}

int filecopy(const string& dstPath, const string& srcPath)
{
	ifstream ires(srcPath, ios::in | ios::binary);
	if (!ires.is_open())//未能打开某一图像文件
	{
		return -1;
	}
	ofstream ores(dstPath, ios::out | ios::binary);
	char buf[256];
	while (!ires.eof())
	{
		ires.read(buf, 256);
		ores.write(buf, ires.gcount());
	}
	ires.close();
	ores.close();
	return 0;
}
bool filetest(const char* path)//测试文件是否能够打开
{
	ifstream file;
	file.open(path);
	bool ret = file.is_open();
	file.close();
	return ret;
}
bool filetest(string path)//测试文件是否能够打开
{
	ifstream file;
	file.open(path);
	bool ret = file.is_open();
	file.close();
	return ret;
}
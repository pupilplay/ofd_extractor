#include "ofd_demo.h"
using namespace std;
bool get_dir(char* file_path)
//�������ļ���·����ʹ�ַ�����Ϊ��һ��Ŀ¼
//�������ļ�·����ʹ�ַ�����Ϊ�ļ������ļ���Ŀ¼
//���ɹ�������һ��������true���������ٷ�����һ�����򷵻�false
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
void CreateDirectoryR(const char* dir)//�ݹ�ش����༶�ļ���
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

void CreateDirectoryR(const string& dir)//�ݹ�ش����༶�ļ���
{
	CreateDirectoryR(dir.c_str());
	return;
}

int unofd(const char* path)//��ѹofd
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
int deofd()//ɾ����ѹ���õ���ʱ�ļ�
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
string get_loc(const string& dst, const string& base)//���ڴ���OFD�µ����·���;���·��
{
	if (dst.front() == '/')
		return dst.substr(1);
	else
		return base.empty() ? dst : base + '/' + dst;
}

int filecopy(const string& dstPath, const string& srcPath)
{
	ifstream ires(srcPath, ios::in | ios::binary);
	if (!ires.is_open())//δ�ܴ�ĳһͼ���ļ�
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
bool filetest(const char* path)//�����ļ��Ƿ��ܹ���
{
	ifstream file;
	file.open(path);
	bool ret = file.is_open();
	file.close();
	return ret;
}
bool filetest(string path)//�����ļ��Ƿ��ܹ���
{
	ifstream file;
	file.open(path);
	bool ret = file.is_open();
	file.close();
	return ret;
}
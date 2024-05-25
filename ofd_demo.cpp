/*
1.多文件批处理
2.图形化
3.日志功能待整理优化，封装
4.attribute考虑使用哈希映射
5.相对路径未完全处理
*/
#define _CRT_SECURE_NO_WARNINGS 1

#include "ofd_demo.h"
using namespace std;
#define LOGGING//是否开启日志
#ifdef LOGGING
string log_path;//日志路径
ofstream log_file;
void log_init()
{
	CreateDirectoryR("log");
	char t[20];
	time_t ad_hoc_t = time(NULL);
	strftime(t, 20, "%Y%m%d%H%M%S", localtime(&ad_hoc_t));
	log_path = "log/" + string(t) + ".log";
	return;
}
#endif
void print_finfo(unz_file_info info)//调试用，输出文件信息
{
	cout << "version = " << info.version << endl;
	cout << "size_filename = " << info.size_filename << endl;
	cout << "uncompressed_size = " << info.uncompressed_size << endl;
	cout << "external_fa = " << info.external_fa << endl;
	cout << "compressed_size = " << info.compressed_size << endl;
	cout << "compression_method = " << info.compression_method << endl;
	cout << "crc = " << info.crc << endl;
	cout << "disk_num_start = " << info.disk_num_start << endl;
	cout << "dosData = " << info.dosDate << endl;
	cout << "flag = " << info.flag << endl;
	cout << "internal_fa = " << info.internal_fa << endl;
	cout << "size_file_comment = " << info.size_file_comment << endl;
	cout << "size_file_extra = " << info.size_file_extra << endl;
	cout << "version_needed = " << info.version_needed << endl;
}
void print_ginfo(unz_global_info info)
{
	cout << "number_entry = " << info.number_entry << endl;//文件与文件夹总数
	cout << "size_comment = " << info.size_comment << endl;//注释的大小
}

int ofd_extract_image(const char* path)
{
#ifdef LOGGING
	log_file.open(log_path, ios::out | ios::app);
	log_file.write("ofd_extract_image\nOFD.xml", 25);
	log_file.put('\n');
	log_file.close();
#endif
	xml_reader root("OFD.xml");
	if (!root.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!root.eof() && root.read().name != "ofd:DocRoot");
#ifdef LOGGING
	log_file.open(log_path, ios::out | ios::app);
	log_file.write(root.get_cur().content.c_str(),root.get_cur().content.length());
	log_file.put('\n');
	log_file.close();
#endif
	xml_reader enter(get_loc(root.get_cur().content,root.get_dir()));
	if (!enter.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!enter.eof() && enter.read().name != "ofd:DocumentRes");
	if (enter.eof())
	{
		cout << "No resources found";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("No resources found",18);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
#ifdef LOGGING
	log_file.open(log_path, ios::out | ios::app);
	log_file.write(root.get_cur().content.c_str(), root.get_cur().content.length());
	log_file.put('\n');
	log_file.close();
#endif
	xml_reader DocumentRes(get_loc(enter.get_cur().content,enter.get_dir()));
	if (!DocumentRes.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!DocumentRes.eof() && DocumentRes.read().name != "ofd:Res");
	string resDir;
	for (auto& attr : DocumentRes.get_cur().attributes)
	{
		if (attr.name == "BaseLoc")
		{
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write(attr.content.c_str(), attr.content.length());
			log_file.put('\n');
			log_file.close();
#endif
			resDir = get_loc(attr.content, DocumentRes.get_dir());
		}
	}
	if (resDir.empty())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!DocumentRes.eof() && DocumentRes.read().name != "ofd:MultiMedia");
	string outputDir = "output";
	CreateDirectory(outputDir.c_str(), NULL);
	while (DocumentRes.get_cur().name == "ofd:MultiMedia")
	{
		element media = DocumentRes.get_cur();
		string Type;
		string Format;
		for (auto& attr : media.attributes)
		{
			if (attr.name == "Type")
				Type = attr.content;
			if (attr.name == "Format")
				Format = attr.content;
		}
		DocumentRes.read();
		if (Type == "Image")
		{
			string resPath;
			string outputPath;
			resPath = get_loc(DocumentRes.get_cur().content, resDir);
			outputPath = get_loc(DocumentRes.get_cur().content, outputDir);
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write(resPath.c_str(), resPath.length());
			log_file.put(' ');
			log_file.write(outputPath.c_str(), outputPath.length());
			log_file.put('\n');
			log_file.close();
#endif
			filecopy(outputPath, resPath);
		}
		//未实现特殊格式处理
		DocumentRes.read();
	}
	DocumentRes.close();
	enter.close();
	root.close();
	return 0;
}
int ofd_extract_text(const char* path)
{
#ifdef LOGGING
	log_file.open(log_path, ios::out | ios::app);
	log_file.write("ofd_extract_text\nOFD.xml", 25);
	log_file.put('\n');
	log_file.close();
#endif
	xml_reader root("OFD.xml");	
	if (!root.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!root.eof() && root.read().name != "ofd:DocRoot");
	xml_reader enter(get_loc(root.get_cur().content, root.get_dir()));
	if (!enter.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!enter.eof() && enter.read().name != "ofd:Page");
	if (enter.eof())
	{
		cout << "No pages found";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("No pages found", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (enter.get_cur().name == "ofd:Page")
	{
		string pagePath;
		for (auto& attr : enter.get_cur().attributes)
		{
			if (attr.name == "BaseLoc")
			{
				pagePath = get_loc(attr.content,enter.get_dir());
			}
		}
		if (pagePath.empty())
		{
			cout << "no page found";
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write("no page found", 14);
			log_file.put('\n');
			log_file.close();
#endif
			return -1;
		}
		xml_reader page(pagePath);
		if (!page.is_open())
		{
			cout << "failed to open";
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write("failed to open", 14);
			log_file.put('\n');
			log_file.close();
#endif
			return -1;
		}
		while (!page.eof() && page.read().name != "ofd:Content");
		int depth = page.depth();
		page.read();
		while (page.depth() > depth)
		{
			if (page.get_cur().name == "ofd:TextCode")
			{
				cout << page.get_cur().content << endl;
			}
			page.read();
		}
		page.close();
		enter.read();
	}
	enter.close();
	root.close();
	return 0;
}
int ofd_extract_attachment(const char* path)
{
	xml_reader root("OFD.xml");
	if (!root.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!root.eof() && root.read().name != "ofd:DocRoot");
	xml_reader enter(get_loc(root.get_cur().content, root.get_dir()));
	if (!enter.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!enter.eof() && enter.read().name != "ofd:Attachments");
	if (enter.eof())
	{
		cout << "No attachment found";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("No attachment found", 19);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	xml_reader attachments(get_loc(enter.get_cur().content, enter.get_dir()));
	if (!attachments.is_open())
	{
		cout << "failed to open";
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("failed to open", 14);
		log_file.put('\n');
		log_file.close();
#endif
		return -1;
	}
	while (!attachments.eof() && attachments.read().name != "ofd:Attachment");
	while (!attachments.eof())
	{
		element_tree attachment(attachments);
		if (!attachment.root()->children.empty())
		{
			string attachmentPath;
			string relativePath;
			if (is_relative(attachment.root()->children.front()->content()))
			{
				relativePath = attachment.root()->children.front()->content();
				attachmentPath = get_loc(relativePath, attachments.get_dir());
			}
			else
			{
				attachmentPath = get_loc(attachment.root()->children.front()->content());
				relativePath = attachmentPath;
				relativePath.erase(0, attachments.get_dir().length() + 1);
			}
			string outputPath = get_loc(relativePath, "output");
			string outputDir = outputPath;
			get_dir(outputDir);
			CreateDirectoryR(outputDir); 
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write(attachmentPath.c_str(), attachmentPath.length());
			log_file.put(' ');
			log_file.write(outputPath.c_str(), outputPath.length());
			log_file.put('\n');
			log_file.close();
#endif
			filecopy(outputPath, attachmentPath);
		}
		else//该attachment没有指定路径
		{
			cout << "an attachment has missed its location\n";
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write("an attachment has missed its location",37);
			log_file.put('\n');
			log_file.close();
#endif
		}
	}
	attachments.close();
	enter.close();
	root.close();
	return 0;
}

int show_help()
{
	cout << "help is unavailable for now, waiting for DongYang Li to cover!";
	return 0;
}
int menu(char* path)//菜单
{
	cout << "operations:\n";
	cout << "1.extract image 2.extract text\n3.extract attachment\n";
select_operation://该标签用于直接跳出多层switch
	switch (getchar())
	{
	case '1':
		ofd_extract_image(path);
		break;
	case '2':
		ofd_extract_text(path);
		break;
	case '3':
		ofd_extract_attachment(path);
		break;
	case '\n'://处理上一次输入时的回车
		goto select_operation;//跳出多层switch，吃掉回车后下一个标签
	default:
		cout << "invalid input";
	}
	while (getchar() != '\n');
	cout << endl;
	return 0;
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, ".utf-8");//设置UTF-8编码
#ifdef LOGGING
	log_init();
#endif
	char path[256] = { 0 };
#ifdef LOGGING
	log_file.open(log_path, ios::out|ios::app);
	char num[4] = { 0 };
	sprintf(num, "%d", argc);
	log_file.write("argc:", 5);
	log_file.write(num, strlen(num));
	log_file.put('\n');
	for (int i = 0; i < argc; i++)
	{
		log_file.write(argv[i], strlen(argv[i]));
		log_file.put('\n');
	}
	log_file.close();
#endif
	switch (argc)
	{
	case 1:
		cout << "input the file path, or '*' to exit\n";
		cin >> path;
		while (*path != '*')
		{
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write("path:", 5);
			log_file.write(path, strlen(path));
			log_file.put('\n');
			log_file.close();
#endif
			if (!filetest(path))
			{
				cout << "failed to open the file";
#ifdef LOGGING
				log_file.open(log_path, ios::out | ios::app);
				log_file.write("failed to open the file", 23);
				log_file.put('\n');
				log_file.close();
#endif
				return -1;
			}
			else
			{
				unofd(path);
				menu(path);
				deofd();
				cout << "input the file path, or '*' to exit\n";
				cin >> path;
			}
		}
		break;
	default://命令行处理
		bool text = false;
		bool attachment = false;
		bool image = false;
		for (int i = 1; i < argc; i++)
		{
			switch (argv[i][0])
			{
			case '-':
				switch (argv[i][1])
				{
				case 'h'://帮助
					return show_help();
					break;
				case 'p':
					i++;
					if (i == argc)
					{
						break;
						//return -1;
					}
					memcpy(path, argv[i], strlen(argv[i]) + 1);
					break;
				case 'i':
					if (image)
					{
						cout << "argument \"-i\" conflicts";
						return -1;
					}
					image = true;
					break;
				case 't':
					if (text)
					{
						cout << "argument \"-t\" conflicts";
						return -1;
					}
					text = true;
					break;
				case 'a':
					if (attachment)
					{
						cout << "argument \"-a\" conflicts";
						return -1;
					}
					attachment = true;
					break;
				default:
					cout << "invalid argument " << "\"" << argv[i] << "\", try argument \"-h\" for help" << endl;
					return -1;
				}
				break;
			default:
				if (i == 1)
				{
					memcpy(path, argv[i], strlen(argv[i]) + 1);
				}
				else
				{
					cout << "invalid argument " << "\"" << argv[i] << "\", try argument \"-h\" for help" << endl;
					return -1;
				}
				break;
			}
		}
		if (*path == 0)
		{
			cout << "file path missing";
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write("file path missing", 17);
			log_file.put('\n');
			log_file.close();
#endif
			return -1;
		}
#ifdef LOGGING
		log_file.open(log_path, ios::out | ios::app);
		log_file.write("path:", 5);
		log_file.write(path, strlen(path));
		log_file.put('\n');
		log_file.close();
#endif
		if (!filetest(path))
		{
			cout << "failed to open the file";
#ifdef LOGGING
			log_file.open(log_path, ios::out | ios::app);
			log_file.write("failed to open the file", 23);
			log_file.put('\n');
			log_file.close();
#endif
			return -1;
		}
		unofd(path);
		if (argc == 2)
		{
			menu(path);
			deofd();
			return 0;
		}
		if (image)
			ofd_extract_image(path);
		if (text)
			ofd_extract_text(path);
		if (attachment)
			ofd_extract_attachment(path);
		deofd();
	}
	return 0;
}
#pragma once
#include "ofd_demo.h"
using namespace std;
struct element//XMLԪ��
{
public:
	struct attribute//XMLԪ������
	{
	public:
		string name;//XMLԪ����������
		string content;//XMLԪ����������
	};
	string name;//Ԫ������
	vector<attribute>attributes;//Ԫ��������
	string content;//���ݣ���Ԫ�ؽ�������Ԫ�أ���Ϊ��
	element();
	element(const element& elem);
	bool empty() const;
	void clear();
};

class xml_reader
{
private:
	ifstream ifs;
	string version;
	string encoding;
	stack<element>root;//���ڼ�¼���ڵ�
	element elem;//���ڼ�¼������ζ����Ԫ��
	bool single;//�����һ��Ԫ���Ƿ�Ϊ����ǩ
	string dir;//���ڼ�¼XML�ļ�����Ŀ¼
public:
	bool eof();
	const string& get_dir();
	int depth();
	const element& get_parent();//���뵱ǰԪ�ص�ֱ�Ӹ�Ԫ�أ����ѵ���EOF���򷵻ؿ�Ԫ��
	const element& get_cur();
	void close();
	bool is_open();
	const element& read();//����һ��Ԫ��
	//��ȡ��˳�����������
	//���ѵ���EOF������һ����Ԫ��
	xml_reader();
	xml_reader(const string& path);
	xml_reader(const char* path);
};
class element_tree//XMLԪ����
{
public:
	struct element_node//XMLԪ�����ڵ�
	{
	public:
		element elem;
		vector<element_node*> children;
		element_node();
		element_node(const element& elemr);
		element_node(const element* elemp);
		string& name();
		vector<element::attribute>& attributes();
		string& content();
		bool is_leaf();
	};
	element_tree();
	element_tree(xml_reader& xr);//��xr���һ�ζ���Ľڵ�Ϊ������Ԫ����
		//�ú����ƶ��ļ�ָ��
	~element_tree();
	element_node* root();
private:
	element_node* _root;//���ڵ�
};
using element_node = element_tree::element_node;
#pragma once
#include "ofd_demo.h"
using namespace std;
struct element//XML元素
{
public:
	struct attribute//XML元素属性
	{
	public:
		string name;//XML元素属性名字
		string content;//XML元素属性内容
	};
	string name;//元素名字
	vector<attribute>attributes;//元素属性组
	string content;//内容，若元素仅包含子元素，则为空
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
	stack<element>root;//用于记录父节点
	element elem;//用于记录最后依次读入的元素
	bool single;//标记上一个元素是否为单标签
	string dir;//用于记录XML文件所在目录
public:
	bool eof();
	const string& get_dir();
	int depth();
	const element& get_parent();//读入当前元素的直接父元素，若已到达EOF，则返回空元素
	const element& get_cur();
	void close();
	bool is_open();
	const element& read();//读入一个元素
	//读取的顺序是深度优先
	//若已到达EOF，返回一个空元素
	xml_reader();
	xml_reader(const string& path);
	xml_reader(const char* path);
};
class element_tree//XML元素树
{
public:
	struct element_node//XML元素树节点
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
	element_tree(xml_reader& xr);//以xr最后一次读入的节点为根构造元素树
		//该函数移动文件指针
	~element_tree();
	element_node* root();
private:
	element_node* _root;//根节点
};
using element_node = element_tree::element_node;
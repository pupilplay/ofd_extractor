#include "ofd_demo.h"
using namespace std;

element::element() {};

element::element(const element& elem) :name(elem.name), attributes(elem.attributes), content(elem.content) {}

bool element::empty() const
{
	return name.empty() && attributes.empty() && content.empty();
}

void element::clear()
{
	name.clear();
	attributes.clear();
	content.clear();
}


bool xml_reader::eof()
{
	return ifs.eof();
}

const string& xml_reader::get_dir()
{
	return dir;
}

int xml_reader::depth()
{
	return root.size();
}

const element& xml_reader::get_parent()//���뵱ǰԪ�ص�ֱ�Ӹ�Ԫ�أ����ѵ���EOF���򷵻ؿ�Ԫ��
{
	if (!this->eof())
		return root.top();
	else
		return element();
}

const element& xml_reader::get_cur()
{
	return elem;
}

void xml_reader::close()
{
	ifs.close();
	return;
}

bool xml_reader::is_open()
{
	return ifs.is_open();
}

const element& xml_reader::read()//����һ��Ԫ��
//��ȡ��˳�����������
//���ѵ���EOF������һ����Ԫ��
{
	if (!single)
		root.emplace(elem);
	single = false;
	elem.clear();
	char buf = ifs.get();
	if (buf == '/')
	{
		while (buf == '/')
		{
			root.pop();
			while (buf = ifs.get())
			{
				if (buf == '<' || buf == EOF)
					break;
			}
			buf = ifs.get();
		}
	}
	if (ifs.eof())
		return elem;
	while (buf != '>' && buf != ' ')//��ȡԪ������
	{
		elem.name += buf;
		buf = ifs.get();
	}
	if (buf != '>')//�ж��Ƿ�������
	{
		//��ȡԪ������
		while (buf != '>')
		{
			//�жϵ���ǩԪ��
			if (buf == '/')
			{
				while (buf != '<')
				{
					buf = ifs.get();
				}
				single = true;
				return elem;
			}
			buf = ifs.get();
			elem.attributes.emplace_back(string());
			while (buf != '=')
			{
				elem.attributes.back().name += buf;
				buf = ifs.get();
			}
			char punctuation = ifs.get();//����
			buf = ifs.get();
			while (buf != punctuation)//�����ڶ�ȡ���������⣬��δ����
			{
				elem.attributes.back().content += buf;
				buf = ifs.get();
			}
			buf = ifs.get();
		}
	}
	if (buf != '<')//�ж��Ƿ��з�Ԫ������
	{
		buf = ifs.get();
		while (buf != '<' && buf != '\n' && buf != '\t')//��ȡԪ������
			//��Ҫת�壡��������������������������������������������������������������������
		{
			while (buf == '&')
			{
				char buf1 = ifs.get();
				char buf2 = ifs.get();
				char buf3 = ifs.get();
				if (buf1 == 'l' && buf2 == 't' && buf3 == ';')
				{
					elem.content += '<';
				}
				else if (buf1 == 'g' && buf2 == 't' && buf3 == ';')
				{
					elem.content += '>';
				}
				else if (buf1 == 'a' && buf2 == 'm' && buf3 == 'p')
				{
					char buf4 = ifs.get();
					if (buf4 == ';')
					{
						elem.content += '&';
					}
					else
					{
						elem.content += buf1;
						elem.content += buf2;
						elem.content += buf3;
						elem.content += buf4;
					}

				}
				else if (buf1 == 'a' && buf2 == 'p' && buf3 == 'o')
				{
					char buf4 = ifs.get();
					char buf5 = ifs.get();
					if (buf4 == 's' && buf5 == ';')
					{
						elem.content += '\'';
					}
					else
					{
						elem.content += buf1;
						elem.content += buf2;
						elem.content += buf3;
						elem.content += buf4;
						elem.content += buf5;
					}
				}
				else if (buf1 == 'q' && buf2 == 'u' && buf3 == 'o')
				{
					char buf4 = ifs.get();
					char buf5 = ifs.get();
					if (buf4 == 't' && buf5 == ';')
					{
						elem.content += '\"';
					}
					else
					{
						elem.content += buf1;
						elem.content += buf2;
						elem.content += buf3;
						elem.content += buf4;
						elem.content += buf5;
					}
				}
				else
				{
					elem.content += buf1;
					elem.content += buf2;
					elem.content += buf3;
				}
				buf = ifs.get();
			}
			elem.content += buf;
			buf = ifs.get();
		}
		while (buf != '<')
		{
			buf = ifs.get();
		}
	}
	return elem;
}

xml_reader::xml_reader()
{
	single = false;
}

xml_reader::xml_reader(const string& path)
{
	single = false;
	ifs.open(path, ios::in);
	while (ifs.get() != '>');
	while (ifs.get() != '<');
	//��ȡ�ļ�����Ŀ¼
	dir = path;
	size_t pos = dir.rfind('/');
	if (pos < dir.length())
		dir.erase(pos, dir.length() - pos);
	else
		dir.clear();
	return;
}

xml_reader::xml_reader(const char* path)
{
	single = false;
	ifs.open(path, ios::in);
	while (ifs.get() != '>');
	while (ifs.get() != '<');
	dir = path;
	size_t pos = dir.rfind('/');
	if (pos < dir.length())
		dir.erase(pos, dir.length() - pos);
	else
		dir.clear();
	return;
}

element_node::element_node() {};

element_node::element_node(const element& elemr) :elem(elemr) {};

element_node::element_node(const element* elemp)
{
	elem.name = elemp->name;
	elem.attributes = elemp->attributes;
	elem.content = elemp->content;
}

string& element_node::name()
{
	return elem.name;
}

vector<element::attribute>& element_node::attributes()
{
	return elem.attributes;
}

string& element_node::content()
{
	return elem.content;
}

bool element_node::is_leaf()
{
	return children.empty();
}

element_tree::element_tree() :_root(NULL) {};

element_tree::element_tree(xml_reader& xr)//��xr���һ�ζ���Ľڵ�Ϊ������Ԫ����
//�ú����ƶ��ļ�ָ��
{
	if (xr.eof()) return;
	_root = new element_node(xr.get_cur());
	stack<element_node*>s;
	s.push(_root);
	int depth = xr.depth();
	xr.read();
	if (xr.depth() <= depth)
		return;
	while (!s.empty())
	{
		s.top()->children.emplace_back(new element_node(xr.get_cur()));
		int depth = xr.depth();
		xr.read();
		if (xr.depth() > depth)
		{
			s.push(s.top()->children.back());
		}
		else if (xr.depth() < depth)
		{
			for (int i = xr.depth(); i < depth; i++)
			{
				if (s.empty())
					return;
				s.pop();
			}
		}
	}
	return;
}

element_tree::~element_tree()
{
	queue<element_node*>q;
	q.push(_root);
	while (!q.empty())
	{
		for (auto node : q.front()->children)
		{
			q.push(node);
		}
		delete q.front();
		q.pop();
	}
	return;
}

element_node* element_tree::root()
{
	return _root;
}

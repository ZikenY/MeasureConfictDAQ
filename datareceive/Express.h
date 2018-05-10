// Express.h: interface for the Express class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESS_H__90379FD0_7D79_4E75_BF40_EE00B3927B1C__INCLUDED_)
#define AFX_EXPRESS_H__90379FD0_7D79_4E75_BF40_EE00B3927B1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class ExpBinder;
struct exp_unit;

// Express类处理文本的数学公式解析与计算
class Express  
{
	char*		m_buf;
	exp_unit*	m_units;
	int			m_unit_n;
	exp_unit* parse(const char** F,const char** L);
public:
	Express();
	virtual ~Express();

	//描述：清理
	void Clear();

	//描述：将文本的公式解析成Express对象
	//返回：如果公式语法错误则返回false,否则返回true
	bool Parse(const char* txt);
	bool Parse(const wchar_t* txt);

	//描述：绑定变量和自定义函数
	void Bind(ExpBinder& eb,int all=false);

	//描述：判断this是否OK
	bool IsGood();

	//描述：计算公式的值
	double Value();

	//描述：输出字符串
	char* ToString(char* str);
};

//ExpBinder用来支持 变量和自定义函数 
class ExpBinder
{
	struct bind;
	char*	m_mem;
	int		m_memsize;
	int		m_memuse;
	bind*	m_head;

	bool add(const char* name,int type,void* para);
public:
	ExpBinder(int memsize=1024);
	~ExpBinder();

	bool Add(const char* name,double* var){ return add(name,-1,var); }
	bool Add(const char* name,double (*fun)()){ return add(name,0,fun); }
	bool Add(const char* name,double (*fun)(double)){ return add(name,1,fun); }
	bool Add(const char* name,double (*fun)(double,double)){ return add(name,2,fun); }
	bool Add(const char* name,double (*fun)(double,double,double)){ return add(name,3,fun); }
	bool Add(const char* name,double (*fun)(double,double,double,double)){ return add(name,4,fun); }
	bool Add(const char* name,double (*fun)(double,double,double,double,double)){ return add(name,5,fun); }
	bool Add(const char* name,double (*fun)(double,double,double,double,double,double)){ return add(name,6,fun); }
	bool Add(const char* name,double (*fun)(double,double,double,double,double,double,double)){ return add(name,7,fun); }
	bool Add(const char* name,double (*fun)(double,double,double,double,double,double,double,double)){ return add(name,8,fun); }
	bool Add(const char* name,double (*fun)(double,double,double,double,double,double,double,double,double)){ return add(name,9,fun); }

	//参数：type指定绑定类型 -1 表示绑定变量 此时OnBind函数需要返回一个double*
	//      type>=0 表示绑定自定义函数 此时OnBind函数需要返回double (*f0)(),或double (*f1)(double),或……
	//		name 要绑定对象的名称
	virtual void* OnBind(const char* name,int type);
};

#endif // !defined(AFX_EXPRESS_H__90379FD0_7D79_4E75_BF40_EE00B3927B1C__INCLUDED_)

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

// Express�ദ���ı�����ѧ��ʽ���������
class Express  
{
	char*		m_buf;
	exp_unit*	m_units;
	int			m_unit_n;
	exp_unit* parse(const char** F,const char** L);
public:
	Express();
	virtual ~Express();

	//����������
	void Clear();

	//���������ı��Ĺ�ʽ������Express����
	//���أ������ʽ�﷨�����򷵻�false,���򷵻�true
	bool Parse(const char* txt);
	bool Parse(const wchar_t* txt);

	//�������󶨱������Զ��庯��
	void Bind(ExpBinder& eb,int all=false);

	//�������ж�this�Ƿ�OK
	bool IsGood();

	//���������㹫ʽ��ֵ
	double Value();

	//����������ַ���
	char* ToString(char* str);
};

//ExpBinder����֧�� �������Զ��庯�� 
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

	//������typeָ�������� -1 ��ʾ�󶨱��� ��ʱOnBind������Ҫ����һ��double*
	//      type>=0 ��ʾ���Զ��庯�� ��ʱOnBind������Ҫ����double (*f0)(),��double (*f1)(double),�򡭡�
	//		name Ҫ�󶨶��������
	virtual void* OnBind(const char* name,int type);
};

#endif // !defined(AFX_EXPRESS_H__90379FD0_7D79_4E75_BF40_EE00B3927B1C__INCLUDED_)

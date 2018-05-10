// Express.cpp: implementation of the Express class.
//
//////////////////////////////////////////////////////////////////////
#include "Express.h"
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct ExpBinder::bind
{
	bind*	m_next;
	void*	m_var;
	int		m_type;
};

ExpBinder::ExpBinder(int memsize)
{
	m_memsize = memsize;
	m_mem = 0;
	m_head = 0;
	m_memuse = 0;
	if(memsize>0)
		m_mem = new char[memsize];
}

ExpBinder::~ExpBinder()
{
	delete m_mem;
}

bool ExpBinder::add(const char* name,int type,void* para)
{
	try
	{
		int n = strlen(name)+1;
		n = 4*((n+3)/4);
		n += sizeof(bind);
		if(m_memsize-m_memuse<n)
			return false;
		bind* p = (bind*)(m_mem+m_memuse);
		m_memuse += n;
		p->m_next = m_head;
		p->m_var = para;
		p->m_type = type;
		strcpy( (char*)(p+1),name );
		m_head = p;
		return true;
	}
	catch(...)
	{return false;}
}

void* ExpBinder::OnBind(const char* name,int type)
{
	bind* p = m_head;
	for(; p; p=p->m_next)
	{
		if(p->m_type!=type)
			continue;
		if( strcmp((char*)(p+1),name)!=0 )
			continue;
		return p->m_var;
	}
	return 0;
}

static int ss_prior(const char* s);
	
//Express内部的数据结构
struct exp_unit
{
	double		m_value;
	const char* m_name;
	union
	{
		double*	m_var;
		double (*m_func0)();
		double (*m_func1)(double);
		double (*m_func2)(double,double);
		double (*m_func3)(double,double,double);
		double (*m_func4)(double,double,double,double);
		double (*m_func5)(double,double,double,double,double);
		double (*m_func6)(double,double,double,double,double,double);
		double (*m_func7)(double,double,double,double,double,double,double);
		double (*m_func8)(double,double,double,double,double,double,double,double);
		double (*m_func9)(double,double,double,double,double,double,double,double,double);
	};
	int			m_para_count;	//当m_para_count是-1的时候此单元是变量,否则是函数
	exp_unit*	m_paras[9];

	void bind_const()
	{
		if( -1!=m_para_count )
			return;
		int c = m_name[0];
		if( c=='.' || (c>='0' && c<='9') )
		{
			m_value = atof(m_name);
			m_var = &m_value;
		}
	}

	double value()
	{
		exp_unit** p = m_paras;
		switch(m_para_count)
		{
		case -1: return *m_var;
		case 0: return m_func0();
		case 1: return m_func1( p[0]->m_value );
		case 2: return m_func2( p[0]->m_value,p[1]->m_value );
		case 3: return m_func3( p[0]->m_value,p[1]->m_value,p[2]->m_value );
		case 4: return m_func4( p[0]->m_value,p[1]->m_value,p[2]->m_value,p[3]->m_value );
		case 5: return m_func5( p[0]->m_value,p[1]->m_value,p[2]->m_value,p[3]->m_value,p[4]->m_value );
		case 6: return m_func6( p[0]->m_value,p[1]->m_value,p[2]->m_value,p[3]->m_value,p[4]->m_value,p[5]->m_value );
		case 7: return m_func7( p[0]->m_value,p[1]->m_value,p[2]->m_value,p[3]->m_value,p[4]->m_value,p[5]->m_value,p[6]->m_value );
		case 8: return m_func8( p[0]->m_value,p[1]->m_value,p[2]->m_value,p[3]->m_value,p[4]->m_value,p[5]->m_value,p[6]->m_value,p[7]->m_value );
		case 9: return m_func9( p[0]->m_value,p[1]->m_value,p[2]->m_value,p[3]->m_value,p[4]->m_value,p[5]->m_value,p[6]->m_value,p[7]->m_value,p[8]->m_value );
		default: return 0;
		}
	}
	
	int ssp()
	{
		if(m_para_count!=2)
			return -1;
		return ss_prior(m_name);
	}

	char* tostring(char* str)
	{
		if( -1!=ssp() )
		{
			int bc;

			bc = m_paras[0]->ssp();
			bc = bc > ssp();
			if( bc ) *str++ = '(';
			str = m_paras[0]->tostring(str);
			if( bc ) *str++ = ')';
			
			strcpy(str,m_name);
			str += strlen(str);
			
			bc = m_paras[1]->ssp();
			bc = bc > ssp();

			if( bc ) *str++ = '(';
			str = m_paras[1]->tostring(str);
			if( bc ) *str++ = ')';
			return str;
		}

		strcpy(str,m_name);
		str += strlen(str);
		if( -1==m_para_count )
			return str;
		*str++ = '('; 
		for(int i=0; i<m_para_count;)
		{
			str = m_paras[i]->tostring(str);
			if(++i<m_para_count)
				*str++ = ',';
		}
		*str++ = ')';
		return str;
	}
};

Express::Express()
{
	m_buf = 0;
	m_unit_n = 0;
	m_units = 0;
}

Express::~Express()
{
	Clear();
}

void Express::Clear()
{
	delete m_buf;
	delete m_units;
		
	m_buf = 0;
	m_unit_n = 0;
	m_units = 0;
}

double Express::Value()
{
	if(m_unit_n==0)
		return 0;
	for(int i=m_unit_n; --i>=0;)
	{
		m_units[i].m_value = m_units[i].value();
	}
	return m_units[0].m_value;
}

static const char* comma = ",";
static const char* left = "(";
static const char* right = ")";
static const char* symbol[] = 
{
	"!","~",0,
	"*","/","%",0,
	"+","-",0,
	"<","<=",">",">=",0,
	"=","==","!=",0, 
	"&",0,
	"^",0,
	"|",0,
	"&&",0,
	"||",0,
	0,0
};

static const char** symA()
{
	static const char* A[256] = { (const char*)-1 };
	if( A[0]!=(const char*)-1 )
		return A;
	memset(A,0,sizeof(A));
	A[2*comma[0]] = comma;
	A[2*left[0]] = left;
	A[2*right[0]] = right;
	const char** p = symbol;
	int i;
	for(;p[0] || p[1]; ++p)
	{
		if(p[0]==0) continue;
		i = 2*(p[0][0]);
		if(p[0][1]) ++i;
		A[i] = p[0];
	}
	return A;
}

static int ss_prior(const char* s)
{
	static const char** ss = symA();
	if(*s<=0) 
		return -1;
	if( ss[2*(*s)]==0 )
		return -1;
	const char** p = symbol;
	for(int n=0; *p; ++n,++p)
	{
		for(; *p; ++p)
		{
			if(*p==s)
				return n;
		}
	}
	return -1;
}

static inline const char* ss_parse(const char*& p)
{
	const char** pp = symA();
	int i = 2*(*p);
	if(i<=0 || pp[i]==0) return 0;
	if(pp[i+1]!=0 && pp[i+1][1]==p[1])
	{
		++p;
		return pp[i+1];
	}
	return pp[i];
}

static inline const char** ss_push(char*& F,char*& L,const char** ss)
{
	if(L==F) return ss;
	*ss = F;
	*L++ = 0;
	F = L;
	return ss+1;
}

const char** symbol_parse(const char* txt,char* F,const char** ss)
{
	char* L; const char* s;
	for(L=F; *txt; ++txt)
	{
		if( isspace(*txt) )
			ss = ss_push(F,L,ss);
		else if( s=ss_parse(txt) )
		{
			ss = ss_push(F,L,ss);
			*ss++ = s;
		}
		else *L++ = *txt;
	}
	return ss_push(F,L,ss);
}

static const char** ss_find(const char** F,const char** L)
{
	const char** I; int m,i,f;
	for(I=0,m=i=0; F<L; ++F)
	{
		if(*F==left) ++m;
		else if(*F==right) --m;
		else if( m!=0 ) continue;
		else
		{
			f =ss_prior(*F);
			if( f<i ) continue;
			i = f; I = F;
		}
	}
	return I;
}

static int cc_find(const char** F,const char** L,const char*** cc)
{
	int m=0, n=0;
	for(; F<L; ++F)
	{
		if(*F==left) ++m;
		else if(*F==right) --m;
		else if(*F==comma && m==0)
		{
			if(++n>9) return -1;
			*cc++ = F;
		}
	}
	return n;
}

exp_unit* Express::parse(const char** F,const char** L)
{
	const char** I;
	exp_unit* e = m_units+(m_unit_n++);
	if( (L-F)==1 )
	{
		e->m_para_count = -1;
		e->m_name = *F;
		return e;
	}
	if( I=ss_find(F,L) )
	{
		if(I==L-1)
			return 0;
		e->m_name = *I;
		if(I==F)
		{
			e->m_para_count = 1;
			e->m_paras[0] = parse(F+1,L);
			return e->m_paras[0]?e:0;
		}
		else
		{
			e->m_para_count = 2;
			if( 0==(e->m_paras[0]=parse(F,I)) )
				return 0;
			e->m_paras[1] = parse(I+1,L);
			return e->m_paras[1]?e:0;
		}
	}
	
	if( L[-1]!=right )
		return 0;

	if( F[0]==left )
	{
		--m_unit_n;
		return parse(F+1,L-1);
	}

	if( F[1]!=left )
		return 0;
	e->m_name = *F++;
	if(L-F==2)
		return e;

	const char** cA[10];
	cA[0] = F;
	e->m_para_count = cc_find(F+1,--L,cA+1);
	if( e->m_para_count<0 )
		return 0;
	cA[++e->m_para_count] = L;
	for(int i=0; i<e->m_para_count; ++i)
	{
		e->m_paras[i] = parse(cA[i]+1,cA[i+1]);
		if( e->m_paras[i]==0 )
			return 0;
	}
	return e;
}

bool Express::IsGood()
{
	if(m_units==0)
		return false;
	int i = m_unit_n;
	while( --i>=0 )
	{
		if(m_units[i].m_var==0)
			return false;
	}
	return true;
}

static ExpBinder& std_eb();

bool Express::Parse(const char* txt)
{
	int i,n,m; const char** ss;
	
	Clear();

	n = strlen(txt);
	m_buf = new char[2*n+1];
	ss = (const char**)_alloca(n*4);
	n = symbol_parse(txt,m_buf,ss)-ss;

	for(m=i=m_unit_n=0; i<n; ++i)
	{
		const char* s = ss[i];
		if(s==comma) continue;
		else if(s==left) ++m;
		else if(s==right) --m;
		else ++m_unit_n;
	}

	if( m!=0 )
	{
		Clear();
		return false;
	}

	m_units = new exp_unit[m_unit_n];
	memset(m_units,0,m_unit_n*sizeof(exp_unit));
	m_unit_n = 0;
	if( !parse(ss,ss+n) )
	{
		Clear();
		return false;
	}

	for(i=m_unit_n; --i>=0; )
	{
		m_units[i].bind_const();
	}

	Bind(std_eb());

	return true;
}

bool Express::Parse(const wchar_t* txt)
{
	int nLen = wcslen(txt)+1;
	char* ctxt = (char*)::alloca(nLen*2+1);
	nLen = ::WideCharToMultiByte( CP_ACP, 0, txt, nLen, ctxt, nLen*2+1, NULL, NULL );
	return Parse(ctxt);
}

void Express::Bind(ExpBinder& eb,int all)
{
	void* bp;
	for(int i=m_unit_n; --i>=0;)
	{
		exp_unit& e = m_units[i];
		if(!all && e.m_var!=0)
			continue;
		bp = eb.OnBind(e.m_name,e.m_para_count);
		if( bp!=0 )
			e.m_var = (double*)bp;
	}
}

char* Express::ToString(char* str)
{
	str[0] = 0;
	if( m_units==0 )
		return str;
	str = m_units->tostring(str);
	*str = 0;
	return str;
}

static double _neg(double v){ return -v; }
static double _pos(double v){ return v; }
static double _int(double v){ return (int)v; }

static double _not(double v){ return !v; } //"!"
static double _not_n(double v){ return ~((int)v); } //"~";
static double _mul(double a,double b){ return a*b; } 
static double _div(double a,double b){ return b==0?0:a/b; } 
static double _add(double a,double b){ return a+b; } 
static double _sub(double a,double b){ return a-b; } 

static double _lt(double a,double b){ return a<b; } 
static double _lte(double a,double b){ return a<=b; } 
static double _gt(double a,double b){ return a>b; } 
static double _gte(double a,double b){ return a>=b; } 

static double _eq(double a,double b){ return a==b; } 
static double _neq(double a,double b){ return a!=b; } 

static double _and(double a,double b){ return a&&b; } 
static double _and_n(double a,double b){ return ((int)a)&((int)b); } 
static double _or(double a,double b){ return a || b; } 
static double _or_n(double a,double b){ return ((int)a)|((int)b); } 
static double _xor_n(double a,double b){ return ((int)a)^((int)b); } 
static double _rnd(){ return (double)rand()/RAND_MAX; }

static ExpBinder& std_eb()
{
	static ExpBinder eb;
	static int init = false;
	if(init) return eb;
	init = true;

	eb.Add("-",_neg);
	eb.Add("+",_pos);
	eb.Add("int",_int);
	eb.Add("!",_not);
	eb.Add("~",_not_n);
	eb.Add("*",_mul);
	eb.Add("/",_div);
	eb.Add("%",fmod); eb.Add("fmod",fmod);
	eb.Add("+",_add);
	eb.Add("-",_sub);
	eb.Add("<",_lt);
	eb.Add("<=",_lte);
	eb.Add(">",_gt);
	eb.Add(">=",_gte);
	eb.Add("=",_eq); eb.Add("==",_eq);
	eb.Add("!=",_neq);
	eb.Add("&&",_and);
	eb.Add("&",_and_n);
	eb.Add("||",_or);
	eb.Add("|",_or_n);
	eb.Add("^",_xor_n);

	eb.Add("abs",fabs);
	eb.Add("fabs",fabs);
	eb.Add("acos",acos);
	eb.Add("asin",asin);
	eb.Add("atan",atan);
	eb.Add("atan2",atan2);
	eb.Add("cos",cos);
	eb.Add("cosh",cosh);
	eb.Add("exp",exp);

	eb.Add("log",log);
	eb.Add("log10",log10);
	eb.Add("pow",pow);
	eb.Add("sin",sin);
	eb.Add("sinh",sinh);
	eb.Add("tan",tan);
	eb.Add("tanh",tanh);
	eb.Add("sqrt",sqrt);

	eb.Add("ceil",ceil);
	eb.Add("floor",floor);
	eb.Add("rand",_rnd);

	return eb;
}

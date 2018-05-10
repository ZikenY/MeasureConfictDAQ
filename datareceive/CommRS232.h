// CommRS232.h: interface for the CCommRS232 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMRS232_H__EF1005EB_3140_44CD_8A61_EAB9161C298B__INCLUDED_)
#define AFX_COMMRS232_H__EF1005EB_3140_44CD_8A61_EAB9161C298B__INCLUDED_

#include <windows.h>
#include <stdio.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommRS232  
{
	HANDLE				m_hFile;
public:
	CCommRS232();
	~CCommRS232();

	BOOL IsOpen()const { return m_hFile!=NULL; }

	BOOL Open(const char* name,int baud,const char* rw="rw");
	BOOL Close();

	void SetTimeOut(int t_read,int t_write);
	int Read(int size,void* mem);
	int Write(int size,const void* mem);

	int WriteString(const char* txt);
};

#endif // !defined(AFX_COMMRS232_H__EF1005EB_3140_44CD_8A61_EAB9161C298B__INCLUDED_)

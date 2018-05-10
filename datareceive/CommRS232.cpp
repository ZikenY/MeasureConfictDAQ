// CommRS232.cpp: implementation of the CCommRS232 class.
//
//////////////////////////////////////////////////////////////////////

#include "CommRS232.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommRS232::CCommRS232()
{
	m_hFile = NULL;
}

CCommRS232::~CCommRS232()
{
	if(m_hFile!=NULL)
		CloseHandle(m_hFile);
}

BOOL CCommRS232::Close()
{
	if(m_hFile==NULL)
		return false;
	CloseHandle(m_hFile);
	m_hFile = NULL;
	return true;
}

BOOL CCommRS232::Open(const char* name,int baud,const char* rw)
{
	if(m_hFile!=NULL)
		return false;

	DWORD mode = 0;
	if(rw[0]=='r' || rw[0]=='R' || rw[1]=='r' || rw[1]=='R')
		mode |= GENERIC_READ;
	if(rw[0]=='w' || rw[0]=='W' || rw[1]=='w' || rw[1]=='W')
		mode |= GENERIC_WRITE;

	m_hFile = ::CreateFile(name,mode,0,NULL,OPEN_EXISTING,0,NULL);
	if(m_hFile==NULL || m_hFile==INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return false;
	}

	BYTE Bbcd[28]={0x1c,0x00,0x00,0x00,0x80,0x25,0x00,0x00,
		0x11,0x30,0x00,0x00,0x00,0x00,0x00,0x08,
		0x00,0x02,0x08,0x03,0x00,0x11,0x13,0x00,
				   0x00,0x00,0x00,0x00};

	DCB dcb;

	memset(&dcb,0,sizeof(dcb));
	GetCommState(m_hFile,&dcb);
	dcb.ByteSize=0x08;
	dcb.BaudRate = baud;	
	if( !SetCommState(m_hFile,&dcb) )
	{
		Close();
		return false;
	}
	// instance an object of COMMTIMEOUTS.
    COMMTIMEOUTS comTimeOut;                   
    // Specify time-out between charactor for receiving.
    comTimeOut.ReadIntervalTimeout = 3;
    // Specify value that is multiplied 
    // by the requested number of bytes to be read. 
    comTimeOut.ReadTotalTimeoutMultiplier = 3;
    // Specify value is added to the product of the 
    // ReadTotalTimeoutMultiplier member
    comTimeOut.ReadTotalTimeoutConstant = 2;
    // Specify value that is multiplied 
    // by the requested number of bytes to be sent. 
    comTimeOut.WriteTotalTimeoutMultiplier = 3;
    // Specify value is added to the product of the 
    // WriteTotalTimeoutMultiplier member
    comTimeOut.WriteTotalTimeoutConstant = 2;
    // set the time-out parameter into device control.
    SetCommTimeouts(m_hFile,&comTimeOut);


	PurgeComm(m_hFile,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	return true;
}

void CCommRS232::SetTimeOut(int t_read,int t_write)
{
	if(m_hFile==NULL)
		return;
	DCB * pdcb;
	pdcb=new DCB;
	COMMTIMEOUTS T;
	T.ReadIntervalTimeout = 10;
	T.ReadTotalTimeoutMultiplier = 1;
	T.ReadTotalTimeoutConstant = t_read;
	T.WriteTotalTimeoutMultiplier = 10;
	T.WriteTotalTimeoutConstant = t_write;
	int iii = SetCommTimeouts(m_hFile,&T);
	GetCommState(m_hFile,pdcb);
	GetCommTimeouts(m_hFile,&T);
}

int CCommRS232::Read(int size,void* mem)
{
	DWORD len = 0;

	if(m_hFile==NULL)
		return -1;

	if( !ReadFile(m_hFile,(LPBYTE)mem, 
		size,&len ,NULL) )
		return -1;

	return len ;
}

int CCommRS232::Write(int size,const void* mem)
{
	DWORD dwWritten = 0;

	if(m_hFile==NULL)
		return -1;

	if( !WriteFile(m_hFile, (BYTE*)mem, size, &dwWritten, NULL) )
		return -1;

	return dwWritten;
}

int CCommRS232::WriteString(const char* txt)
{
	return Write( strlen(txt),txt );
}

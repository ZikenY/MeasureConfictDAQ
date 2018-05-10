// datareceive.h : main header file for the DATARECEIVE application
//

#if !defined(AFX_DATARECEIVE_H__F1DF51A6_F28B_402A_B922_EFB22BB907C7__INCLUDED_)
#define AFX_DATARECEIVE_H__F1DF51A6_F28B_402A_B922_EFB22BB907C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDatareceiveApp:
// See datareceive.cpp for the implementation of this class
//

class CDatareceiveApp : public CWinApp
{
public:
	CDatareceiveApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatareceiveApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CDatareceiveApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATARECEIVE_H__F1DF51A6_F28B_402A_B922_EFB22BB907C7__INCLUDED_)

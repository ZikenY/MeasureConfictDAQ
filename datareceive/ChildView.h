// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__3A66763B_7402_43FF_92A8_B04E9DCA897A__INCLUDED_)
#define AFX_CHILDVIEW_H__3A66763B_7402_43FF_92A8_B04E9DCA897A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommRS232.h"
#include "StringFuncs.h"
#include <list>
#include "Express.h"

using namespace std;
using namespace easymap;

/////////////////////////////////////////////////////////////////////////////
// CChildView window

const long CURVE_ORIGIN_X = 375;
const long CURVE_ORIGIN_Y = 250;
const long SENSOR_NUMBER = 6;
const long MAX_QUEUE_LENGTH = 130;
const long CURVESPACE_WIDTH = 450;

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

    //RS232 wrapper
    CCommRS232 _com;

    //thread running flag
    bool _running;

    //container for temperature data
    //each line contains 6 data which are retrieved from 6 sensors
    Strings _databuffer;
    bool savefile(const string& filename);

    //a queue of the 6 sets of values which are used to draw curve
    struct sixvalues
    {
        double ts[SENSOR_NUMBER];
    };
    list<sixvalues> _valuesqueue;

    //decipher the received data
    virtual bool _getdata();
    virtual void _extractdata(const unsigned char* databuff);

    //send command frame to the device
    bool send(long cmd, long datalength, const unsigned char* data);
    bool bindhand();
    bool unbindhand();

    //let the device to collect data
    bool startcollect();
    bool stopcollect();

    //transform voltage value to Fahrenheit
    float voltage2fahrenheit(long sensorindex, short voltage);

    //thread function
    static void g_thread(void* p);

    //show the data frame sent or received
    string _peeksent, _peekreceived;
    void peekframe(const unsigned char* data, bool send);

    //msgs of the process
    Strings _Msgs;
    void showmessage(const string& msg);

    Express _expresses[SENSOR_NUMBER];
    ExpBinder _ebs[SENSOR_NUMBER];
    double _variable_xs[SENSOR_NUMBER];
    struct Config
    {
        string port;
        string bandrate;
        string timeout;
        string filename;
        string sensorformulars[SENSOR_NUMBER];
        string samplerate;
    } config;
    void saveconfig();
    void loadconfig();
    string getnextfilename();

    void paintmessage(CPaintDC &dc);
    void paintcurve(CPaintDC &dc);

    long calcX(long number);
    long calcY(double value);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	public:
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnClearbuffer();
	afx_msg void OnSave();
	afx_msg void OnIncreaseRate();
	afx_msg void OnDecreaseRate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__3A66763B_7402_43FF_92A8_B04E9DCA897A__INCLUDED_)

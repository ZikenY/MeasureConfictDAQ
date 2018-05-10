// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "datareceive.h"
#include "ChildView.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
    _running = false;
    config.bandrate = "9600";
    config.port = "COM3";
    config.timeout = "1000";
    config.filename = "datareceived";
    config.samplerate = "0.5";


    char filename[MAX_PATH];
    GetModuleFileName(NULL, filename, MAX_PATH);
    string inifilename = RemoveExtNamePart(filename) + ".ini";
    string inifile;
    File2String(inifilename, inifile);
    string key = "port";
    ini_findkeyvalue(inifile, key, config.port);
    key = "bandrate";
    ini_findkeyvalue(inifile, key, config.bandrate);
    key = "timeout";
    ini_findkeyvalue(inifile, key, config.timeout);
    key = "filename";
    ini_findkeyvalue(inifile, key, config.filename);

    for (long i=0; i<SENSOR_NUMBER; i++)
    {
        config.sensorformulars[i] = "x";
        key = "sensorformulars" + IntToStr(i);
        ini_findkeyvalue(inifile, key, config.sensorformulars[i]);
        _expresses[i].Parse(config.sensorformulars[i].c_str());
        _ebs[i].Add("x", &(_variable_xs[i]));
        _expresses[i].Bind(_ebs[i]);
    }

    key = "samplerate";
    ini_findkeyvalue(inifile, key, config.samplerate);
}

CChildView::~CChildView()
{
    string inifile;

    string key = "port";
    ini_setkeyvalue(inifile, key, config.port);
    key = "bandrate";
    ini_setkeyvalue(inifile, key, config.bandrate);
    key = "timeout";
    ini_setkeyvalue(inifile, key, config.timeout);
    key = "filename";
    ini_setkeyvalue(inifile, key, config.filename);

    for (long i=0; i<SENSOR_NUMBER; i++)
    {
        key = "sensorformulars" + IntToStr(i);
        ini_setkeyvalue(inifile, key, config.sensorformulars[i]);
    }

    key = "samplerate";
    ini_setkeyvalue(inifile, key, config.samplerate);

    char filename[MAX_PATH];
    GetModuleFileName(NULL, filename, MAX_PATH);
    string inifilename = RemoveExtNamePart(filename) + ".ini";
    String2File(inifile, inifilename);
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_COMMAND(ID_START, OnStart)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_COMMAND(ID_CLEARBUFFER, OnClearbuffer)
	ON_COMMAND(ID_SAVE, OnSave)
	ON_WM_CLOSE()
	ON_COMMAND(ID_INCREASE_RATE, OnIncreaseRate)
	ON_COMMAND(ID_DECREASE_RATE, OnDecreaseRate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void showbuffer(CDC& dc, unsigned short number)
{
    //bit operation test
    unsigned short sensorValue = number;
    unsigned short maskhigh = 0x00ff;  //conserve low bits
    char lowbyte = maskhigh & sensorValue;
    char highbyte = sensorValue >> 8;

    unsigned short checklow = maskhigh & lowbyte;
    unsigned short checkhigh = highbyte << 8;
    unsigned short final = checkhigh | checklow;

    string totalnum = "total # in buffer: " + IntToStr(final);
    dc.TextOut(500, 10, totalnum.c_str());
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    RECT rect; rect.left=0; rect.top=0; rect.right=rect.left+1024; rect.bottom=rect.top+512;
    CBrush brush; brush.CreateSolidBrush(RGB(255,255,255));
    dc.FillRect(&rect, &brush);

    this->paintmessage(dc);
    this->paintcurve(dc);

    string samplerate = "sample rate: " + config.samplerate + " per sec";
    dc.TextOut(CURVE_ORIGIN_X+300, 420, samplerate.c_str());
}

void CChildView::paintmessage(CPaintDC &dc)
{
    while (_Msgs.GetLineCount() > 16)
    {
        _Msgs.DeleteLine(0);
    }

    CString line = ">";
    dc.TextOut(2, 10, line);

    for (long i=0; i<_Msgs.GetLineCount(); i++)
    {
        string s;
        _Msgs.GetLine(_Msgs.GetLineCount()-1-i, s);
        line = s.c_str();
        dc.TextOut(12, i*20+10, line);
    }

    dc.TextOut(12, 400, _peeksent.c_str());
    dc.TextOut(12, 423, _peekreceived.c_str());
}

void CChildView::paintcurve(CPaintDC &dc)
{
    showbuffer(dc, _databuffer.GetLineCount());

    //draw the axises
    dc.MoveTo(CURVE_ORIGIN_X, CURVE_ORIGIN_Y);
    dc.LineTo(CURVE_ORIGIN_X+CURVESPACE_WIDTH+30, CURVE_ORIGIN_Y);
    dc.MoveTo(CURVE_ORIGIN_X, CURVE_ORIGIN_Y-200);
    dc.LineTo(CURVE_ORIGIN_X, CURVE_ORIGIN_Y+200);
    dc.MoveTo(CURVE_ORIGIN_X+CURVESPACE_WIDTH+30, CURVE_ORIGIN_Y);
    dc.LineTo(CURVE_ORIGIN_X+CURVESPACE_WIDTH, CURVE_ORIGIN_Y-9);
    dc.MoveTo(CURVE_ORIGIN_X+CURVESPACE_WIDTH+30, CURVE_ORIGIN_Y);
    dc.LineTo(CURVE_ORIGIN_X+CURVESPACE_WIDTH, CURVE_ORIGIN_Y+9);
    string totalnum = "time";
    dc.TextOut(CURVE_ORIGIN_X+CURVESPACE_WIDTH+16, CURVE_ORIGIN_Y+9, totalnum.c_str());
    totalnum = "Fahrenheit";
    dc.TextOut(CURVE_ORIGIN_X, CURVE_ORIGIN_Y-200, totalnum.c_str());
//    totalnum = "0";
//    dc.TextOut(CURVE_ORIGIN_X-10, CURVE_ORIGIN_Y, totalnum.c_str());
//    totalnum = "+";
//    dc.TextOut(CURVE_ORIGIN_X-10, CURVE_ORIGIN_Y-29, totalnum.c_str());
//    totalnum = "-";
//    dc.TextOut(CURVE_ORIGIN_X-10, CURVE_ORIGIN_Y+23, totalnum.c_str());

    //prepare the scene
    long rop2saved = dc.SetROP2(R2_COPYPEN);
    COLORREF colors[SENSOR_NUMBER] = {
        RGB(0, 0, 255),
        RGB(0, 180, 100),
        RGB(255, 0, 0),
        RGB(200, 0, 200),
        RGB(50, 20, 10),
        RGB(230, 180, 25)
        };

    //draw data in _valuesqueue
    for (long j=0; j<SENSOR_NUMBER; j++)
    {
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, colors[SENSOR_NUMBER-1-j]);
        CPen* pPensaved = (CPen*)dc.SelectObject(&pen);

        string label = "T" + IntToStr(SENSOR_NUMBER-1-j) +":";
//        dc.TextOut(CURVE_ORIGIN_X+200+j*59, 362, label.c_str());
//        RECT rect; rect.left=CURVE_ORIGIN_X+225+j*59; rect.top=366; rect.right=rect.left+19; rect.bottom=rect.top+14;
        dc.TextOut(CURVE_ORIGIN_X+460, 60+j*30, label.c_str());
        RECT rect; rect.left=CURVE_ORIGIN_X+435; rect.top=65+j*30; rect.right=rect.left+19; rect.bottom=rect.top+14;
        CBrush brush; brush.CreateSolidBrush(colors[j]);
        dc.FillRect(&rect, &brush);
        brush.DeleteObject();

        long i = 0;
        list<sixvalues>::const_iterator it = _valuesqueue.begin();
        if (it != _valuesqueue.end())
        {
            string ts = SingleToStr((*it).ts[SENSOR_NUMBER-1-j]);
            dc.TextOut(CURVE_ORIGIN_X+490, 60+j*30, ts.c_str());
            dc.MoveTo(calcX(i), calcY((*it).ts[j])+90-30*j);
        }

        while (it != _valuesqueue.end())
        {
            dc.LineTo(calcX(i), calcY((*it).ts[j])+90-30*j);
            i++;
            it++;
        }

        dc.SelectObject(pPensaved);
        pen.DeleteObject();
    }

    dc.SetROP2(rop2saved);
}

//100¸öµã£¬¼ÆËãxÖáÏòÓÒµÄÎ»ÖÃ
//´ÓCURVE_ORIGIN_X¿ªÊ¼ÑÓÉìµ½ÓÒ²à500¸öÏñËØ£¬Ò»¹²100¸öµã
long CChildView::calcX(long number)
{
    //Ê×ÏÈÊÇÔÚÆÁÄ»×óÉÏ½ÇÌø¶¯£¬Õý·½Ïò

    //Ëõ·Å±ÈÀýÎªXÖáÓÐÐ§³¤¶È³ýÒÔµã¸öÊý£¬¾ùÔÈ·Ö²¼µ½XÖá
    long r = number*CURVESPACE_WIDTH/MAX_QUEUE_LENGTH;

    //Æ½ÒÆµ½origin´¦£
    return r+CURVE_ORIGIN_X;
}

long CChildView::calcY(double value)
{
    //×¢ÒâyÖáÊÇ·´ÏòµÄ
    long r = -value;//ÏÖÔÚÊÇÒÔÆÁÄ»×óÉÏ½ÇÎªoriginÌø¶¯£¬µ«·½ÏòÊÇ¶ÔµÄ

    //·ù¶È¼Ó´ó
    r = r*1.7;

    //Æ½ÒÆµ½origin´¦
    return r + CURVE_ORIGIN_Y;
}

void CChildView::showmessage(const string& msg)
{
    _Msgs.AppendLine(msg);
    this->Invalidate();
    this->UpdateWindow();
}

void CChildView::OnStart() 
{
    if (!_com.IsOpen())
    {
        showmessage("open the port first");
        return;
    }


    if (_running)
    {
        showmessage("already has started");
        return;
    }

    if (!startcollect())
    {
        showmessage("start failed. check the device.");
        return;
    }

    _running = true;
    _beginthread(g_thread, 0, this);
}

void CChildView::OnStop() 
{
    if (!_com.IsOpen())
    {
        showmessage("open the port first");
        return;
    }

    if (!_running)
    {
        showmessage("start it first");
        return;
    }

    _running = false;

    stopcollect();

    showmessage("...");
    showmessage("...");

    if (true)
    {
        showmessage("the device stopped, please save the buffer to file.");
    }
    else
    {
        showmessage("stop failed. check the device.");
    }
}

void CChildView::OnConnect() 
{
    if (_com.IsOpen())
    {
        showmessage("already opened");
        return;
    }

	if(!_com.Open(config.port.c_str(), StrToInt(config.bandrate)))
    {
        showmessage("failed to open the port!");
        return;
    }

    _com.SetTimeOut(StrToInt(config.timeout), StrToInt(config.timeout));

    if (!bindhand())
    {
        _com.Close();
        showmessage("failed to bind hands!");
        return;
    }

    showmessage("bind hands successed");
}

void CChildView::OnDisconnect()
{
    if (!_com.IsOpen())
    {
        showmessage("open the port first");
        return;
    }

    if (_running)
        this->OnStop();

    if (!unbindhand())
    {
        showmessage("failed to unbind hands!");
    }

    _com.Close();

    showmessage("port closed");
}

void CChildView::OnClearbuffer() 
{
    if (_databuffer.GetLineCount() <= 0)
    {
        showmessage("the buffer is empty");
        return;
    }

    _databuffer.SetText("");
    _valuesqueue.clear();
    showmessage("the buffer has been cleared");
}

void CChildView::OnSave() 
{
    string filename = getnextfilename();
    this->savefile(filename);
}




//working thread£¬È«¾Öº¯Êý£¬µ÷ÓÃ´«½øÀ´µÄthis->_getdata()£¬ÂÖÑ¯COM¶Ë¿Ú½ÓÊÕÊý¾Ý
void CChildView::g_thread(void* p)
{
    CChildView& me = *(CChildView*)p;

    if (!me._running)
        return;

    while (me._getdata())
    {
        if (!me._running)
        {
            break;
        }
    }

    me._running = false;
}


//receiving(working) function£¬²éÑ¯_com½ÓÊÕÊý¾Ý
bool CChildView::_getdata()
{
    unsigned char buff[18];
    ::memset(buff, 0, 18);

    long receivebytes = _com.Read(18, buff);
    //data frame has fixed length which is 18
    //retrieve the whole frame each time
    if (18 != receivebytes)
    {
//        return false;
//        _com.flush();
        return true;
    }

    peekframe(buff, false);


    //Êý¾Ý´«ÊäcmdÎª0x12
    if (buff[2] == 0x12)
    {
        //extract temperature data
        this->_extractdata(buff+3);
        Sleep(30);
    }
    else if (buff[2] == 0x11)
    {
        //set sample rate response
        showmessage("sample rate modified");
    }

    return true;
}

//transform voltage value to Fahrenheit
float CChildView::voltage2fahrenheit(long sensorindex, short voltage)
{
    _variable_xs[sensorindex] = voltage;
    return _expresses[sensorindex].Value();
}

//retrieve the 6 temperature data from the databuff
//total 12 bytes
void CChildView::_extractdata(const unsigned char* databuff)
{
    string dataline;
    sixvalues ts;

    //combine the 6 temperate data (from frame) to a line, divided by space
    for (long i=0; i<SENSOR_NUMBER; i++)
    {
        short temp = 0;
        memcpy(&temp, databuff+i*2, 2);
        float tmp = voltage2fahrenheit(i, temp);
        dataline = dataline + SingleToStr(tmp) + " ";

        ts.ts[i] = tmp;
    }

    //save it
    _databuffer.AppendLine(Trim(dataline));
    showmessage("received: " + dataline);

    _valuesqueue.push_front(ts);
    if (_valuesqueue.size() > MAX_QUEUE_LENGTH)
    {
        _valuesqueue.pop_back();
    }
}



//--------------------------------------------------
//  bindhand request:
//  0xff 0x02 0x10 0x?? 0x?? 0x?? 0x?? 0x?? 0x??
//    0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0xff 0x03
//
//  bindhand response:
//  0xff 0x02 0x11 0x?? 0x?? 0x?? 0x?? 0x?? 0x??
//    0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0xff 0x03
//--------------------------------------------------
bool CChildView::bindhand()
{
    unsigned char ccc = StrToFloat(config.samplerate)*10;//½«Ð¡Ê÷±ä³ÉÖ¤Êé

    unsigned char buff[18];
    ::memset(buff, 0, 18);
    //ÎÕÊÖÇëÇócmdÎª0x10
    if (send(0x10, 1, &ccc))
    {
        if (!this->_running)
        {
            Sleep(200);
            ::memset(buff, 0, 18);
            _com.Read(18, buff);
            peekframe(buff, false);
            this->Invalidate();
            this->UpdateWindow();
        }
    }

    //Ç°¶ËÉè±¸´«»ØµÄÎÕÊÖ»ØÖ´µÄcmdÎ»±ØÐëÎª0x11
    if (buff[2] != 0x11)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------
//  bindhand request:
//  0xff 0x02 0x12 0x?? 0x?? 0x?? 0x?? 0x?? 0x??
//    0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0xff 0x03
//
//  bindhand response:
//  0xff 0x02 0x13 0x?? 0x?? 0x?? 0x?? 0x?? 0x??
//    0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0xff 0x03
//--------------------------------------------------
bool CChildView::unbindhand()
{
    if (_running)
        return false;

    unsigned char buff[18];
    ::memset(buff, 0, 18);
    //unbandÃüÁîÎª0x12
    if (send(0x12, 0, NULL))
    {
        Sleep(200);
        ::memset(buff, 0, 18);
        _com.Read(18, buff);
        peekframe(buff, false);
        this->Invalidate();
        this->UpdateWindow();
    }

    //»ØÖ´Îª0x13
    if (buff[2] != 0x13)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------
//  start collecting: (0x20)
//  0xff 0x02 0x20 0x?? 0x?? 0x?? 0x?? 0x?? 0x??
//    0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0xff 0x03
//
//  response cmd would be 0x24
//--------------------------------------------------
bool CChildView::startcollect()
{
    unsigned char buff[18];
    ::memset(buff, 0, 18);

    if (send(0x20, 0, NULL))
    {
        Sleep(50);
        while (buff[2] != 0x24)
        {
            if (_com.Read(18, buff) <= 0)
            {
                break;
            }
        }

        peekframe(buff, false);
        this->Invalidate();
        this->UpdateWindow();
    }

    return (buff[2] == 0x24) ? true:false;
}

//--------------------------------------------------
//  start collecting: (0x21)
//  0xff 0x02 0x20 0x?? 0x?? 0x?? 0x?? 0x?? 0x??
//    0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0xff 0x03
//
//  response cmd would be 0x25
//--------------------------------------------------
bool CChildView::stopcollect()
{
    char buff[18];
    ::memset(buff, 0, 18);

    if (send(0x21, 0, NULL))
    {
        /*
        while (buff[2] != 0x25)
        {
            if (_com.Read(18, buff) <= 0)
            {
                break;
            }
        }

        peekframe(buff, false);
        this->Invalidate();
        this->UpdateWindow();
        */
    }

    return true;//(buff[2] == 0x25) ? true:false;
}


//-----------------------------------------------
//  the fixed length of the frame is 18 bytes
//
//  0xff 0x02¿ªÍ·½ô½Ó×Å1¸öbyteµÄcmd
//  12 bytes for data (from buff+3 to buff+14)
//  make buff+15 a xor varifying byte (ÎÒ³Ô±¥ÁË³ÅµÄ)
//  make 0xff 0x23 as end sigh
bool CChildView::send(long cmd, long bytelength, const unsigned char* data)
{
    //max data length is 12 bytes (6 words)
    if (bytelength > 12)
        return false;

    //fixed length 6 + data length 12
    unsigned char buff[18];
    ::memset(buff, 0, 18);

    //write header of the frame
    buff[0] = (unsigned char)0xff;
    buff[1] = (unsigned char)0x02;
    //write command(type) of the frame
    buff[2] = (char)cmd;

    if (bytelength > 0)
    {
        //write data of the frame
        ::memcpy(buff+3, data, bytelength);

        //write xor varifying part
        buff[15] = data[0];
        for (long i=1; i<bytelength; i++)
        {
            buff[15] ^= data[i];
        }
    }


    //write tail of the frame
    buff[16] = (unsigned char)0xff;
    buff[17] = (unsigned char)0x03;

    peekframe(buff, true);
    this->Invalidate();
    this->UpdateWindow();

    return 18 == _com.Write(18, buff);
}

//data must be the whole frame with fixed 18 bytes width
void CChildView::peekframe(const unsigned char* data, bool send)
{
    string peek;

    for (long i=0; i<18; i++)
    {
        char buff[10];
        memset(buff, 0, 10);
        sprintf(buff, "%02x ", (unsigned char)(data[i]));
        peek = peek + buff;
    }

    if (send)
    {
        _peeksent = "latest sent frame: " + peek;
    }
    else
    {
        _peekreceived = "latest received frame: " + peek;
    }
}

bool CChildView::savefile(const string& filename)
{
    if (_databuffer.GetLineCount() <= 0)
    {
        showmessage("the buffer is empty!");
        return false;
    }

    string txt = _databuffer.GetText();
    if (String2File(txt, filename))
    {
        showmessage("saved file: " + filename);
        return true;
    }
    else
    {
        showmessage("saved failed!!!");
        return false;
    }
}


string CChildView::getnextfilename()
{
    string temp = Trim(config.filename);
    string path = GetDirectoryPart(temp);
    if (!path.compare(""))
    {
        char d[255];
        ::GetCurrentDirectory(255, d);
        path = string(d);
    }

    string ext = GetExtNamePart(temp);
    if (!ext.compare(""))
    {
        ext = "txt";
    }

    temp = RemoveExtNamePart(RemoveDirectoryPart(temp));

    while (1)
    {
        string filetest = path + "\\" + temp + "_" + IntToStr(::GetTickCount()) + "." + ext;

        WIN32_FIND_DATA FileData;
        HANDLE h = ::FindFirstFile(filetest.c_str(), &FileData);
        if (h == INVALID_HANDLE_VALUE)
        {
            temp = filetest;
            break;
        }
        else
        {
            ::CloseHandle(h);
        }
    }

    return temp;
}

void CChildView::OnDecreaseRate() 
{
    float samplerate = StrToFloat(config.samplerate);
    if (samplerate > 59.9)
    {
        samplerate = 60;
    }
    else
    {
        samplerate = samplerate + 0.1;
    }

    config.samplerate = FloatToStr(samplerate);
    this->bindhand();
}

void CChildView::OnIncreaseRate() 
{
    float samplerate = StrToFloat(config.samplerate);
    if (samplerate < 0.2)
    {
        samplerate = (float)0.1;
    }
    else
    {
        samplerate = samplerate - 0.1;
    }

    config.samplerate = FloatToStr(samplerate);
    this->bindhand();
}


// TestFontCacheDlg.cpp : implementation file
//

#include "stdafx.h"
#include <iterator>
#include "TestFontCache.h"
#include "TestFontCacheDlg.h"
#include "afxdialogex.h"
#include "RandomFontRequester.h"
#include "SimpleFontProvider.h"
#include "FontEnumerator.h"
#include "FontCache.h"
#include "StopWatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <regex>

namespace std { using tstring = basic_string<TCHAR>; }
namespace std { using tregex = basic_regex<TCHAR>; }
namespace std { using tmatch = match_results<tstring::const_iterator>; }

bool get_pe_name()
{
	std::tstring exports;
	static const std::tregex reg_exports(_T(R"(Section contains the following exports for (\S.*))"));
	std::tmatch match;
	if (!std::regex_search(exports, match, reg_exports))
		return false;

	return true;
}


// CTestFontCacheDlg dialog


CTestFontCacheDlg::CTestFontCacheDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTFONTCACHE_DIALOG, pParent)
	, m_fontProvider(new SimpleFontProvider{})
	, m_cacheSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestFontCacheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_REQUESTS, m_editRequestsNumber);
	DDX_Control(pDX, IDC_EDIT_TIME, m_editTime);
	DDX_Control(pDX, IDC_RADIO_NOCACHE, m_buttonNone);
	DDX_Control(pDX, IDC_EDIT_FONTS_NUMBER, m_editNumberOfFonts);
	DDX_Control(pDX, IDC_CHECK_ALLFONTS, m_checkAllFonts);
	DDX_Control(pDX, IDC_SPIN_FONTS_NUMBER, m_spinFontsNumber);
	DDX_Control(pDX, IDC_SPIN_CACHE_SIZE, m_spinCacheSize);
	DDX_Control(pDX, IDC_EDIT_CACHE_SIZE, m_editCacheSize);
	DDX_Text(pDX, IDC_EDIT_CACHE_SIZE, m_cacheSize);
	DDX_Control(pDX, IDC_EDIT_TOTAL_FONTS, m_editTotalFonts);
	DDX_Control(pDX, IDC_SPIN_SPIN_REQUESTS, m_spinRequests);
}

BEGIN_MESSAGE_MAP(CTestFontCacheDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CTestFontCacheDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_CHECK_ALLFONTS, &CTestFontCacheDlg::OnBnClickedCheckAllfonts)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_NOCACHE, IDC_RADIO_UNORDERED_MAP, &CTestFontCacheDlg::OnCacheTypeChanged)
	ON_EN_CHANGE(IDC_EDIT_CACHE_SIZE, &CTestFontCacheDlg::OnEnChangeEditCacheSize)
END_MESSAGE_MAP()


// CTestFontCacheDlg message handlers

BOOL CTestFontCacheDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CPaintDC dc(this);
	FontEnumerator fe{ dc.m_hDC };
	m_allFontNames = fe.GetFontNames();

	m_editRequestsNumber.SetWindowText(_T("1000"));
	m_spinRequests.SetRange32(1, 10000);
	CString numberOfFonts;
	numberOfFonts.Format(_T("(%d)"), m_allFontNames.size());
	m_editTotalFonts.SetWindowText(numberOfFonts);
	m_editNumberOfFonts.SetWindowText(_T("5"));
	m_spinFontsNumber.SetRange32(1, m_allFontNames.size());
	m_editCacheSize.SetWindowText(_T("20"));
	m_spinCacheSize.SetRange32(1, m_allFontNames.size());
	m_buttonNone.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestFontCacheDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestFontCacheDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestFontCacheDlg::OnBnClickedButtonRun()
{
	std::vector<CString> selectedFonts;
	if (m_checkAllFonts.GetCheck() != BST_CHECKED)
	{
		CString str;
		m_editNumberOfFonts.GetWindowText(str);
		int numberOfFonts = _ttol(str);
		std::copy_n(m_allFontNames.begin(), numberOfFonts, std::back_inserter(selectedFonts));
	}
	else
		selectedFonts = m_allFontNames;

	CString str;
	m_editRequestsNumber.GetWindowText(str);
	int numberOfRequests = _ttol(str);

	unsigned seed = 3;

	RandomFontRequester rfr(selectedFonts, m_fontProvider.get());

	Stopwatch sw;
	rfr.RequestFonts(numberOfRequests, seed);

	auto time = sw.Stop();
	str.Format(_T("%f"), time.count());
	m_editTime.SetWindowText(str);
}

void CTestFontCacheDlg::OnBnClickedCheckAllfonts()
{
	m_editNumberOfFonts.EnableWindow(m_checkAllFonts.GetCheck() != BST_CHECKED);
}

void CTestFontCacheDlg::OnCacheTypeChanged(UINT nID)
{
	switch (nID)
	{
	case IDC_RADIO_NOCACHE:
		m_fontProvider.reset(new SimpleFontProvider{});
		m_editCacheSize.EnableWindow(FALSE);
		m_spinCacheSize.EnableWindow(FALSE);
		break;
	case IDC_RADIO_UNORDERED_MAP:
		m_fontProvider.reset(new FontCache(m_cacheSize));
		m_editCacheSize.EnableWindow(TRUE);
		m_spinCacheSize.EnableWindow(TRUE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CTestFontCacheDlg::OnEnChangeEditCacheSize()
{
	if (m_editCacheSize.GetSafeHwnd())
	{
		UpdateData();
		m_fontProvider->Resize(m_cacheSize);
	}
}


// TestFontCacheDlg.h : header file
//

#pragma once
#include <vector>
#include <memory>
#include "afxwin.h"
#include "afxcmn.h"
#include "FontProvider.h"

// CTestFontCacheDlg dialog
class CTestFontCacheDlg : public CDialogEx
{
// Construction
public:
	CTestFontCacheDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTFONTCACHE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedCheckAllfonts();
	afx_msg void OnCacheTypeChanged(UINT);
	afx_msg void OnEnChangeEditCacheSize();
	DECLARE_MESSAGE_MAP()
private:
	CEdit m_editRequestsNumber;
	CSpinButtonCtrl m_spinRequests;
	CEdit m_editTime;
	CButton m_buttonNone;
	CButton m_checkAllFonts;
	CEdit m_editTotalFonts;
	CEdit m_editNumberOfFonts;
	CSpinButtonCtrl m_spinFontsNumber;
	CEdit m_editCacheSize;
	CSpinButtonCtrl m_spinCacheSize;

	std::vector<CString> m_allFontNames;
	std::unique_ptr<FontProvider> m_fontProvider;
	long m_cacheSize{ 20 };
};

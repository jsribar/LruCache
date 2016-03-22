
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
	DECLARE_MESSAGE_MAP()
private:
	CEdit m_editRequestsNumber;
	CEdit m_editTime;
	CButton m_buttonNone;
	CEdit m_editNumberOfFonts;
	CButton m_checkAllFonts;

	std::vector<CString> m_allFontNames;
	CSpinButtonCtrl m_spinFontsNumber;
	std::unique_ptr<FontProvider> m_fontProvider;
};

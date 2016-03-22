#pragma once
#include "d:\Projects\AVL\Concerto_V50\Concerto_V50\ConcertoCommon\SvTools\TOOLS\INC\LIST.HXX"
#include "FontProvider.h"

class CFontInfo
{
public:
	CFont *m_pFont;
	DWORD  m_nTickCount;
	int    m_nHeight;

	CFontInfo(CFont* pFont, int nHeight) : m_pFont(pFont), m_nHeight(nHeight)
	{
		m_nTickCount = ::GetTickCount();
	}
	~CFontInfo()
	{
		if (m_pFont)
		{
			if (m_pFont->m_hObject)
				m_pFont->DeleteObject();
			delete m_pFont;
		}
	}
};

DECLARE_LIST(CFontInfoList, CFontInfo*);

class FontMgr : public FontProvider
{
	CFontInfoList m_FontInfoList;
public:
	FontMgr() {}
	~FontMgr();
	CFont *GetFont(const LPLOGFONT pLF);
	void   DeleteOldFonts(ULONG aException = LIST_ENTRY_NOTFOUND);

	CFont* ProvideFont(LOGFONT* lf) override;
};


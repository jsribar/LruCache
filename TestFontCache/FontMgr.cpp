#include "stdafx.h"
#include "FontMgr.h"

FontMgr::~FontMgr()
{
	for (ULONG i = 0; i < m_FontInfoList.Count(); i++)
	{
		CFontInfo *pFontInfo = m_FontInfoList.GetPObject(i);
		delete pFontInfo;
	}
	m_FontInfoList.Clear();
}


CFont* FontMgr::GetFont(const LPLOGFONT pLF)
{
	CFont *pFont = NULL;

	int start = 0;
	int end = m_FontInfoList.Count();
	int pos = end / 2;
	if (end)
		end--;

	// find start of sequence of fonts with the height larger or equal to the height of the new font
	while (pos > start || pos < end)
	{
		int height = m_FontInfoList.GetPObject(pos)->m_nHeight;
		if (pLF->lfHeight <= height)
			end = pos;
		else
			start = pos + 1;
		pos = start + (end - start) / 2;
	}

	// search if font exists already in the list
	while (pos < m_FontInfoList.Count())
	{
		CFontInfo* pFontInfo = m_FontInfoList.GetPObject(pos);
		pFont = pFontInfo->m_pFont;
		if (pFont->m_hObject)
		{
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			if (lf.lfHeight == pLF->lfHeight)
			{
				if (lf.lfWeight == pLF->lfWeight &&
					!_tcscmp(lf.lfFaceName, pLF->lfFaceName) &&
					lf.lfEscapement == pLF->lfEscapement &&
					lf.lfItalic == pLF->lfItalic &&
					lf.lfOrientation == pLF->lfOrientation &&
					lf.lfUnderline == pLF->lfUnderline)
				{
					// font is found: update its counter and return it
					pFontInfo->m_nTickCount = GetTickCount();
					return pFont;
				}
			}
			else
				break;
		}
		else
		{
			ASSERT(FALSE);
			delete pFontInfo;
			m_FontInfoList.Remove(pos);
			pos--;
		}
		pos++;
	}

	// font isn't in the list
	pFont = new CFont;
	if (!pFont->CreateFontIndirect(pLF))
	{
		delete pFont;
		return NULL;
	}

	m_FontInfoList.Insert(new CFontInfo(pFont, pLF->lfHeight), pos);

	if (m_FontInfoList.Count() > 200)
		DeleteOldFonts(pos);

	return pFont;
}

void FontMgr::DeleteOldFonts(ULONG aException)
{
	if (m_FontInfoList.Count() < 50)
		return;

	DWORD Threshold = GetTickCount();

	if (Threshold <= 60000)
		return;

	Threshold -= 60000;

	ULONG i = m_FontInfoList.Count();
	while (i > 0)
	{
		--i;
		CFontInfo* pFontInfo = m_FontInfoList.GetPObject(i);

		if (pFontInfo->m_nTickCount < Threshold && i != aException)
		{
			delete pFontInfo;
			m_FontInfoList.Remove(i);
		}
	}
}

CFont* FontMgr::ProvideFont(LOGFONT* lf)
{
	return GetFont(lf);
}

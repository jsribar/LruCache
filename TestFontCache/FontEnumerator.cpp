#include "stdafx.h"
#include "FontEnumerator.h"


BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID pFontNames)
{
	std::vector<CString>* fontNames = static_cast<std::vector<CString>*>(pFontNames);

	// Record the number of raster, TrueType, and vector  
	// fonts in the font-count array.  

	if (FontType & TRUETYPE_FONTTYPE)
	{
		fontNames->push_back(lplf->lfFaceName);
	}
	return TRUE;
}

FontEnumerator::FontEnumerator(HDC hdc)
{
	EnumFontFamilies(hdc, (LPCTSTR)NULL, (FONTENUMPROC)EnumFamCallBack, (LPARAM)(&m_fontNames));
}

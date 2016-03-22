#pragma once

#include "../TimedCache/LruCache.h"
#include "FontProvider.h"
#include <afxwin.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

//if (lf.lfHeight == pLF->lfHeight)
//{
//	if (lf.lfWeight == pLF->lfWeight &&
//		!_tcscmp(lf.lfFaceName, pLF->lfFaceName) &&
//		lf.lfEscapement == pLF->lfEscapement &&
//		lf.lfItalic == pLF->lfItalic &&
//		lf.lfOrientation == pLF->lfOrientation &&
//		lf.lfUnderline == pLF->lfUnderline)
//	{

namespace std
{
	template<>
	struct hash<LOGFONT>
	{
		size_t operator() (const LOGFONT& lf) const
		{
			return lf.lfHeight ^ lf.lfEscapement ^ lf.lfOrientation ^
				lf.lfWeight >> 3 ^ lf.lfItalic ^ (lf.lfUnderline >> 1) ^
				hash<_tstring>()(lf.lfFaceName);
		}
	};

	bool operator==(const LOGFONT& left, const LOGFONT& right)
	{
		return left.lfHeight == right.lfHeight &&
			left.lfEscapement == right.lfEscapement &&
			left.lfOrientation == right.lfOrientation &&
			left.lfWeight == right.lfWeight &&
			left.lfItalic == right.lfItalic &&
			left.lfUnderline == right.lfUnderline &&
			_tcscmp(left.lfFaceName, right.lfFaceName) == 0;
	}
}

struct FontGenerator
{
	CFont* operator() (LOGFONT lf) const
	{
		CFont* fnt = new CFont;
		fnt->CreateFontIndirect(&lf);
		return fnt;
	}
};

class FontCache : public LruCache<CFont*, LOGFONT, FontGenerator>, public FontProvider
{
public:
	FontCache(int cleanupThreshold = 50, size_t maxLifetime = 60000, size_t cleanUpPeriod = 20000) : LruCache<CFont*, LOGFONT, FontGenerator>(FontGenerator(), cleanupThreshold, maxLifetime, cleanUpPeriod)
	{
	}

	CFont* ProvideFont(LOGFONT* lf) override
	{
		return GetItem(*lf);
	}
};
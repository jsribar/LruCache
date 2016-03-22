#include "stdafx.h"
#include <random>
#include "RandomFontRequester.h"
#include "FontProvider.h"

RandomFontRequester::RandomFontRequester(const std::vector<CString> fontNames, FontProvider* provider) : m_fontNames(fontNames), m_provider(provider)
{
}

RandomFontRequester::~RandomFontRequester()
{
}

void RandomFontRequester::RequestFonts(int requestsNumber, unsigned seed)
{
	m_generator.seed(seed);
	while (--requestsNumber >= 0)
	{
		LOGFONT lf{ 0 };
		lf.lfHeight = -10; // GetRandomHeight(8, 17);
		lf.lfWeight = FW_NORMAL; // GetRandomWeight();
		//lf.lfItalic = GetRandomItalic();
		_tcscpy_s(lf.lfFaceName, GetRandomFontName().GetString());
		m_provider->ProvideFont(&lf);
	}
}

const CString& RandomFontRequester::GetRandomFontName()
{
	return m_fontNames.at(m_generator() % m_fontNames.size());
}

LONG RandomFontRequester::GetRandomHeight(int from, int to)
{
	return from + m_generator() % (to - from);
}

LONG RandomFontRequester::GetRandomWeight()
{
	return m_generator() % 2 ? FW_NORMAL : FW_BOLD;
}

BOOL RandomFontRequester::GetRandomItalic()
{
	return m_generator() % 2 ? TRUE : FALSE;
}

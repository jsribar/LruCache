#include "stdafx.h"
#include "SimpleFontProvider.h"


SimpleFontProvider::SimpleFontProvider()
{
}


SimpleFontProvider::~SimpleFontProvider()
{
}

CFont* SimpleFontProvider::ProvideFont(LOGFONT* lf)
{
	font = std::make_unique<CFont>();
	font->CreateFontIndirect(lf);
	return font.get();
}

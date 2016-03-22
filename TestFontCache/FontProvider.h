#pragma once

class FontProvider
{
public:
	virtual CFont* ProvideFont(LOGFONT* lf) = 0;
};
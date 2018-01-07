#pragma once

class FontProvider
{
public:
	virtual CFont* ProvideFont(LOGFONT* lf) = 0;
	virtual void Resize(size_t newSize) {};
};
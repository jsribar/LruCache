#pragma once
#include <memory>
#include "FontProvider.h"

class SimpleFontProvider : public FontProvider
{
public:
	SimpleFontProvider();
	~SimpleFontProvider();

	CFont* ProvideFont(LOGFONT* lf) override;

private:
	std::unique_ptr<CFont> font;
};


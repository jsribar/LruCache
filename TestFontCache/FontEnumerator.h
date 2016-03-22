#pragma once
#include <vector>

class FontEnumerator
{
public:
	FontEnumerator(HDC hdc);

	const std::vector<CString>& GetFontNames() const { return m_fontNames; }

private:
	std::vector<CString> m_fontNames;
};


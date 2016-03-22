#pragma once
#include <vector>
#include <random>

class FontProvider;

class RandomFontRequester
{
public:
	RandomFontRequester(const std::vector<CString> fontNames, FontProvider* provider);
	~RandomFontRequester();

	void RequestFonts(int requestsNumber, unsigned seed);

private:
	std::minstd_rand0 m_generator;
	FontProvider* m_provider;
	const std::vector<CString> m_fontNames;

	const CString& GetRandomFontName();
	LONG GetRandomHeight(int from, int to);
	LONG GetRandomWeight();
	BOOL GetRandomItalic();
};


#ifndef FONT_H
#define FONT_H

#include "global.h"
#include "PngLoader.h"

struct SCoords
{
    int miLeft, miTop, miRight, miBottom;

    SCoords(int aLeft = 0, int aUp = 0, int aRight = 0, int aDown = 0) : miLeft(aLeft), miTop(aUp), miRight(aRight), miBottom(aDown) {}
};

struct SCharacter
{
    char mcValue;
    SCoords mKerning;
    SCoords mTexCoords;

    SCharacter() {}
};

class CFontScript
{
    friend class CFont;

    CFontScript(const CFontScript&) {}
    CFontScript& operator=(const CFontScript&) { return *this; }
public:
    CFontScript();
    ~CFontScript();

    void ReadScript(std::string aPath);
    void WriteScript(std::string aPath, bool aExample = false);

protected:
    std::vector<SCharacter> mChars; //!< Alpha-Numerics of the Font.
    GLuint mTexId; //!< Texture ID.
    std::string mPngPath;
    size_t miHexColor;

private:
    void ParseLine(std::string aLine);
};

class CFont
{
    friend class CFontScript;

public:
    CFont();
    CFont(const CFont& aCls);
    ~CFont();

    CFont& operator=(const CFont& aCls);

    void Load(std::string aPath);
    void Save(std::string aPath, std::string aPngPath);

    void DrawString(std::string aStr, size_t aX, size_t aY, size_t aSizePx);
    void DrawString(long aNum, size_t aX, size_t aY, size_t aSizePx);

    void SetColor_F(float aRed, float aGreen, float aBlue);
    void SetColor_H(long aHexValue);

    void AddCharacter(SCharacter aChar);

private:
    CFontScript *mpScript;

    float mfRed;
    float mfGreen;
    float mfBlue;

    size_t miWidth;
    size_t miHeight;
};

#endif // FONT_H

#include <set>
#include <fstream>
#include <sstream>
#include "Font.h"

void IntToStr(int aiNumber, char *apCharBuffer, size_t aiBufferSize)
{
    // Validate arguments.
    if (nullptr == apCharBuffer)
    {
        apCharBuffer = new char[aiBufferSize];
    }

    if((aiBufferSize <= 0) || (apCharBuffer == NULL))
    {
        if((aiBufferSize > 0) && (apCharBuffer != NULL))
        {
            *apCharBuffer = '\0';
        }

        std::cout<< std::endl<< "IntToChar: UNABLE TO PARSE INPUT! [N/A]"<< std::endl;
        return;
    }

    snprintf(apCharBuffer, aiBufferSize, "%d", aiNumber);
}

size_t CharToDecimal(char aChar)
{
    return aChar >= '0' && aChar <= '9' ? aChar - '0' : 10;
}

size_t StrToHex(std::string aStr)
{
    std::stringstream sStrm(aStr);
    int iRtn = 0;

    sStrm >> std::hex >> iRtn;

    return iRtn;
}

CFontScript::CFontScript() : mTexId(0), mPngPath(""), miHexColor(0x000000) {}

CFontScript::~CFontScript()
{
    mChars.clear();
}

void CFontScript::ReadScript(std::string aPath)
{
    if (!aPath.empty())
    {
        std::fstream fStrm;

        fStrm.open(aPath.data(), std::ios_base::in);

        if (fStrm.good())
        {
            char *pLine = new char[4096];

            while (!fStrm.eof())
            {
                fStrm.getline(pLine, 4096, '\n');

                if (pLine[0] == '<')
                {
                    ParseLine(pLine);
                }
                else if (pLine[0] == 'P')
                {
                    char *pValue = strtok(pLine, "\"");

                    pValue = strtok(nullptr, "\"");
                    mPngPath = pValue;
                }
                else if (pLine[0] == 'D')
                {
                    char *pValue = strtok(pLine, "=");

                    pValue = strtok(nullptr, "\n");
                    miHexColor = StrToHex(pValue);
                }
                else if (!strcmp(pLine, "EOF"))
                {
                    break;
                }
            }

            delete[] pLine;
        }
    }
}

void CFontScript::WriteScript(std::string aPath, bool aExample)
{
    if (!aPath.empty())
    {
        if (aExample)
        {
            // Write the example script.
            FILE *pF = fopen(aPath.data(), "w+");

            if (pF)
            {
                // The format is:
                // <char>[kerning left, up, right, down]{texcoord left, top, right, bottom}
                // For this example, all kerning values equal zero and the texture coords going right increase by 20.

                fwrite("# BEGIN EXAMPLE\n", strlen("# BEGIN EXAMPLE\n"), 1, pF);
                fwrite("Png=\"Example.png\"\n\n", strlen("Png=\"Example.png\"\n\n"), 1, pF);
                fwrite("# ALPHA-NUMERICS\n", strlen("# ALPHA-NUMERICS\n"), 1, pF);

                size_t iHorzCoord = 0;
                const size_t iWidth = 20;
                for (char iChar = ' '; iChar <= '~'; ++iChar)
                {
                    char *pTmp = new char[5];
                    IntToStr(iHorzCoord, pTmp, 10);

                    std::string sLine = "<";
                    sLine += iChar;
                    sLine.append(">[0,0,0,0]{");
                    sLine.append(pTmp);
                    sLine.append(",0,");

                    iHorzCoord += iWidth;
                    IntToStr(iHorzCoord, pTmp, 10);

                    sLine.append(pTmp);
                    sLine.append(",");

                    IntToStr(iWidth, pTmp, 10);

                    sLine.append(pTmp);
                    sLine.append("}\n");

                    fwrite(sLine.data(), sLine.size(), 1, pF);

                    delete pTmp;
                }

                fwrite("\n# DEFAULT COLOR - RED\n", strlen("\n# DEFAULT COLOR - RED\n"), 1, pF);
                fwrite("DColor=FF0000\n\n", strlen("DColor=FF0000\n\n"), 1, pF);

                fwrite("# END-OF-FILE\n", strlen("# END-OF-FILE\n"), 1, pF);
                fwrite("EOF\n", strlen("EOF\n"), 1, pF);

                fclose(pF);
            }
        }
        else
        {
            // Write the script.
            FILE *pF = fopen(aPath.data(), "w+");

            if (pF)
            {
                fwrite("# BEGIN FONT SCRIPT\n", strlen("# BEGIN FONT SCRIPT\n"), 1, pF);

                std::string path = "Png=\"";
                path.append(mPngPath);
                path.append("\"\n\n");

                fwrite(path.data(), path.length(), 1, pF);
                fwrite("# ALPHA-NUMERICS\n", strlen("# ALPHA-NUMERICS\n"), 1, pF);

                for (size_t iIdx = 0; iIdx < mChars.size(); ++iIdx)
                {
                    SCharacter sChar = mChars.at(iIdx);

                    const size_t iBuffSize = 10;
                    char *pTmp = new char[iBuffSize];
                    IntToStr(sChar.mKerning.miLeft, pTmp, iBuffSize);

                    std::string sLine = "<";
                    sLine += sChar.mcValue;
                    sLine.append(">");
                    sLine.append("[0,0,0,0]");
//                    sLine.append(">[");
//                    sLine.append(pTmp);
//                    sLine.append(",");
//                    IntToStr(sChar.mKerning.miTop, pTmp, iBuffSize);
//                    sLine.append(pTmp);
//                    sLine.append(",");
//                    IntToStr(sChar.mKerning.miRight, pTmp, iBuffSize);
//                    sLine.append(pTmp);
//                    sLine.append(",");
//                    IntToStr(sChar.mKerning.miBottom, pTmp, iBuffSize);
//                    sLine.append(pTmp);
//                    sLine.append("]");
                    sLine.append("{");
                    IntToStr(sChar.mTexCoords.miLeft, pTmp, iBuffSize);
                    sLine.append(pTmp);
                    sLine.append(",");
                    IntToStr(sChar.mTexCoords.miTop, pTmp, iBuffSize);
                    sLine.append(pTmp);
                    sLine.append(",");
                    IntToStr(sChar.mTexCoords.miRight, pTmp, iBuffSize);
                    sLine.append(pTmp);
                    sLine.append(",");
                    IntToStr(sChar.mTexCoords.miBottom, pTmp, iBuffSize);
                    sLine.append(pTmp);
                    sLine.append("}\n");

                    fwrite(sLine.data(), sLine.size(), 1, pF);

                    delete[] pTmp;
                }

                const size_t c_iBuffSize = 16;
                char *pTmp = new char[c_iBuffSize]; // RRGGBB
                snprintf(pTmp, c_iBuffSize, "DColor=%X\n\n", miHexColor);

                fwrite("\n# DEFAULT COLOR\n", strlen("\n# DEFAULT COLOR\n"), 1, pF);
                fwrite(pTmp, strlen(pTmp), 1, pF);

                fwrite("# END-OF-FILE\n", strlen("# END-OF-FILE\n"), 1, pF);
                fwrite("EOF\n", strlen("EOF\n"), 1, pF);

                delete[] pTmp;

                fclose(pF);
            }
        }
    }
}

void CFontScript::ParseLine(std::string aLine)
{
    if (!aLine.empty())
    {
        SCharacter sChar;
        std::string delimeters = "<>[]{},";

        char *pStr = strtok((char*)aLine.data(), delimeters.data());
        sChar.mcValue = *pStr;

        pStr = strtok(NULL, delimeters.data());
        sChar.mKerning.miLeft = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mKerning.miTop = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mKerning.miRight = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mKerning.miBottom = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mTexCoords.miLeft = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mTexCoords.miTop = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mTexCoords.miRight = atoi(pStr);

        pStr = strtok(NULL, delimeters.data());
        sChar.mTexCoords.miBottom = atoi(pStr);

        mChars.push_back(sChar);
    }
}

CFont::CFont() : mpScript(nullptr), mfRed(0.0), mfGreen(0.0), mfBlue(0.0), miWidth(0), miHeight(0) {}

CFont::CFont(const CFont &aCls) : mpScript(aCls.mpScript), mfRed(aCls.mfRed), mfGreen(aCls.mfGreen), mfBlue(aCls.mfBlue), miWidth(aCls.miWidth), miHeight(aCls.miHeight) {}

CFont::~CFont()
{
    if (nullptr != mpScript)
    {
        delete mpScript;
    }
}

CFont& CFont::operator =(const CFont& aCls)
{
    if (this != &aCls)
    {
        mpScript = aCls.mpScript;
        mfRed = aCls.mfRed;
        mfGreen = aCls.mfGreen;
        mfBlue = aCls.mfBlue;
        miWidth = aCls.miWidth;
        miHeight = aCls.miHeight;
    }

    return *this;
}

void CFont::Load(std::string aPath)
{
    if (nullptr == mpScript)
    {
        mpScript = new CFontScript();
    }

    CPng png;
    mpScript->ReadScript(aPath);

    if (png.LoadPng(mpScript->mPngPath))
    {
        glGenTextures(1, &mpScript->mTexId);
        glBindTexture(GL_TEXTURE_2D, mpScript->mTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

#if !defined(USE_GLU)
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, png.miWidth, png.miHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, png.mpData);
#endif
#if defined(USE_GLU)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, png.miWidth, png.miHeight, GL_RGBA, GL_UNSIGNED_BYTE, png.mpData);
#endif

        miWidth = png.miWidth;
        miHeight = png.miHeight;
    }
}

void CFont::Save(std::string aPath, std::string aPngPath)
{
    if (nullptr == mpScript)
    {
        mpScript = new CFontScript();
    }

    if (!aPath.empty())
    {
        mpScript->mPngPath = aPngPath;
        mpScript->WriteScript(aPath, false);
    }
}

void CFont::SetColor_F(float aRed, float aGreen, float aBlue)
{
    mfRed = aRed;
    mfGreen = aGreen;
    mfBlue = aBlue;
}

void CFont::SetColor_H(long aHexValue)
{
    float fRed = aHexValue & 0xFF0000;
    float fGreen = aHexValue & 0x00FF00;
    float fBlue = aHexValue & 0x0000FF;

    mfRed = fRed / 0xFF;
    mfGreen = fGreen / 0xFF;
    mfBlue = fBlue / 0xFF;
}

void CFont::DrawString(long aNum, size_t aX, size_t aY, size_t aSizePx)
{
    char *pTmp = new char[4096];
    snprintf(pTmp, 4096, "%ld", aNum);
    DrawString(pTmp, aX, aY, aSizePx);
}

void CFont::DrawString(std::string aStr, size_t aX, size_t aY, size_t aSizePx)
{
    if (!aStr.empty())
    {
        /*
         * Each character is mapped to it's appropriate ASCII value, so no searching for characters is neccessary. The drawing
         * algorithm does however, need to take into account the kerning before drawing the geometry. This is done by either
         * adding (+) or subtracting (-) the kerning value to/from the geometry coordinates. Each new letter should also move
         * the geometry width in pixels from the last kerned coordinate.
         */

        SCharacter sChar; // The current character object.
        char cChar = '\0'; // The current character.
        SRect sGeoCoords; // The current geometry coordinates.
        SRect sTexCoords; // The current texture coordinates.

        // Set the initial geometry.
        sGeoCoords.miX = aX;
        sGeoCoords.miY = aY;
        sGeoCoords.miX2 = aX + aSizePx;
        sGeoCoords.miY2 = aY + aSizePx;

        for (size_t iIdx = 0; iIdx < aStr.size(); ++iIdx)
        {
            cChar = aStr.at(iIdx);

            if (' ' <= cChar && '~' >= cChar)
            {
                // The character is good to use.
                sChar = mpScript->mChars.at(cChar - ' ');

                if (sChar.mcValue == cChar)
                {
                    // Kern the geometry.
                    if (0 == iIdx)
                    {
                        sGeoCoords.miX2 += sChar.mKerning.miLeft + sChar.mKerning.miRight;
                        sGeoCoords.miY  += sChar.mKerning.miTop;
                        sGeoCoords.miY2 += sChar.mKerning.miBottom;
                    }
                    else
                    {
                        sGeoCoords.miX  += sChar.mKerning.miLeft;
                        sGeoCoords.miX2 += sChar.mKerning.miRight;
                        sGeoCoords.miY  += sChar.mKerning.miTop;
                        sGeoCoords.miY2 += sChar.mKerning.miBottom;
                    }

                    // Setup texture coordinates.
                    sTexCoords.miX = sChar.mTexCoords.miLeft / miWidth;
                    sTexCoords.miY = sChar.mTexCoords.miTop / miHeight;
                    sTexCoords.miX2 = sChar.mTexCoords.miRight / miWidth;
                    sTexCoords.miY2 = sChar.mTexCoords.miBottom / miHeight;

                    // We can now draw and texture the geometry.
                    glColor3f(mfRed, mfGreen, mfBlue);
                    glBindTexture(GL_TEXTURE_2D, mpScript->mTexId);
                    glBegin(GL_QUADS);
                        glTexCoord2i(sTexCoords.miX, sTexCoords.miY);
                        glVertex2i(sGeoCoords.miX, sGeoCoords.miY);

                        glTexCoord2i(sTexCoords.miX, sTexCoords.miY2);
                        glVertex2i(sGeoCoords.miX, sGeoCoords.miY2);

                        glTexCoord2i(sTexCoords.miX2, sTexCoords.miY2);
                        glVertex2i(sGeoCoords.miX2, sGeoCoords.miY2);

                        glTexCoord2i(sTexCoords.miX2, sTexCoords.miY);
                        glVertex2i(sGeoCoords.miX2, sGeoCoords.miY);
                    glEnd();

                    sGeoCoords.miX += aSizePx;
                    sGeoCoords.miX2 += aSizePx;
                }
            }
        }
    }
}

void CFont::AddCharacter(SCharacter aChar)
{
    if (nullptr == mpScript)
    {
        mpScript = new CFontScript();
    }

    mpScript->mChars.push_back(aChar);
}

#include "spritescript.h"

//#define Size(x) strlen(x)

size_t StringToSizeT(char *pNumber)
{
    size_t iRtn = 0;

    if (nullptr != pNumber)
    {
        size_t iNumDigits = strlen(pNumber);
        iRtn = 0;

        for(size_t iIdx = 0; iIdx < iNumDigits; ++iIdx)
        {
            char cTmp = pNumber[iIdx] - '0';

            if (cTmp >= 0 && cTmp <= 9)
            {
                if (0 != cTmp)
                {
                    iRtn *= 10;
                    iRtn += static_cast<size_t>(cTmp);
                }
                else if (iIdx > 0 && iIdx < iNumDigits)
                {
                    iRtn *= 10;
                }
            }
        }
    }

    return iRtn;
}

bool StringHasChar(std::string aStr, char aChar)
{
    return aStr.find(aChar) != std::string::npos ? true : false;
}

char *DeleteChars(char *aData, size_t iStartPos = 0, size_t iAmount = 1)
{
    size_t iSize = strlen(aData);
    char *pRtn = new char[iSize+1];
    memset(pRtn, 0, iSize);

    size_t iIdx = 0;
    for(iIdx = iStartPos; iIdx < strlen(aData); ++iIdx)
    {
        pRtn[iIdx - iStartPos] = aData[iIdx];
    }

    ++iIdx;
    pRtn[iIdx - iStartPos] = '\0';

    memcpy(aData, pRtn, iSize);
    delete[] pRtn;
    return aData;
}

std::string GetPath(std::string aPath)
{
    if (!aPath.empty())
    {
        size_t iIdx = aPath.find_last_of('/');

        if (std::string::npos == iIdx)
        {
            iIdx = aPath.find_last_of('\\');

            if (std::string::npos == iIdx)
            {
                return "";
            }
        }

        aPath.erase(aPath.begin()+iIdx, aPath.end());
        aPath.append("/");
    }

    return aPath;
}

CSpriteScript::CSpriteScript() : mpTmp(nullptr) {}

CSpriteScript::~CSpriteScript()
{
    if (nullptr != mpTmp)
    {
        if (nullptr != mpTmp->mpGLCoordinates)
        {
            delete[] mpTmp->mpGLCoordinates;
        }

        delete mpTmp;
    }
}

SSprite* CSpriteScript::LoadScript(std::string aPath)
{
    if (nullptr == mpTmp)
    {
        mpTmp = new SSprite();
    }

    if (!aPath.empty())
    {
        FILE *pF = fopen(aPath.data(), "rb");

        if (nullptr != pF)
        {
            fseek(pF, 0, SEEK_END);
            size_t iLength = ftell(pF);
            fseek(pF, 0, SEEK_SET);

            char *pRawData = new char[iLength+1];
            memset(pRawData, 0, iLength+1);

            fread(pRawData, sizeof(char), iLength, pF);
            char* pData = RemoveReturnCarriage(pRawData);
            fclose(pF);

            char *pProps = new char[strlen(pData)+1];
            memcpy(pProps, pData, strlen(pData));

            size_t iPropstrlen = ReadProperties(GetPath(aPath), pProps);

            if (0 != iPropstrlen)
            {
                ReadCoordinates(DeleteChars(pData, iPropstrlen, strlen(pData) - iPropstrlen));
            }

            delete[] pProps;
            delete[] pRawData;
            delete[] pData;
        }
    }

    return mpTmp;
}

size_t CSpriteScript::ReadCoordinates(char* apData)
{
    size_t iRead = 0;

    if (nullptr != apData)
    {
        if (!mpTmp->msPngPath.empty())
        {
            char *pTok = "";
            size_t iNumRead = 0;
            char *pTmp = new char[strlen(apData)+1];
            memset(pTmp, 0, strlen(apData));
            while(nullptr != pTok)
            {
                memset(pTmp, 0, strlen(pTmp));
                memcpy(pTmp, apData, strlen(apData));

                // Remove the beginning identifier.
                pTok = strtok(pTmp, ":");
                iRead += strlen(pTok);
                pTok = strtok(nullptr, ";");
                iRead += strlen(pTok);

                // Change the data into a numerical value.
                size_t iNumFrames = StringToSizeT(pTok);
                mpTmp->miNumFrames = iNumFrames;
                pTok = strtok(nullptr, "}");

                if (mpTmp->miNumFrames <= iNumRead)
                {
                    break;
                }

                if (nullptr == mpTmp->mpGLCoordinates)
                {
                    mpTmp->mpGLCoordinates = new SRect2D_F[iNumFrames];
                }

                for (size_t iIdx = 0; iIdx < iNumRead; ++iIdx)
                {
                    pTok = strtok(nullptr, "}");
                }

                iRead += ParseCoords(iNumRead, pTok);
                ++iNumRead;
            }

            if (nullptr != pTmp)
            {
                delete[] pTmp;
            }
        }
    }

    return iRead;
}

size_t CSpriteScript::ReadProperties(std::string aCurrPath, char* apData)
{
    size_t iRead = 0;

    if (nullptr != apData)
    {
        // Remove the beginning identifier.
        char *pTok = strtok(apData, ":");
        iRead += strlen(pTok);

        // Name.
        pTok = strtok(nullptr, "=");
        iRead += strlen(pTok);
        pTok = strtok(nullptr, ";");
        iRead += strlen(pTok);
        mpTmp->msName.append(pTok);

        // Png name.
        pTok = strtok(nullptr, "=");
        iRead += strlen(pTok);
        pTok = strtok(nullptr, ";");
        iRead += strlen(pTok);
        mpTmp->msPngPath.append(aCurrPath);
        mpTmp->msPngPath.append(pTok);

        // Png strlenensions : Width.
        pTok = strtok(nullptr, "=");
        iRead += strlen(pTok);
        pTok = strtok(nullptr, ";");
        iRead += strlen(pTok);
        mpTmp->mPngDimen.miWidth = StringToSizeT(pTok);

        // Png strlenensions : Height.
        pTok = strtok(nullptr, "=");
        iRead += strlen(pTok);
        pTok = strtok(nullptr, ";");
        iRead += strlen(pTok);
        mpTmp->mPngDimen.miHeight = StringToSizeT(pTok);

        // Framerate.
        pTok = strtok(nullptr, "=");
        iRead += strlen(pTok);
        pTok = strtok(nullptr, ";");
        iRead += strlen(pTok);
        mpTmp->miFramerate = StringToSizeT(pTok);
    }

    return iRead;
}

size_t CSpriteScript::ParseCoords(size_t aIndex, char *apLine)
{
    size_t iRead = 0;

    if (nullptr != apLine)
    {
        if (!mpTmp->msPngPath.empty())
        {
            SRect2D_F glCoords;

            char *pTok = strtok(apLine, ",");
            iRead += strlen(pTok);
            size_t iCoord = StringToSizeT(pTok);
            glCoords.miX = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miWidth);

            pTok = strtok(nullptr, ",");
            iRead += strlen(pTok);
            iCoord = StringToSizeT(pTok);
            glCoords.miY = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miHeight);

            pTok = strtok(nullptr, ",");
            iRead += strlen(pTok);
            iCoord = StringToSizeT(pTok);
            glCoords.miWidth = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miWidth);

            pTok = strtok(nullptr, ",");
            iRead += strlen(pTok);
            iCoord = StringToSizeT(pTok);
            glCoords.miHeight = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miHeight);

            mpTmp->mpGLCoordinates[aIndex] = glCoords;
        }
    }

    return iRead;
}

char* CSpriteScript::RemoveReturnCarriage(std::string aData)
{
    std::string sStr = aData;
    if (!aData.empty())
    {
        while (std::string::npos != sStr.find('\r'))
        {
            size_t iIdx = sStr.find_first_of('\r');
            sStr.erase(sStr.begin()+iIdx, sStr.begin()+iIdx+1);
        }

        while (std::string::npos != sStr.find('\n'))
        {
            size_t iIdx = sStr.find_first_of('\n');
            sStr.erase(sStr.begin()+iIdx, sStr.begin()+iIdx+1);
        }
    }

    char *pRtn = new char[sStr.length()+1];
    memset(pRtn, 0, sStr.length()+1);
    strcpy(pRtn, sStr.data());
    return pRtn;
}

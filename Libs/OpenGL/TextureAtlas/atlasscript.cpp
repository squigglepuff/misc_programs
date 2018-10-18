#include "atlasscript.h"

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

CAtlasScript::CAtlasScript() : mpTmp(nullptr) {}

CAtlasScript::~CAtlasScript()
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

SAtlas* CAtlasScript::LoadScript(std::string aPath)
{
    if (nullptr == mpTmp)
    {
        mpTmp = new SAtlas();
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
            char* pData = RemoveCarriages(pRawData);
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

size_t CAtlasScript::ReadCoordinates(char* apData)
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
                size_t iNumItems = StringToSizeT(pTok);
                mpTmp->miNumItems = iNumItems;
                pTok = strtok(nullptr, "}");

                if (mpTmp->miNumItems <= iNumRead)
                {
                    break;
                }

                if (nullptr == mpTmp->mpGLCoordinates)
                {
                    mpTmp->mpGLCoordinates = new SRect_F[iNumItems];
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

size_t CAtlasScript::ReadProperties(std::string aCurrPath, char* apData)
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
        mpTmp->mPngDimen.miX2 = StringToSizeT(pTok);

        // Png strlenensions : Height.
        pTok = strtok(nullptr, "=");
        iRead += strlen(pTok);
        pTok = strtok(nullptr, ";");
        iRead += strlen(pTok);
        mpTmp->mPngDimen.miY2 = StringToSizeT(pTok);
    }

    return iRead;
}

size_t CAtlasScript::ParseCoords(size_t aIndex, char *apLine)
{
    size_t iRead = 0;

    if (nullptr != apLine)
    {
        if (!mpTmp->msPngPath.empty())
        {
            SRect_F glCoords;

            char *pTok = strtok(apLine, ",");
            iRead += strlen(pTok);
            size_t iCoord = StringToSizeT(pTok);
            glCoords.mnX = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miX2);

            pTok = strtok(nullptr, ",");
            iRead += strlen(pTok);
            iCoord = StringToSizeT(pTok);
            glCoords.mnY = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miY2);

            pTok = strtok(nullptr, ",");
            iRead += strlen(pTok);
            iCoord = StringToSizeT(pTok);
            glCoords.mnX2 = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miX2);

            pTok = strtok(nullptr, ",");
            iRead += strlen(pTok);
            iCoord = StringToSizeT(pTok);
            glCoords.mnY2 = static_cast<float>(iCoord) / static_cast<float>(mpTmp->mPngDimen.miY2);

            mpTmp->mpGLCoordinates[aIndex] = glCoords;
        }
    }

    return iRead;
}

char* CAtlasScript::RemoveCarriages(std::string aData)
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


SRect_F CAtlasScript::ReturnTexCoords(ETile aeTile)
{
    switch(aeTile)
    {
        case ETIle_Apple:
        case ETIle_Grape:
        case ETIle_Lemon:
        case ETIle_Orange:
        case ETIle_Pear:
        case ETIle_AppleSelect:
        case ETIle_GrapeSelect:
        case ETIle_LemonSelect:
        case ETIle_PearSelect:
            return HandleNormal(aeTile);

        // Golden Fruit.
        case ETIle_Gold_Apple:
        case ETIle_Gold_Grape:
        case ETIle_Gold_Lemon:
        case ETIle_Gold_Orange:
        case ETIle_Gold_Pear:
        case ETIle_Gold_AppleSelect:
        case ETIle_Gold_GrapeSelect:
        case ETIle_Gold_LemonSelect:
        case ETIle_Gold_OrangeSelect:
        case ETIle_Gold_PearSelect:
            return HandleGold(aeTile);

        // Misc.
        case ETile_Can:
        case ETIle_Cherry:
        case ETIle_CanSelect:
        case ETile_CherrySelect:
        case ETIle_Crate:
            return HandleMisc(aeTile);
    }

    return SRect_F();
}

SRect_F CAtlasScript::HandleNormal(ETile aeTile)
{
    switch(aeTile)
    {
        case ETIle_Apple:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[0] : SRect_F();
        case ETIle_Grape:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[1] : SRect_F();
        case ETIle_Lemon:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[2] : SRect_F();
        case ETIle_Orange:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[3] : SRect_F();
        case ETIle_Pear:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[4] : SRect_F();
        case ETIle_AppleSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[5] : SRect_F();
        case ETIle_GrapeSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[6] : SRect_F();
        case ETIle_LemonSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[7] : SRect_F();
        case ETIle_OrangeSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[8] : SRect_F();
        case ETIle_PearSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[9] : SRect_F();
    }

    return SRect_F();
}

SRect_F CAtlasScript::HandleGold(ETile aeTile)
{
    switch(aeTile)
    {
        case ETIle_Gold_Apple:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[10] : SRect_F();
        case ETIle_Gold_Grape:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[11] : SRect_F();
        case ETIle_Gold_Lemon:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[12] : SRect_F();
        case ETIle_Gold_Orange:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[13] : SRect_F();
        case ETIle_Gold_Pear:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[14] : SRect_F();
        case ETIle_Gold_AppleSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[15] : SRect_F();
        case ETIle_Gold_GrapeSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[16] : SRect_F();
        case ETIle_Gold_LemonSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[17] : SRect_F();
        case ETIle_Gold_OrangeSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[18] : SRect_F();
        case ETIle_Gold_PearSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[19] : SRect_F();
    }

    return SRect_F();
}

SRect_F CAtlasScript::HandleMisc(ETile aeTile)
{
    switch(aeTile)
    {
        case ETile_Can:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[20] : SRect_F();
        case ETIle_CanSelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[21] : SRect_F();
        case ETIle_Cherry:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[22] : SRect_F();
        case ETile_CherrySelect:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[23] : SRect_F();
        case ETIle_Crate:
            return (nullptr != mpTmp->mpGLCoordinates) ? mpTmp->mpGLCoordinates[24] : SRect_F();
    }

    return SRect_F();
}

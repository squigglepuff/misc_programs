#ifndef SPRITESCRIPT_H
#define SPRITESCRIPT_H

#include "structs.h"

class CAtlasScript
{
    CAtlasScript(const CAtlasScript&) {}
    CAtlasScript& operator=(const CAtlasScript&) { return *this; }

public:
    CAtlasScript();
    ~CAtlasScript();

    SAtlas* LoadScript(std::string aPath);
    SRect_F ReturnTexCoords(ETile aeTile);

private:
    size_t ReadCoordinates(char *apData);
    size_t ReadProperties(std::string aCurrPath, char *apData);

    size_t ParseCoords(size_t aIndex, char *apLine);
    char *RemoveCarriages(std::string aData);

    SRect_F HandleNormal(ETile aeTile);
    SRect_F HandleGold(ETile aeTile);
    SRect_F HandleMisc(ETile aeTile);

    SAtlas* mpTmp;
};

#endif // SPRITESCRIPT_H

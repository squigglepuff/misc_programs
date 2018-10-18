#ifndef SPRITESCRIPT_H
#define SPRITESCRIPT_H

#include "structs.h"

class CSpriteScript
{
    CSpriteScript(const CSpriteScript&) {}
    CSpriteScript& operator=(const CSpriteScript&) { return *this; }

public:
    CSpriteScript();
    ~CSpriteScript();

    SSprite* LoadScript(std::string aPath);

private:
    size_t ReadCoordinates(char *apData);
    size_t ReadProperties(std::string aCurrPath, char *apData);

    size_t ParseCoords(size_t aIndex, char *apLine);
    char *RemoveReturnCarriage(std::string aData);

    SSprite* mpTmp;
};

#endif // SPRITESCRIPT_H

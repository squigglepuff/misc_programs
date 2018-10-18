#ifndef SPRITE_H
#define SPRITE_H

#include "spritescript.h"
#include "AnimatedSequence.h"
#include "PngLoader.h"

class CSprite
{
public:
    CSprite();
    CSprite(const CSprite& aCls);
    virtual ~CSprite();

    CSprite& operator=(const CSprite& aCls);

    void Paint(CGlobalAnimator *pAnim);
    bool Load(std::string aPath);

    SRect2D GetDimensions();
    void SetDimensions(SRect2D aDimensions);

protected:
    GLuint mTexture;
    SSprite *mpSprite;

    CSpriteScript *mpScript;
    SRect2D_F mOldCoords;
};

#endif // SPRITE_H

#include "sprite.h"

CSprite::CSprite() : mpSprite(nullptr), mpScript(nullptr) {}

CSprite::CSprite(const CSprite &aCls) : mpSprite(aCls.mpSprite), mpScript(aCls.mpScript) {}

CSprite::~CSprite()
{
    if (nullptr != mpScript)
    {
        delete mpScript;
    }

    if (glIsTexture(mTexture))
    {
        glDeleteTextures(1, &mTexture);
    }
}

CSprite& CSprite::operator =(const CSprite& aCls)
{
    if (this != &aCls)
    {
        if (nullptr == mpSprite)
        {
            mpSprite = new SSprite();
        }

        memcpy(mpSprite, aCls.mpSprite, sizeof(SSprite));

        if (nullptr == mpScript)
        {
            mpScript = new CSpriteScript();
        }

        memcpy(mpScript, aCls.mpScript, sizeof(CSpriteScript));
    }

    return *this;
}

void CSprite::Paint(CGlobalAnimator *pAnim)
{
    if (nullptr != mpSprite || nullptr == pAnim)
    {
        SRect2D pos = mpSprite->mDimensions;
        assert(GetFrame() < GetNumFrames());
        SRect2D_F coords = mpSprite->mpGLCoordinates[GetFrame()];

        if (coords.miX == 0 && coords.miY == 0 &&
                coords.miWidth == 0 && coords.miHeight == 0)
        {
            coords = mOldCoords;
        }

        glBindTexture(GL_TEXTURE_2D, mTexture);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(mOldCoords.miX, mOldCoords.miY);
            glVertex2i(pos.miX, pos.miY);

            glTexCoord2f(mOldCoords.miX, mOldCoords.miHeight);
            glVertex2i(pos.miX, pos.miHeight);

            glTexCoord2f(mOldCoords.miWidth, mOldCoords.miHeight);
            glVertex2i(pos.miWidth, pos.miHeight);

            glTexCoord2f(mOldCoords.miWidth, mOldCoords.miY);
            glVertex2i(pos.miWidth, pos.miY);
        glEnd();

        Update();

        mOldCoords = coords;
    }
}

bool CSprite::Load(std::string aPath)
{
    if (nullptr == mpScript)
    {
        mpScript = new CSpriteScript();
    }

    if (!aPath.empty())
    {
        mpSprite = mpScript->LoadScript(aPath);

        SetNumFrames(mpSprite->miNumFrames);
        SetFramerate(mpSprite->miFramerate);
        SetFrame(0);

        if (nullptr != mpSprite)
        {
            CPng png;
            if(png.LoadPng(mpSprite->msPngPath))
            {
                glGenTextures(1, &mTexture);
                glBindTexture(GL_TEXTURE_2D, mTexture);
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

                return true;
            }
        }
    }

    return false;
}

SRect2D CSprite::GetDimensions()
{
    if (nullptr != mpSprite)
    {
        return mpSprite->mDimensions;
    }

    return SRect2D();
}

void CSprite::SetDimensions(SRect2D aDimensions)
{
    if (nullptr != mpSprite)
    {
        mpSprite->mDimensions = aDimensions;
    }
}

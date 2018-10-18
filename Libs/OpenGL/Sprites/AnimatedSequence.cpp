#include "AnimatedSequence.h"

CAnimSequence::CAnimSequence() : miFrame(0), miNumFrames(0), miFramerate(60), miUpdateTimer(0) {}

CAnimSequence::CAnimSequence(const CAnimSequence &aCls) : miFrame(aCls.miFrame), miNumFrames(aCls.miNumFrames), miFramerate(aCls.miFramerate), miUpdateTimer(aCls.miUpdateTimer) {}

CAnimSequence::~CAnimSequence() {}

CAnimSequence& CAnimSequence::operator=(const CAnimSequence& aCls)
{
    if (this != &aCls)
    {
        miFrame = aCls.miFrame;
        miNumFrames = aCls.miNumFrames;
        miFramerate = aCls.miFramerate;
        miUpdateTimer =aCls.miUpdateTimer;
    }

    return *this;
}

size_t CAnimSequence::GetFrame()
{
    return miFrame;
}

size_t CAnimSequence::GetFramerate()
{
    return miFramerate;
}

size_t CAnimSequence::GetNumFrames()
{
    return miNumFrames;
}

void CAnimSequence::SetFrame(size_t aFrame)
{
    if (miNumFrames > aFrame)
    {
        miFrame = aFrame;
    }
}

void CAnimSequence::SetFramerate(size_t aRate)
{
    miFramerate = aRate;
}

void CAnimSequence::SetNumFrames(size_t aNumber)
{
    miNumFrames = aNumber;
}

void CAnimSequence::Update()
{
    printf("Frame number is: %d.\nNumber of frames: %d.\n", miFrame, miNumFrames);

    if ((1000/miFramerate) <= miUpdateTimer)
    {
        if ((miFrame+1) < miNumFrames)
        {
            ++miFrame;
        }
        else
        {
            miFrame = 0;
        }

        miUpdateTimer = 0;
    }
    else
    {
#if defined(USE_NTSC)
        miUpdateTimer += FPS_NTSC;
#endif

#if defined(USE_PAL)
        miUpdateTimer += FPS_PAL;
#endif
    }
}

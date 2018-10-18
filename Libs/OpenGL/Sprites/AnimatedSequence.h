#ifndef ANIMATEDSEQUENCE_H
#define ANIMATEDSEQUENCE_H

#include "structs.h"

#define USE_NTSC

#if !defined(FPS_NTSC)
#define FPS_NTSC (1000/60) // Should equal 16.666666667
#endif

#if !defined(FPS_PAL)
#define FPS_PAL (1000/50) // Should equal 20.
#endif

class CAnimSequence
{
public:
    CAnimSequence();
    CAnimSequence(const CAnimSequence& aCls);
    ~CAnimSequence();

    CAnimSequence& operator=(const CAnimSequence& aCls);

    void Update();

    size_t GetFrame();
    size_t GetFramerate();
    size_t GetNumFrames();

    void SetFrame(size_t aFrame = 0);
    void SetFramerate(size_t aRate = 60);
    void SetNumFrames(size_t aNumber);

protected:
    size_t miFrame;
    size_t miNumFrames;
    size_t miFramerate;
    size_t miUpdateTimer;
};

class CGlobalAnimator
{
    friend class CAnimSequence;

public:
    CGlobalAnimator();
    CGlobalAnimator(const CGlobalAnimator& aCls);
    ~CGlobalAnimator();

    CGlobalAnimator& operator=(const CGlobalAnimator& aCls);

    void Update(int *pInt);

    bool AddSequence(CAnimSequence aSeq);
    bool RemoveSequence(CAnimSequence aSeq);

private:
    std::vector<CAnimSequence> mSequences;
};

#endif // ANIMATEDSEQUENCE_H

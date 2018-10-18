#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <GL/gl.h>

#if defined(USE_GLU)
#include <GL/glu.h>
#endif

struct SRect2D
{
    size_t miX, miY, miWidth, miHeight;

    SRect2D(size_t aX = 0, size_t aY = 0, size_t aWidth = 0, size_t aHeight = 0) : miX(aX), miY(aY), miWidth(aWidth), miHeight(aHeight) {}
    SRect2D(const SRect2D& aStruct) : miX(aStruct.miX), miY(aStruct.miY), miWidth(aStruct.miWidth), miHeight(aStruct.miHeight) {}
};

struct SRect2D_F
{
    float miX, miY, miWidth, miHeight;

    SRect2D_F(float aX = 0, float aY = 0, float aWidth = 0, float aHeight = 0) : miX(aX), miY(aY), miWidth(aWidth), miHeight(aHeight) {}
    SRect2D_F(const SRect2D_F& aStruct) : miX(aStruct.miX), miY(aStruct.miY), miWidth(aStruct.miWidth), miHeight(aStruct.miHeight) {}
};

struct SSprite
{
    SRect2D mDimensions;
    SRect2D_F* mpGLCoordinates;
    size_t miNumFrames;
    size_t miFramerate;
    std::string msPngPath;
    std::string msName;
    SRect2D mPngDimen;

    SSprite() : mDimensions(0), mpGLCoordinates(0), miNumFrames(0), miFramerate(0), msPngPath(""), msName(""), mPngDimen(0) {}
    SSprite(SRect2D aDimens, SRect2D_F* apCoords, size_t aNumFrames, size_t aFramerate, std::string aPngPath, std::string aName, SRect2D aPngDimen)
        : mDimensions(aDimens), mpGLCoordinates(apCoords), miNumFrames(aNumFrames), miFramerate(aFramerate), msPngPath(aPngPath), msName(aName), mPngDimen(aPngDimen) {}
};

#endif // STRUCTS_H

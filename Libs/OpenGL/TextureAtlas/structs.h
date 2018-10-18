#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <GL/gl.h>

#if defined(USE_GLU)
#include <GL/glu.h>
#endif

#define FPS_NTSC (1000/60)

struct SRect
{
    size_t miX, miY, miX2, miY2;

    SRect(size_t aX = 0, size_t aY = 0, size_t aWidth = 0, size_t aHeight = 0) : miX(aX), miY(aY), miX2(aWidth), miY2(aHeight) {}
    SRect(const SRect& aStruct) : miX(aStruct.miX), miY(aStruct.miY), miX2(aStruct.miX2), miY2(aStruct.miY2) {}
};

struct SRect_F
{
    float mnX, mnY, mnX2, mnY2;

    SRect_F(float aX = 0, float aY = 0, float aWidth = 0, float aHeight = 0) : mnX(aX), mnY(aY), mnX2(aWidth), mnY2(aHeight) {}
    SRect_F(const SRect_F& aStruct) : mnX(aStruct.mnX), mnY(aStruct.mnY), mnX2(aStruct.mnX2), mnY2(aStruct.mnY2) {}
};

enum ETile
{
    ETIle_Apple,
    ETIle_Grape,
    ETIle_Lemon,
    ETIle_Orange,
    ETIle_Pear,
    ETIle_AppleSelect,
    ETIle_GrapeSelect,
    ETIle_LemonSelect,
    ETIle_OrangeSelect,
    ETIle_PearSelect,

    // Golden Fruit.
    ETIle_Gold_Apple,
    ETIle_Gold_Grape,
    ETIle_Gold_Lemon,
    ETIle_Gold_Orange,
    ETIle_Gold_Pear,
    ETIle_Gold_AppleSelect,
    ETIle_Gold_GrapeSelect,
    ETIle_Gold_LemonSelect,
    ETIle_Gold_OrangeSelect,
    ETIle_Gold_PearSelect,

    // Misc.
    ETile_Can,
    ETIle_Cherry,
    ETIle_CanSelect,
    ETile_CherrySelect,
    ETIle_Crate
};

struct SAtlas
{
    SRect mDimensions;
    SRect_F* mpGLCoordinates;
    size_t miNumItems;
    std::string msPngPath;
    std::string msName;
    SRect mPngDimen;

    SAtlas() : mDimensions(0), mpGLCoordinates(nullptr), miNumItems(0), msPngPath(""), msName(""), mPngDimen(0) {}
    SAtlas(SRect aDimens, SRect_F* apCoords, size_t aNumFrames, std::string aPngPath, std::string aName, SRect aPngDimen)
        : mDimensions(aDimens), mpGLCoordinates(apCoords), miNumItems(aNumFrames), msPngPath(aPngPath), msName(aName), mPngDimen(aPngDimen) {}
};

#endif // STRUCTS_H

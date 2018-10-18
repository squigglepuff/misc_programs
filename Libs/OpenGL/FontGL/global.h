#define _DEBUG_

#ifndef GLOBAL_H
#define GLOBAL_H

/*
    This file declares all the globally used structs, macros, functions, and enums.
*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>
#include <queue>
#include <math.h>
#include <time.h>
#include <inttypes.h>
#include <map>

// OpenGL Headers
#include <GL/gl.h>
//#include <GL/glu.h>

// This macro is used to get the size of an array.
#define DIM(x) (sizeof(x)/sizeof(*(x)))

// This macro is used for reporting issues.
#define REPORT(aSys, aMsg, aSndr) (std::cout<< "{"<< aSys<< "}["<< clock()<< "] "<< aMsg<< "<"<< aSndr<<">"<< std::endl)

// These are game specific types.
typedef unsigned char byte; // Equals 1 byte in memory on standard 80386 architechture.
typedef unsigned short word;
typedef unsigned long dword; // double-word
typedef unsigned long long qword; // quad-word

#ifdef __unix__
typedef __int8_t __int8;
typedef __int16_t __int16;
typedef __int32_t __int32;
typedef __int64_t __int64;
typedef __intptr_t __intptr;
#endif

// This enum contains all the game states.
enum EState
{
    EState_Init,
    EState_MainMenu,
    EState_OptMenu,
    EState_MapScreen,
    EState_Playing,
    EState_Exit,
    EState_Crash // Special case, should only show if the code did something it shouldn't!
};

// This enum contains all the possible messages that can be sent during exectution.
enum EMessage
{
    EMessage_UpdateGL,
    EMessage_UpdateAL,
    EMessage_UpdatePhysics,
    EMessage_Quit
};

// This struct is simply a rectangle.
struct SRect
{
    // X and Y are the coordinates for the top-left corner while X2 and Y2 are for the bottom-right corner.
    size_t miX, miY, miX2, miY2;

    SRect(size_t aX1 = 0, size_t aY1 = 0, size_t aX2 = 0, size_t aY2 = 0) : miX(aX1), miY(aY1), miX2(aX2), miY2(aY2) {}
};

// This struct contains all the CVars used in the game.
struct SGlobalCVars
{
    bool mbExit; // Exit the game?
    bool mbSave; // Save the current progress?
    bool mbReload; // Reload ALL assets?

    float mbMouseSpeed; // The Mouse speed.

    int miSndMasterVol; // The master sound volume.
    int miSndBgMusicVol; // The background music volume.
    int miSndFxVol; // The sound effects volume.
    int miALSndCard; // The sound card identifier; Defaults to 0.

    std::string msRndrRes; // The rendering resolution.

    int miAntiAliasLvl; // The antialiasing level. Should NEVER be 0. Appropriate values are: 1, 2, 4, 6, 8
    int miGLRndrDevice; // The OpenGL rendering device identifier; Should always be 1.

    SGlobalCVars() : mbExit(false), mbSave(false), mbReload(false),
        mbMouseSpeed(1.0), miSndMasterVol(100), miSndBgMusicVol(75), miSndFxVol(100), miALSndCard(0),
        msRndrRes("800x600"), miAntiAliasLvl(1), miGLRndrDevice(1) {}
};

extern SGlobalCVars g_CVars;
extern EState g_GameState;
extern std::queue<EMessage> g_GameQueue;
extern std::vector<std::string> g_GameLog;
extern std::map<std::string, int> g_KeyCodes;

extern void Log(std::string aSys, std::string aMsg, std::string aSndr);

#endif // GLOBAL_H

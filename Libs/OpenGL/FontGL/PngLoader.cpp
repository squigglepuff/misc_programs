/*
SUNLIME STUDIOS COMMERCIAL PRODUCT LICENSE

DEFINITIONS - 
 Modern: 1976 - Today
 Mark: a "watermark" or logo
 Owner: The person who created said thing
 Distributor: Any person or persons who distributes said product
 Developer: Any person or persons that creates a product
 Software: Any product that is the result of compiled code on any modern computer system
 Art: Any image that has a resolution greater than 32 pixels by 32 pixels
 Music: Any sound data on a modern computer system that is greater than 5 Megabytes and/or is longer than 5 seconds
 Document: Any text file that has 10 lines of complete text or 250+ letters
 
GENERAL - 
 All products made and/or distributed under this license, belongs to the owner of the software.
 All distributors MUST give FULL credit to the developer and take none for himself.
 Developers have the right to chose their distributors and remove any (if not all) unauthorized copies.
 Distributors have the right to place their mark upon the product
 All developers have the right to either distribute the source of the project or keep that project closed
 All distributors have the right - under the authorization of the developers - to distribute any source as they see fit
 All developers have the right to sell the product without the help of of a distributor.
 All distributors reserve the right to use the name of the product without the use of false advertising
 
PENELTIES - 
 If a distributor violates either this license or an argreement under this license, 
the developer/owner reserves the right to strip the distributor of ALL power over the porduct
 If a developer/owner violates either this license of an agreement under this license,
All rights are passed to the distributor effective/immediately.
*/

#include "PngLoader.h"

CPng::CPng()
{
//    FreeImage_Initialise();
}

CPng::~CPng()
{
//    FreeImage_DeInitialise();
    delete[] mpData;
    delete mpBitmap;
}

bool CPng::LoadPng(std::string astrPath)
{
    if(astrPath.empty())
    {
        return false;
    }

    mpBitmap = FreeImage_Load(FIF_PNG, astrPath.data());

    if(!mpBitmap)
    {
        return false;
    }

    if(!FreeImage_HasPixels(mpBitmap))
    {
        return false;
    }

    FreeImage_FlipVertical(mpBitmap);
    SwapRedBlue32(mpBitmap);

    miWidth = FreeImage_GetWidth(mpBitmap);
    miHeight = FreeImage_GetHeight(mpBitmap);
    miBits = FreeImage_GetBPP(mpBitmap);

    mpData = new byte[(miWidth * miHeight * 4)];
    mpData = FreeImage_GetBits(mpBitmap);

    return true;
}

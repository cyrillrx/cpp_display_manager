//
// Created by Cyril Leroux on 21/04/2015.
//

#include "display_manager.h"

pointf DisplayManager::GetRectCoef(rect<int> rect1, rect<int> rect2)
{
    pointf coef;
    coef.x = (float) (rect2.width()) / (float) (rect1.width());
    coef.y = (float) (rect2.height()) / (float) (rect1.height());

    return coef;
}

rect<int> DisplayManager::ApplyCoef(rect<int> rectIn, pointf coef)
{
    rect<int> rectOut;
    rectOut.left = (int) ((float) rectIn.left * coef.x);
    rectOut.top = (int) ((float) rectIn.top * coef.y);
    rectOut.right = (int) ((float) rectIn.right * coef.x);
    rectOut.bottom = (int) ((float) rectIn.bottom * coef.y);

    return rectOut;
}

rect<int> DisplayManager::ApplyOffest(rect<int> relativePos, const int& offsetX, const int& offsetY)
{
    rect<int> absPos;
    absPos.left = relativePos.left + offsetX;
    absPos.top = relativePos.top + offsetY;
    absPos.right = relativePos.right + offsetX;
    absPos.bottom = relativePos.bottom + offsetY;

    return absPos;
}
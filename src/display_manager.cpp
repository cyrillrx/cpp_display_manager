//
// Created by Cyril Leroux on 21/04/2015.
//

#include "display_manager.h"

pointf DisplayManager::GetRectCoef(rect rect1, rect rect2)
{
    pointf coef;
    coef.x = (float) (rect2.get_width()) / (float) (rect1.get_width());
    coef.y = (float) (rect2.get_height()) / (float) (rect1.get_height());

    return coef;
}

rect* DisplayManager::ApplyCoef(rect rectIn, pointf coef)
{
    auto* rectOut = new rect();
    rectOut->left = (int) ((float) rectIn.left * coef.x);
    rectOut->top = (int) ((float) rectIn.top * coef.y);
    rectOut->right = (int) ((float) rectIn.right * coef.x);
    rectOut->bottom = (int) ((float) rectIn.bottom * coef.y);

    return rectOut;
}

rect* DisplayManager::ApplyOffest(rect relativePos, const int& offsetX, const int& offsetY)
{
    auto* absPos = new rect();
    absPos->left = relativePos.left + offsetX;
    absPos->top = relativePos.top + offsetY;
    absPos->right = relativePos.right + offsetX;
    absPos->bottom = relativePos.bottom + offsetY;

    return absPos;
}
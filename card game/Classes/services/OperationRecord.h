#pragma once
#include "cocos2d.h"
#include "OperationType.h"

class CardView;

struct OperationRecord {
    OperationType type;
    CardView* card1;    // 操作涉及的卡牌1
    CardView* card2;    // 操作涉及的卡牌2 (DESK_TO_HAND时为顶部牌)
    cocos2d::Vec2 originalPosition; // 原始位置
    bool wasTopCard;    // 操作前是否是顶部牌
    bool wasVisible;   // 操作前是否可见
};

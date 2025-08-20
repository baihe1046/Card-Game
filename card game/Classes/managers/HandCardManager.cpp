#include "HandCardManager.h"
#include "controllers/GameController.h"
#include <cocos2d.h>

/**
 * @brief 构造函数
 * @param controller 游戏控制器指针
 */
HandCardManager::HandCardManager(GameController* controller)
    : _controller(controller) {
    CCASSERT(controller != nullptr, "GameController cannot be nullptr");
}

/**
 * @brief 处理手牌触摸事件
 * @param card 被触摸的卡牌视图
 */
void HandCardManager::handleCardTouch(CardView* card) {

    // 参数有效性检查
    if (!card || !_controller)
    {
        CCLOG("Error: Invalid parameters in handleCardTouch");
        return;
    }

    // 检查是否为顶部卡牌
    if (card->isTopCard()) 
    {
        CCLOG("Controller: Top card touched at position: (%.0f, %.0f)",
            card->getPositionX(), card->getPositionY());
        return;
    }

    // 获取当前顶部卡牌
    CardView* currentTopCard = _controller->getCurrentTopCard();
    if (!currentTopCard)
    {
        CCLOG("HandCardManager: No top card in hand stack");
        return;
    }

    // 确保当前顶部牌状态正确
    if (!currentTopCard->isTopCard()) {
        CCLOG("Warning: Current top card is not marked as top! Fixing...");
        currentTopCard->setIsTopCard(true); 
    }

    // 记录操作前状态
    OperationRecord record;
    record.type = OperationType::HAND_SWAP;
    record.card1 = card;
    record.card2 = currentTopCard;
    record.originalPosition = card->getPosition();
    record.wasTopCard = card->isTopCard();
    record.wasVisible = card->isVisible();

    // 执行交换操作
    const cocos2d::Vec2 tempPos = card->getPosition();
    card->setPosition(currentTopCard->getPosition());
    currentTopCard->setPosition(tempPos);

    // 更新顶部牌状态
    card->setIsTopCard(true);
    currentTopCard->setIsTopCard(false);

    // 确保原顶部牌可见
    currentTopCard->setVisible(true);

    // 更新控制器中的顶部牌引用
    _controller->setCurrentTopCard(card);

    // 保存操作记录
    _controller->getUndoManager()->pushRecord(record);

    CCLOG("HandCardManager: Cards swapped. New top card: %p, Old top card: %p",
        card, currentTopCard);
}

/**
 * @brief 撤销操作
 * @param record 要撤销的操作记录
 */
void HandCardManager::undoOperation(const OperationRecord& record) {

    // 参数有效性检查
    if (!record.card1 || !record.card2 || !_controller)
    {
        CCLOG("Error: Invalid operation record in undoOperation");
        return;
    }

    // 交换回位置
    const cocos2d::Vec2 tempPos = record.card1->getPosition();
    record.card1->setPosition(record.card2->getPosition());
    record.card2->setPosition(tempPos);

    // 恢复顶部牌身份
    record.card1->setIsTopCard(record.wasTopCard);
    record.card2->setIsTopCard(!record.wasTopCard);

    // 恢复可见性
    record.card1->setVisible(record.wasVisible);
    record.card2->setVisible(true);

    // 更新当前顶部牌
    CardView* newTopCard = nullptr;
    if (record.card1->isTopCard()) {
        newTopCard = record.card1;
    }
    else if (record.card2->isTopCard()) {
        newTopCard = record.card2;
    }
    if (newTopCard)
    {
        _controller->setCurrentTopCard(newTopCard);
        CCLOG("HandCardManager: Undo successful. Restored top card: %p", newTopCard);
    }
    else
    {
        CCLOG("Error: No top card after undo operation!");
    }

    // 确保只有一个顶部牌
    if (record.card1->isTopCard() && record.card2->isTopCard()) {
        CCLOG("Warning: Both cards marked as top after undo! Fixing...");
        record.card2->setIsTopCard(false);
    }
}
#include "DeskCardManager.h"
#include "controllers/GameController.h"
#include "views/GameScene.h"

//定义常量
const int CARD_FACE_DIFFERENCE = 1;

DeskCardManager::DeskCardManager(GameController* controller)
    : _controller(controller) {
    //空指针检查
    CCASSERT(controller, "GameController cannot be null");
}

void DeskCardManager::handleCardTouch(CardView* card) {
    if (!card) {
        CCLOG("[DeskCardManager] Error: Touching null card");
        return;
    }

    GameScene* gameScene = _controller->getGameScene();
    UndoManager* undoManager = _controller->getUndoManager();

    CardView* currentTopCard = _controller->getCurrentTopCard();
    if (!currentTopCard) {
        CCLOG("No top card in hand stack");
        return;
    }

    // 确保当前顶部牌是有效的
    if (!currentTopCard->isTopCard()) {
        CCLOG("Warning: Current top card is not marked as top!");
        currentTopCard->setIsTopCard(true); // 强制修复状态
    }

    int diff = std::abs(card->getCardFace() - currentTopCard->getCardFace());
    if (diff != CARD_FACE_DIFFERENCE) {
        CCLOG("[DeskCardManager] Difference is not %d: %d", CARD_FACE_DIFFERENCE, diff);
        return;
    }

    //创建操作记录
    OperationRecord record;
    record.type = OperationType::DESK_TO_HAND;
    record.card1 = card;
    record.card2 = currentTopCard;
    record.originalPosition = card->getPosition();
    record.wasTopCard = false; // 桌面牌原本不是顶部牌
    record.wasVisible = card->isVisible();

    // 执行操作
    card->setPosition(currentTopCard->getPosition());
    card->setIsTopCard(true); // 成为新顶部牌

    // 清除原顶部牌状态
    currentTopCard->setVisible(false);
    currentTopCard->setIsTopCard(false);

    // 更新控制器中的顶部牌引用
    _controller->setCurrentTopCard(card);

    // 保存操作记录
    _controller->getUndoManager()->pushRecord(record);

    // 通知场景更新卡片归属
    if (gameScene) {
        gameScene->moveCardToBottomLayer(card);
    }

    CCLOG("Desk card moved to hand top position. New top: %p, Old top hidden: %p",
        card, currentTopCard);
}

void DeskCardManager::undoOperation(const OperationRecord& record) {
    // 空指针检查
    if (!record.card1 || !record.card2) {
        CCLOG("[DeskCardManager] Error: Null card in undo operation");
        return;
    }
    GameScene* gameScene = _controller->getGameScene();

    // 恢复桌面牌位置和状态
    record.card1->setPosition(record.originalPosition);
    record.card1->setIsTopCard(record.wasTopCard); // 恢复为非顶部牌
    record.card1->setVisible(record.wasVisible);

    // 恢复原顶部牌
    record.card2->setVisible(true);
    record.card2->setIsTopCard(true); 

    // 更新当前顶部牌引用
    _controller->setCurrentTopCard(record.card2);

    // 通知场景更新卡片归属
    if (gameScene) {
        gameScene->moveCardToTopLayer(record.card1);
    }

    CCLOG("Undo: Desk card moved back. Restored top card: %p", record.card2);

}
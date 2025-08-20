#include "HandCardManager.h"
#include "controllers/GameController.h"
#include <cocos2d.h>

/**
 * @brief ���캯��
 * @param controller ��Ϸ������ָ��
 */
HandCardManager::HandCardManager(GameController* controller)
    : _controller(controller) {
    CCASSERT(controller != nullptr, "GameController cannot be nullptr");
}

/**
 * @brief �������ƴ����¼�
 * @param card �������Ŀ�����ͼ
 */
void HandCardManager::handleCardTouch(CardView* card) {

    // ������Ч�Լ��
    if (!card || !_controller)
    {
        CCLOG("Error: Invalid parameters in handleCardTouch");
        return;
    }

    // ����Ƿ�Ϊ��������
    if (card->isTopCard()) 
    {
        CCLOG("Controller: Top card touched at position: (%.0f, %.0f)",
            card->getPositionX(), card->getPositionY());
        return;
    }

    // ��ȡ��ǰ��������
    CardView* currentTopCard = _controller->getCurrentTopCard();
    if (!currentTopCard)
    {
        CCLOG("HandCardManager: No top card in hand stack");
        return;
    }

    // ȷ����ǰ������״̬��ȷ
    if (!currentTopCard->isTopCard()) {
        CCLOG("Warning: Current top card is not marked as top! Fixing...");
        currentTopCard->setIsTopCard(true); 
    }

    // ��¼����ǰ״̬
    OperationRecord record;
    record.type = OperationType::HAND_SWAP;
    record.card1 = card;
    record.card2 = currentTopCard;
    record.originalPosition = card->getPosition();
    record.wasTopCard = card->isTopCard();
    record.wasVisible = card->isVisible();

    // ִ�н�������
    const cocos2d::Vec2 tempPos = card->getPosition();
    card->setPosition(currentTopCard->getPosition());
    currentTopCard->setPosition(tempPos);

    // ���¶�����״̬
    card->setIsTopCard(true);
    currentTopCard->setIsTopCard(false);

    // ȷ��ԭ�����ƿɼ�
    currentTopCard->setVisible(true);

    // ���¿������еĶ���������
    _controller->setCurrentTopCard(card);

    // ���������¼
    _controller->getUndoManager()->pushRecord(record);

    CCLOG("HandCardManager: Cards swapped. New top card: %p, Old top card: %p",
        card, currentTopCard);
}

/**
 * @brief ��������
 * @param record Ҫ�����Ĳ�����¼
 */
void HandCardManager::undoOperation(const OperationRecord& record) {

    // ������Ч�Լ��
    if (!record.card1 || !record.card2 || !_controller)
    {
        CCLOG("Error: Invalid operation record in undoOperation");
        return;
    }

    // ������λ��
    const cocos2d::Vec2 tempPos = record.card1->getPosition();
    record.card1->setPosition(record.card2->getPosition());
    record.card2->setPosition(tempPos);

    // �ָ����������
    record.card1->setIsTopCard(record.wasTopCard);
    record.card2->setIsTopCard(!record.wasTopCard);

    // �ָ��ɼ���
    record.card1->setVisible(record.wasVisible);
    record.card2->setVisible(true);

    // ���µ�ǰ������
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

    // ȷ��ֻ��һ��������
    if (record.card1->isTopCard() && record.card2->isTopCard()) {
        CCLOG("Warning: Both cards marked as top after undo! Fixing...");
        record.card2->setIsTopCard(false);
    }
}
#pragma once
#include "views/CardView.h"
#include "services/OperationRecord.h"
#include "UndoManager.h"

class GameController;

class HandCardManager {
public:
    explicit HandCardManager(GameController* controller);

    void handleCardTouch(CardView* card);
    void undoOperation(const OperationRecord& record);

private:
    GameController* _controller;
};
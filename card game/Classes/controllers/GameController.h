#pragma once

#include <vector>

#include "models/CardData.h"

#include "managers/GameManager.h"

#include "views/CardView.h"

#include "managers/UndoManager.h"

#include "managers/HandCardManager.h"

#include "managers/DeskCardManager.h"

class GameScene;

class GameController {

public:

	explicit GameController(GameScene* scene, std::vector<CardData>* playfield, std::vector<CardData>* stack);

	~GameController();

	void logPlayfield() const;

	void logStack() const;

	std::vector<CardData>* getPlayfieldCards() { return _gameManager->getPlayfieldCards(); }

	std::vector<CardData>* getStackCards() { return _gameManager->getStackCards(); }

	// 事件处理

	void handleUndo();

	void handleDeskCardTouch(CardView* card);

	void handleHandCardTouch(CardView* card);

	// 访问器方法

	CardView* getCurrentTopCard() { return _currentTopCard; }

	void setCurrentTopCard(CardView* card);

	GameScene* getGameScene() { return _gameScene; }

	UndoManager* getUndoManager() { return &_undoManager; }

private:

	GameManager* _gameManager = nullptr;

	GameScene* _gameScene = nullptr;

	CardView* _currentTopCard = nullptr;

	// Manager类

	UndoManager _undoManager;

	HandCardManager _handCardManager;

	DeskCardManager _deskCardManager;

};
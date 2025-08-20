#include <algorithm>

#include "cocos2d.h"

#include "views/GameScene.h"

#include "GameController.h"

USING_NS_CC;

GameController::GameController(GameScene* scene, std::vector<CardData>* playfield, std::vector<CardData>* stack)

	: _gameScene(scene),

	_handCardManager(this),

	_deskCardManager(this)

{

	_gameManager = new GameManager(playfield, stack);

	logPlayfield();

	logStack();

	// 初始化时没有顶部牌

	_currentTopCard = nullptr;

}

GameController::~GameController() {

	delete _gameManager;

	CCLOG("GameController Destroyed");

}

void GameController::logPlayfield() const {

	for (size_t i = 0; i < _gameManager->getPlayfieldCards()->size(); ++i) {

		const CardData& card = (*_gameManager->getPlayfieldCards())[i];

		CCLOG("Playfield[%zu]: Face=%d, Suit=%d, Position=(%d, %d)",

			i, card.cardFace, card.cardSuit, card.posX, card.posY);

	}

}

void GameController::logStack() const {

	for (size_t i = 0; i < _gameManager->getStackCards()->size(); ++i) {

		const CardData& card = (*_gameManager->getStackCards())[i];

		CCLOG("Stack[%zu]: Face=%d, Suit=%d, Position=(%d, %d)",

			i, card.cardFace, card.cardSuit, card.posX, card.posY);

	}

}

void GameController::handleUndo() {

	CCLOG("Undo operation triggered by controller");

	if (_undoManager.isEmpty()) {

		CCLOG("Nothing to undo");

		return;

	}

	// 保存当前顶部牌状态用于日志

	CardView* preUndoTopCard = _currentTopCard;

	OperationRecord record = _undoManager.popRecord();

	switch (record.type) {

	case OperationType::HAND_SWAP:

		_handCardManager.undoOperation(record);

		CCLOG("Undo: Hand cards swapped back. Pre-undo top: %p, Post-undo top: %p",

			preUndoTopCard, _currentTopCard);

		break;

	case OperationType::DESK_TO_HAND:

		_deskCardManager.undoOperation(record);

		CCLOG("Undo: Desk card moved back. Pre-undo top: %p, Post-undo top: %p",

			preUndoTopCard, _currentTopCard);

		break;

	}

	// 后置条件检查

	if (_currentTopCard && !_currentTopCard->isTopCard()) {

		CCLOG("Error: Current top card reference is not marked as top! Fixing...");

		_currentTopCard->setIsTopCard(true);

	}

}

void GameController::handleDeskCardTouch(CardView* card) {

	_deskCardManager.handleCardTouch(card);

}

void GameController::handleHandCardTouch(CardView* card) {

	_handCardManager.handleCardTouch(card);

}

void GameController::setCurrentTopCard(CardView* card) {

	// 清除原顶部牌状态

	if (_currentTopCard && _currentTopCard != card) {

		// 确保之前的顶部牌状态被清除

		if (_currentTopCard->isTopCard()) {

			CCLOG("Clearing top card status from previous top: %p", _currentTopCard);

			_currentTopCard->setIsTopCard(false);

		}

	}

	// 设置新顶部牌

	_currentTopCard = card;

	// 确保新顶部牌状态正确

	if (_currentTopCard && !_currentTopCard->isTopCard()) {

		CCLOG("New top card not marked as top! Fixing...");

		_currentTopCard->setIsTopCard(true);

	}

	CCLOG("Set current top card to: %p", card);

}
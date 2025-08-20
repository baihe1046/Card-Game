#pragma once

#include <vector>
#include <string>

#include "cocos2d.h"
#include "models/CardData.h"
#include "utils/FileNameLoadUtil.h"

/**
 * ������ͼ�࣬�����Ƶ���ʾ�ͽ���
 */
class CardView : public cocos2d::Sprite {

public:
	/**
	 * ����������ͼ
	 * @param cardData ��������
	 * @return ������ͼʵ��
	 */
	static CardView* create(CardData cardData);

	/**
	 * ʹ��ģ�����ݳ�ʼ������
	 * @param cardData ��������
	 * @return �Ƿ��ʼ���ɹ�
	 */
	bool initWithModel(CardData cardData);

	/**
	 * �����Ƿ�Ϊ������
	 * @param isTop �Ƿ�Ϊ������
	 */
	void setIsTopCard(bool isTop) { _isTopCard = isTop; }

	/**
	* ��ȡ�Ƿ�Ϊ������
	* @return �Ƿ�Ϊ������
	*/
	bool isTopCard() const { return _isTopCard; }

	/**
	 * ��ȡ����X����
	 * @return X����
	 */
	int getPosX() const { return _cardData.posX; }

	/**
	 * ��ȡ����Y����
	 * @return Y����
	 */
	int getPosY() const { return _cardData.posY; }

	/**
	 * ��ȡ������ֵ
	 * @return ������ֵ
	 */
	int getCardFace() const { return _cardData.cardFace; }

	/**
	* ��ȡ���ƻ�ɫ
	* @return ���ƻ�ɫ
	*/
	int getCardSuit() const { return _cardData.cardSuit; }

	/**
	* ���õ�ǰ������
	* @param card ������ָ��
	*/
	void setCurrentTopCard(CardView* card) { _currentTopCard = card; }

private:
	CardView* _currentTopCard = nullptr; // ��ǰ������
private:
	bool _isTopCard = false; // �Ƿ�Ϊ������
	CardData _cardData;//��������
	std::vector<std::string> _pixResList;//������Դ�б�
	
	/**
	 * ��������
	 */
	void createCard();
	std::vector<std::string> _getTextureNameForCard();
};
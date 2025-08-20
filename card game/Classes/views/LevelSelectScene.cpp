#include "LevelSelectScene.h"
#include "ui/CocosGUI.h"
#include "utils/JsonUtils.h"
#include "GameScene.h"
#include <string>

USING_NS_CC;

// ��������
const int LEVEL_COUNT = 6;         // �ؿ�����
const int COLUMNS = 3;             // ����
const int ROWS = 2;                // ����
const float BUTTON_WIDTH = 160.0f;  // ��ť���
const float BUTTON_HEIGHT = 80.0f; // ��ť�߶�
const float SPACING_X = 180.0f;    // ˮƽ���
const float SPACING_Y = 200.0f;    // ��ֱ���
const float TITLE_FONT_SIZE = 200.0f; // ���������С
const float BUTTON_FONT_SIZE = 36.0f; // ��ť�����С
const Color4B BG_COLOR = Color4B(240, 240, 240, 255); // ������ɫ(ǳ��ɫ)

Scene* LevelSelectScene::createScene() {
    auto scene = Scene::create();
    auto layer = LevelSelectScene::create();
    scene->addChild(layer);
    return scene;
}

bool LevelSelectScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // ������Ļ�ߴ�Ϊ 1080x2080
    auto winSize = Director::getInstance()->getWinSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���ñ�����ɫΪǳɫ
    auto background = LayerColor::create(BG_COLOR);
    this->addChild(background);

    // ��ӱ���
    auto titleLabel = Label::createWithTTF("Level Select", "fonts/Marker Felt.ttf", TITLE_FONT_SIZE);
    titleLabel->setPosition(Vec2(origin.x + winSize.width / 2,
        origin.y + winSize.height - TITLE_FONT_SIZE));
    titleLabel->setColor(Color3B(0, 0, 0)); // ���ñ�����ɫΪ��ɫ
    this->addChild(titleLabel, 1);

    //��ӿ��ƻ�ɫװ��
    auto star = Sprite::create("car.png");
    if (star) {
        star->setPosition(Vec2(origin.x + winSize.width / 2, origin.y + winSize.height - 300));
        this->addChild(star, 1);
    }

    // ���м�����ʼλ��
    float totalWidth = COLUMNS * BUTTON_WIDTH + (COLUMNS - 1) * SPACING_X;
    float totalHeight = ROWS * BUTTON_HEIGHT + (ROWS - 1) * SPACING_Y;
    float startX = origin.x + (winSize.width - totalWidth) / 2+80;
    float startY = origin.y + (winSize.height - totalHeight) / 2 + totalHeight / 2; // ��̬���㴹ֱλ��

    // Ԥ���ذ�ť����
    Director::getInstance()->getTextureCache()->addImage("button_normal.png");
    Director::getInstance()->getTextureCache()->addImage("button_pressed.png");

    // �����ؿ���ť
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLUMNS; ++col) {
            int levelNumber = row * COLUMNS + col + 1;

            auto button = ui::Button::create(
                "button_normal.png",
                "button_pressed.png"
            );
            button->setContentSize(Size(BUTTON_WIDTH, BUTTON_HEIGHT));
            button->setTitleText(std::to_string(levelNumber));
            button->setTitleFontSize(BUTTON_FONT_SIZE);
            button->setTitleColor(Color3B(0, 0, 0)); // ��ť������ɫ

      // ���㰴ťλ��
      float posX = startX + col * (BUTTON_WIDTH + SPACING_X);
      float posY = startY - row * (BUTTON_HEIGHT + SPACING_Y);
      button->setPosition(Vec2(posX, posY));

      // ��Ӵ����¼�������
      button->addTouchEventListener([this, levelNumber](Ref* sender, ui::Widget::TouchEventType type) {
          if (type == ui::Widget::TouchEventType::ENDED) {
              CCLOG("Selected Level: %d", levelNumber);

      // ��֤�ؿ������Ƿ����
      auto playfieldCards = JsonUtils::parsePlayfield(levelNumber);
      auto stackCards = JsonUtils::parseStack(levelNumber);

      if (playfieldCards && stackCards) {
          CCLOG("Level %d data loaded successfully", levelNumber);
          // �л�����Ϸ����
          Director::getInstance()->pushScene(
              TransitionFade::create(0.5f, GameScene::createScene(levelNumber))
          );
      }
      else {
          CCLOG("Error: Failed to load data for level %d", levelNumber);
          // �������һ����ʾ��Ϣ
          auto alert = Label::createWithTTF("Level data not available", "fonts/Marker Felt.ttf", 30);
          alert->setPosition(Director::getInstance()->getWinSize() / 2);
          alert->setColor(Color3B(255, 0, 0));
          this->addChild(alert, 10);
          // 2����Ƴ���ʾ
          alert->runAction(Sequence::create(
              DelayTime::create(2.0f),
              RemoveSelf::create(),
              nullptr
          ));
      }
          }
          });

      this->addChild(button);
        }
    }

    return true;
}
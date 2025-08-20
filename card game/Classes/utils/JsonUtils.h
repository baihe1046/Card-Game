#pragma once
#include "cocos2d.h"
#include <vector>
#include <string>
#include "models/CardData.h"


class JsonUtils {
public:
    // 解析 playfield 数据
    static std::vector<CardData>* parsePlayfield(int levelNumber);

    // 解析 stack 数据
    static std::vector<CardData>* parseStack(int levelNumber);

    // 打印卡片数据
    static void printCards(const std::vector<CardData>& cards, const std::string& prefix = "");
};
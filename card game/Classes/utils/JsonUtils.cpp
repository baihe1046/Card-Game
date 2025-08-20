#include "JsonUtils.h"
#include "cocos2d.h"
#include "json/document.h"
#include <sstream>
#include <map>
#include <string>

USING_NS_CC;

// 静态缓存，存储每个关卡的playfield卡片数据
static std::map<int, std::vector<CardData>> s_cachedPlayfields;

// 静态缓存，存储每个关卡的stack卡片数据
static std::map<int, std::vector<CardData>> s_cachedStacks;

/**
 * 从JSON对象中获取指定键的整数值
 * @param obj JSON对象
 * @param key 要查找的键名
 * @param defaultValue 不存在或类型错误时返回的默认值
 * @return 找到的整数值
 */
int getIntFromValue(const rapidjson::Value& obj, const std::string& key,int defaultValue = 0) {
    auto member = obj.FindMember(key.c_str());
    if (member != obj.MemberEnd() && member->value.IsInt()) {
        return member->value.GetInt();
    }
    CCLOG("Warning: Key '%s' not found or not an integer, using default value: %d", key.c_str(), defaultValue);
    return defaultValue;
}

/**
 * 从JSON对象中获取Position的x坐标值
 * @param obj JSON对象
 * @return Position的x坐标，如果不存在则返回0
 */
int getPosXFromValue(const rapidjson::Value& obj) {
    if (obj.HasMember("Position") && obj["Position"].IsObject()) {
        return getIntFromValue(obj["Position"], "x");
    }
    CCLOG("Warning: Position not found in JSON object");
    return 0;
}

/**
 * 从JSON对象中获取Position的y坐标值
 * @param obj JSON对象
 * @return Position的y坐标，如果不存在则返回0
 */
int getPosYFromValue(const rapidjson::Value& obj) {
    if (obj.HasMember("Position") && obj["Position"].IsObject()) {
        return getIntFromValue(obj["Position"], "y");
    }
    CCLOG("Warning: Position not found in JSON object");
    return 0;
}

/**
 * 从JSON数组解析卡片数据
 * @param array JSON数组
 * @return 解析后的卡片数据向量
 */
std::vector<CardData> parseCardsFromArray(const rapidjson::Value& array) {
    std::vector<CardData> cards;
    if (!array.IsArray()) {
        CCLOG("Error: Expected array but got invalid type");
        return cards;
    }

    cards.reserve(array.Size()); // 预分配内存以提高性能
    for (const auto& item : array.GetArray()) {
        CardData data;
        data.cardFace = getIntFromValue(item, "CardFace");
        data.cardSuit = getIntFromValue(item, "CardSuit");
        data.posX = getPosXFromValue(item);
        data.posY = getPosYFromValue(item);
        cards.push_back(data);
    }
    return cards;
}

/**
 * 从JSON文件加载所有关卡数据到缓存中
 * 如果缓存已存在数据，则直接返回
 */
void loadAllLevelsFromJson() {

    // 检查缓存是否已加载
    if (!s_cachedPlayfields.empty() && !s_cachedStacks.empty()) {
        return;
    }

    // 获取JSON文件路径并读取内容
    std::string jsonPath = FileUtils::getInstance()->fullPathForFilename("levelConfig.json");
    if (!FileUtils::getInstance()->isFileExist(jsonPath)) {
        CCLOG("Error: levelConfig.json not found at path: %s", jsonPath.c_str());
        return;
    }
    std::string jsonData = FileUtils::getInstance()->getStringFromFile(jsonPath);
    if (jsonData.empty()) {
        CCLOG("Error: levelConfig.json is empty");
        return;
    }

    // 解析JSON文档
    rapidjson::Document document;
    document.Parse(jsonData.c_str());

    // 检查JSON解析是否出错

    if (document.HasParseError()) {
        CCLOG("JSON Parse Error in levelConfig.json: %s",rapidjson::GetParseErrorFunc(document.GetParseError()));
        return;
    }

    // 加载1到6关的数据
    for (int i = 1; i <= 6; ++i) {
        // 加载playfield数据
        std::string playfieldKey = "playfield" + std::to_string(i);
        if (document.HasMember(playfieldKey.c_str())) {
            s_cachedPlayfields[i] = parseCardsFromArray(document[playfieldKey.c_str()]);
            CCLOG("Loaded playfield data for level %d with %zu cards", i, s_cachedPlayfields[i].size());
        }
        else {
            CCLOG("Warning: Playfield data for level %d not found in JSON", i);
        }

        // 加载stack数据
        std::string stackKey = "stack" + std::to_string(i);
        if (document.HasMember(stackKey.c_str())) {
            s_cachedStacks[i] = parseCardsFromArray(document[stackKey.c_str()]);
            CCLOG("Loaded stack data for level %d with %zu cards", i, s_cachedStacks[i].size());
        }
        else {
            CCLOG("Warning: Stack data for level %d not found in JSON", i);
        }
    }
}

/**
 * 解析指定关卡的playfield数据
 * @param levelNumber 关卡编号
 * @return 指向playfield数据的指针，如果不存在则返回nullptr
 */
std::vector<CardData>* JsonUtils::parsePlayfield(int levelNumber) {
    // 确保数据已加载
    loadAllLevelsFromJson();

    // 查找指定关卡的数据
    auto it = s_cachedPlayfields.find(levelNumber);
    if (it != s_cachedPlayfields.end()) {
        return &it->second;
    }
    CCLOG("Error: Playfield Level %d not found in cache", levelNumber);
    return nullptr;
}

/**
 * 解析指定关卡的stack数据
 * @param levelNumber 关卡编号
 * @return 指向stack数据的指针，如果不存在则返回nullptr
 */
std::vector<CardData>* JsonUtils::parseStack(int levelNumber) {
    // 确保数据已加载
    loadAllLevelsFromJson();

    // 查找指定关卡的数据
    auto it = s_cachedStacks.find(levelNumber);
    if (it != s_cachedStacks.end()) {
        return &it->second;
    }
    CCLOG("Error: Stack Level %d not found in cache", levelNumber);
    return nullptr;
}
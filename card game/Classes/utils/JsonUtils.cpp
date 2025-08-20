#include "JsonUtils.h"
#include "cocos2d.h"
#include "json/document.h"
#include <sstream>
#include <map>
#include <string>

USING_NS_CC;

// ��̬���棬�洢ÿ���ؿ���playfield��Ƭ����
static std::map<int, std::vector<CardData>> s_cachedPlayfields;

// ��̬���棬�洢ÿ���ؿ���stack��Ƭ����
static std::map<int, std::vector<CardData>> s_cachedStacks;

/**
 * ��JSON�����л�ȡָ����������ֵ
 * @param obj JSON����
 * @param key Ҫ���ҵļ���
 * @param defaultValue �����ڻ����ʹ���ʱ���ص�Ĭ��ֵ
 * @return �ҵ�������ֵ
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
 * ��JSON�����л�ȡPosition��x����ֵ
 * @param obj JSON����
 * @return Position��x���꣬����������򷵻�0
 */
int getPosXFromValue(const rapidjson::Value& obj) {
    if (obj.HasMember("Position") && obj["Position"].IsObject()) {
        return getIntFromValue(obj["Position"], "x");
    }
    CCLOG("Warning: Position not found in JSON object");
    return 0;
}

/**
 * ��JSON�����л�ȡPosition��y����ֵ
 * @param obj JSON����
 * @return Position��y���꣬����������򷵻�0
 */
int getPosYFromValue(const rapidjson::Value& obj) {
    if (obj.HasMember("Position") && obj["Position"].IsObject()) {
        return getIntFromValue(obj["Position"], "y");
    }
    CCLOG("Warning: Position not found in JSON object");
    return 0;
}

/**
 * ��JSON���������Ƭ����
 * @param array JSON����
 * @return ������Ŀ�Ƭ��������
 */
std::vector<CardData> parseCardsFromArray(const rapidjson::Value& array) {
    std::vector<CardData> cards;
    if (!array.IsArray()) {
        CCLOG("Error: Expected array but got invalid type");
        return cards;
    }

    cards.reserve(array.Size()); // Ԥ�����ڴ����������
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
 * ��JSON�ļ��������йؿ����ݵ�������
 * ��������Ѵ������ݣ���ֱ�ӷ���
 */
void loadAllLevelsFromJson() {

    // ��黺���Ƿ��Ѽ���
    if (!s_cachedPlayfields.empty() && !s_cachedStacks.empty()) {
        return;
    }

    // ��ȡJSON�ļ�·������ȡ����
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

    // ����JSON�ĵ�
    rapidjson::Document document;
    document.Parse(jsonData.c_str());

    // ���JSON�����Ƿ����

    if (document.HasParseError()) {
        CCLOG("JSON Parse Error in levelConfig.json: %s",rapidjson::GetParseErrorFunc(document.GetParseError()));
        return;
    }

    // ����1��6�ص�����
    for (int i = 1; i <= 6; ++i) {
        // ����playfield����
        std::string playfieldKey = "playfield" + std::to_string(i);
        if (document.HasMember(playfieldKey.c_str())) {
            s_cachedPlayfields[i] = parseCardsFromArray(document[playfieldKey.c_str()]);
            CCLOG("Loaded playfield data for level %d with %zu cards", i, s_cachedPlayfields[i].size());
        }
        else {
            CCLOG("Warning: Playfield data for level %d not found in JSON", i);
        }

        // ����stack����
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
 * ����ָ���ؿ���playfield����
 * @param levelNumber �ؿ����
 * @return ָ��playfield���ݵ�ָ�룬����������򷵻�nullptr
 */
std::vector<CardData>* JsonUtils::parsePlayfield(int levelNumber) {
    // ȷ�������Ѽ���
    loadAllLevelsFromJson();

    // ����ָ���ؿ�������
    auto it = s_cachedPlayfields.find(levelNumber);
    if (it != s_cachedPlayfields.end()) {
        return &it->second;
    }
    CCLOG("Error: Playfield Level %d not found in cache", levelNumber);
    return nullptr;
}

/**
 * ����ָ���ؿ���stack����
 * @param levelNumber �ؿ����
 * @return ָ��stack���ݵ�ָ�룬����������򷵻�nullptr
 */
std::vector<CardData>* JsonUtils::parseStack(int levelNumber) {
    // ȷ�������Ѽ���
    loadAllLevelsFromJson();

    // ����ָ���ؿ�������
    auto it = s_cachedStacks.find(levelNumber);
    if (it != s_cachedStacks.end()) {
        return &it->second;
    }
    CCLOG("Error: Stack Level %d not found in cache", levelNumber);
    return nullptr;
}
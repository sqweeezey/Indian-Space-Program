#pragma once
#include <string>

class UpgradeSet {
public:
    UpgradeSet(std::string name, int price, bool isUnlocked, float speedBonus, float aeroBonus)
        : mName(name)
        , mPrice(price)
        , mIsUnlocked(isUnlocked)
        , mSpeedBonus(speedBonus)
        , mAeroBonus(aeroBonus) {
    }

    std::string getName() const { return mName; }
    int getPrice() const { return mPrice; }
    bool getIsUnlocked() const { return mIsUnlocked; }
    void unlock() { mIsUnlocked = true; }

    float getSpeedBonus() const { return mSpeedBonus; }
    float getAeroBonus() const { return mAeroBonus; }

private:
    std::string mName;
    int mPrice;
    bool mIsUnlocked;
    float mSpeedBonus;
    float mAeroBonus;
};

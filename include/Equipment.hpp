#pragma once
#include <string>

// --- БАЗОВЫЙ КЛАСС ---
class Equipment {
public:
    Equipment(std::string name, int price, bool isUnlocked)
        : name(name), price(price), isUnlocked(isUnlocked) {
    }

    virtual ~Equipment() = default;

    // Чистая виртуальная функция
    virtual float getPowerMultiplier() const = 0;

    std::string getName() const { return name; }
    int getPrice() const { return price; }
    bool getIsUnlocked() const { return isUnlocked; }
    void unlock() { isUnlocked = true; }

protected:
    std::string name;
    int price;
    bool isUnlocked;
};

// --- НАСЛЕДНИКИ ---

class Motor : public Equipment {
public:
    Motor(std::string name, int price, bool isUnlocked, float speedBonus)
        : Equipment(name, price, isUnlocked), mSpeedBonus(speedBonus) {
    }

    float getPowerMultiplier() const override {
        return mSpeedBonus;
    }

private:
    float mSpeedBonus;
};

class Tire : public Equipment {
public:
    Tire(std::string name, int price, bool isUnlocked, float aeroBonus)
        : Equipment(name, price, isUnlocked), mAeroBonus(aeroBonus) {
    }

    float getPowerMultiplier() const override {
        return mAeroBonus;
    }

private:
    float mAeroBonus;
};

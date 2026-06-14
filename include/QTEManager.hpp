#pragma once
#include <SFML/System.hpp>

class QTEManager {
public:
    QTEManager()
        : mMultiplier(0.0f), mIsActive(false), mMovingUp(true), mSpeed(1.5f) {
    }

    // Запуск ползунка
    void start() {
        mMultiplier = 0.0f;
        mIsActive = true;
        mMovingUp = true;
    }

    // Остановка ползунка и возврат результата
    float stop() {
        mIsActive = false;
        return mMultiplier; // Возвращает от 0.0 до 1.0
    }

    // Обновление логики (должно вызываться каждый кадр)
    void update(sf::Time deltaTime) {
        if (!mIsActive) return;

        // Вычисляем, насколько сдвинуть ползунок в этом кадре
        float delta = mSpeed * deltaTime.asSeconds();

        if (mMovingUp) {
            mMultiplier += delta;
            if (mMultiplier >= 1.0f) {
                mMultiplier = 1.0f;
                mMovingUp = false; // Дошли до конца, меняем направление
            }
        }
        else {
            mMultiplier -= delta;
            if (mMultiplier <= 0.0f) {
                mMultiplier = 0.0f;
                mMovingUp = true; // Дошли до начала, меняем направление
            }
        }
    }

    // Геттеры для отрисовки интерфейса в будущем
    float getMultiplier() const { return mMultiplier; }
    bool isActive() const { return mIsActive; }

private:
    float mMultiplier; // Текущее значение силы (0.0 - 1.0)
    bool mIsActive;    // Работает ли ползунок сейчас
    bool mMovingUp;    // Направление движения ползунка
    float mSpeed;      // Скорость бегунка
};

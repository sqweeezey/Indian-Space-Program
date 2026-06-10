#pragma once
#include <iostream>
#include <vector>
#include <SFML/System/Vector2.hpp>

class Rocket {
public:
    Rocket()
        : mVelocityX(0.0f), mVelocityY(0.0f), mDistance(0.0f), mAltitude(0.0f), mMaxDistance(0.0f), mIsFlying(false), mGravity(9.81f) {
    }

    void launch(float qteMultiplier) {
        float power = 500.0f + (1500.0f * qteMultiplier);
        mVelocityX = power * 0.7f;
        mVelocityY = power * 0.7f;

        mDistance = 0.0f;
        mAltitude = 0.0f;
        mMaxDistance = 0.0f;
        mIsFlying = true;

        std::cout << "[Lot] Wystrzal! Sila poczatkowa: " << power << std::endl;
    }

    void update(float dt) {
        if (!mIsFlying) return;

        mDistance += mVelocityX * dt;
        mVelocityY -= mGravity * 100.0f * dt;
        mAltitude += mVelocityY * dt;

        if (mDistance > mMaxDistance) {
            mMaxDistance = mDistance;
        }

        if (mAltitude <= 0.0f) {
            mAltitude = 0.0f;
            mVelocityX = 0.0f;
            mVelocityY = 0.0f;
            mIsFlying = false;

            std::cout << "[Lot] Ladowanie! Przeleciales: " << mMaxDistance << " m" << std::endl;
        }
    }

    // --- Ќќ¬јя ‘”Ќ ÷»я: –асчет точек траектории дл€ отрисовки ---
    std::vector<sf::Vector2f> getTrajectoryPoints(float qteMultiplier, sf::Vector2f startPos) {
        std::vector<sf::Vector2f> points;

        // —читаем стартовую скорость точно так же, как при запуске
        float power = 500.0f + (1500.0f * qteMultiplier);
        float vx = power * 0.7f;
        float vy = power * 0.7f;

        float tempDistance = 0.0f;
        float tempAltitude = 0.0f;
        float tempVelocityY = vy;
        float g = mGravity * 100.0f;

        float simDt = 0.03f; // Ўаг симул€ции (чем меньше, тем плотнее точки)

        // ѕросчитываем 60 шагов вперед
        for (int i = 0; i < 60; ++i) {
            tempDistance += vx * simDt;
            tempVelocityY -= g * simDt;
            tempAltitude += tempVelocityY * simDt;

            if (tempAltitude < 0.0f) break; // —имул€ци€ до земли

            // ѕереводим физические координаты в экранные:
            // X растет вправо, Y уменьшаетс€ вверх (в SFML 0 сверху экрана)
            float screenX = startPos.x + tempDistance;
            float screenY = startPos.y - tempAltitude;

            points.push_back(sf::Vector2f(screenX, screenY));
        }
        return points;
    }

    bool isFlying() const { return mIsFlying; }
    float getDistance() const { return mDistance; }
    float getAltitude() const { return mAltitude; }

private:
    float mVelocityX, mVelocityY;
    float mDistance, mAltitude;
    float mMaxDistance;
    bool mIsFlying;
    float mGravity;
};
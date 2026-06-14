#pragma once
#include <iostream>
#include <vector>
#include <SFML/System/Vector2.hpp>

// Переименовали, чтобы не конфликтовало с Equipment.hpp
struct RocketEngine { float powerMult; };
struct RocketTire { float dragMult; };

class Rocket {
public:
    Rocket() : mVelocityX(0.0f), mVelocityY(0.0f), mDistance(0.0f), mAltitude(0.0f),
        mIsFlying(false), mGravity(9.81f), mLastQte(0.0f) {

        mEngine = { 0.5f };
        mTire = { 1.5f };
    }

    void upgrade(float enginePower, float tireDrag) {
        mEngine.powerMult = enginePower;
        mTire.dragMult = tireDrag;
    }

    void launch(float qteMultiplier) {
        mLastQte = qteMultiplier; // Запоминаем силу для отрисовки идеальной линии!

        float power = (500.0f + (1500.0f * qteMultiplier)) * mEngine.powerMult;

        mVelocityX = power * 0.85f;
        mVelocityY = power * 0.5f;

        mDistance = 0.0f; mAltitude = 0.0f; mIsFlying = true;
    }

    // dt передаем, но игнорируем, используя жесткое время (fixedDt)
    void update(float /* dt */) {
        if (!mIsFlying) return;

        // Фиксированный шаг времени (как 60 FPS), чтобы физика не плавала от лагов
        float fixedDt = 1.0f / 60.0f;

        mVelocityX -= (mVelocityX * 0.5f * mTire.dragMult) * fixedDt;
        mVelocityY -= (mGravity * 100.0f) * fixedDt;

        mDistance += mVelocityX * fixedDt;
        mAltitude += mVelocityY * fixedDt;

        if (mAltitude <= 0.0f) {
            mAltitude = 0.0f;
            mIsFlying = false;
        }
    }

    std::vector<sf::Vector2f> getTrajectoryPoints(float qteMultiplier, sf::Vector2f startPos) {
        std::vector<sf::Vector2f> points;
        float power = (500.0f + (1500.0f * qteMultiplier)) * mEngine.powerMult;
        float vx = power * 0.85f;
        float vy = power * 0.5f;
        float dist = 0.0f, alt = 0.0f, vY = vy, vX = vx;

        // Идеально совпадает с fixedDt из функции update
        float simDt = 1.0f / 60.0f;

        for (int i = 0; i < 150; ++i) {
            vX -= (vX * 0.5f * mTire.dragMult) * simDt;
            vY -= (mGravity * 100.0f) * simDt;
            dist += vX * simDt;
            alt += vY * simDt;

            if (alt < 0.0f) break;

            if (i % 3 == 0) {
                points.push_back(sf::Vector2f(startPos.x + dist, startPos.y - alt));
            }
        }
        return points;
    }

    bool isFlying() const { return mIsFlying; }
    float getDistance() const { return mDistance; }
    float getAltitude() const { return mAltitude; }
    float getLastQte() const { return mLastQte; } // Геттер для силы

private:
    RocketEngine mEngine;
    RocketTire mTire;
    float mVelocityX, mVelocityY, mDistance, mAltitude, mGravity;
    bool mIsFlying;
    float mLastQte;
};

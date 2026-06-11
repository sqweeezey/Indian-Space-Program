#pragma once
#include <iostream>
#include <vector>
#include <SFML/System/Vector2.hpp>

// Переименовали, чтобы не было конфликта с Equipment.hpp напарника!
struct RocketEngine { float powerMult; };
struct RocketTire { float dragMult; };

class Rocket {
public:
    Rocket() : mVelocityX(0.0f), mVelocityY(0.0f), mDistance(0.0f), mAltitude(0.0f),
        mIsFlying(false), mGravity(9.81f) {
        // НАЧАЛЬНЫЙ УРОВЕНЬ
        mEngine = { 0.5f };
        mTire = { 1.5f };
    }

    void upgrade(float enginePower, float tireDrag) {
        mEngine.powerMult = enginePower;
        mTire.dragMult = tireDrag;
    }

    void launch(float qteMultiplier) {
        float power = (800.0f + (2000.0f * qteMultiplier)) * mEngine.powerMult;

        mVelocityX = power * 0.85f;
        mVelocityY = power * 0.5f;

        mDistance = 0.0f; mAltitude = 0.0f; mIsFlying = true;
    }

    void update(float dt) {
        if (!mIsFlying) return;

        mVelocityX -= (mVelocityX * 0.5f * mTire.dragMult) * dt;
        mVelocityY -= (mGravity * 100.0f) * dt;

        mDistance += mVelocityX * dt;
        mAltitude += mVelocityY * dt;

        if (mAltitude <= 0.0f) {
            mAltitude = 0.0f; mIsFlying = false;
        }
    }

    std::vector<sf::Vector2f> getTrajectoryPoints(float qteMultiplier, sf::Vector2f startPos) {
        std::vector<sf::Vector2f> points;
        float power = (500.0f + (1500.0f * qteMultiplier)) * mEngine.powerMult;
        float vx = power * 0.85f;
        float vy = power * 0.5f;
        float dist = 0.0f, alt = 0.0f, vY = vy, vX = vx;

        for (int i = 0; i < 60; ++i) {
            vX -= (vX * 0.5f * mTire.dragMult) * 0.03f;
            vY -= (mGravity * 100.0f) * 0.03f;
            dist += vX * 0.03f;
            alt += vY * 0.03f;
            if (alt < 0.0f) break;
            points.push_back(sf::Vector2f(startPos.x + dist, startPos.y - alt));
        }
        return points;
    }

    bool isFlying() const { return mIsFlying; }
    float getDistance() const { return mDistance; }
    float getAltitude() const { return mAltitude; }

private:
    RocketEngine mEngine; // Заменили тип
    RocketTire mTire;     // Заменили тип
    float mVelocityX, mVelocityY, mDistance, mAltitude, mGravity;
    bool mIsFlying;
};
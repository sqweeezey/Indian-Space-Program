#pragma once
#include <vector>
#include <cmath>
#include <SFML/System/Vector2.hpp>

#define M_PI 3.14159265358979323846

struct RocketEngine { float powerMult; };
struct RocketTire { float dragMult; float bounciness; };

class Rocket {
public:
    Rocket() : mVelocityX(0.0f), mVelocityY(0.0f), mDistance(0.0f), mAltitude(0.0f),
        mIsFlying(false), mGravity(9.81f), mLastPowerMult(0.0f), mLastAngle(45.0f) {

        mEngine = { 0.5f };
        mTire = { 1.5f, 0.4f };
    }

    void upgrade(float enginePower, float tireDrag, float tireBounciness = 0.4f) {
        mEngine.powerMult = enginePower;
        mTire.dragMult = tireDrag;
        mTire.bounciness = tireBounciness;
    }

    void launch(float powerMultiplier, float angleDegrees, float startAltitude) {
        mLastPowerMult = powerMultiplier;
        mLastAngle = angleDegrees;

        float totalPower = 1900.0f * mEngine.powerMult * powerMultiplier;
        float angleRad = angleDegrees * (M_PI / 180.0f);

        mVelocityX = totalPower * std::cos(angleRad);
        mVelocityY = totalPower * std::sin(angleRad);

        mDistance = 0.0f;
        mAltitude = startAltitude;
        mIsFlying = true;
    }

    void update(float /* dt */) {
        if (!mIsFlying) return;

        float fixedDt = 1.0f / 60.0f;

        mVelocityX -= (mVelocityX * 0.5f * mTire.dragMult) * fixedDt;
        mVelocityY -= (mGravity * 100.0f) * fixedDt;

        mDistance += mVelocityX * fixedDt;
        mAltitude += mVelocityY * fixedDt;

        // --- УЛЕТЕЛО ЗА ЭКРАН (Конец полета без ожидания) ---
        if (mDistance > 1300.0f) {
            mIsFlying = false;
            return;
        }

        if (mAltitude <= 0.0f) {
            mAltitude = 0.0f;
            mVelocityY = -mVelocityY * (mTire.bounciness * 0.75f);
            mVelocityX = mVelocityX * 0.45f;

            if (std::abs(mVelocityY) < 60.0f && mVelocityX < 30.0f) {
                mIsFlying = false;
            }
        }
    }

    std::vector<sf::Vector2f> getTrajectoryPoints(float powerMultiplier, float angleDegrees, sf::Vector2f startPos) {
        std::vector<sf::Vector2f> points;
        float angleRad = angleDegrees * (M_PI / 180.0f);

        float step = 20.0f;
        int maxDots = 10;

        for (int i = 1; i <= maxDots; ++i) {
            float dx = std::cos(angleRad) * (step * i);
            float dy = std::sin(angleRad) * (step * i);
            points.push_back(sf::Vector2f(startPos.x + dx, startPos.y - dy));
        }
        return points;
    }

    bool isFlying() const { return mIsFlying; }
    float getDistance() const { return mDistance; }
    float getAltitude() const { return mAltitude; }
    float getLastPowerMult() const { return mLastPowerMult; }
    float getLastAngle() const { return mLastAngle; }

private:
    RocketEngine mEngine;
    RocketTire mTire;
    float mVelocityX, mVelocityY, mDistance, mAltitude, mGravity;
    bool mIsFlying;
    float mLastPowerMult, mLastAngle;
};
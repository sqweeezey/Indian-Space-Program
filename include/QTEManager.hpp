#pragma once
#include <SFML/System.hpp>

enum class QTEState { INACTIVE, POWER_SELECT, ANGLE_SELECT, DONE };

class QTEManager {
public:
    QTEManager() : mState(QTEState::INACTIVE), mPowerIndicator(0.0f), mAngle(45.0f),
        mMovingUp(true), mPowerSpeed(1.5f), mAngleSpeed(50.0f), mFinalPowerMult(0.0f) {
    }

    void start() {
        mState = QTEState::POWER_SELECT;
        mPowerIndicator = 0.0f;
        mAngle = 10.0f;
        mMovingUp = true;
        mFinalPowerMult = 0.0f;
    }

    bool isActive() const { return mState != QTEState::INACTIVE && mState != QTEState::DONE; }
    QTEState getState() const { return mState; }
    float getPowerIndicator() const { return mPowerIndicator; }
    float getAngle() const { return mAngle; }
    float getFinalPowerMult() const { return mFinalPowerMult; }

    void update(sf::Time deltaTime) {
        if (!isActive()) return;
        float dt = deltaTime.asSeconds();

        if (mState == QTEState::POWER_SELECT) {
            if (mMovingUp) {
                mPowerIndicator += mPowerSpeed * dt;
                if (mPowerIndicator >= 1.0f) { mPowerIndicator = 1.0f; mMovingUp = false; }
            }
            else {
                mPowerIndicator -= mPowerSpeed * dt;
                if (mPowerIndicator <= 0.0f) { mPowerIndicator = 0.0f; mMovingUp = true; }
            }
        }
        else if (mState == QTEState::ANGLE_SELECT) {
            if (mMovingUp) {
                mAngle += mAngleSpeed * dt;
                if (mAngle >= 80.0f) { mAngle = 80.0f; mMovingUp = false; }
            }
            else {
                mAngle -= mAngleSpeed * dt;
                if (mAngle <= 10.0f) { mAngle = 10.0f; mMovingUp = true; }
            }
        }
    }

    bool pressSpace() {
        if (mState == QTEState::POWER_SELECT) {
            // --- НОВЫЕ РАЗМЕРЫ ЗОН ---
            // Зеленая зона: от 0.4 до 0.6 (стала в 2 раза шире)
            if (mPowerIndicator >= 0.4f && mPowerIndicator <= 0.6f) {
                mFinalPowerMult = 1.0f;
                mAngleSpeed = 35.0f;
            }
            // Желтая зона: от 0.2 до 0.4 И от 0.6 до 0.8
            else if ((mPowerIndicator >= 0.2f && mPowerIndicator < 0.4f) ||
                (mPowerIndicator > 0.6f && mPowerIndicator <= 0.8f)) {
                mFinalPowerMult = 0.6f;
                mAngleSpeed = 75.0f;
            }
            // Красная зона: только самые края (меньше 0.2 или больше 0.8)
            else {
                mFinalPowerMult = 0.15f;
                mAngleSpeed = 160.0f;
            }

            mState = QTEState::ANGLE_SELECT;
            mMovingUp = true;
            return false;
        }
        else if (mState == QTEState::ANGLE_SELECT) {
            mState = QTEState::DONE;
            return true;
        }
        return false;
    }

private:
    QTEState mState;
    float mPowerIndicator;
    float mAngle;
    bool mMovingUp;
    float mPowerSpeed;
    float mAngleSpeed;
    float mFinalPowerMult;
};
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>

enum class HitResult { KILLED, DAMAGED, IGNORED };

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void setPosition(float x, float y) = 0;
    virtual sf::Vector2f getPosition() const = 0;
};

class Target : public GameObject {
public:
    Target(int reward) : coinReward(reward), active(true), speed(0.0f), position(0, 0) {}

    int getReward() const { return coinReward; }
    bool isActive() const { return active; }
    void setActive(bool state) { active = state; }
    void setSpeed(float s) { speed = s; }
    float getSpeed() const { return speed; }

    void setPosition(float x, float y) override { position = sf::Vector2f(x, y); }
    sf::Vector2f getPosition() const override { return position; }

    virtual HitResult takeDamage(float damage) {
        setActive(false);
        return HitResult::KILLED;
    }

    virtual bool isBoss() const { return false; }
    virtual bool isDrone() const { return false; }

protected:
    int coinReward;
    bool active;
    float speed;
    sf::Vector2f position;
};

class Bird : public Target {
public:
    Bird() : Target(50) {
        if (mTexture.loadFromFile("bird1.png")) {
            mSprite.setTexture(mTexture);
            frames = {
                sf::IntRect(46, 0, 171, 270), sf::IntRect(253, 0, 180, 270),
                sf::IntRect(460, 0, 175, 270), sf::IntRect(660, 0, 170, 270),
                sf::IntRect(860, 0, 185, 270), sf::IntRect(1077, 0, 156, 270)
            };
            mSprite.setTextureRect(frames[0]);
            mSprite.setOrigin(85.5f, 95.0f);
            mSprite.setScale(0.45f, 0.45f);
        }
    }

    void update(sf::Time deltaTime) override {
        position.x += speed * deltaTime.asSeconds();
        mSprite.setPosition(position);

        mElapsedTime += deltaTime.asSeconds();
        if (mElapsedTime >= 0.08f) {
            mElapsedTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % 6;
            mSprite.setTextureRect(frames[mCurrentFrame]);
            mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
        }

        if (speed < 0.0f) mSprite.setScale(-0.45f, 0.45f);
        else mSprite.setScale(0.45f, 0.45f);
    }

    void draw(sf::RenderWindow& window) override { if (active) window.draw(mSprite); }
    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite; sf::Texture mTexture;
    std::vector<sf::IntRect> frames; int mCurrentFrame = 0; float mElapsedTime = 0.0f;
};

class Drone : public Target {
public:
    Drone() : Target(150) {
        if (mTexture.loadFromFile("drone1.png")) {
            mSprite.setTexture(mTexture);
            frames = {
                sf::IntRect(20, 0, 210, 270), sf::IntRect(267, 0, 160, 270),
                sf::IntRect(466, 0, 178, 270), sf::IntRect(675, 0, 166, 270),
                sf::IntRect(880, 0, 204, 270), sf::IntRect(1087, 0, 170, 270)
            };
            mSprite.setTextureRect(frames[0]);
            mSprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
            mSprite.setScale(0.4f, 0.4f);
        }
    }

    bool isDrone() const override { return true; }

    void update(sf::Time deltaTime) override {
        position.x += speed * deltaTime.asSeconds();
        mSprite.setPosition(position);

        mElapsedTime += deltaTime.asSeconds();
        if (mElapsedTime >= 0.06f) {
            mElapsedTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % 6;
            mSprite.setTextureRect(frames[mCurrentFrame]);
            mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
        }

        if (speed < 0.0f) mSprite.setScale(-0.4f, 0.4f);
        else mSprite.setScale(0.4f, 0.4f);
    }

    void draw(sf::RenderWindow& window) override { if (active) window.draw(mSprite); }
    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite; sf::Texture mTexture;
    std::vector<sf::IntRect> frames; int mCurrentFrame = 0; float mElapsedTime = 0.0f;
};

class Cow : public Target {
public:
    Cow() : Target(-150) {
        if (mTexture.loadFromFile("cow1.png")) {
            mSprite.setTexture(mTexture);
            frames = {
                sf::IntRect(52, 0, 133, 270), sf::IntRect(225, 0, 125, 270),
                sf::IntRect(378, 0, 131, 270), sf::IntRect(521, 0, 140, 270), sf::IntRect(702, 0, 133, 270)
            };
            mSprite.setTextureRect(frames[0]);
            mSprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
            mSprite.setScale(0.68f, 0.68f);
        }
    }

    void update(sf::Time deltaTime) override {
        position.x += speed * deltaTime.asSeconds();
        mSprite.setPosition(position);

        mElapsedTime += deltaTime.asSeconds();
        if (mElapsedTime >= 0.1f) {
            mElapsedTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % 5;
            mSprite.setTextureRect(frames[mCurrentFrame]);
            mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
        }
        if (speed < 0.0f) mSprite.setScale(-0.68f, 0.68f);
        else mSprite.setScale(0.68f, 0.68f);
    }

    void draw(sf::RenderWindow& window) override { if (active) window.draw(mSprite); }
    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite; sf::Texture mTexture;
    std::vector<sf::IntRect> frames; int mCurrentFrame = 0; float mElapsedTime = 0.0f;
};

// ==========================================
// --- БОСС ---
// ==========================================
enum class BossState { MOVING, HOVERING, DASHING };

class Boss : public Target {
public:
    Boss() : Target(1000) {
        mMaxHp = 5.25f;
        mHp = mMaxHp;
        mIsEnraged = false;

        mState = BossState::MOVING;
        mStateTimer = 3.0f;
        speedY = 150.0f;
        speedX = -100.0f;
        mTextureChangedInHover = false;

        // Загружаем обе текстуры заранее
        if (mTexture1.loadFromFile("boss1.png")) {
            int h = mTexture1.getSize().y;
            frames = {
                sf::IntRect(8, 0, 312, h),
                sf::IntRect(326, 0, 308, h),
                sf::IntRect(641, 0, 314, h),
                sf::IntRect(960, 0, 311, h)
            };
            mSprite.setTexture(mTexture1);
            mSprite.setTextureRect(frames[0]);
            mSprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
            mSprite.setScale(0.30f, 0.30f);
        }

        if (!mTexture2.loadFromFile("boss2.png")) {
            std::cerr << "[SFML Error] Failed to load boss2.png!" << std::endl;
        }

        mHpBg.setSize(sf::Vector2f(100.0f, 10.0f));
        mHpBg.setOrigin(50.0f, 5.0f);
        mHpBg.setFillColor(sf::Color(50, 50, 50));
        mHpBg.setOutlineThickness(1.5f);
        mHpBg.setOutlineColor(sf::Color::Black);

        mHpBar.setSize(sf::Vector2f(100.0f, 10.0f));
        mHpBar.setOrigin(50.0f, 5.0f);
        mHpBar.setFillColor(sf::Color(220, 50, 50));
    }

    bool isBoss() const override { return true; }

    // --- ФУНКЦИЯ ПРОВЕРКИ ЩИТА ДЛЯ КЛАССА GAME ---
    bool isShieldActive() const {
        return (mState == BossState::HOVERING && mTextureChangedInHover);
    }

    HitResult takeDamage(float damage) override {
        // Если у босса сработала задержка (он уже мигает от прошлого попадания), игнорируем
        if (mFlashTimer > 0.0f) {
            return HitResult::IGNORED;
        }

        // --- ПРОВЕРКА НА ДВЕ СЕКУНДЫ ЩИТА ---
        if (mState == BossState::HOVERING && mTextureChangedInHover) {
            // Если горит вторая текстура, ХП НЕ отнимаются и КРАСНЫМ он НЕ становится. 
            // Сразу возвращаем DAMAGED, чтобы ракета в Game.cpp просто взорвалась
            return HitResult::DAMAGED;
        }

        // Включаем мигание красным цветом ТОЛЬКО если щит не активен!
        mFlashTimer = 0.4f;

        // Если щита нет — отнимаем здоровье как обычно
        mHp -= damage;

        if (mHp <= mMaxHp * 0.5f && !mIsEnraged) {
            mIsEnraged = true;
            speedY *= 1.5f;
            speedX *= 1.5f;
        }

        if (mHp <= 0.0f) {
            setActive(false);
            return HitResult::KILLED;
        }

        return HitResult::DAMAGED;
    }

    void update(sf::Time deltaTime) override {
        float dt = deltaTime.asSeconds();
        mStateTimer -= dt;

        // --- ЛОГИКА СОСТОЯНИЙ И ПЕРЕКЛЮЧЕНИЯ ТЕКСТУР ---
        if (mState == BossState::MOVING) {
            position.y += speedY * dt;
            position.x += speedX * dt;

            if (position.y < 200.0f || position.y > 500.0f) speedY = -speedY;
            if (position.x < 600.0f || position.x > 1150.0f) speedX = -speedX;

            if (mStateTimer <= 0.0f) {
                mState = BossState::HOVERING;
                mStateTimer = mIsEnraged ? 2.0f : 4.0f; // Стоит в 2 раза дольше (4 секунды)
                mTextureChangedInHover = false;

                mSprite.setTexture(mTexture1);
                mSprite.setTextureRect(frames[mCurrentFrame]);
            }
        }
        else if (mState == BossState::HOVERING) {
            float halfTime = mIsEnraged ? 1.0f : 2.0f;

            // Переключаем на boss2.png ровно один раз по прошествии половины времени (последние 2 секунды)
            if (mStateTimer <= halfTime && !mTextureChangedInHover) {
                mSprite.setTexture(mTexture2);

                // Аккуратно сбрасываем прямоугольник под полный размер boss2.png без флага true
                sf::Vector2u size = mTexture2.getSize();
                mSprite.setTextureRect(sf::IntRect(0, 0, size.x, size.y));

                mTextureChangedInHover = true;
            }

            if (mStateTimer <= 0.0f) {
                mState = BossState::DASHING;
                mStateTimer = 1.0f;

                mSprite.setTexture(mTexture1);
                mSprite.setTextureRect(frames[mCurrentFrame]);

                speedX = (rand() % 2 == 0) ? -600.0f : 600.0f;
                speedY = (rand() % 2 == 0) ? -200.0f : 200.0f;

                if (mIsEnraged) { speedX *= 1.5f; speedY *= 1.5f; }
            }
        }
        else if (mState == BossState::DASHING) {
            position.x += speedX * dt;
            position.y += speedY * dt;

            if (position.x < 600.0f || position.x > 1150.0f) speedX = -speedX;
            if (position.y < 200.0f || position.y > 550.0f) speedY = -speedY;

            if (mStateTimer <= 0.0f) {
                mState = BossState::MOVING;
                mStateTimer = 3.0f;

                mSprite.setTexture(mTexture1);
                mSprite.setTextureRect(frames[mCurrentFrame]);

                speedX = (rand() % 2 == 0) ? -150.0f : 150.0f;
                speedY = (rand() % 2 == 0) ? -150.0f : 150.0f;
                if (mIsEnraged) { speedX *= 1.5f; speedY *= 1.5f; }
            }
        }

        // --- АНИМАЦИЯ КАДРОВ ---
        if (mState != BossState::HOVERING || !mTextureChangedInHover) {
            mElapsedTime += dt;
            float currentAnimSpeed = mIsEnraged ? 0.08f : 0.15f;

            if (mElapsedTime >= currentAnimSpeed) {
                mElapsedTime = 0.0f;
                mCurrentFrame = (mCurrentFrame + 1) % frames.size();
                mSprite.setTextureRect(frames[mCurrentFrame]);
                mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
            }
        }
        else {
            // Если включен щит boss2.png, центрируем его по реальным габаритам картинки
            sf::Vector2u size = mTexture2.getSize();
            mSprite.setOrigin(size.x / 2.0f, size.y / 2.0f);
        }

        // Эффект получения урона (подсветка красным цветом)
        if (mFlashTimer > 0.0f) {
            mFlashTimer -= dt;
            mSprite.setColor(sf::Color(255, 100, 100));
        }
        else {
            if (mIsEnraged) mSprite.setColor(sf::Color(255, 200, 200));
            else mSprite.setColor(sf::Color::White);
        }

        // --- ТУТ НАСТРАИВАЕМ РАЗМЕР (МАСШТАБ) ДЛЯ КАЖДОГО БОССА ---
        float scaleX = 0.30f; // Это размер по умолчанию для первой формы (0.30f)
        float scaleY = 0.30f;

        // Если босс стоит и включилась вторая текстура щита (boss2)
        if (mState == BossState::HOVERING && mTextureChangedInHover) {
            scaleX = 0.40f; // Сделайте это число больше/меньше, чтобы изменить масштаб щита
            scaleY = 0.40f;
        }

        // Поворот модельки влево/вправо с учетом масштаба
        if (speedX < 0.0f) {
            mSprite.setScale(-scaleX, scaleY);
        }
        else {
            mSprite.setScale(scaleX, scaleY);
        }

        mSprite.setPosition(position);

        mHpBg.setPosition(position.x, position.y - 80.0f);
        mHpBar.setPosition(position.x, position.y - 80.0f);

        float hpPercent = mHp / mMaxHp;
        if (hpPercent < 0.0f) hpPercent = 0.0f;
        mHpBar.setSize(sf::Vector2f(100.0f * hpPercent, 10.0f));
    }

    void draw(sf::RenderWindow& window) override {
        if (active) {
            window.draw(mSprite);
            window.draw(mHpBg);
            window.draw(mHpBar);
        }
    }

    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite;
    sf::Texture mTexture1;
    sf::Texture mTexture2;

    std::vector<sf::IntRect> frames;
    int mCurrentFrame = 0;
    float mElapsedTime = 0.0f;

    sf::RectangleShape mHpBg;
    sf::RectangleShape mHpBar;

    float mMaxHp;
    float mHp;
    float mFlashTimer = 0.0f;
    bool mIsEnraged;

    BossState mState;
    float mStateTimer;
    float speedY;
    float speedX;

    bool mTextureChangedInHover;
};

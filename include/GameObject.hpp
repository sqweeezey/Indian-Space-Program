#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>

// --- БАЗОВЫЙ КЛАСС ---
class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;

    // Удобный сеттер для x и y
    virtual void setPosition(float x, float y) = 0;
    virtual sf::Vector2f getPosition() const = 0;
};

// --- ЦЕЛЬ ---
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

protected:
    int coinReward;
    bool active;
    float speed;
    sf::Vector2f position;
};

// --- ПТИЦА ---
class Bird : public Target {
public:
    Bird() : Target(50) {
        if (mTexture.loadFromFile("bird1.png")) {
            mSprite.setTexture(mTexture);

            // Твои точные границы:
            frames = {
                sf::IntRect(46, 0, 171, 270),   // Высота 250 вместо 218
                sf::IntRect(253, 0, 180, 270),
                sf::IntRect(460, 0, 175,270),
                sf::IntRect(660, 0, 170, 270),
                sf::IntRect(860, 0, 185, 270),
                sf::IntRect(1077, 0, 156, 270)
            };

            mSprite.setTextureRect(frames[0]);

            // Важно: ставим origin по центру самого первого кадра
            // Ширина 171 / 2 = 85.5, Высота 218 / 2 = 109
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

            // Обновляем прямоугольник вырезки
            mSprite.setTextureRect(frames[mCurrentFrame]);

            // Сбрасываем origin под каждый конкретный кадр, чтобы птица не прыгала
            // Если птица все равно дрожит, закомментируй следующую строку:
            mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
        }

        if (speed < 0.0f) mSprite.setScale(-0.45f, 0.45f);
        else mSprite.setScale(0.45f, 0.45f);
    }

    void draw(sf::RenderWindow& window) override {
        if (active) window.draw(mSprite);
    }

    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite;
    sf::Texture mTexture;
    std::vector<sf::IntRect> frames;
    int mCurrentFrame = 0;
    float mElapsedTime = 0.0f;
};

// --- ДРОН ---
class Drone : public Target {
public:
    Drone() : Target(150) {
        if (mTexture.loadFromFile("drone1.png")) {
            mSprite.setTexture(mTexture);

            // Твои точные координаты: (X_начало, Y_начало, Ширина, Высота)
            frames = {
                sf::IntRect(20, 0, 210, 270),   // 20-230
                sf::IntRect(267, 0, 160, 270),  // 267-427
                sf::IntRect(466, 0, 178, 270),  // 466-644
                sf::IntRect(675, 0, 166, 270),  // 675-841
                sf::IntRect(880, 0, 204, 270),  // 880-1084
                sf::IntRect(1087, 0, 170, 270)  // 1087-1257
            };

            mSprite.setTextureRect(frames[0]);

            // Центрируем Origin по первому кадру
            mSprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
            mSprite.setScale(0.4f, 0.4f);
        }
    }

    void update(sf::Time deltaTime) override {
        // --- ЛОГИКА ЗАВИСАНИЯ В ВОЗДУХЕ ---
        if (mIsHovering) {
            mHoverTimer -= deltaTime.asSeconds();
            if (mHoverTimer <= 0.0f) {
                mIsHovering = false;
                speed = -speed; // Меняем направление после того как отвис!
            }
        }
        else {
            position.x += speed * deltaTime.asSeconds();

            // Шанс зависнуть в воздухе (0.5% каждый кадр, чтобы не дергался слишком часто)
            if (rand() % 1000 < 5) {
                mIsHovering = true;
                mHoverTimer = 0.5f + static_cast<float>(rand() % 15) / 10.0f; // Висит от 0.5 до 2.0 секунд
            }
        }

        mSprite.setPosition(position);

        // --- ЛОГИКА АНИМАЦИИ (работает даже когда дрон висит на месте) ---
        mElapsedTime += deltaTime.asSeconds();
        if (mElapsedTime >= 0.06f) {
            mElapsedTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % 6;

            // Меняем кадр
            mSprite.setTextureRect(frames[mCurrentFrame]);

            // Динамически меняем центр (origin) под размер текущего кадра.
            mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
        }

        // Поворот по оси X
        if (speed < 0.0f) mSprite.setScale(-0.4f, 0.4f);
        else mSprite.setScale(0.4f, 0.4f);
    }

    void draw(sf::RenderWindow& window) override {
        if (active) window.draw(mSprite);
    }

    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite;
    sf::Texture mTexture;
    std::vector<sf::IntRect> frames;
    int mCurrentFrame = 0;
    float mElapsedTime = 0.0f;

    // Переменные для зависания
    float mHoverTimer = 0.0f;
    bool mIsHovering = false;
};

// --- КОРОВА (ПРЕПЯТСТВИЕ) ---
class Cow : public Target {
public:
    Cow() : Target(-150) { // Штраф за сбитие (как награда за дрона, только с минусом)
        if (mTexture.loadFromFile("cow1.png")) {
            mSprite.setTexture(mTexture);

            // Твои точные координаты кадров: (X, Y, Ширина, Высота)
            frames = {
                sf::IntRect(52,  0, 133, 270), // Кадр 1
                sf::IntRect(225, 0, 125, 270), // Кадр 2
                sf::IntRect(378, 0, 131, 270), // Кадр 3 (Самый пик прыжка)
                sf::IntRect(521, 0, 140, 270), // Кадр 4
                sf::IntRect(702, 0, 133, 270)  // Кадр 5
            };

            mSprite.setTextureRect(frames[0]);
            mSprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
            mSprite.setScale(0.68f, 0.68f); // Такой же масштаб, как у птицы
        }
    }

    void update(sf::Time deltaTime) override {
        position.x += speed * deltaTime.asSeconds();
        mSprite.setPosition(position);

        mElapsedTime += deltaTime.asSeconds();
        // Прыгает чуть медленнее, чем машет крыльями птица
        if (mElapsedTime >= 0.1f) {
            mElapsedTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % 5; // 5 кадров анимации

            mSprite.setTextureRect(frames[mCurrentFrame]);
            mSprite.setOrigin(frames[mCurrentFrame].width / 2.0f, frames[mCurrentFrame].height / 2.0f);
        }

        // Разворот спрайта по направлению движения
        if (speed < 0.0f) mSprite.setScale(-0.68f, 0.68f);
        else mSprite.setScale(0.68f, 0.68f);
    }

    void draw(sf::RenderWindow& window) override {
        if (active) window.draw(mSprite);
    }

    sf::FloatRect getBounds() const override { return mSprite.getGlobalBounds(); }

private:
    sf::Sprite mSprite;
    sf::Texture mTexture;
    std::vector<sf::IntRect> frames;
    int mCurrentFrame = 0;
    float mElapsedTime = 0.0f;
};
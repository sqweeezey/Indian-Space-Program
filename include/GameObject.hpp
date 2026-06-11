#pragma once
#include <SFML/Graphics.hpp>

// --- БАЗОВЫЙ ИГРОВОЙ ОБЪЕКТ ---
class GameObject {
public:
    GameObject() = default;
    virtual ~GameObject() = default;

    virtual void update(sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0; // НОВОЕ: для коллизий

    void setPosition(float x, float y) {
        position = sf::Vector2f(x, y);
        updateShapePosition();
    }
    sf::Vector2f getPosition() const { return position; }

protected:
    sf::Vector2f position;
    virtual void updateShapePosition() = 0; // Внутреннее обновление графики
};

// --- ПРОМЕЖУТОЧНЫЙ КЛАСС: ЦЕЛЬ ---
class Target : public GameObject {
public:
    Target(int reward) : coinReward(reward), active(true), speed(0.0f) {}

    int getReward() const { return coinReward; }

    // НОВОЕ: Управление состоянием и скоростью
    bool isActive() const { return active; }
    void setActive(bool state) { active = state; }
    void setSpeed(float s) { speed = s; }
    float getSpeed() const { return speed; }

protected:
    int coinReward;
    bool active;
    float speed;
};

// --- КОНКРЕТНЫЕ ЦЕЛИ ---

class Bird : public Target {
public:
    // Птица дает 50 монет
    Bird() : Target(50) {
        shape.setRadius(15.0f);
        shape.setFillColor(sf::Color::Red); // Птица - красный круг
        shape.setOrigin(15.0f, 15.0f);
    }

    void update(sf::Time deltaTime) override {
        position.x += speed * deltaTime.asSeconds();
        updateShapePosition();
    }

    void draw(sf::RenderWindow& window) override {
        if (active) window.draw(shape);
    }

    sf::FloatRect getBounds() const override { return shape.getGlobalBounds(); }

protected:
    sf::CircleShape shape;
    void updateShapePosition() override { shape.setPosition(position); }
};

class Drone : public Target {
public:
    // Дрон дает 150 монет!
    Drone() : Target(150) {
        shape.setSize(sf::Vector2f(30.0f, 30.0f));
        shape.setFillColor(sf::Color::Blue); // Дрон - синий квадрат
        shape.setOrigin(15.0f, 15.0f);
    }

    void update(sf::Time deltaTime) override {
        position.x += speed * deltaTime.asSeconds();
        updateShapePosition();
    }

    void draw(sf::RenderWindow& window) override {
        if (active) window.draw(shape);
    }

    sf::FloatRect getBounds() const override { return shape.getGlobalBounds(); }

protected:
    sf::RectangleShape shape;
    void updateShapePosition() override { shape.setPosition(position); }
};
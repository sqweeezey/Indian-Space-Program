#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <memory> // Для умных указателей
#include "QTEManager.hpp"
#include "Rocket.hpp"
#include "GameObject.hpp"
#include "Equipment.hpp" // Подключаем магазин напарника

enum class GameState { MENU, SHOP, PLAY, PAUSE };

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void saveProgress();
    void loadProgress();
    void applyUpgrades();
    void spawnTarget(int index); // Функция для создания цели (Птица или Дрон)

private:
    sf::RenderWindow mWindow;
    GameState mCurrentState;
    int mCoins;
    bool mWasFlying;

    QTEManager mQteManager;
    Rocket mRocket;
    sf::CircleShape mTireVisual;

    // НОВОЕ: Универсальный список целей (Птицы и Дроны вместе)
    std::vector<std::unique_ptr<Target>> mTargets;

    sf::Font mFont;

    // НОВОЕ: Списки оборудования из кода напарника
    std::vector<Motor> mMotors;
    std::vector<Tire> mTires;
    int mCurrentMotorIndex;
    int mCurrentTireIndex;
};
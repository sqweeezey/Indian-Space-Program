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
    sf::Texture mTireTexture;
    sf::Sprite mTireVisual;

    // -- - НОВОЕ: Текстура и Спрайт Персонажа - Механика-- -
        sf::Texture mCharacterTexture;
    sf::Sprite mCharacterSprite;

    // --- НОВОЕ: Параметры анимации механика ---
    std::vector<sf::IntRect> mCharacterFrames; // Прямоугольники кадров
    int mCharacterCurrentFrame;                 // Текущий кадр
    sf::Clock mAnimationClock;                 // Таймер для смены кадров
    bool mIsCelebrating;                       // Флаг: празднуем ли мы сейчас?
    float mFrameDuration;                      // Длительность одного кадра
    int mCelebrateStartFrame;                 // Кадр, с которого начинается радость (3-й)

    std::vector<std::unique_ptr<Target>> mTargets;

    sf::Font mFont;

    // --- НОВОЕ: Элементы UI для счета ---
    sf::RectangleShape mScoreBackground;
    sf::Text mScoreText;
    sf::CircleShape mCoinIcon;

    // Вместо mMotors и mTires делаем один вектор сетов:
    std::vector<UpgradeSet> mUpgrades;

    // Вместо двух индексов — один общий текущий уровень прокачки:
    int mCurrentUpgradeLevel;

    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;

    sf::Texture mLauncherTexture;
    sf::Sprite mLauncherSprite;
    sf::Texture mShopBgTexture;
    sf::Sprite  mShopBgSprite;

    sf::Texture mBoardTexture;
    sf::Sprite  mBoardSprite;

    // Временные текстуры/спрайты для витрины магазина
    sf::Texture mPreviewEngineTextures[3];
    sf::Texture mPreviewTireTextures[3];
    sf::Sprite  mPreviewEngineSprites[3];
    sf::Sprite  mPreviewTireSprites[3];

    // --- ПЛАШКИ ИНТЕРФЕЙСА МАГАЗИНА ---
    sf::Texture mNewCoinBgTexture;
    sf::Sprite  mNewCoinBgSprite;

    sf::Texture mPriceBgTexture;
    sf::Sprite  mPriceBgSprites[3];

    sf::Texture mBuyKeyTexture;
    sf::Sprite  mBuyKeySprites[3];
    // Добавь это в приватную секцию (private:) класса Game в файле Game.hpp:
    sf::Texture mMenuBgTexture;
    sf::Sprite  mMenuBgSprite;
    sf::Sprite  mWorkshopBtnSprite;
    sf::Sprite  mStreetBtnSprite;
    sf::Text    mMenuTitleText;
    sf::Text    mWorkshopBtnText;
    sf::Text    mStreetBtnText;
};

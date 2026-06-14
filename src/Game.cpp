#include "Game.hpp"
#include "GameObject.hpp"
#include <cstdlib>
#include <ctime>

Game::Game()
    : mWindow(sf::VideoMode(1280, 720), "Warsztat Jugaad")
    , mCurrentState(GameState::MENU)
    , mCoins(0)
    , mWasFlying(false)
    , mCurrentUpgradeLevel(0)
    , mCharacterCurrentFrame(0)
    , mIsCelebrating(false)
    , mFrameDuration(0.12f)
    , mCelebrateStartFrame(2)
{
    srand(static_cast<unsigned int>(time(0)));
    mWindow.setFramerateLimit(60);
    loadProgress();

    if (!mFont.loadFromFile("VT323-Regular.ttf")) {
        std::cout << "[Blad] Brak czcionki!" << std::endl;
    }

    // --- НАСТРОЙКА ИНТЕРФЕЙСА СЧЕТА ---
    mScoreBackground.setSize(sf::Vector2f(200.0f, 45.0f));
    mScoreBackground.setFillColor(sf::Color(35, 45, 56));
    mScoreBackground.setOutlineThickness(3.0f);
    mScoreBackground.setOutlineColor(sf::Color::White);
    mScoreBackground.setPosition(20.0f, 20.0f);

    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(30);
    mScoreText.setFillColor(sf::Color::White);
    mScoreText.setPosition(35.0f, 22.0f);

    mCoinIcon.setRadius(12.0f);
    mCoinIcon.setFillColor(sf::Color(255, 215, 0));
    mCoinIcon.setPosition(180.0f, 30.0f);

    if (!mBackgroundTexture.loadFromFile("game_bg.png")) {
        std::cout << "[Blad] Nie mozna zaladowac game_bg.png!" << std::endl;
    }
    else {
        mBackgroundSprite.setTexture(mBackgroundTexture);
        float scaleX = 1280.0f / mBackgroundTexture.getSize().x;
        float scaleY = 720.0f / mBackgroundTexture.getSize().y;
        mBackgroundSprite.setScale(scaleX, scaleY);
    }

    // --- НАСТРОЙКА ПЕРСОНАЖА ---
    if (!mCharacterTexture.loadFromFile("character_sheet.png")) {
        std::cout << "[Blad] Nie mozna zaladowac character_sheet.png!" << std::endl;
    }
    else {
        mCharacterSprite.setTexture(mCharacterTexture);

        mCharacterFrames.clear();
        mCharacterFrames.push_back(sf::IntRect(16, 0, 171, 600));
        mCharacterFrames.push_back(sf::IntRect(209, 0, 191, 600));
        mCharacterFrames.push_back(sf::IntRect(411, 0, 211, 600));
        mCharacterFrames.push_back(sf::IntRect(627, 0, 191, 600));

        mCharacterSprite.setTextureRect(mCharacterFrames[0]);
        mCharacterSprite.setOrigin(mCharacterFrames[0].width / 2.0f, mCharacterFrames[0].height / 2.0f);

        mCharacterSprite.setPosition(110.0f, 520.0f);
        mCharacterSprite.setScale(0.45f, 0.45f);
    }

    // --- НАСТРОЙКА НАБОРОВ ОБНОВЛЕНИЙ ---
    mUpgrades.push_back(UpgradeSet("Zestaw: Garazny Chlam", 0, true, 0.5f, 1.5f));
    mUpgrades.push_back(UpgradeSet("Zestaw: Diesel-Pak", 150, false, 1.0f, 1.0f));
    mUpgrades.push_back(UpgradeSet("Zestaw: Ultimate-Craft", 350, false, 2.0f, 0.4f));

    // --- НАСТРОЙКА ГРАФИКИ МАГАЗИНА ---
    if (!mShopBgTexture.loadFromFile("fon_mag.png")) {
        std::cout << "[Blad] Nie mozna zaladowac fon_mag.png!" << std::endl;
    }
    else {
        mShopBgSprite.setTexture(mShopBgTexture);
        float sX = 1280.0f / mShopBgTexture.getSize().x;
        float sY = 720.0f / mShopBgTexture.getSize().y;
        mShopBgSprite.setScale(sX, sY);
    }

    if (!mBoardTexture.loadFromFile("tablica.png")) {
        std::cout << "[Blad] Nie mozna zaladowac tablica.png!" << std::endl;
    }
    else {
        mBoardSprite.setTexture(mBoardTexture);
        mBoardSprite.setOrigin(mBoardTexture.getSize().x / 2.0f, mBoardTexture.getSize().y / 2.0f);
        mBoardSprite.setPosition(640.0f, 350.0f);
        mBoardSprite.setScale(0.85f, 0.85f);
    }

    // Загружаем мини-превью для каждой из трех витрин
    for (int i = 0; i < 3; ++i) {
        std::string eFile = "engine" + std::to_string(i + 1) + ".png";
        std::string tFile = "tire" + std::to_string(i + 1) + ".png";

        if (mPreviewEngineTextures[i].loadFromFile(eFile)) {
            mPreviewEngineSprites[i].setTexture(mPreviewEngineTextures[i]);
            mPreviewEngineSprites[i].setOrigin(mPreviewEngineTextures[i].getSize().x / 2.0f, mPreviewEngineTextures[i].getSize().y / 2.0f);
        }
        if (mPreviewTireTextures[i].loadFromFile(tFile)) {
            mPreviewTireSprites[i].setTexture(mPreviewTireTextures[i]);
            mPreviewTireSprites[i].setOrigin(mPreviewTireTextures[i].getSize().x / 2.0f, mPreviewTireTextures[i].getSize().y / 2.0f);
        }
    }

    // Загрузка новых элементов интерфейса магазина
    if (mNewCoinBgTexture.loadFromFile("monetybg2.png")) {
        mNewCoinBgSprite.setTexture(mNewCoinBgTexture);
        mNewCoinBgSprite.setPosition(30.0f, 20.0f);
    }

    if (mPriceBgTexture.loadFromFile("pricebg.png")) {
        for (int i = 0; i < 3; ++i) {
            mPriceBgSprites[i].setTexture(mPriceBgTexture);
            mPriceBgSprites[i].setOrigin(mPriceBgTexture.getSize().x / 2.0f, mPriceBgTexture.getSize().y / 2.0f);
        }
    }

    if (mBuyKeyTexture.loadFromFile("buykey.png")) {
        for (int i = 0; i < 3; ++i) {
            mBuyKeySprites[i].setTexture(mBuyKeyTexture);
            mBuyKeySprites[i].setOrigin(mBuyKeyTexture.getSize().x / 2.0f, mBuyKeyTexture.getSize().y / 2.0f);
        }
    }

    applyUpgrades();

    for (int i = 0; i < 3; ++i) {
        mTargets.push_back(nullptr);
        spawnTarget(i);
    }
}

void Game::spawnTarget(int index) {
    float randomY = 100.0f + static_cast<float>(rand() % 400);
    int chance = rand() % 100;

    if (chance < 50) {
        mTargets[index] = std::make_unique<Bird>();
    }
    else if (chance < 80) {
        mTargets[index] = std::make_unique<Drone>();
    }
    else {
        mTargets[index] = std::make_unique<Cow>();
    }

    int side = rand() % 2;
    float baseSpeed = 100.0f + static_cast<float>(rand() % 150);

    if (side == 0) {
        mTargets[index]->setPosition(-50.0f, randomY);
        mTargets[index]->setSpeed(baseSpeed);
    }
    else {
        mTargets[index]->setPosition(1330.0f, randomY);
        mTargets[index]->setSpeed(-baseSpeed);
    }
}

void Game::applyUpgrades() {
    float enginePwr = mUpgrades[mCurrentUpgradeLevel].getSpeedBonus();
    float tireDrg = mUpgrades[mCurrentUpgradeLevel].getAeroBonus();
    mRocket.upgrade(enginePwr, tireDrg);

    std::string engineFile = "engine" + std::to_string(mCurrentUpgradeLevel + 1) + ".png";
    if (!mLauncherTexture.loadFromFile(engineFile)) {
        std::cout << "[Blad] Nie mozna zaladowac " << engineFile << "!" << std::endl;
    }
    else {
        mLauncherSprite.setTexture(mLauncherTexture, true);
        mLauncherSprite.setOrigin(mLauncherTexture.getSize().x / 2.0f, mLauncherTexture.getSize().y / 2.0f);

        if (mCurrentUpgradeLevel == 0) {
            mLauncherSprite.setScale(0.45f, 0.45f);
            mLauncherSprite.setPosition(210.0f, 600.0f);
        }
        else if (mCurrentUpgradeLevel == 1) {
            mLauncherSprite.setScale(0.29f, 0.29f);
            mLauncherSprite.setPosition(230.0f, 580.0f);
        }
        else if (mCurrentUpgradeLevel == 2) {
            mLauncherSprite.setScale(0.28f, 0.28f);
            mLauncherSprite.setPosition(210.0f, 600.0f);
        }
    }

    std::string tireFile = "tire" + std::to_string(mCurrentUpgradeLevel + 1) + ".png";
    if (!mTireTexture.loadFromFile(tireFile)) {
        std::cout << "[Blad] Nie mozna zaladowac " << tireFile << "!" << std::endl;
    }
    else {
        mTireVisual.setTexture(mTireTexture, true);
        mTireVisual.setOrigin(mTireTexture.getSize().x / 2.0f, mTireTexture.getSize().y / 2.0f);

        if (mCurrentUpgradeLevel == 0) {
            mTireVisual.setScale(0.18f, 0.18f);
        }
        else if (mCurrentUpgradeLevel == 1) {
            mTireVisual.setScale(0.07f, 0.07f);
        }
        else if (mCurrentUpgradeLevel == 2) {
            mTireVisual.setScale(0.45f, 0.45f);
        }
    }
}

void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) mWindow.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Num1) mCurrentState = GameState::MENU;
            if (event.key.code == sf::Keyboard::Num2) mCurrentState = GameState::SHOP;
            if (event.key.code == sf::Keyboard::Num3) mCurrentState = GameState::PLAY;
            if (event.key.code == sf::Keyboard::Escape) {
                if (mCurrentState == GameState::PLAY) mCurrentState = GameState::PAUSE;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::Space && mCurrentState == GameState::PLAY) {
                if (mQteManager.isActive()) {
                    float result = mQteManager.stop();
                    mRocket.launch(result);
                }
            }
        }

        // --- ОБРАБОТКА КЛИКОВ МЫШКОЙ В МАГАЗИНЕ ---
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (mCurrentState == GameState::SHOP) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(mWindow);
                
                // КРИТИЧЕСКАЯ ИСПРАВЛЕННАЯ СТРОКА: Переводим пиксели монитора в координаты игры (1280x720)
                sf::Vector2f mousePos = mWindow.mapPixelToCoords(pixelPos);

                // ТВОИ ТОЧНЫЕ КООРДИНАТЫ КНОПКИ (ИЗ REBDER), ТЕПЕРЬ ОНИ СВЯЗАНЫ ДЛЯ КЛИКА РОДНОГО РЕЖИМА
                sf::Vector2f buttonPositions[3] = {
                    sf::Vector2f(408.0f, 513.0f),  // Левая кнопка
                    sf::Vector2f(640.0f, 513.0f),  // Центральная кнопка
                    sf::Vector2f(872.0f, 513.0f)   // Правая кнопка
                };

                for (int i = 0; i < 3; ++i) {
                    // Создаем хитбокс вокруг твоих кнопок buykey (с учетом оригинального масштаба)
                    sf::FloatRect btnBounds(
                        buttonPositions[i].x - 85.0f, // Смещение влево от центра
                        buttonPositions[i].y - 25.0f, // Смещение вверх от центра
                        170.0f, 50.0f                 // Ширина и высота кликабельной зоны
                    );

                    // Проверяем попадание по пересчитанной координате мыши
                    if (btnBounds.contains(mousePos.x, mousePos.y)) {
                        // Если сет заблокирован — покупаем его
                        if (!mUpgrades[i].getIsUnlocked()) {
                            if (mCoins >= mUpgrades[i].getPrice()) {
                                mCoins -= mUpgrades[i].getPrice();
                                mUpgrades[i].unlock();
                                mCurrentUpgradeLevel = i; 
                                applyUpgrades();
                                std::cout << "[Shop] Kupiono zestaw: " << mUpgrades[i].getName() << std::endl;
                            } else {
                                std::cout << "[Shop] Za malo monet!" << std::endl;
                            }
                        } 
                        // Если уже куплен — свободно переключаемся (Equip)
                        else {
                            mCurrentUpgradeLevel = i;
                            applyUpgrades();
                            std::cout << "[Shop] Wybrano zestaw level: " << i << std::endl;
                        }

                        saveProgress();
                        break; 
                    }
                }
            }
        }
    }
}

void Game::update(sf::Time deltaTime) {
    if (mCurrentState != GameState::PLAY) return;

    mScoreText.setString("MONETY: " + std::to_string(mCoins));

    if (mIsCelebrating) {
        if (mAnimationClock.getElapsedTime().asSeconds() > mFrameDuration) {
            mAnimationClock.restart();
            mCharacterCurrentFrame++;

            if (mCharacterCurrentFrame >= static_cast<int>(mCharacterFrames.size())) {
                mCharacterCurrentFrame = 0;
                mIsCelebrating = false;
            }

            mCharacterSprite.setTextureRect(mCharacterFrames[mCharacterCurrentFrame]);
            mCharacterSprite.setOrigin(mCharacterFrames[mCharacterCurrentFrame].width / 2.0f,
                mCharacterFrames[mCharacterCurrentFrame].height / 2.0f);
        }
    }
    else {
        mCharacterSprite.setTextureRect(mCharacterFrames[0]);
        mCharacterSprite.setOrigin(mCharacterFrames[0].width / 2.0f, mCharacterFrames[0].height / 2.0f);
    }

    for (size_t i = 0; i < mTargets.size(); ++i) {
        if (!mTargets[i]->isActive()) continue;

        mTargets[i]->update(deltaTime);

        sf::Vector2f pos = mTargets[i]->getPosition();
        float spd = mTargets[i]->getSpeed();

        bool flewRight = (spd > 0.0f && pos.x > 1300.0f);
        bool flewLeft = (spd < 0.0f && pos.x < -100.0f);

        if (flewRight || flewLeft) {
            spawnTarget(static_cast<int>(i));
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !mRocket.isFlying()) {
        if (!mQteManager.isActive()) {
            mQteManager.start();
        }
    }

    if (!mRocket.isFlying()) mQteManager.update(deltaTime);

    mRocket.update(deltaTime.asSeconds());

    if (mRocket.isFlying()) {
        sf::Vector2f startPos(290.0f, 550.0f);
        if (mCurrentUpgradeLevel == 1) {
            startPos = sf::Vector2f(340.0f, 510.0f);
        }
        else if (mCurrentUpgradeLevel == 2) {
            startPos = sf::Vector2f(305.0f, 575.0f);
        }

        mTireVisual.setPosition(startPos.x + mRocket.getDistance(), startPos.y - mRocket.getAltitude());
        mTireVisual.rotate(360.0f * deltaTime.asSeconds());

        sf::FloatRect tireBox = mTireVisual.getGlobalBounds();
        for (size_t i = 0; i < mTargets.size(); ++i) {
            if (mTargets[i]->isActive() && tireBox.intersects(mTargets[i]->getBounds())) {
                mTargets[i]->setActive(false);
                mCoins += mTargets[i]->getReward();

                if (mCoins < 0) mCoins = 0;

                std::cout << "[Gra] Trafienie! + " << mTargets[i]->getReward() << " monet! Stan: " << mCoins << std::endl;

                if (mTargets[i]->getReward() > 0 && !mIsCelebrating && mCharacterCurrentFrame == 0) {
                    mIsCelebrating = true;
                    mCharacterCurrentFrame = mCelebrateStartFrame;
                    mCharacterSprite.setTextureRect(mCharacterFrames[mCharacterCurrentFrame]);
                    mAnimationClock.restart();
                }

                spawnTarget(static_cast<int>(i));
            }
        }
    }

    bool currentlyFlying = mRocket.isFlying();
    if (mWasFlying && !currentlyFlying) {
        mCoins += static_cast<int>(mRocket.getDistance() / 10.0f);
        saveProgress();
    }
    mWasFlying = currentlyFlying;
}

void Game::render() {
    sf::Color bgColor;
    switch (mCurrentState) {
    case GameState::MENU:  bgColor = sf::Color(30, 30, 50);   break;
    case GameState::SHOP:  bgColor = sf::Color(50, 50, 30);   break;
    case GameState::PLAY:  bgColor = sf::Color(30, 50, 30);   break;
    case GameState::PAUSE: bgColor = sf::Color(50, 30, 30);   break;
    }
    mWindow.clear(bgColor);

    if (mCurrentState == GameState::SHOP) {
        // 1. Рисуем задний фон мастерской (склад)
        mWindow.draw(mShopBgSprite);

        // 2. Настраиваем и рисуем металлическую доску Workshop Specials
        mBoardSprite.setOrigin(mBoardTexture.getSize().x / 2.0f, mBoardTexture.getSize().y / 2.0f);
        mBoardSprite.setPosition(640.0f, 350.0f);

        // ТВОИ ЛИЧНЫЕ МАСШТАБЫ ТАБЛИЦЫ
        float boardScaleX = 0.42f;
        float boardScaleY = 0.35f;

        mBoardSprite.setScale(boardScaleX, boardScaleY);
        mWindow.draw(mBoardSprite);

        struct AbsoluteShelfConfig {
            int engIdx, tireIdx;
            float engScale, tireScale;
            float engX, engY;
            float tireX, tireY;
            float priceX, priceY;
            float btnX, btnY;
            std::string setName;
        };

        // ТВОИ ЛИЧНЫЕ КООРДИНАТЫ И НАЗВАНИЯ СЕТОВ
        AbsoluteShelfConfig shelves[3] = {
            // ЛЕВАЯ ВИТРИНА (Tuc-Tuc Set)
            {
                0, 0,
                0.3f, 0.11f,
                410.0f, 330.0f,
                450.0f, 300.0f,
                408.0f, 462.0f,
                408.0f, 513.0f,
                "Tuc-Tuc Set"
            },

            // ЦЕНТРАЛЬНАЯ ВИТРИНА (Abdurahman Set)
            {
                1, 1,
                0.16f, 0.04f,
                630.0f, 325.0f,
                680.0f, 290.0f,
                640.0f, 462.0f,
                640.0f, 513.0f,
                "Abdurahman Set"
            },

            // ПРАВАЯ ВИТРИНА (Jugaad Mad Set)
            {
                2, 2,
                0.16f, 0.3f,
                860.0f, 335.0f,
                915.0f, 321.0f,
                872.0f, 462.0f,
                872.0f, 513.0f,
                "Jugaad Mad Set"
            }
        };

        // =========================================================================
        //  ПЕРЕМЕННАЯ ВЫСОТЫ ДЛЯ ВСЕХ ТРЕХ НАЗВАНИЙ СЕТОВ СРАЗУ
        // =========================================================================
        float setsTitleHeight = 220.0f; // Меняй это число, чтобы сдвинуть ВСЕ ТРИ названия (меньше — выше, больше — ниже)
        // =========================================================================

        sf::Text uiText;
        uiText.setFont(mFont);
        uiText.setCharacterSize(22);
        uiText.setFillColor(sf::Color(235, 230, 215));

        for (int i = 0; i < 3; ++i) {
            AbsoluteShelfConfig cfg = shelves[i];

            // Рисуем Двигатель
            mPreviewEngineSprites[cfg.engIdx].setPosition(cfg.engX, cfg.engY);
            mPreviewEngineSprites[cfg.engIdx].setScale(cfg.engScale, cfg.engScale);
            mWindow.draw(mPreviewEngineSprites[cfg.engIdx]);

            // Рисуем Шину / Пилу
            mPreviewTireSprites[cfg.tireIdx].setPosition(cfg.tireX, cfg.tireY);
            mPreviewTireSprites[cfg.tireIdx].setScale(cfg.tireScale, cfg.tireScale);
            mWindow.draw(mPreviewTireSprites[cfg.tireIdx]);

            // --- НАЗВАНИЕ СЕТА НАД ВИТРИНОЙ ---
            sf::Text headerText;
            headerText.setFont(mFont);
            headerText.setCharacterSize(26);
            headerText.setStyle(sf::Text::Bold);
            headerText.setFillColor(sf::Color(45, 40, 35));
            headerText.setString(cfg.setName);
            headerText.setOrigin(headerText.getLocalBounds().width / 2.0f, headerText.getLocalBounds().height / 2.0f);

            // Связали общую высоту со всеми тремя заголовками
            headerText.setPosition(cfg.priceX, setsTitleHeight);
            mWindow.draw(headerText);

            // --- РЕНДЕР ТАБЛИЧКИ ЦЕНЫ (pricebg.png) ---
            mPriceBgSprites[i].setPosition(cfg.priceX, cfg.priceY);
            mPriceBgSprites[i].setScale(0.45f, 0.45f);
            mWindow.draw(mPriceBgSprites[i]);

            if (mUpgrades[i].getPrice() == 0 || mUpgrades[i].getIsUnlocked()) {
                uiText.setString("Unlocked");
            }
            else {
                uiText.setString(std::to_string(mUpgrades[i].getPrice()) + " Coins");
            }
            uiText.setOrigin(uiText.getLocalBounds().width / 2.0f, uiText.getLocalBounds().height / 2.0f);
            uiText.setPosition(cfg.priceX, cfg.priceY - 5.0f);
            mWindow.draw(uiText);

            // --- РЕНДЕР КНОПКИ КЛИКА (buykey.png) ---
            mBuyKeySprites[i].setPosition(cfg.btnX, cfg.btnY);
            mBuyKeySprites[i].setScale(0.55f, 0.55f);
            mWindow.draw(mBuyKeySprites[i]);

            if (!mUpgrades[i].getIsUnlocked()) {
                uiText.setString("Purchase");
            }
            else if (mCurrentUpgradeLevel == i) {
                uiText.setString("Equipped");
            }
            else {
                uiText.setString("Equip");
            }
            uiText.setOrigin(uiText.getLocalBounds().width / 2.0f, uiText.getLocalBounds().height / 2.0f);
            uiText.setPosition(cfg.btnX, cfg.btnY - 5.0f);
            mWindow.draw(uiText);
        }

        // --- ГЛАВНЫЙ ЗАГОЛОВОК ДОСКИ "Workshop Specials" (УПРАВЛЯЕТСЯ ОТДЕЛЬНО) ---
        sf::Text mainTitleText;
        mainTitleText.setFont(mFont);
        mainTitleText.setCharacterSize(36);
        mainTitleText.setStyle(sf::Text::Bold);
        mainTitleText.setFillColor(sf::Color(245, 235, 210));
        mainTitleText.setOutlineColor(sf::Color(45, 40, 35));
        mainTitleText.setOutlineThickness(1.5f);
        mainTitleText.setString("Workshop Specials");
        mainTitleText.setOrigin(mainTitleText.getLocalBounds().width / 2.0f, mainTitleText.getLocalBounds().height / 2.0f);

        // Эта строчка настраивает высоту ТОЛЬКО для главной вывески доски:
        mainTitleText.setPosition(640.0f, 170.0f);

        mWindow.draw(mainTitleText);

        // --- ВЕРХНИЙ СЧЕТЧИК МОНЕТ (monetybg2.png) ---
        mWindow.draw(mNewCoinBgSprite);

        sf::Text coinText;
        coinText.setFont(mFont);
        coinText.setCharacterSize(26);
        coinText.setFillColor(sf::Color(245, 205, 75));
        coinText.setString(std::to_string(mCoins));
        coinText.setOrigin(coinText.getLocalBounds().width / 2.0f, coinText.getLocalBounds().height / 2.0f);
        coinText.setPosition(mNewCoinBgSprite.getPosition().x + mNewCoinBgTexture.getSize().x / 2.0f, mNewCoinBgSprite.getPosition().y + 52.0f);
        mWindow.draw(coinText);

        sf::Text tipText;
        tipText.setFont(mFont);
        tipText.setCharacterSize(24);
        tipText.setFillColor(sf::Color::White);
        tipText.setPosition(50.0f, 670.0f);
        tipText.setString("Nacisnij [3] aby wrocic do gry");
        mWindow.draw(tipText);
    }

    if (mCurrentState == GameState::PLAY) {
        mWindow.draw(mBackgroundSprite);
        mWindow.draw(mCharacterSprite);
        mWindow.draw(mLauncherSprite);

        for (const auto& target : mTargets) {
            target->draw(mWindow);
        }

        if (mQteManager.isActive() || mRocket.isFlying()) {
            sf::Vector2f startPos(290.0f, 550.0f);
            if (mCurrentUpgradeLevel == 1) {
                startPos = sf::Vector2f(340.0f, 510.0f);
            }
            else if (mCurrentUpgradeLevel == 2) {
                startPos = sf::Vector2f(305.0f, 575.0f);
            }

            float currentMult = mRocket.isFlying() ? mRocket.getLastQte() : mQteManager.getMultiplier();

            auto points = mRocket.getTrajectoryPoints(currentMult, startPos);
            for (const auto& pos : points) {
                sf::CircleShape dot(3.0f);
                dot.setOrigin(3.0f, 3.0f);
                dot.setPosition(pos);
                mWindow.draw(dot);
            }
        }

        if (mRocket.isFlying()) {
            mWindow.draw(mTireVisual);
        }
        else {
            if (mCurrentUpgradeLevel == 0) {
                mTireVisual.setPosition(290.0f, 550.0f);
            }
            else if (mCurrentUpgradeLevel == 1) {
                mTireVisual.setPosition(340.0f, 510.0f);
            }
            else if (mCurrentUpgradeLevel == 2) {
                mTireVisual.setPosition(305.0f, 575.0f);
            }
            else {
                mTireVisual.setPosition(275.0f, 560.0f);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                mTireVisual.rotate(1200.0f * (1.0f / 60.0f));
            }
            else {
                mTireVisual.setRotation(0.0f);
            }
            mWindow.draw(mTireVisual);
        }

        mWindow.draw(mScoreBackground);
        mWindow.draw(mScoreText);
        mWindow.draw(mCoinIcon);
    }

    mWindow.display();
}

void Game::saveProgress() { std::ofstream file("save.txt"); if (file.is_open()) { file << mCoins; file.close(); } }
void Game::loadProgress() { std::ifstream file("save.txt"); if (file.is_open()) { file >> mCoins; file.close(); } }

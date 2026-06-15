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

    if (!mFont.loadFromFile("VT323-Regular.ttf")) {
        std::cout << "[Error] Font missing!" << std::endl;
    }

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
        std::cout << "[Error] Could not load game_bg.png!" << std::endl;
    }
    else {
        mBackgroundSprite.setTexture(mBackgroundTexture);
        float scaleX = 1280.0f / mBackgroundTexture.getSize().x;
        float scaleY = 720.0f / mBackgroundTexture.getSize().y;
        mBackgroundSprite.setScale(scaleX, scaleY);
    }

    if (!mCharacterTexture.loadFromFile("character_sheet.png")) {
        std::cout << "[Error] Could not load character_sheet.png!" << std::endl;
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

    // --- ИСПРАВЛЕНА МОЩНОСТЬ 3-ГО ДВИГАТЕЛЯ (1.2f -> 0.85f) ---
    mUpgrades.push_back(UpgradeSet("Tuc-Tuc Set", 0, true, 0.35f, 1.5f));
    mUpgrades.push_back(UpgradeSet("Abdurahman Set", 150, false, 0.55f, 1.0f));
    mUpgrades.push_back(UpgradeSet("Jugaad Mad Set", 350, false, 0.85f, 0.4f));

    if (!mShopBgTexture.loadFromFile("fon_mag.png")) {
        std::cout << "[Error] Could not load fon_mag.png!" << std::endl;
    }
    else {
        mShopBgSprite.setTexture(mShopBgTexture);
        float sX = 1280.0f / mShopBgTexture.getSize().x;
        float sY = 720.0f / mShopBgTexture.getSize().y;
        mShopBgSprite.setScale(sX, sY);
    }

    if (!mBoardTexture.loadFromFile("tablica.png")) {
        std::cout << "[Error] Could not load tablica.png!" << std::endl;
    }
    else {
        mBoardSprite.setTexture(mBoardTexture);
        mBoardSprite.setOrigin(static_cast<float>(mBoardTexture.getSize().x) / 2.0f, static_cast<float>(mBoardTexture.getSize().y) / 2.0f);
        mBoardSprite.setPosition(640.0f, 350.0f);
        mBoardSprite.setScale(0.85f, 0.85f);
    }

    for (int i = 0; i < 3; ++i) {
        std::string eFile = "engine" + std::to_string(i + 1) + ".png";
        std::string tFile = "tire" + std::to_string(i + 1) + ".png";

        if (mPreviewEngineTextures[i].loadFromFile(eFile)) {
            mPreviewEngineSprites[i].setTexture(mPreviewEngineTextures[i]);
            mPreviewEngineSprites[i].setOrigin(static_cast<float>(mPreviewEngineTextures[i].getSize().x) / 2.0f, static_cast<float>(mPreviewEngineTextures[i].getSize().y) / 2.0f);
        }
        if (mPreviewTireTextures[i].loadFromFile(tFile)) {
            mPreviewTireSprites[i].setTexture(mPreviewTireTextures[i]);
            mPreviewTireSprites[i].setOrigin(static_cast<float>(mPreviewTireTextures[i].getSize().x) / 2.0f, static_cast<float>(mPreviewTireTextures[i].getSize().y) / 2.0f);
        }
    }

    if (mNewCoinBgTexture.loadFromFile("monetybg2.png")) {
        mNewCoinBgSprite.setTexture(mNewCoinBgTexture);
        mNewCoinBgSprite.setPosition(30.0f, 20.0f);
    }

    if (mPriceBgTexture.loadFromFile("pricebg.png")) {
        for (int i = 0; i < 3; ++i) {
            mPriceBgSprites[i].setTexture(mPriceBgTexture);
            mPriceBgSprites[i].setOrigin(static_cast<float>(mPriceBgTexture.getSize().x) / 2.0f, static_cast<float>(mPriceBgTexture.getSize().y) / 2.0f);
        }
    }

    if (mBuyKeyTexture.loadFromFile("buykey.png")) {
        for (int i = 0; i < 3; ++i) {
            mBuyKeySprites[i].setTexture(mBuyKeyTexture);
            mBuyKeySprites[i].setOrigin(static_cast<float>(mBuyKeyTexture.getSize().x) / 2.0f, static_cast<float>(mBuyKeyTexture.getSize().y) / 2.0f);
        }
    }

    if (!mMenuBgTexture.loadFromFile("menubg.png")) {
        std::cout << "[Error] Could not load menubg.png!" << std::endl;
    }
    else {
        mMenuBgSprite.setTexture(mMenuBgTexture);
        float sX = 1280.0f / mMenuBgTexture.getSize().x;
        float sY = 720.0f / mMenuBgTexture.getSize().y;
        mMenuBgSprite.setScale(sX, sY);
    }

    mMenuTitleText.setFont(mFont);
    mMenuTitleText.setStyle(sf::Text::Bold);
    mMenuTitleText.setFillColor(sf::Color(255, 215, 0));
    mMenuTitleText.setOutlineColor(sf::Color::Black);
    mMenuTitleText.setOutlineThickness(4.0f);
    mMenuTitleText.setString("TIRE LAUNCH");

    mWorkshopBtnSprite.setTexture(mBuyKeyTexture);
    mWorkshopBtnText.setFont(mFont);
    mWorkshopBtnText.setFillColor(sf::Color::White);
    mWorkshopBtnText.setString("Workshop");

    mStreetBtnSprite.setTexture(mBuyKeyTexture);
    mStreetBtnText.setFont(mFont);
    mStreetBtnText.setFillColor(sf::Color::White);
    mStreetBtnText.setString("Street");

    loadProgress();
    applyUpgrades();

    for (int i = 0; i < 3; ++i) {
        mTargets.push_back(nullptr);
        spawnTarget(i);
    }
}

void Game::spawnTarget(int index) {
    float randomY;
    int chance = rand() % 100;

    if (chance < 45) {
        mTargets[index] = std::make_unique<Bird>();
        randomY = 150.0f + static_cast<float>(rand() % 200);
    }
    else if (chance < 75) {
        mTargets[index] = std::make_unique<Drone>();
        randomY = 30.0f + static_cast<float>(rand() % 90);
    }
    else {
        mTargets[index] = std::make_unique<Cow>();
        randomY = 50.0f + static_cast<float>(rand() % 550);
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
        std::cout << "[Error] Could not load " << engineFile << "!" << std::endl;
    }
    else {
        mLauncherSprite.setTexture(mLauncherTexture, true);
        mLauncherSprite.setOrigin(static_cast<float>(mLauncherTexture.getSize().x) / 2.0f, static_cast<float>(mLauncherTexture.getSize().y) / 2.0f);

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
        std::cout << "[Error] Could not load " << tireFile << "!" << std::endl;
    }
    else {
        mTireVisual.setTexture(mTireTexture, true);
        mTireVisual.setOrigin(static_cast<float>(mTireTexture.getSize().x) / 2.0f, static_cast<float>(mTireTexture.getSize().y) / 2.0f);

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
                if (!mRocket.isFlying()) {
                    if (!mQteManager.isActive()) {
                        mQteManager.start();
                    }
                    else {
                        bool readyToLaunch = mQteManager.pressSpace();
                        if (readyToLaunch) {
                            sf::Vector2f st(290.0f, 550.0f);
                            if (mCurrentUpgradeLevel == 1) st = sf::Vector2f(340.0f, 510.0f);
                            else if (mCurrentUpgradeLevel == 2) st = sf::Vector2f(305.0f, 575.0f);

                            float groundY = 635.0f;
                            float initialAlt = groundY - st.y;

                            float finalPwr = mQteManager.getFinalPowerMult();

                            // --- НАЧИСЛЕНИЕ МОНЕТ ЗА ПОПАДАНИЕ В ЗОНУ ---
                            if (finalPwr == 1.0f) {
                                mCoins += 10;
                                std::cout << "[Game] Perfect Timing! +10 Coins" << std::endl;
                            }
                            else if (finalPwr == 0.6f) {
                                mCoins += 5;
                                std::cout << "[Game] Good Timing! +5 Coins" << std::endl;
                            }

                            mRocket.launch(finalPwr, mQteManager.getAngle(), initialAlt);
                        }
                    }
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(mWindow);
            sf::Vector2f mousePos = mWindow.mapPixelToCoords(pixelPos);

            if (mCurrentState == GameState::MENU) {
                float menuBtnScale = 1.1f;
                float menuBtnCenterX = 530.0f;
                float workshopBtnY = 300.0f;
                float streetBtnY = 400.0f;

                float btnW = static_cast<float>(mBuyKeyTexture.getSize().x) * menuBtnScale;
                float btnH = static_cast<float>(mBuyKeyTexture.getSize().y) * menuBtnScale;

                sf::FloatRect workshopBounds(menuBtnCenterX, workshopBtnY, btnW, btnH);
                sf::FloatRect streetBounds(menuBtnCenterX, streetBtnY, btnW, btnH);

                if (workshopBounds.contains(mousePos.x, mousePos.y)) {
                    mCurrentState = GameState::SHOP;
                }
                else if (streetBounds.contains(mousePos.x, mousePos.y)) {
                    mCurrentState = GameState::PLAY;
                }
            }
            else if (mCurrentState == GameState::SHOP) {
                sf::Vector2f buttonPositions[3] = {
                    sf::Vector2f(408.0f, 513.0f),
                    sf::Vector2f(640.0f, 513.0f),
                    sf::Vector2f(872.0f, 513.0f)
                };

                for (int i = 0; i < 3; ++i) {
                    float w = 170.0f * 1.15f;
                    float h = 50.0f * 1.15f;
                    sf::FloatRect btnBounds(
                        buttonPositions[i].x - w / 2.0f,
                        buttonPositions[i].y - h / 2.0f,
                        w, h
                    );

                    if (btnBounds.contains(mousePos.x, mousePos.y)) {
                        if (!mUpgrades[i].getIsUnlocked()) {
                            if (mCoins >= mUpgrades[i].getPrice()) {
                                mCoins -= mUpgrades[i].getPrice();
                                mUpgrades[i].unlock();
                                mCurrentUpgradeLevel = i;
                                applyUpgrades();
                            }
                        }
                        else {
                            mCurrentUpgradeLevel = i;
                            applyUpgrades();
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

    mScoreText.setString("COINS: " + std::to_string(mCoins));

    if (!mCharacterFrames.empty()) {
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

    if (!mRocket.isFlying()) mQteManager.update(deltaTime);

    mRocket.update(deltaTime.asSeconds());

    if (mRocket.isFlying()) {
        sf::Vector2f startPos(290.0f, 550.0f);
        if (mCurrentUpgradeLevel == 1) startPos = sf::Vector2f(340.0f, 510.0f);
        else if (mCurrentUpgradeLevel == 2) startPos = sf::Vector2f(305.0f, 575.0f);

        float groundY = 635.0f;

        mTireVisual.setPosition(startPos.x + mRocket.getDistance(), groundY - mRocket.getAltitude());
        mTireVisual.rotate(360.0f * deltaTime.asSeconds());

        sf::FloatRect tireBox = mTireVisual.getGlobalBounds();

        float shrinkX = tireBox.width * 0.3f;
        float shrinkY = tireBox.height * 0.3f;
        sf::FloatRect smallTireBox(
            tireBox.left + shrinkX,
            tireBox.top + shrinkY,
            tireBox.width - shrinkX * 2.0f,
            tireBox.height - shrinkY * 2.0f
        );

        for (size_t i = 0; i < mTargets.size(); ++i) {
            if (mTargets[i]->isActive() && smallTireBox.intersects(mTargets[i]->getBounds())) {
                mTargets[i]->setActive(false);
                mCoins += mTargets[i]->getReward();

                if (mCoins < 0) mCoins = 0;

                if (mTargets[i]->getReward() > 0 && !mIsCelebrating && mCharacterCurrentFrame == 0) {
                    mIsCelebrating = true;
                    mCharacterCurrentFrame = mCelebrateStartFrame;

                    if (!mCharacterFrames.empty()) {
                        mCharacterSprite.setTextureRect(mCharacterFrames[mCharacterCurrentFrame]);
                    }
                    mAnimationClock.restart();
                }

                spawnTarget(static_cast<int>(i));
            }
        }
    }

    bool currentlyFlying = mRocket.isFlying();
    if (mWasFlying && !currentlyFlying) {
        // --- Убрано начисление монет за дистанцию ---
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

    if (mCurrentState == GameState::MENU) {
        mWindow.draw(mMenuBgSprite);

        float titleX = 650.0f; float titleY = 100.0f; int titleSize = 110;
        float btnCenterX = 530.0f; float workshopY = 300.0f; float streetY = 400.0f;
        float btnScale = 1.1f; int textBtnSize = 34;
        float textOffsetX = 108.0f; float textOffsetY = 25.0f;

        mMenuTitleText.setCharacterSize(titleSize);
        mMenuTitleText.setOrigin(mMenuTitleText.getLocalBounds().width / 2.0f, mMenuTitleText.getLocalBounds().height / 2.0f);
        mMenuTitleText.setPosition(titleX, titleY);
        mWindow.draw(mMenuTitleText);

        mWorkshopBtnSprite.setPosition(btnCenterX, workshopY);
        mWorkshopBtnSprite.setScale(btnScale, btnScale);
        mWindow.draw(mWorkshopBtnSprite);

        mWorkshopBtnText.setCharacterSize(textBtnSize);
        mWorkshopBtnText.setOrigin(mWorkshopBtnText.getLocalBounds().width / 2.0f, mWorkshopBtnText.getLocalBounds().height / 2.0f);
        mWorkshopBtnText.setPosition(btnCenterX + textOffsetX, workshopY + textOffsetY);
        mWindow.draw(mWorkshopBtnText);

        mStreetBtnSprite.setPosition(btnCenterX, streetY);
        mStreetBtnSprite.setScale(btnScale, btnScale);
        mWindow.draw(mStreetBtnSprite);

        mStreetBtnText.setCharacterSize(textBtnSize);
        mStreetBtnText.setOrigin(mStreetBtnText.getLocalBounds().width / 2.0f, mStreetBtnText.getLocalBounds().height / 2.0f);
        mStreetBtnText.setPosition(btnCenterX + textOffsetX, streetY + textOffsetY);
        mWindow.draw(mStreetBtnText);
    }
    else if (mCurrentState == GameState::SHOP) {
        mWindow.draw(mShopBgSprite);

        mBoardSprite.setOrigin(static_cast<float>(mBoardTexture.getSize().x) / 2.0f, static_cast<float>(mBoardTexture.getSize().y) / 2.0f);
        mBoardSprite.setPosition(640.0f, 350.0f);
        mBoardSprite.setScale(0.42f, 0.35f);
        mWindow.draw(mBoardSprite);

        struct AbsoluteShelfConfig {
            int engIdx, tireIdx; float engScale, tireScale; float engX, engY;
            float tireX, tireY; float priceX, priceY; float btnX, btnY; std::string setName;
        };

        AbsoluteShelfConfig shelves[3] = {
            { 0, 0, 0.3f, 0.11f, 410.0f, 330.0f, 450.0f, 300.0f, 408.0f, 462.0f, 408.0f, 513.0f, "Tuc-Tuc Set" },
            { 1, 1, 0.16f, 0.04f, 630.0f, 325.0f, 680.0f, 290.0f, 640.0f, 462.0f, 640.0f, 513.0f, "Abdurahman Set" },
            { 2, 2, 0.16f, 0.3f, 860.0f, 335.0f, 915.0f, 321.0f, 872.0f, 462.0f, 872.0f, 513.0f, "Jugaad Mad Set" }
        };

        sf::Text uiText;
        uiText.setFont(mFont);
        uiText.setCharacterSize(22);
        uiText.setFillColor(sf::Color(235, 230, 215));

        for (int i = 0; i < 3; ++i) {
            AbsoluteShelfConfig cfg = shelves[i];

            mPreviewEngineSprites[cfg.engIdx].setPosition(cfg.engX, cfg.engY);
            mPreviewEngineSprites[cfg.engIdx].setScale(cfg.engScale, cfg.engScale);
            mWindow.draw(mPreviewEngineSprites[cfg.engIdx]);

            mPreviewTireSprites[cfg.tireIdx].setPosition(cfg.tireX, cfg.tireY);
            mPreviewTireSprites[cfg.tireIdx].setScale(cfg.tireScale, cfg.tireScale);
            mWindow.draw(mPreviewTireSprites[cfg.tireIdx]);

            sf::Text headerText;
            headerText.setFont(mFont);
            headerText.setCharacterSize(26);
            headerText.setStyle(sf::Text::Bold);
            headerText.setFillColor(sf::Color(45, 40, 35));
            headerText.setString(cfg.setName);
            headerText.setOrigin(headerText.getLocalBounds().width / 2.0f, headerText.getLocalBounds().height / 2.0f);
            headerText.setPosition(cfg.priceX, 220.0f);
            mWindow.draw(headerText);

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

            mBuyKeySprites[i].setPosition(cfg.btnX, cfg.btnY);
            mBuyKeySprites[i].setScale(0.55f, 0.55f);
            mWindow.draw(mBuyKeySprites[i]);

            if (!mUpgrades[i].getIsUnlocked()) uiText.setString("Purchase");
            else if (mCurrentUpgradeLevel == i) uiText.setString("Equipped");
            else uiText.setString("Equip");

            uiText.setOrigin(uiText.getLocalBounds().left + uiText.getLocalBounds().width / 2.0f,
                uiText.getLocalBounds().top + uiText.getLocalBounds().height / 2.0f);
            uiText.setPosition(cfg.btnX, cfg.btnY - 1.0f);
            mWindow.draw(uiText);
        }

        sf::Text mainTitleText;
        mainTitleText.setFont(mFont);
        mainTitleText.setCharacterSize(36);
        mainTitleText.setStyle(sf::Text::Bold);
        mainTitleText.setFillColor(sf::Color(245, 235, 210));
        mainTitleText.setOutlineColor(sf::Color(45, 40, 35));
        mainTitleText.setOutlineThickness(1.5f);
        mainTitleText.setString("Workshop Specials");
        mainTitleText.setOrigin(mainTitleText.getLocalBounds().width / 2.0f, mainTitleText.getLocalBounds().height / 2.0f);
        mainTitleText.setPosition(640.0f, 170.0f);
        mWindow.draw(mainTitleText);

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
        tipText.setPosition(50.0f, 645.0f);
        tipText.setString("Press [1] to return to Main Menu");
        mWindow.draw(tipText);

        tipText.setPosition(50.0f, 675.0f);
        tipText.setString("Press [3] to go out to the Street");
        mWindow.draw(tipText);
    }
    else if (mCurrentState == GameState::PLAY) {
        mWindow.draw(mBackgroundSprite);
        mWindow.draw(mCharacterSprite);
        mWindow.draw(mLauncherSprite);

        for (const auto& target : mTargets) {
            target->draw(mWindow);
        }

        sf::Vector2f startPos(290.0f, 550.0f);
        if (mCurrentUpgradeLevel == 1) startPos = sf::Vector2f(340.0f, 510.0f);
        else if (mCurrentUpgradeLevel == 2) startPos = sf::Vector2f(305.0f, 575.0f);

        if (mRocket.isFlying() || mQteManager.getState() == QTEState::ANGLE_SELECT) {
            float pwr = mRocket.isFlying() ? mRocket.getLastPowerMult() : mQteManager.getFinalPowerMult();
            float ang = mRocket.isFlying() ? mRocket.getLastAngle() : mQteManager.getAngle();

            auto points = mRocket.getTrajectoryPoints(pwr, ang, startPos);
            for (const auto& pos : points) {
                sf::CircleShape dot(3.0f);
                dot.setOrigin(3.0f, 3.0f);
                dot.setFillColor(sf::Color(255, 255, 255, 180));
                dot.setPosition(pos);
                mWindow.draw(dot);
            }
        }

        if (mRocket.isFlying()) {
            mWindow.draw(mTireVisual);
        }
        else {
            mTireVisual.setPosition(startPos);

            // --- КРУТИМ КОЛЕСО НА ЭТАПЕ ВЫБОРА УГЛА ---
            if (mQteManager.getState() == QTEState::ANGLE_SELECT) {
                float spinSpeed = 800.0f * mQteManager.getFinalPowerMult(); // Чем точнее попал, тем быстрее жужжит
                mTireVisual.rotate(spinSpeed * (1.0f / 60.0f));
            }
            else {
                mTireVisual.setRotation(0.0f);
            }
            mWindow.draw(mTireVisual);
        }

        if (mQteManager.isActive()) {
            float barW = 400.0f;
            float barH = 30.0f;
            float barX = 640.0f - barW / 2.0f;
            float barY = 620.0f;

            sf::RectangleShape bg(sf::Vector2f(barW, barH));
            bg.setPosition(barX, barY);
            bg.setFillColor(sf::Color(200, 50, 50));
            bg.setOutlineThickness(2.0f);
            bg.setOutlineColor(sf::Color::Black);
            mWindow.draw(bg);

            sf::RectangleShape yellowZone(sf::Vector2f(barW * 0.6f, barH));
            yellowZone.setPosition(barX + barW * 0.2f, barY);
            yellowZone.setFillColor(sf::Color(200, 200, 50));
            mWindow.draw(yellowZone);

            sf::RectangleShape greenZone(sf::Vector2f(barW * 0.2f, barH));
            greenZone.setPosition(barX + barW * 0.4f, barY);
            greenZone.setFillColor(sf::Color(50, 200, 50));
            mWindow.draw(greenZone);

            float indicator = mQteManager.getPowerIndicator();
            sf::RectangleShape cursor(sf::Vector2f(6.0f, barH + 10.0f));
            cursor.setOrigin(3.0f, 5.0f);
            cursor.setPosition(barX + barW * indicator, barY);
            cursor.setFillColor(sf::Color::White);
            cursor.setOutlineThickness(1.0f);
            cursor.setOutlineColor(sf::Color::Black);
            mWindow.draw(cursor);
        }

        static sf::Texture playCoinTexture;
        static bool isPlayCoinLoaded = false;
        if (!isPlayCoinLoaded) {
            if (playCoinTexture.loadFromFile("monetybg1.png")) {
                isPlayCoinLoaded = true;
            }
        }

        if (isPlayCoinLoaded) {
            sf::Sprite playCoinSprite(playCoinTexture);

            float coinBgScale = 0.05f;
            float coinBgX = 15.0f;
            float coinBgY = 15.0f;

            float textInBgOffsetX = 0.0f;
            float textInBgOffsetY = -3.0f;

            playCoinSprite.setPosition(coinBgX, coinBgY);
            playCoinSprite.setScale(coinBgScale, coinBgScale);
            mWindow.draw(playCoinSprite);

            sf::Text playCoinText;
            playCoinText.setFont(mFont);
            playCoinText.setCharacterSize(22);
            playCoinText.setFillColor(sf::Color(60, 60, 60));
            playCoinText.setString(std::to_string(mCoins));

            playCoinText.setOrigin(playCoinText.getLocalBounds().left + playCoinText.getLocalBounds().width / 2.0f,
                playCoinText.getLocalBounds().top + playCoinText.getLocalBounds().height / 2.0f);

            float centerX = coinBgX + (static_cast<float>(playCoinTexture.getSize().x) * coinBgScale) / 2.0f;
            float centerY = coinBgY + (static_cast<float>(playCoinTexture.getSize().y) * coinBgScale) * 0.72f;

            playCoinText.setPosition(centerX + textInBgOffsetX, centerY + textInBgOffsetY);
            mWindow.draw(playCoinText);
        }

        sf::Text playNavigationText;
        playNavigationText.setFont(mFont);
        playNavigationText.setCharacterSize(24);
        playNavigationText.setFillColor(sf::Color::White);
        playNavigationText.setOutlineColor(sf::Color::Black);
        playNavigationText.setOutlineThickness(1.5f);
        playNavigationText.setPosition(50.0f, 675.0f);
        playNavigationText.setString("Press [1] for Main Menu | Press [2] for Workshop");
        mWindow.draw(playNavigationText);
    }

    mWindow.display();
}

void Game::saveProgress() {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << mCoins << "\n";
        file << mCurrentUpgradeLevel << "\n";

        for (int i = 0; i < 3; ++i) {
            file << (mUpgrades[i].getIsUnlocked() ? 1 : 0) << " ";
        }
        file.close();
    }
}

void Game::loadProgress() {
    std::ifstream file("save.txt");
    if (file.is_open()) {
        file >> mCoins;
        file >> mCurrentUpgradeLevel;

        for (int i = 0; i < 3; ++i) {
            int unlocked;
            file >> unlocked;
            if (unlocked == 1) {
                mUpgrades[i].unlock();
            }
        }
        file.close();
    }
}
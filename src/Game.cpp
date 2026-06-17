#include "Game.hpp"
#include "GameObject.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath> 

// ==========================================
// --- ¬—œŒÃŒ√¿“≈À‹Õ€≈ Ã≈“Œƒ€ (–≈‘¿ “Œ–»Õ√) ---
// ==========================================
void Game::initButton(sf::Sprite& sprite, sf::Text& text, const std::string& str, float posX, float posY, float scale, int fontSize) {
    sprite.setTexture(mBuyKeyTexture);
    sprite.setOrigin(static_cast<float>(mBuyKeyTexture.getSize().x) / 2.0f, static_cast<float>(mBuyKeyTexture.getSize().y) / 2.0f);
    sprite.setPosition(posX, posY);
    sprite.setScale(scale, scale);

    text.setFont(mFont);
    text.setCharacterSize(fontSize);
    text.setFillColor(sf::Color::White);
    text.setString(str);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(posX, posY - 4.0f);
}

void Game::initProgressBar(sf::RectangleShape& bg, sf::RectangleShape& bar, sf::Text& text, const std::string& str, sf::Color color, float posY) {
    bg.setSize(sf::Vector2f(200.0f, 20.0f));
    bg.setFillColor(sf::Color(50, 50, 50, 200));
    bg.setOutlineThickness(2.0f);
    bg.setOutlineColor(sf::Color::Black);
    bg.setPosition(1050.0f, posY);

    bar.setSize(sf::Vector2f(0.0f, 20.0f));
    bar.setFillColor(color);
    bar.setPosition(1050.0f, posY);

    text.setFont(mFont);
    text.setCharacterSize(22);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(1.5f);
    text.setOutlineColor(sf::Color::Black);
    text.setPosition(1050.0f, posY - 27.0f);
    text.setString(str);
}

void Game::resetGameProgress() {
    mCoins = 0;
    mCurrentUpgradeLevel = 0;
    mMobsKilled = 0;
    mCowsHit = 0;
    mIsBossActive = false;
    mIsBossWarningActive = false;
    mIsGameOverActive = false;
    mIsVictoryScreenActive = false;
    mUpgrades.clear();

    mUpgrades.push_back(UpgradeSet("Tuc-Tuc Set", 0, true, 0.4125f, 1.5f));
    mUpgrades.push_back(UpgradeSet("Abdurahman Set", 300, false, 0.55f, 1.0f));
    mUpgrades.push_back(UpgradeSet("Jugaad Mad Set", 1000, false, 0.75f, 0.4f));

    applyUpgrades();
    saveProgress();

    mRocket = Rocket();
    applyUpgrades();
    mQteManager = QTEManager();
    mCurrentState = GameState::MENU;
    std::cout << "[Game] Progress Reset to ZERO!" << std::endl;
}
// ==========================================

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
    , mMobsKilled(0)
    , mMobsForBoss(10)
    , mIsBossActive(false)
    , mCowsHit(0)
    , mCowsForGameOver(5)
    , mHitSlowdownTimer(0.0f)
    , mIsBossWarningActive(false)
    , mBossWarningTimer(0.0f)
    , mIsGameOverActive(false)
    , mGameOverTimer(0.0f)
    , mIsVictoryScreenActive(false)
    , mVictoryAnimTimer(0.0f)
{
    srand(static_cast<unsigned int>(time(0)));
    mWindow.setFramerateLimit(60);

    // --- 1. «¿√–”« ¿ ¡¿«Œ¬€’ ¿——≈“Œ¬ ---
    if (!mFont.loadFromFile("VT323-Regular.ttf")) std::cout << "[Error] Font missing!" << std::endl;
    if (!mBuyKeyTexture.loadFromFile("buykey.png")) std::cout << "[Error] buykey.png missing!" << std::endl;

    // --- 2. Õ¿—“–Œ… ¿  ÕŒœŒ  » ÿ ¿À ◊≈–≈« ”Õ»¬≈–—¿À‹Õ€≈ ‘”Õ ÷»» ---
    float btnCenterX = 640.0f; // »—œ–¿¬À≈ÕŒ: “ÂÔÂ¸ ˆÂÌÚ ÍÌÓÔÓÍ Ó‚ÌÓ ÔÓÒÂÂ‰ËÌÂ ˝Í‡Ì‡
    initButton(mWorkshopBtnSprite, mWorkshopBtnText, "Workshop", btnCenterX, 260.0f);
    initButton(mStreetBtnSprite, mStreetBtnText, "Street", btnCenterX, 360.0f);
    initButton(mInstBtnSprite, mInstBtnText, "Instructions", btnCenterX, 460.0f);
    initButton(mResetBtnSprite, mResetBtnText, "RESET", btnCenterX, 560.0f);
    mResetBtnText.setFillColor(sf::Color(255, 100, 100)); //  ‡ÒÌ˚È ˆ‚ÂÚ ‰Îˇ Ò·ÓÒ‡

    initButton(mVictoryQuitBtnSprite, mVictoryQuitBtnText, "Quit", 460.0f, 620.0f, 1.2f, 38);
    initButton(mVictoryContBtnSprite, mVictoryContBtnText, "Continue", 820.0f, 620.0f, 1.2f, 38);

    initProgressBar(mCowProgressBarBg, mCowProgressBar, mCowProgressText, "COWS HIT:", sf::Color(255, 50, 50), 35.0f);
    initProgressBar(mBossProgressBarBg, mBossProgressBar, mBossProgressText, "DRONES KILLED:", sf::Color(255, 150, 0), 92.0f);

    // --- 3. Õ¿—“–Œ… ¿ “≈ —“Œ¬ » ¬»«”¿À‹Õ€’ ›À≈Ã≈Õ“Œ¬ ---
    mGameOverText.setFont(mFont);
    mGameOverText.setCharacterSize(130);
    mGameOverText.setStyle(sf::Text::Bold);
    mGameOverText.setFillColor(sf::Color(255, 30, 30));
    mGameOverText.setOutlineColor(sf::Color::Black);
    mGameOverText.setOutlineThickness(7.0f);
    mGameOverText.setString("GAME OVER!");
    mGameOverText.setOrigin(mGameOverText.getLocalBounds().width / 2.0f, mGameOverText.getLocalBounds().height / 2.0f);
    mGameOverText.setPosition(640.0f, 330.0f);

    mWarningText.setFont(mFont);
    mWarningText.setCharacterSize(100);
    mWarningText.setStyle(sf::Text::Bold);
    mWarningText.setFillColor(sf::Color(255, 50, 50));
    mWarningText.setOutlineColor(sf::Color::Black);
    mWarningText.setOutlineThickness(5.0f);
    mWarningText.setString("WARNING: BOSS APPROACHING!");
    mWarningText.setOrigin(mWarningText.getLocalBounds().width / 2.0f, mWarningText.getLocalBounds().height / 2.0f);
    mWarningText.setPosition(640.0f, 300.0f);

    mRedFlashRect.setSize(sf::Vector2f(1280.0f, 720.0f));
    mRedFlashRect.setFillColor(sf::Color(255, 0, 0, 0));

    mLightBeam1.setPointCount(4);
    mLightBeam1.setPoint(0, sf::Vector2f(-180.0f, -1200.0f));
    mLightBeam1.setPoint(1, sf::Vector2f(180.0f, -1200.0f));
    mLightBeam1.setPoint(2, sf::Vector2f(20.0f, 0.0f));
    mLightBeam1.setPoint(3, sf::Vector2f(-20.0f, 0.0f));
    mLightBeam1.setFillColor(sf::Color(255, 255, 200, 0));
    mLightBeam1.setPosition(200.0f, 800.0f);

    mLightBeam2 = mLightBeam1;
    mLightBeam2.setPosition(1080.0f, 800.0f);

    if (mCupTexture.loadFromFile("cup.png")) {
        mCupSprite.setTexture(mCupTexture);
        mCupSprite.setOrigin(mCupTexture.getSize().x / 2.0f, mCupTexture.getSize().y / 2.0f);
        mCupSprite.setPosition(640.0f, 350.0f);
        mCupSprite.setScale(0.8f, 0.8f);
    }

    mCongratsText.setFont(mFont);
    mCongratsText.setCharacterSize(80);
    mCongratsText.setStyle(sf::Text::Bold);
    mCongratsText.setFillColor(sf::Color(255, 215, 0));
    mCongratsText.setOutlineColor(sf::Color::Black);
    mCongratsText.setOutlineThickness(4.0f);
    mCongratsText.setString("CONGRATULATIONS!");
    mCongratsText.setOrigin(mCongratsText.getLocalBounds().left + mCongratsText.getLocalBounds().width / 2.0f,
        mCongratsText.getLocalBounds().top + mCongratsText.getLocalBounds().height / 2.0f);
    mCongratsText.setPosition(640.0f, 100.0f);

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

    if (mBackgroundTexture.loadFromFile("game_bg.png")) {
        mBackgroundSprite.setTexture(mBackgroundTexture);
        mBackgroundSprite.setScale(1280.0f / mBackgroundTexture.getSize().x, 720.0f / mBackgroundTexture.getSize().y);
    }

    if (mCharacterTexture.loadFromFile("character_sheet.png")) {
        mCharacterSprite.setTexture(mCharacterTexture);
        mCharacterFrames = { sf::IntRect(16, 0, 171, 600), sf::IntRect(209, 0, 191, 600),
                             sf::IntRect(411, 0, 211, 600), sf::IntRect(627, 0, 191, 600) };
        mCharacterSprite.setTextureRect(mCharacterFrames[0]);
        mCharacterSprite.setOrigin(mCharacterFrames[0].width / 2.0f, mCharacterFrames[0].height / 2.0f);
        mCharacterSprite.setPosition(110.0f, 520.0f);
        mCharacterSprite.setScale(0.45f, 0.45f);
    }

    mUpgrades.push_back(UpgradeSet("Tuc-Tuc Set", 0, true, 0.4125f, 1.5f));
    mUpgrades.push_back(UpgradeSet("Abdurahman Set", 300, false, 0.55f, 1.0f));
    mUpgrades.push_back(UpgradeSet("Jugaad Mad Set", 1000, false, 0.75f, 0.4f));

    if (mShopBgTexture.loadFromFile("fon_mag.png")) {
        mShopBgSprite.setTexture(mShopBgTexture);
        mShopBgSprite.setScale(1280.0f / mShopBgTexture.getSize().x, 720.0f / mShopBgTexture.getSize().y);
    }

    if (mBoardTexture.loadFromFile("tablica.png")) {
        mBoardSprite.setTexture(mBoardTexture);
        mBoardSprite.setOrigin(mBoardTexture.getSize().x / 2.0f, mBoardTexture.getSize().y / 2.0f);
        mBoardSprite.setPosition(640.0f, 350.0f);
        mBoardSprite.setScale(0.42f, 0.35f); // »—œ–¿¬À≈ÕŒ: ¬ÂÌÛÎË Ô‡‚ËÎ¸Ì˚È ‡ÁÏÂ ‰Îˇ Á‡‰ÌÂ„Ó ÙÓÌ‡
    }

    if (mInstBgTexture.loadFromFile("instbg.png")) {
        mInstBgSprite.setTexture(mInstBgTexture);
        mInstBgSprite.setScale(1280.0f / mInstBgTexture.getSize().x, 720.0f / mInstBgTexture.getSize().y);
    }

    if (mInstExitBtnTexture.loadFromFile("exitbutton.png")) {
        mInstExitBtnSprite.setTexture(mInstExitBtnTexture);
        mInstExitBtnSprite.setOrigin(mInstExitBtnTexture.getSize().x / 2.0f, mInstExitBtnTexture.getSize().y / 2.0f);
        mInstExitBtnSprite.setPosition(1200.0f, 60.0f);
        float exitScale = 80.0f / mInstExitBtnTexture.getSize().x;
        mInstExitBtnSprite.setScale(exitScale, exitScale);
    }

    for (int i = 0; i < 3; ++i) {
        if (mPreviewEngineTextures[i].loadFromFile("engine" + std::to_string(i + 1) + ".png")) {
            mPreviewEngineSprites[i].setTexture(mPreviewEngineTextures[i]);
            mPreviewEngineSprites[i].setOrigin(mPreviewEngineTextures[i].getSize().x / 2.0f, mPreviewEngineTextures[i].getSize().y / 2.0f);
        }
        if (mPreviewTireTextures[i].loadFromFile("tire" + std::to_string(i + 1) + ".png")) {
            mPreviewTireSprites[i].setTexture(mPreviewTireTextures[i]);
            mPreviewTireSprites[i].setOrigin(mPreviewTireTextures[i].getSize().x / 2.0f, mPreviewTireTextures[i].getSize().y / 2.0f);
        }
    }

    if (mNewCoinBgTexture.loadFromFile("monetybg2.png")) mNewCoinBgSprite.setTexture(mNewCoinBgTexture);
    mNewCoinBgSprite.setPosition(30.0f, 20.0f);

    if (mPriceBgTexture.loadFromFile("pricebg.png")) {
        for (int i = 0; i < 3; ++i) {
            mPriceBgSprites[i].setTexture(mPriceBgTexture);
            mPriceBgSprites[i].setOrigin(mPriceBgTexture.getSize().x / 2.0f, mPriceBgTexture.getSize().y / 2.0f);
        }
    }

    for (int i = 0; i < 3; ++i) {
        mBuyKeySprites[i].setTexture(mBuyKeyTexture);
        mBuyKeySprites[i].setOrigin(mBuyKeyTexture.getSize().x / 2.0f, mBuyKeyTexture.getSize().y / 2.0f);
    }

    if (mMenuBgTexture.loadFromFile("menubg.png")) {
        mMenuBgSprite.setTexture(mMenuBgTexture);
        mMenuBgSprite.setScale(1280.0f / mMenuBgTexture.getSize().x, 720.0f / mMenuBgTexture.getSize().y);
    }

    mMenuTitleText.setFont(mFont);
    mMenuTitleText.setCharacterSize(110);
    mMenuTitleText.setStyle(sf::Text::Bold);
    mMenuTitleText.setFillColor(sf::Color(255, 215, 0));
    mMenuTitleText.setOutlineColor(sf::Color::Black);
    mMenuTitleText.setOutlineThickness(4.0f);
    mMenuTitleText.setString("TIRE LAUNCHER");
    mMenuTitleText.setOrigin(mMenuTitleText.getLocalBounds().width / 2.0f, mMenuTitleText.getLocalBounds().height / 2.0f);
    mMenuTitleText.setPosition(640.0f, 100.0f); // »—œ–¿¬À≈ÕŒ: ¬˚‡‚ÌˇÎ Á‡„ÓÎÓ‚ÓÍ Ó‚ÌÓ ÔÓ ˆÂÌÚÛ

    loadProgress();
    applyUpgrades();

    for (int i = 0; i < 5; ++i) {
        mTargets.push_back(nullptr);
        spawnTarget(i);
    }
}

void Game::spawnTarget(int index) {
    float randomY;
    int chance = rand() % 100;

    if (chance < 35) {
        mTargets[index] = std::make_unique<Bird>();
        randomY = 150.0f + static_cast<float>(rand() % 200);
    }
    else if (chance < 85) {
        mTargets[index] = std::make_unique<Drone>();
        randomY = 30.0f + static_cast<float>(rand() % 90);
    }
    else {
        mTargets[index] = std::make_unique<Cow>();
        randomY = 50.0f + static_cast<float>(rand() % 550);
    }

    int side = rand() % 2;
    float baseSpeed = 150.0f + static_cast<float>(rand() % 200);

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
    if (mLauncherTexture.loadFromFile(engineFile)) {
        mLauncherSprite.setTexture(mLauncherTexture, true);
        mLauncherSprite.setOrigin(mLauncherTexture.getSize().x / 2.0f, mLauncherTexture.getSize().y / 2.0f);

        float engScales[] = { 0.45f, 0.29f, 0.28f };
        sf::Vector2f engPos[] = { {210.0f, 600.0f}, {230.0f, 580.0f}, {210.0f, 600.0f} };

        mLauncherSprite.setScale(engScales[mCurrentUpgradeLevel], engScales[mCurrentUpgradeLevel]);
        mLauncherSprite.setPosition(engPos[mCurrentUpgradeLevel]);
    }

    std::string tireFile = "tire" + std::to_string(mCurrentUpgradeLevel + 1) + ".png";
    if (mTireTexture.loadFromFile(tireFile)) {
        mTireVisual.setTexture(mTireTexture, true);
        mTireVisual.setOrigin(mTireTexture.getSize().x / 2.0f, mTireTexture.getSize().y / 2.0f);

        float tireScales[] = { 0.18f, 0.07f, 0.45f };
        mTireVisual.setScale(tireScales[mCurrentUpgradeLevel], tireScales[mCurrentUpgradeLevel]);
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

        if (mIsGameOverActive) continue;

        if (mIsVictoryScreenActive) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(mWindow);
                sf::Vector2f mousePos = mWindow.mapPixelToCoords(pixelPos);

                if (mVictoryQuitBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    mWindow.close();
                }
                else if (mVictoryContBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    mIsVictoryScreenActive = false;
                    mVictoryAnimTimer = 0.0f;
                }
            }
            continue;
        }

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

                            if (finalPwr == 1.0f) {
                                mCoins += 10;
                                std::cout << "[Game] Perfect Timing! +10 Coins" << std::endl;
                            }
                            else if (finalPwr >= 0.6f && finalPwr < 1.0f) {
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

            if (mCurrentState == GameState::INSTRUCTIONS) {
                if (mInstExitBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    mCurrentState = GameState::MENU;
                }
                continue;
            }

            if (mCurrentState == GameState::MENU) {
                if (mWorkshopBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    mCurrentState = GameState::SHOP;
                }
                else if (mStreetBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    mCurrentState = GameState::PLAY;
                }
                else if (mInstBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    mCurrentState = GameState::INSTRUCTIONS;
                }
                else if (mResetBtnSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    resetGameProgress();
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
                    sf::FloatRect btnBounds(buttonPositions[i].x - w / 2.0f, buttonPositions[i].y - h / 2.0f, w, h);

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

    float dt = deltaTime.asSeconds();

    if (mIsVictoryScreenActive) {
        mVictoryAnimTimer += dt;

        float t = mVictoryAnimTimer / 1.5f;
        if (t > 1.0f) t = 1.0f;

        float smoothT = std::sin(t * 3.14159f / 2.0f);
        float yOffset = 800.0f * (1.0f - smoothT);

        mCupSprite.setPosition(640.0f, 350.0f + yOffset);
        mCongratsText.setPosition(640.0f, 100.0f + yOffset);
        mVictoryQuitBtnSprite.setPosition(460.0f, 620.0f + yOffset);
        mVictoryQuitBtnText.setPosition(460.0f, 616.0f + yOffset);
        mVictoryContBtnSprite.setPosition(820.0f, 620.0f + yOffset);
        mVictoryContBtnText.setPosition(820.0f, 616.0f + yOffset);

        int beamAlpha = static_cast<int>(60.0f * smoothT);
        mLightBeam1.setFillColor(sf::Color(255, 255, 200, beamAlpha));
        mLightBeam2.setFillColor(sf::Color(255, 255, 200, beamAlpha));

        float angle1 = 20.0f + std::sin(mVictoryAnimTimer * 2.0f) * 15.0f;
        float angle2 = -20.0f + std::cos(mVictoryAnimTimer * 2.3f) * 15.0f;
        mLightBeam1.setRotation(angle1);
        mLightBeam2.setRotation(angle2);

        return;
    }

    sf::Time scaledTime = sf::seconds(dt);

    if (mIsGameOverActive) {
        mGameOverTimer -= dt;

        float pulseFactor = 1.0f + std::abs(std::sin(mGameOverTimer * 8.0f)) * 0.12f;
        mGameOverText.setScale(pulseFactor, pulseFactor);

        float elapsedPercent = (3.0f - mGameOverTimer) / 3.0f;
        if (elapsedPercent > 1.0f) elapsedPercent = 1.0f;
        int alphaValue = static_cast<int>(elapsedPercent * 255.0f);
        mRedFlashRect.setFillColor(sf::Color(0, 0, 0, alphaValue));

        for (size_t i = 0; i < mTargets.size(); ++i) {
            if (mTargets[i]->isActive()) mTargets[i]->update(scaledTime);
        }
        mRocket.update(dt);

        if (mGameOverTimer <= 0.0f) {
            resetGameProgress();
        }
        return;
    }

    mScoreText.setString("COINS: " + std::to_string(mCoins));

    if (mMobsKilled >= mMobsForBoss && !mIsBossActive && !mIsBossWarningActive && mCoins >= 50) {
        mIsBossWarningActive = true;
        mBossWarningTimer = 3.0f;
        mMobsKilled = 0;
    }

    if (mIsBossWarningActive) {
        mBossWarningTimer -= deltaTime.asSeconds();

        int alpha = static_cast<int>(std::abs(std::sin(mBossWarningTimer * 3.14159f)) * 130.0f);
        mRedFlashRect.setFillColor(sf::Color(255, 0, 0, alpha));

        if (mBossWarningTimer <= 0.0f) {
            mIsBossWarningActive = false;
            mIsBossActive = true;

            mTargets[0] = std::make_unique<Boss>();
            mTargets[0]->setPosition(800.0f, 300.0f);
            mTargets[0]->setSpeed(0.0f);
        }
    }

    float pct = static_cast<float>(mMobsKilled) / mMobsForBoss;
    if (pct > 1.0f) pct = 1.0f;
    mBossProgressBar.setSize(sf::Vector2f(200.0f * pct, 20.0f));
    mBossProgressText.setString("DRONES KILLED: " + std::to_string(mMobsKilled) + "/" + std::to_string(mMobsForBoss));

    float cowPct = static_cast<float>(mCowsHit) / mCowsForGameOver;
    if (cowPct > 1.0f) cowPct = 1.0f;
    mCowProgressBar.setSize(sf::Vector2f(200.0f * cowPct, 20.0f));
    mCowProgressText.setString("COWS HIT: " + std::to_string(mCowsHit) + "/" + std::to_string(mCowsForGameOver));

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
                mCharacterSprite.setOrigin(mCharacterFrames[mCharacterCurrentFrame].width / 2.0f, mCharacterFrames[mCharacterCurrentFrame].height / 2.0f);
            }
        }
        else {
            mCharacterSprite.setTextureRect(mCharacterFrames[0]);
            mCharacterSprite.setOrigin(mCharacterFrames[0].width / 2.0f, mCharacterFrames[0].height / 2.0f);
        }
    }

    for (size_t i = 0; i < mTargets.size(); ++i) {
        if (!mTargets[i]->isActive()) continue;

        mTargets[i]->update(scaledTime);
        sf::Vector2f pos = mTargets[i]->getPosition();
        float spd = mTargets[i]->getSpeed();

        bool flewRight = (spd > 0.0f && pos.x > 1300.0f);
        bool flewLeft = (spd < 0.0f && pos.x < -100.0f);

        if (flewRight || flewLeft) {
            spawnTarget(static_cast<int>(i));
        }
    }

    if (!mRocket.isFlying()) mQteManager.update(scaledTime);

    mRocket.update(dt);

    if (mRocket.isFlying()) {
        sf::Vector2f startPos(290.0f, 550.0f);
        if (mCurrentUpgradeLevel == 1) startPos = sf::Vector2f(340.0f, 510.0f);
        else if (mCurrentUpgradeLevel == 2) startPos = sf::Vector2f(305.0f, 575.0f);

        float groundY = 635.0f;

        mTireVisual.setPosition(startPos.x + mRocket.getDistance(), groundY - mRocket.getAltitude());

        float currentEngineBonus = mUpgrades[mCurrentUpgradeLevel].getSpeedBonus();
        float currentQTEBonus = mRocket.getLastPowerMult();
        mTireVisual.rotate(360.0f * currentEngineBonus * currentQTEBonus * dt);

        sf::FloatRect tireBox = mTireVisual.getGlobalBounds();

        float shrinkX = tireBox.width * 0.3f;
        float shrinkY = tireBox.height * 0.3f;
        sf::FloatRect smallTireBox(tireBox.left + shrinkX, tireBox.top + shrinkY, tireBox.width - shrinkX * 2.0f, tireBox.height - shrinkY * 2.0f);

        for (size_t i = 0; i < mTargets.size(); ++i) {
            if (mTargets[i]->isActive() && smallTireBox.intersects(mTargets[i]->getBounds())) {

                float damage = mRocket.getLastPowerMult() * mUpgrades[mCurrentUpgradeLevel].getSpeedBonus();
                HitResult hitRes = mTargets[i]->takeDamage(damage);

                if (hitRes == HitResult::IGNORED) {
                    continue;
                }
                else if (hitRes == HitResult::SHIELDED) {
                    std::cout << "[Game] Boss blocked the attack!" << std::endl;
                    mRocket.bounceOffShield();
                    mRocket.reduceSpeed(0.5f);
                    mHitSlowdownTimer = 0.1f;
                }
                else if (hitRes == HitResult::DAMAGED) {
                    mCoins += 15;
                    std::cout << "[Game] Hit Boss! +15 Coins" << std::endl;
                    mRocket.bounceOffShield();
                    mHitSlowdownTimer = 0.3f;
                }
                else if (hitRes == HitResult::KILLED) {
                    if (mTargets[i]->isBoss()) {
                        mIsBossActive = false;
                        mHitSlowdownTimer = 1.0f;

                        mIsVictoryScreenActive = true;
                        mVictoryAnimTimer = 0.0f;
                    }
                    else {
                        if (!mIsBossActive && !mIsBossWarningActive && mTargets[i]->isDrone()) {
                            mMobsKilled++;
                        }

                        if (dynamic_cast<Cow*>(mTargets[i].get())) {
                            mCowsHit++;

                            if (mIsBossActive && mTargets[0] && mTargets[0]->isBoss()) {
                                mTargets[0]->heal(0.75f);
                                std::cout << "[Game] Boss healed by hitting a cow!" << std::endl;
                            }

                            if (mCowsHit >= mCowsForGameOver) {
                                mIsGameOverActive = true;
                                mGameOverTimer = 3.0f;
                                mRocket.reduceSpeed(0.0f);
                                std::cout << "[Game Over triggered] Cows limit reached." << std::endl;
                                break;
                            }
                        }

                        mHitSlowdownTimer = 0.2f;
                    }

                    mCoins += mTargets[i]->getReward();
                    if (mCoins < 0) mCoins = 0;

                    if (mTargets[i]->getReward() > 0 && !mIsCelebrating && mCharacterCurrentFrame == 0) {
                        mIsCelebrating = true;
                        mCharacterCurrentFrame = mCelebrateStartFrame;
                        if (!mCharacterFrames.empty()) mCharacterSprite.setTextureRect(mCharacterFrames[mCharacterCurrentFrame]);
                        mAnimationClock.restart();
                    }
                    spawnTarget(static_cast<int>(i));
                }
            }
        }
    }

    bool currentlyFlying = mRocket.isFlying();
    if (mWasFlying && !currentlyFlying) saveProgress();
    mWasFlying = currentlyFlying;
}

void Game::render() {
    sf::Color bgColor;
    switch (mCurrentState) {
    case GameState::MENU:  bgColor = sf::Color(30, 30, 50);   break;
    case GameState::SHOP:  bgColor = sf::Color(50, 50, 30);   break;
    case GameState::PLAY:  bgColor = sf::Color(30, 50, 30);   break;
    case GameState::PAUSE: bgColor = sf::Color(50, 30, 30);   break;
    case GameState::INSTRUCTIONS: bgColor = sf::Color(20, 20, 30); break;
    }
    mWindow.clear(bgColor);

    if (mCurrentState == GameState::MENU) {
        mWindow.draw(mMenuBgSprite);
        mWindow.draw(mMenuTitleText);

        mWindow.draw(mWorkshopBtnSprite);
        mWindow.draw(mWorkshopBtnText);
        mWindow.draw(mStreetBtnSprite);
        mWindow.draw(mStreetBtnText);
        mWindow.draw(mInstBtnSprite);
        mWindow.draw(mInstBtnText);
        mWindow.draw(mResetBtnSprite);
        mWindow.draw(mResetBtnText);
    }
    else if (mCurrentState == GameState::INSTRUCTIONS) {
        mWindow.draw(mInstBgSprite);
        mWindow.draw(mInstExitBtnSprite);

        sf::Text tipText;
        tipText.setFont(mFont);
        tipText.setCharacterSize(28);
        tipText.setFillColor(sf::Color::White);
        tipText.setOutlineColor(sf::Color::Black);
        tipText.setOutlineThickness(2.0f);
        tipText.setPosition(40.0f, 660.0f);
        tipText.setString("Press [1] or Click Exit Button to return to Main Menu");
        mWindow.draw(tipText);
    }
    else if (mCurrentState == GameState::SHOP) {
        mWindow.draw(mShopBgSprite);
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

            uiText.setOrigin(uiText.getLocalBounds().left + uiText.getLocalBounds().width / 2.0f, uiText.getLocalBounds().top + uiText.getLocalBounds().height / 2.0f);
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

        for (const auto& target : mTargets) target->draw(mWindow);

        mWindow.draw(mCowProgressBarBg);
        mWindow.draw(mCowProgressBar);
        mWindow.draw(mCowProgressText);

        if (!mIsBossActive && !mIsBossWarningActive) {
            mWindow.draw(mBossProgressBarBg);
            mWindow.draw(mBossProgressBar);
            mWindow.draw(mBossProgressText);
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
            if (mQteManager.getState() == QTEState::ANGLE_SELECT) {
                float currentEngineBonus = mUpgrades[mCurrentUpgradeLevel].getSpeedBonus();
                float spinSpeed = 800.0f * mQteManager.getFinalPowerMult() * currentEngineBonus;
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
            if (playCoinTexture.loadFromFile("monetybg1.png")) isPlayCoinLoaded = true;
        }

        if (isPlayCoinLoaded) {
            sf::Sprite playCoinSprite(playCoinTexture);
            float coinBgScale = 0.05f;
            playCoinSprite.setPosition(15.0f, 15.0f);
            playCoinSprite.setScale(coinBgScale, coinBgScale);
            mWindow.draw(playCoinSprite);

            sf::Text playCoinText;
            playCoinText.setFont(mFont);
            playCoinText.setCharacterSize(22);
            playCoinText.setFillColor(sf::Color(60, 60, 60));
            playCoinText.setString(std::to_string(mCoins));
            playCoinText.setOrigin(playCoinText.getLocalBounds().left + playCoinText.getLocalBounds().width / 2.0f, playCoinText.getLocalBounds().top + playCoinText.getLocalBounds().height / 2.0f);

            float centerX = 15.0f + (playCoinTexture.getSize().x * coinBgScale) / 2.0f;
            float centerY = 15.0f + (playCoinTexture.getSize().y * coinBgScale) * 0.72f;
            playCoinText.setPosition(centerX, centerY - 3.0f);
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

        if (mIsBossWarningActive) {
            mWindow.draw(mRedFlashRect);
            mWindow.draw(mWarningText);
        }

        if (mIsGameOverActive) {
            mWindow.draw(mRedFlashRect);
            mWindow.draw(mGameOverText);
        }

        if (mIsVictoryScreenActive) {
            float t = mVictoryAnimTimer / 1.5f;
            if (t > 1.0f) t = 1.0f;
            float smoothT = std::sin(t * 3.14159f / 2.0f);

            sf::RectangleShape darkOverlay(sf::Vector2f(1280.0f, 720.0f));
            darkOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<int>(200.0f * smoothT)));
            mWindow.draw(darkOverlay);

            mWindow.draw(mLightBeam1);
            mWindow.draw(mLightBeam2);

            mWindow.draw(mCupSprite);
            mWindow.draw(mCongratsText);

            mWindow.draw(mVictoryQuitBtnSprite);
            mWindow.draw(mVictoryQuitBtnText);
            mWindow.draw(mVictoryContBtnSprite);
            mWindow.draw(mVictoryContBtnText);
        }
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
            if (unlocked == 1) mUpgrades[i].unlock();
        }
        file.close();
    }
}
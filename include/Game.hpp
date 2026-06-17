#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <fstream>
#include "Rocket.hpp"
#include "Equipment.hpp"
#include "GameObject.hpp"
#include "QTEManager.hpp"

enum class GameState { MENU, SHOP, PLAY, PAUSE, INSTRUCTIONS };

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void applyUpgrades();
    void spawnTarget(int index);
    void saveProgress();
    void loadProgress();

    // --- НОВЫЕ ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (РЕФАКТОРИНГ) ---
    void initButton(sf::Sprite& sprite, sf::Text& text, const std::string& str, float posX, float posY, float scale = 1.1f, int fontSize = 34);
    void initProgressBar(sf::RectangleShape& bg, sf::RectangleShape& bar, sf::Text& text, const std::string& str, sf::Color color, float posY);
    void resetGameProgress();
    // ----------------------------------------------------

    sf::RenderWindow mWindow;
    GameState mCurrentState;

    sf::Font mFont;
    int mCoins;
    bool mWasFlying;

    sf::RectangleShape mScoreBackground;
    sf::Text mScoreText;
    sf::CircleShape mCoinIcon;

    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;

    sf::Texture mLauncherTexture;
    sf::Sprite mLauncherSprite;

    sf::Texture mTireTexture;
    sf::Sprite mTireVisual;

    sf::Texture mCharacterTexture;
    sf::Sprite mCharacterSprite;
    std::vector<sf::IntRect> mCharacterFrames;

    Rocket mRocket;
    QTEManager mQteManager;
    std::vector<std::unique_ptr<Target>> mTargets;

    std::vector<UpgradeSet> mUpgrades;
    int mCurrentUpgradeLevel;

    sf::Texture mShopBgTexture;
    sf::Sprite mShopBgSprite;
    sf::Texture mBoardTexture;
    sf::Sprite mBoardSprite;

    sf::Texture mPreviewEngineTextures[3];
    sf::Texture mPreviewTireTextures[3];
    sf::Sprite mPreviewEngineSprites[3];
    sf::Sprite mPreviewTireSprites[3];

    sf::Texture mNewCoinBgTexture;
    sf::Sprite mNewCoinBgSprite;

    sf::Texture mPriceBgTexture;
    sf::Sprite mPriceBgSprites[3];

    sf::Texture mBuyKeyTexture;
    sf::Sprite mBuyKeySprites[3];

    int mCharacterCurrentFrame;
    bool mIsCelebrating;
    sf::Clock mAnimationClock;
    float mFrameDuration;
    int mCelebrateStartFrame;

    sf::Texture mMenuBgTexture;
    sf::Sprite  mMenuBgSprite;
    sf::Sprite  mWorkshopBtnSprite;
    sf::Sprite  mStreetBtnSprite;
    sf::Text    mMenuTitleText;
    sf::Text    mWorkshopBtnText;
    sf::Text    mStreetBtnText;

    sf::Sprite  mInstBtnSprite;
    sf::Text    mInstBtnText;
    sf::Texture mInstBgTexture;
    sf::Sprite  mInstBgSprite;

    sf::Texture mInstExitBtnTexture;
    sf::Sprite  mInstExitBtnSprite;

    sf::Sprite  mResetBtnSprite;
    sf::Text    mResetBtnText;

    int mMobsKilled;
    int mMobsForBoss;
    bool mIsBossActive;

    int mCowsHit;
    int mCowsForGameOver;
    sf::RectangleShape mCowProgressBarBg;
    sf::RectangleShape mCowProgressBar;
    sf::Text mCowProgressText;

    sf::RectangleShape mBossProgressBarBg;
    sf::RectangleShape mBossProgressBar;
    sf::Text mBossProgressText;

    float mHitSlowdownTimer;

    bool mIsBossWarningActive;
    float mBossWarningTimer;
    sf::RectangleShape mRedFlashRect;
    sf::Text mWarningText;

    bool mIsGameOverActive;
    float mGameOverTimer;
    sf::Text mGameOverText;

    bool mIsVictoryScreenActive;
    float mVictoryAnimTimer;

    sf::ConvexShape mLightBeam1;
    sf::ConvexShape mLightBeam2;

    sf::Texture mCupTexture;
    sf::Sprite  mCupSprite;
    sf::Text    mCongratsText;
    sf::Sprite  mVictoryQuitBtnSprite;
    sf::Text    mVictoryQuitBtnText;
    sf::Sprite  mVictoryContBtnSprite;
    sf::Text    mVictoryContBtnText;
};
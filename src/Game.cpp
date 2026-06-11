#include "Game.hpp"

Game::Game()
    : mWindow(sf::VideoMode(1280, 720), "Warsztat Jugaad")
    , mCurrentState(GameState::MENU)
    , mCoins(0)
    , mWasFlying(false)
    , mCurrentMotorIndex(0)
    , mCurrentTireIndex(0)
{
    srand(static_cast<unsigned int>(time(0)));
    mWindow.setFramerateLimit(60);
    loadProgress();

    if (!mFont.loadFromFile("VT323-Regular.ttf")) {
        std::cout << "[Blad] Brak czcionki!" << std::endl;
    }

    // 1. ИНИЦИАЛИЗАЦИЯ МАГАЗИНА (Классы напарника)
    // Motor(name, price, isUnlocked, speedBonus)
    mMotors.push_back(Motor("Stary Silnik", 0, true, 0.5f));
    mMotors.push_back(Motor("Silnik Diesla", 150, false, 1.0f));
    mMotors.push_back(Motor("Turbo Jugaad", 350, false, 2.0f));

    // Tire(name, price, isUnlocked, aeroBonus)
    mTires.push_back(Tire("Lysa Opona", 0, true, 1.5f));
    mTires.push_back(Tire("Opona od Riksz", 150, false, 1.0f));
    mTires.push_back(Tire("Opona Sportowa", 350, false, 0.4f));

    applyUpgrades();

    mTireVisual.setRadius(15.0f);
    mTireVisual.setFillColor(sf::Color(80, 80, 80));
    mTireVisual.setOrigin(15.0f, 15.0f);

    // 2. СПАВН ПЕРВЫХ ЦЕЛЕЙ
    for (int i = 0; i < 3; ++i) {
        mTargets.push_back(nullptr); // Резервируем место
        spawnTarget(i);              // Заполняем функцией
    }
}

void Game::spawnTarget(int index) {
    float randomY = 100.0f + static_cast<float>(rand() % 400); // 100 (высоко) до 500 (низко)

    // ЛОГИКА: Если высота от 100 до 300 (высоко) -> 50% шанс на Дрона
    if (randomY < 300.0f && (rand() % 2 == 0)) {
        mTargets[index] = std::make_unique<Drone>();
    }
    else {
        mTargets[index] = std::make_unique<Bird>();
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
    float enginePwr = mMotors[mCurrentMotorIndex].getPowerMultiplier();
    float tireDrg = mTires[mCurrentTireIndex].getPowerMultiplier();
    mRocket.upgrade(enginePwr, tireDrg);
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

            // МАГАЗИН С ИСПОЛЬЗОВАНИЕМ КЛАССОВ НАПАРНИКА
            if (mCurrentState == GameState::SHOP) {
                // Покупка/Выбор Двигателя (Кнопка E)
                if (event.key.code == sf::Keyboard::E && mCurrentMotorIndex < mMotors.size() - 1) {
                    int nextIdx = mCurrentMotorIndex + 1;
                    if (!mMotors[nextIdx].getIsUnlocked() && mCoins >= mMotors[nextIdx].getPrice()) {
                        mCoins -= mMotors[nextIdx].getPrice();
                        mMotors[nextIdx].unlock();
                        mCurrentMotorIndex = nextIdx;
                        applyUpgrades();
                    }
                    else if (mMotors[nextIdx].getIsUnlocked()) {
                        mCurrentMotorIndex = nextIdx;
                        applyUpgrades();
                    }
                }
                // Покупка/Выбор Покрышки (Кнопка T)
                if (event.key.code == sf::Keyboard::T && mCurrentTireIndex < mTires.size() - 1) {
                    int nextIdx = mCurrentTireIndex + 1;
                    if (!mTires[nextIdx].getIsUnlocked() && mCoins >= mTires[nextIdx].getPrice()) {
                        mCoins -= mTires[nextIdx].getPrice();
                        mTires[nextIdx].unlock();
                        mCurrentTireIndex = nextIdx;
                        applyUpgrades();
                    }
                    else if (mTires[nextIdx].getIsUnlocked()) {
                        mCurrentTireIndex = nextIdx;
                        applyUpgrades();
                    }
                }
            }

            if (event.key.code == sf::Keyboard::Space && mCurrentState == GameState::PLAY) {
                if (!mQteManager.isActive() && !mRocket.isFlying()) {
                    mQteManager.start();
                }
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
    }
}

void Game::update(sf::Time deltaTime) {
    if (mCurrentState != GameState::PLAY) return;

    // 1. ДВИЖЕНИЕ ЦЕЛЕЙ
    for (size_t i = 0; i < mTargets.size(); ++i) {
        if (!mTargets[i]->isActive()) continue;

        mTargets[i]->update(deltaTime);

        sf::Vector2f pos = mTargets[i]->getPosition();
        float spd = mTargets[i]->getSpeed();

        bool flewRight = (spd > 0.0f && pos.x > 1300.0f);
        bool flewLeft = (spd < 0.0f && pos.x < -100.0f);

        // Если улетели за экран - пересоздаем
        if (flewRight || flewLeft) {
            spawnTarget(i);
        }
    }

    if (!mRocket.isFlying()) mQteManager.update(deltaTime);
    mRocket.update(deltaTime.asSeconds());

    // 2. СТОЛКНОВЕНИЯ
    if (mRocket.isFlying()) {
        sf::Vector2f startPos(100.0f, 650.0f);
        mTireVisual.setPosition(startPos.x + mRocket.getDistance(), startPos.y - mRocket.getAltitude());

        sf::FloatRect tireBox = mTireVisual.getGlobalBounds();
        for (size_t i = 0; i < mTargets.size(); ++i) {
            if (mTargets[i]->isActive() && tireBox.intersects(mTargets[i]->getBounds())) {
                mTargets[i]->setActive(false);
                mCoins += mTargets[i]->getReward(); // Дрон дает 150, Птица 50
                std::cout << "[Gra] Trafienie! + " << mTargets[i]->getReward() << " monet! Stan: " << mCoins << std::endl;
                // Сразу спавним новую цель на замену сбитой
                spawnTarget(i);
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
        sf::Text shopText;
        shopText.setFont(mFont);
        shopText.setCharacterSize(30);
        shopText.setFillColor(sf::Color::White);
        shopText.setPosition(50.f, 50.f);

        int nxtM = (mCurrentMotorIndex < mMotors.size() - 1) ? mCurrentMotorIndex + 1 : mCurrentMotorIndex;
        std::string mTxt = (mCurrentMotorIndex < mMotors.size() - 1) ? "Cena: " + std::to_string(mMotors[nxtM].getPrice()) : "MAX";

        int nxtT = (mCurrentTireIndex < mTires.size() - 1) ? mCurrentTireIndex + 1 : mCurrentTireIndex;
        std::string tTxt = (mCurrentTireIndex < mTires.size() - 1) ? "Cena: " + std::to_string(mTires[nxtT].getPrice()) : "MAX";

        shopText.setString(
            "--- WARSZTAT JUGAAD ---\n"
            "Dostepne monety: " + std::to_string(mCoins) + "\n\n"
            "[E] Silnik: " + mMotors[mCurrentMotorIndex].getName() + "\n"
            "    -> Nastepny: " + mMotors[nxtM].getName() + " | " + mTxt + "\n\n"
            "[T] Opony: " + mTires[mCurrentTireIndex].getName() + "\n"
            "    -> Nastepny: " + mTires[nxtT].getName() + " | " + tTxt + "\n\n"
            "Nacisnij 3 aby wrocic na ulice"
        );
        mWindow.draw(shopText);
    }

    if (mCurrentState == GameState::PLAY) {
        for (const auto& target : mTargets) {
            target->draw(mWindow);
        }

        if (mQteManager.isActive()) {
            sf::Vector2f startPos(100.0f, 650.0f);
            auto points = mRocket.getTrajectoryPoints(mQteManager.getMultiplier(), startPos);
            for (const auto& pos : points) {
                sf::CircleShape dot(3.0f);
                dot.setPosition(pos);
                mWindow.draw(dot);
            }
        }
        if (mRocket.isFlying()) mWindow.draw(mTireVisual);
    }

    mWindow.display();
}

void Game::saveProgress() { std::ofstream file("save.txt"); if (file.is_open()) { file << mCoins; file.close(); } }
void Game::loadProgress() { std::ifstream file("save.txt"); if (file.is_open()) { file >> mCoins; file.close(); } }
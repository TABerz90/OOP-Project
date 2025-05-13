#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

class TextureLoader {
public:
    static const int FRAME_WIDTH = 32;
    static const int FRAME_HEIGHT = 32;
    static const int FRAME_COUNT = 10;

    static bool load(Texture& texture, const string& filename) {
        if (!texture.loadFromFile(filename)) {
            cerr << "Failed to load texture: " << filename << endl;
            return false;
        }
        return true;
    }

    static IntRect getFrameRect(int frameIndex) {
        return IntRect(frameIndex * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT);
    }
};

class Player {
private:
    int HP;
    int coins;
    int maxHP;
    int powerBoost;
    int powerDuration;
    int currentMana;
    int maxMana;
    Sprite sprite;
    Texture standingTexture;
    Texture dyingTexture;
    int currentFrame;
    int state; // 0=standing, 1=attacking, 2=dying
    Clock frameClock;

public:
    Player() : HP(25), coins(100), maxHP(25), powerBoost(0), powerDuration(0), currentMana(5), maxMana(5),
        currentFrame(0), state(0) {
        if (TextureLoader::load(standingTexture, "player standing.png") &&
            TextureLoader::load(dyingTexture, "player dying.png")) {
            sprite.setTexture(standingTexture);
            sprite.setTextureRect(TextureLoader::getFrameRect(0));
        }
        sprite.setScale(2.f, 2.f);
    }

    int getHP() const { return HP; }
    int getCoins() const { return coins; }
    int getPowerBoost() const { return powerBoost; }
    int getPowerDuration() const { return powerDuration; }
    void decHP() { HP--; }
    void buy(int cardVal) { coins -= cardVal; }
    void takeDMG(int val) {
        HP -= val;
        if (HP <= 0) state = 2;
    }
    void heal(int val) {
        HP = HP + val;
        if (HP > maxHP) HP = maxHP;
    }
    void increaseCoins(int val) { coins += val; }
    void setPowerBoost(int val) { powerBoost = val; }
    void setPowerDuration(int val) { powerDuration = val; }
    void decrementPowerDuration() { if (powerDuration > 0) powerDuration--; }
    int getCurrentMana() const { return currentMana; }
    int getMaxMana() const { return maxMana; }
    void spendMana(int amount) { currentMana -= amount; }
    void resetMana() { currentMana = maxMana; }
    void increaseMaxMana(int amount) { maxMana += amount; }
    int getMaxHP() { return maxHP; }
    void setMaxHP(int val) { maxHP = val; }
    void setMaxMana(int val) { maxMana = val; }
    void setCurrentMana(int val) { currentMana = val; }

    void updateSprite(float deltaTime) {
        if (frameClock.getElapsedTime().asSeconds() > 0.1f) {
            if (state == 2) { // Dying
                sprite.setTexture(dyingTexture);
                if (currentFrame < TextureLoader::FRAME_COUNT - 1) {
                    currentFrame++;
                }
            }
            else { // Standing
                sprite.setTexture(standingTexture);
                currentFrame = (currentFrame + 1) % TextureLoader::FRAME_COUNT;
            }
            sprite.setTextureRect(TextureLoader::getFrameRect(currentFrame));
            frameClock.restart();
        }
    }

    Sprite& getSprite() { return sprite; }
    bool isAlive() const { return HP > 0; }
};

class Enemy {
protected:
    int HP;
    bool alive;
    int exhaustValue;
    int exhaustDuration;
    int enemyType; // 0 = cronie, 1 = captain, 2 = boss
    Sprite sprite;
    Texture standingTexture;
    Texture dyingTexture;
    int currentFrame;
    int state; // 0=standing, 1=attacking, 2=dying, 3=healing
    Clock frameClock;

public:
    Enemy() : HP(10), alive(true), exhaustValue(0), exhaustDuration(0), enemyType(0),
        currentFrame(0), state(0) {
    }

    int getHP() const { return HP; }
    void setHP(int val) { HP = val; }
    bool isAlive() const { return alive; }
    void setAlive(bool status) { alive = status; }
    int getEnemyType() const { return enemyType; }
    virtual void takeDMG(int val) {
        HP -= val;
        if (HP <= 0) {
            alive = false;
            state = 2; // Dying state
        }
    }
    virtual void attack(Player& player) = 0;
    virtual void updateSprite(float deltaTime) {
        if (frameClock.getElapsedTime().asSeconds() > 0.1f) {
            if (state == 2) { // Dying
                sprite.setTexture(dyingTexture);
                if (currentFrame < TextureLoader::FRAME_COUNT - 1) {
                    currentFrame++;
                }
            }
            else { // Standing
                sprite.setTexture(standingTexture);
                currentFrame = (currentFrame + 1) % TextureLoader::FRAME_COUNT;
            }
            sprite.setTextureRect(TextureLoader::getFrameRect(currentFrame));
            frameClock.restart();
        }
    }
    Sprite& getSprite() { return sprite; }

    virtual ~Enemy() {}
};

class Cronie : public Enemy {
public:
    Cronie() {
        setHP(5);
        enemyType = 0;
        cout << "Cronie Deployed" << endl;
        if (TextureLoader::load(standingTexture, "Cronies Standing.png") &&
            TextureLoader::load(dyingTexture, "Cronies Dying.png")) {
            sprite.setTexture(standingTexture);
            sprite.setTextureRect(TextureLoader::getFrameRect(0));
        }
        sprite.setScale(2.f, 2.f);
        sprite.setOrigin(TextureLoader::FRAME_WIDTH / 2, TextureLoader::FRAME_HEIGHT / 2);
        sprite.setScale(-2.f, 2.f); // Flip horizontally
    }

    void attack(Player& player) override {
        player.decHP();
        state = 1; // Attacking state
    }

    void updateSprite(float deltaTime) override {
        Enemy::updateSprite(deltaTime);
    }
};

class Captain : public Enemy {
public:
    Captain() {
        setHP(7);
        enemyType = 1;
        cout << "Captain Deployed" << endl;
        if (TextureLoader::load(standingTexture, "Captain Standing.png") &&
            TextureLoader::load(dyingTexture, "Captain Dying.png")) {
            sprite.setTexture(standingTexture);
            sprite.setTextureRect(TextureLoader::getFrameRect(0));
        }
        sprite.setScale(2.f, 2.f);
        sprite.setOrigin(TextureLoader::FRAME_WIDTH / 2, TextureLoader::FRAME_HEIGHT / 2);
        sprite.setScale(-2.f, 2.f); // Flip horizontally
    }

    void attack(Player& player) override {
        int dmg = rand() % 3;
        player.takeDMG(dmg);
        state = 1; // Attacking state
    }

    void updateSprite(float deltaTime) override {
        Enemy::updateSprite(deltaTime);
    }
};

class Boss : public Enemy {
public:
    Boss() {
        setHP(15);
        enemyType = 2;
        cout << "Boss Deployed" << endl;
        if (TextureLoader::load(standingTexture, "Boss Standing.png") &&
            TextureLoader::load(dyingTexture, "Boss Dying.png")) {
            sprite.setTexture(standingTexture);
            sprite.setTextureRect(TextureLoader::getFrameRect(0));
        }
        sprite.setScale(2.f, 2.f);
        sprite.setOrigin(TextureLoader::FRAME_WIDTH / 2, TextureLoader::FRAME_HEIGHT / 2);
        sprite.setScale(-2.f, 2.f); // Flip horizontally
    }

    void attack(Player& player) override {
        int dmg = rand() % 5;
        player.takeDMG(dmg);
        state = 1; // Attacking state
    }

    void heal() {
        if (rand() % 6 > 2) {
            setHP(getHP() + 2);
            state = 3; // Healing state
        }
    }

    void updateSprite(float deltaTime) override {
        Enemy::updateSprite(deltaTime);
    }
};

class Card {
protected:
    bool AOE;
    int cardID;
    bool isUnlocked;
    Texture texture;
    Sprite sprite;
public:
    Card() : AOE(false), cardID(0), isUnlocked(false) {
        sprite.setScale(2.f, 2.f);
    }

    bool isAOE() const { return AOE; }
    int getID() const { return cardID; }
    bool getUnlockStatus() const { return isUnlocked; }
    void setAOE(bool val) { AOE = val; }
    void setID(int val) { cardID = val; }
    void unlockCard() { isUnlocked = true; }
    Sprite& getSprite() { return sprite; }

    virtual void play(Player* player, Enemy** enemies, int enemyCount) {
        cout << "Base card played (does nothing)\n";
    }

    virtual ~Card() {}
};

class SlashCard : public Card {
private:
    static int attack;
public:
    SlashCard() {
        setID(0);
        unlockCard();
        if (TextureLoader::load(texture, "Slash.png")) {
            sprite.setTexture(texture);
        }
    }

    static void upgrade() { ++attack; }

    void play(Player* player, Enemy** enemies, int enemyCount) override {
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]->isAlive()) {
                slashing(*enemies[i]);
                break;
            }
        }
    }

    void slashing(Enemy& enemy) const {
        if (enemy.isAlive()) enemy.takeDMG(attack);
    }
};
int SlashCard::attack = 3;

class HealCard : public Card {
private:
    static int healing;
public:
    HealCard() {
        setID(1);
        unlockCard();
        if (TextureLoader::load(texture, "HEAL.png")) {
            sprite.setTexture(texture);
        }
    }

    static void upgrade() { ++healing; }

    void play(Player* player, Enemy** enemies, int enemyCount) override {
        heal(*player);
    }

    void heal(Player& player) { player.heal(healing); }
};
int HealCard::healing = 2;

class DrainCard : public Card {
private:
    static int drainAttack, drainHealing;
public:
    DrainCard() {
        setID(2);
        if (TextureLoader::load(texture, "Drain.png")) {
            sprite.setTexture(texture);
        }
    }

    static void upgrade() { ++drainAttack; ++drainHealing; }

    void play(Player* player, Enemy** enemies, int enemyCount) override {
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]->isAlive()) {
                lifeSteal(*enemies[i], *player);
                break;
            }
        }
    }

    void lifeSteal(Enemy& enemy, Player& player) {
        enemy.takeDMG(drainAttack);
        player.heal(drainHealing);
    }
};
int DrainCard::drainAttack = 2;
int DrainCard::drainHealing = 1;

class Inquisition : public Card {
private:
    static int INattack;
public:
    Inquisition() {
        setAOE(true);
        setID(3);
        if (TextureLoader::load(texture, "Inquisition.png")) {
            sprite.setTexture(texture);
        }
    }

    static void upgrade() { INattack++; }

    void play(Player* player, Enemy** enemies, int enemyCount) override {
        inquisition(enemies, enemyCount);
    }

    void inquisition(Enemy** enemies, int size) {
        for (int i = 0; i < size; i++) {
            if (enemies[i]->isAlive()) enemies[i]->takeDMG(INattack);
        }
    }
};
int Inquisition::INattack = 2;

class MagickaCard : public Card {
private:
    static int magickaDamage;
    static bool used;
public:
    MagickaCard() {
        setID(5);
        setAOE(true);
        if (TextureLoader::load(texture, "MAGICKA.png")) {
            sprite.setTexture(texture);
        }
    }

    static void reset() { used = false; }

    void play(Player* player, Enemy** enemies, int enemyCount) override {
        if (used) {
            cout << "Magicka already used this battle!\n";
            return;
        }

        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]->isAlive()) {
                enemies[i]->takeDMG(magickaDamage);
            }
        }
        used = true;
        cout << "UNLIMITED POWERRRR! All enemies take " << magickaDamage << " damage!\n";
    }
};
int MagickaCard::magickaDamage = 20;
bool MagickaCard::used = false;

class Deck {
private:
    static const int MAX_CARDS = 25;
    vector<Card*> cards;  // Changed from array to vector for easier management
    bool hasMagickaCard;

public:
    Deck() : hasMagickaCard(false) {
        // Initialize with basic cards
        for (int i = 0; i < 4; i++) {
            addCard(new SlashCard());
        }
        for (int i = 0; i < 2; i++) {
            addCard(new HealCard());
        }
        shuffle();
    }

    ~Deck() {
        for (Card* card : cards) {
            delete card;
        }
    }

    bool addCard(Card* newCard) {
        if (cards.size() >= MAX_CARDS) return false;

        if (newCard->getID() == 5) { // Magicka card
            if (hasMagickaCard) {
                delete newCard;
                return false;
            }
            hasMagickaCard = true;
        }

        cards.push_back(newCard);
        return true;
    }

    void shuffle() {
        random_shuffle(cards.begin(), cards.end());
    }

    Card* draw() {
        if (cards.empty()) return nullptr;

        Card* card = cards.back();
        cards.pop_back();
        return card;
    }

    void discard(Card* card) {
        if (cards.size() < MAX_CARDS) {
            cards.insert(cards.begin(), card); // Add to bottom of deck
        }
    }

    void returnToDeck(Card* card) {
        if (cards.size() < MAX_CARDS) {
            cards.push_back(card); // Add to top of deck
        }
    }

    int getCardCount() const { return cards.size(); }
    int getMaxCards() const { return MAX_CARDS; }
};


class Battle {
private:
    Player& player;
    Deck& deck;
    int node;
    RenderWindow& window;

    Enemy* enemies[4];
    int enemyCount;
    Card* hand[4];
    int cardsInHand;
    bool playerTurn;
    bool battleOver;
    bool playerWon;
    int selectedCard;
    int selectedEnemy;
    Text actionText;
    Text turnText;

    Texture bgTexture;
    Sprite background;
    Font font;

    RectangleShape hpBox;
    RectangleShape manaBox;
    Text hpText;
    Text manaText;

    Clock deltaClock;

    const float PLAYER_SCALE = 2.0f;
    const float ENEMY_SCALE = 2.0f;
    const float CARD_SCALE = 0.1f;

    enum BattleState {
        SELECT_CARD,
        SELECT_ENEMY,
        PROCESSING
    };
    BattleState currentState;

    // Card display members
    const Vector2f CARD_POSITIONS[4] = {
        Vector2f(250, 575),
        Vector2f(400, 575),
        Vector2f(550, 575),
        Vector2f(700, 575)
    };

    void setupEnemies() {
        if (node < 3) {
            enemyCount = 3;
            for (int i = 0; i < enemyCount; i++) {
                enemies[i] = new Cronie();
            }
        }
        else if (node >= 3 && node < 7) {
            enemyCount = 4;
            for (int i = 0; i < enemyCount; i++) {
                enemies[i] = (rand() % 100 < 75) ? (Enemy*)(new Cronie()) : (Enemy*)(new Captain());
            }
        }
        else {
            enemyCount = 4;
            enemies[0] = new Boss();
            for (int i = 1; i < enemyCount; i++) {
                enemies[i] = (rand() % 100 < 75) ? (Enemy*)(new Cronie()) : (Enemy*)(new Captain());
            }
        }

        float startY = 150;
        float spacing = (600 - startY) / (enemyCount + 1);
        for (int i = 0; i < enemyCount; i++) {
            float x = 800;
            float y = startY + spacing * (i + 1);
            enemies[i]->getSprite().setPosition(x, y);
            enemies[i]->getSprite().setScale(-ENEMY_SCALE, ENEMY_SCALE);
            enemies[i]->getSprite().setOrigin(
                enemies[i]->getSprite().getLocalBounds().width / 2,
                enemies[i]->getSprite().getLocalBounds().height / 2
            );
        }
    }

    void setupUI() {
        player.getSprite().setPosition(200, 360);
        player.getSprite().setScale(PLAYER_SCALE, PLAYER_SCALE);
        player.getSprite().setOrigin(
            player.getSprite().getLocalBounds().width / 2,
            player.getSprite().getLocalBounds().height / 2
        );

        hpBox.setSize(Vector2f(200, 30));
        hpBox.setPosition(900, 650);
        hpBox.setFillColor(Color(200, 50, 50, 200));

        hpText.setFont(font);
        hpText.setCharacterSize(24);
        hpText.setPosition(910, 650);

        manaBox.setSize(Vector2f(200, 30));
        manaBox.setPosition(1150, 650);
        manaBox.setFillColor(Color(50, 50, 200, 200));

        manaText.setFont(font);
        manaText.setCharacterSize(24);
        manaText.setPosition(1160, 650);

        actionText.setFont(font);
        actionText.setCharacterSize(24);
        actionText.setPosition(50, 600);
        actionText.setFillColor(Color::White);

        turnText.setFont(font);
        turnText.setCharacterSize(36);
        turnText.setFillColor(Color::White);
        updateTurnText();
    }

    void fillHand() {
        // Clear current hand first
        for (int i = 0; i < 4; i++) {
            if (hand[i]) {
                deck.discard(hand[i]);
                hand[i] = nullptr;
            }
        }

        // Draw new cards
        cardsInHand = 0;
        for (int i = 0; i < 4; i++) {
            hand[i] = deck.draw();
            if (hand[i]) {
                cardsInHand++;
                hand[i]->getSprite().setScale(CARD_SCALE, CARD_SCALE);
            }
        }
        updateActionText();
    }

    void cleanup() {
        for (int i = 0; i < enemyCount; i++) {
            delete enemies[i];
            enemies[i] = nullptr;
        }
        for (int i = 0; i < 4; i++) {
            if (hand[i]) {
                deck.discard(hand[i]);
                hand[i] = nullptr;
            }
        }
        cardsInHand = 0;
    }

    void awardCoins() {
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]) {
                switch (enemies[i]->getEnemyType()) {
                case 0: player.increaseCoins(15); break;
                case 1: player.increaseCoins(25); break;
                case 2: player.increaseCoins(50); break;
                }
            }
        }
        player.increaseCoins(50);
    }

    void updateActionText() {
        string text;
        if (currentState == SELECT_CARD) {
            text = "Select a card:\n";
            for (int i = 0; i < 4; i++) {
                if (hand[i]) {
                    text += to_string(i + 1) + ") ";
                    switch (hand[i]->getID()) {
                    case 0: text += "Slash"; break;
                    case 1: text += "Heal"; break;
                    case 2: text += "Drain"; break;
                    case 3: text += "Inquisition"; break;
                    case 5: text += "Magicka"; break;
                    }
                    text += " (Cost: 1)\n";
                }
            }
            text += "Press ENTER to end turn";
        }
        else if (currentState == SELECT_ENEMY) {
            text = "Select target:\n";
            int aliveCount = 0;
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i] && enemies[i]->isAlive()) {
                    aliveCount++;
                    text += to_string(aliveCount) + ") ";
                    switch (enemies[i]->getEnemyType()) {
                    case 0: text += "Cronie"; break;
                    case 1: text += "Captain"; break;
                    case 2: text += "Boss"; break;
                    }
                    text += "\n";
                }
            }
            text += "Press ESC to cancel";
        }
        else {
            text = "Processing...";
        }
        actionText.setString(text);
    }

    void updateTurnText() {
        turnText.setString(playerTurn ? "Player Turn" : "Enemy Turn");
        turnText.setPosition(640 - turnText.getLocalBounds().width / 2, 20);
    }

    void handleCardSelection(int cardNum) {
        if (cardNum < 1 || cardNum > 4 || !hand[cardNum - 1] || player.getCurrentMana() < 1) {
            return;
        }

        selectedCard = cardNum - 1;
        if (hand[selectedCard]->getID() == 1) { // Heal card
            playSelectedCard(-1); // -1 means target player
        }
        else {
            currentState = SELECT_ENEMY;
        }
        updateActionText();
    }

    void handleEnemySelection(int enemyNum) {
        if (enemyNum < 1) return;

        int aliveCount = 0;
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] && enemies[i]->isAlive()) {
                aliveCount++;
                if (aliveCount == enemyNum) {
                    playSelectedCard(i);
                    return;
                }
            }
        }
    }

    void playSelectedCard(int targetEnemy) {
        if (selectedCard == -1 || player.getCurrentMana() < 1) return;

        if (hand[selectedCard]->getID() == 1) { // Heal card
            hand[selectedCard]->play(&player, enemies, enemyCount);
        }
        else if (targetEnemy >= 0) {
            hand[selectedCard]->play(&player, &enemies[targetEnemy], 1);
        }

        player.spendMana(1);
        deck.discard(hand[selectedCard]); // Return to discard pile
        hand[selectedCard] = nullptr;
        cardsInHand--;

        selectedCard = -1;
        currentState = SELECT_CARD;
        updateActionText();
    }

    void endPlayerTurn() {
        playerTurn = false;
        updateTurnText();
    }

    void updateBattleState(float dt) {
        hpText.setString("HP: " + to_string(player.getHP()) + "/" + to_string(player.getMaxHP()));
        manaText.setString("Mana: " + to_string(player.getCurrentMana()) + "/" + to_string(player.getMaxMana()));

        player.updateSprite(dt);
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]) {
                enemies[i]->updateSprite(dt);
            }
        }

        if (!playerTurn && currentState != PROCESSING) {
            currentState = PROCESSING;
            updateActionText();
            updateTurnText();

            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i] && enemies[i]->isAlive()) {
                    enemies[i]->attack(player);
                    window.display();
                    sleep(milliseconds(300));
                }
            }

            playerTurn = true;
            player.resetMana();
            MagickaCard::reset();
            fillHand();
            currentState = SELECT_CARD;
            updateTurnText();
            updateActionText();
        }
    }

    bool checkBattleEnd() {
        bool allEnemiesDead = true;
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] && enemies[i]->isAlive()) {
                allEnemiesDead = false;
                break;
            }
        }

        if (allEnemiesDead) {
            awardCoins();
            battleOver = true;
            playerWon = true;
            return true;
        }

        if (!player.isAlive()) {
            battleOver = true;
            playerWon = false;
            return true;
        }

        return false;
    }

    void render() {
        window.clear();
        window.draw(background);

        // Draw enemies
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]) {
                window.draw(enemies[i]->getSprite());
            }
        }

        // Draw player
        window.draw(player.getSprite());

        // Draw cards
        for (int i = 0; i < 4; i++) {
            if (hand[i]) {
                // Position cards vertically
                hand[i]->getSprite().setPosition(CARD_POSITIONS[i]);
                hand[i]->getSprite().setRotation(0);
                window.draw(hand[i]->getSprite());
            }
        }

        // Draw UI
        window.draw(hpBox);
        window.draw(manaBox);
        window.draw(hpText);
        window.draw(manaText);
        window.draw(actionText);
        window.draw(turnText);

        window.display();
    }

public:
    Battle(Player& p, Deck& d, int n, RenderWindow& w) :
        player(p), deck(d), node(n), window(w),
        playerTurn(true), battleOver(false), playerWon(false),
        cardsInHand(0), selectedCard(-1), selectedEnemy(-1),
        currentState(SELECT_CARD) {

        for (int i = 0; i < 4; i++) {
            enemies[i] = nullptr;
            hand[i] = nullptr;
        }

        if (!bgTexture.loadFromFile("battle.png") || !font.loadFromFile("Fonts/American Captain.ttf")) {
            cerr << "Failed to load battle resources!" << endl;
            return;
        }
        background.setTexture(bgTexture);

        setupEnemies();
        setupUI();
        fillHand();
    }

    ~Battle() {
        cleanup();
    }

    bool run() {
        while (window.isOpen() && !battleOver) {
            Time deltaTime = deltaClock.restart();

            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return false;
                }

                if (event.type == Event::KeyPressed && playerTurn) {
                    if (event.key.code == Keyboard::Enter) {
                        endPlayerTurn();
                    }
                    else if (currentState == SELECT_CARD) {
                        if (event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num4) {
                            handleCardSelection(event.key.code - Keyboard::Num1 + 1);
                        }
                    }
                    else if (currentState == SELECT_ENEMY) {
                        if (event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num4) {
                            handleEnemySelection(event.key.code - Keyboard::Num1 + 1);
                        }
                        else if (event.key.code == Keyboard::Escape) {
                            currentState = SELECT_CARD;
                            selectedCard = -1;
                            updateActionText();
                        }
                    }
                }
            }

            updateBattleState(deltaTime.asSeconds());

            if (checkBattleEnd()) {
                break;
            }

            render();
        }

        return playerWon;
    }
};
class Shop {
private:
    Texture crossTexture;
    Sprite crossSprite;
    Texture cardTextures[5]; // 0=Slash, 1=Heal, 2=Inquisition, 3=Drain, 4=Magicka
    Texture upgradeTextures[3]; // 0=RefillHP, 1=IncreaseHP, 2=IncreaseMana
    Sprite cardSprites[5];
    Sprite upgradeSprites[3];
    Font font;
    Text priceTexts[8]; // 5 cards + 3 upgrades
    Text selectionTexts[8]; // Numbers for selection
    Text coinText;
    int prices[5] = { 50, 50, 100, 100, 200 }; // Card upgrade/unlock prices
    int upgradePrices[3] = { 20, 50, 50 }; // RefillHP, IncreaseHP, IncreaseMana prices
    bool unlocked[5] = { true, true, false, false, false }; // Slash/Heal start unlocked

    // Card positions
    const Vector2f CARD_POSITIONS[5] = {
        Vector2f(200, 150),  // Slash (1)
        Vector2f(400, 150),  // Heal (2)
        Vector2f(600, 150),  // Inquisition (3)
        Vector2f(200, 350),  // Drain (4)
        Vector2f(400, 350)   // Magicka (5)
    };

    // Upgrade positions
    const Vector2f UPGRADE_POSITIONS[3] = {
        Vector2f(600, 350),  // Refill HP (6)
        Vector2f(800, 150),  // Increase HP (7)
        Vector2f(800, 350)   // Increase Mana (8)
    };

public:
    Shop() {
        // Load textures
        if (!crossTexture.loadFromFile("cross.png")) {
            cerr << "Failed to load cross texture!" << endl;
        }
        crossSprite.setTexture(crossTexture);
        crossSprite.setPosition(1200, 20);

        // Load card textures
        string cardFiles[5] = { "slash.png", "heal.png", "inquisition.png", "drain.png", "MAGICKA.png" };
        for (int i = 0; i < 5; i++) {
            if (!cardTextures[i].loadFromFile(cardFiles[i])) {
                cerr << "Failed to load card texture: " << cardFiles[i] << endl;
            }
            cardSprites[i].setTexture(cardTextures[i]);
            cardSprites[i].setPosition(CARD_POSITIONS[i]);
            cardSprites[i].setScale(0.1f, 0.1f); // Smaller card size
        }

        // Load upgrade textures (made smaller)
        string upgradeFiles[3] = { "rhp.png", "ihp.png", "im.png" };
        for (int i = 0; i < 3; i++) {
            if (!upgradeTextures[i].loadFromFile(upgradeFiles[i])) {
                cerr << "Failed to load upgrade texture: " << upgradeFiles[i] << endl;
            }
            upgradeSprites[i].setTexture(upgradeTextures[i]);
            upgradeSprites[i].setPosition(UPGRADE_POSITIONS[i]);
            upgradeSprites[i].setScale(0.2f, 0.2f);
        }

        // Load font
        if (!font.loadFromFile("Fonts/American Captain.ttf")) {
            cerr << "Failed to load font!" << endl;
        }

        // Setup price and selection texts
        for (int i = 0; i < 8; i++) {
            priceTexts[i].setFont(font);
            priceTexts[i].setCharacterSize(18); // Slightly smaller text
            priceTexts[i].setFillColor(Color::White);

            selectionTexts[i].setFont(font);
            selectionTexts[i].setCharacterSize(24);
            selectionTexts[i].setFillColor(Color::Yellow);
            selectionTexts[i].setString("[" + to_string(i + 1) + "]");
        }

        // Position price texts further below cards/upgrades
        for (int i = 0; i < 5; i++) {
            priceTexts[i].setPosition(CARD_POSITIONS[i].x, CARD_POSITIONS[i].y + 60); // Pushed down
            selectionTexts[i].setPosition(CARD_POSITIONS[i].x - 30, CARD_POSITIONS[i].y);
        }
        for (int i = 0; i < 3; i++) {
            priceTexts[i + 5].setPosition(UPGRADE_POSITIONS[i].x, UPGRADE_POSITIONS[i].y + 120); // Pushed down
            selectionTexts[i + 5].setPosition(UPGRADE_POSITIONS[i].x - 30, UPGRADE_POSITIONS[i].y);
        }

        // Setup coin display in bottom right
        coinText.setFont(font);
        coinText.setCharacterSize(30);
        coinText.setFillColor(Color::Yellow);
        coinText.setPosition(1000, 650); // Bottom right position
    }

    bool run(Player& player, Deck& deck, RenderWindow& window) {
        // Instruction text
        Text instructions;
        instructions.setFont(font);
        instructions.setCharacterSize(24);
        instructions.setFillColor(Color::White);
        instructions.setString("Press 1-8 to select, ESCAPE to exit");
        instructions.setPosition(50, 600);

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return false;
                }

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Escape) {
                        return true; // Exit shop
                    }

                    // Handle number key presses
                    if (event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num8) {
                        int selection = event.key.code - Keyboard::Num1; // 0-7

                        if (selection < 5) { // Card selection
                            if (player.getCoins() >= prices[selection]) {
                                player.buy(prices[selection]);
                                if (selection < 2 || unlocked[selection]) { // Upgrade
                                    switch (selection) {
                                    case 0: SlashCard::upgrade(); break;
                                    case 1: HealCard::upgrade(); break;
                                    case 2: Inquisition::upgrade(); break;
                                    case 3: DrainCard::upgrade(); break;
                                        // Magicka (4) is not upgradable
                                    }
                                    prices[selection] += 25;
                                }
                                else { // Unlock
                                    unlocked[selection] = true;
                                    int cardsToAdd = (selection == 4) ? 1 : 4;

                                    for (int j = 0; j < cardsToAdd; j++) {
                                        Card* newCard = nullptr;
                                        switch (selection) {
                                        case 2: newCard = new Inquisition(); break;
                                        case 3: newCard = new DrainCard(); break;
                                        case 4: newCard = new MagickaCard(); break;
                                        }
                                        if (newCard) deck.addCard(newCard);
                                    }
                                }
                            }
                        }
                        else if (selection >= 5 && selection < 8) { // Upgrade selection
                            int upgradeIndex = selection - 5;
                            if (player.getCoins() >= upgradePrices[upgradeIndex]) {
                                player.buy(upgradePrices[upgradeIndex]);
                                switch (upgradeIndex) {
                                case 0: // Refill HP
                                    player.heal(player.getMaxHP() - player.getHP());
                                    break;
                                case 1: // Increase HP
                                    player.setMaxHP(player.getMaxHP() + 5);
                                    player.heal(5);
                                    upgradePrices[upgradeIndex] += 20;
                                    break;
                                case 2: // Increase Mana
                                    player.increaseMaxMana(1);
                                    upgradePrices[upgradeIndex] += 25;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // Update price texts
            for (int i = 0; i < 5; i++) {
                if (i < 2 || unlocked[i]) {
                    priceTexts[i].setString("Upgrade: " + to_string(prices[i]) + " coins");
                }
                else {
                    priceTexts[i].setString("Unlock: " + to_string(prices[i]) + " coins");
                }
                priceTexts[i].setFillColor(
                    player.getCoins() >= prices[i] ? Color::White : Color::Red
                );
            }

            // Update upgrade price texts
            for (int i = 0; i < 3; i++) {
                priceTexts[i + 5].setString(to_string(upgradePrices[i]) + " coins");
                priceTexts[i + 5].setFillColor(
                    player.getCoins() >= upgradePrices[i] ? Color::White : Color::Red
                );
            }

            // Update coin display
            coinText.setString("Coins: " + to_string(player.getCoins()));

            // Draw everything
            window.clear(Color::Black);

            // Draw cards
            for (int i = 0; i < 5; i++) {
                window.draw(cardSprites[i]);
                window.draw(priceTexts[i]);
                window.draw(selectionTexts[i]);
            }

            // Draw upgrades
            for (int i = 0; i < 3; i++) {
                window.draw(upgradeSprites[i]);
                window.draw(priceTexts[i + 5]);
                window.draw(selectionTexts[i + 5]);
            }

            // Draw UI elements
            window.draw(crossSprite);
            window.draw(coinText);
            window.draw(instructions);

            window.display();
        }

        return false;
    }
};
class Map {
private:
    RenderWindow& window;
    Player& player;
    Deck& deck;

    struct Node {
        Vector2f position;
        int type; // 0=battle, 1=shop, 2=refill
        bool active;
        bool visited;
        Sprite sprite;
    };

    vector<Node> nodes;
    int currentNode;
    vector<int> currentOptions;

    Texture nodeTextures[3];
    Texture bgTexture;
    Sprite background;
    Font font;

    Text headerText;
    Text nodeInfoText;
    Text healthText;
    RectangleShape healthBox;

    const float NODE_SCALE = 0.3f;
    const Color VISITED_COLOR = Color(150, 150, 150, 200);
    const Color ACTIVE_COLOR = Color::White;
    const Color INACTIVE_COLOR = Color(100, 100, 100, 150);

    void setupNodes() {
        nodes.push_back({
            Vector2f(100, 360), 0, true, false,
            Sprite(nodeTextures[0])
            });

        float secondColX = 250;
        nodes.push_back({
            Vector2f(secondColX, 200), 1, false, false,
            Sprite(nodeTextures[1])
            });
        nodes.push_back({
            Vector2f(secondColX, 520), 0, false, false,
            Sprite(nodeTextures[0])
            });

        nodes.push_back({
            Vector2f(400, 360), 0, false, false,
            Sprite(nodeTextures[0])
            });

        float fourthColX = 550;
        nodes.push_back({
            Vector2f(fourthColX, 200), 0, false, false,
            Sprite(nodeTextures[0])
            });
        nodes.push_back({
            Vector2f(fourthColX, 520), 1, false, false,
            Sprite(nodeTextures[1])
            });

        float fifthColX = 700;
        nodes.push_back({
            Vector2f(fifthColX, 250), 1, false, false,
            Sprite(nodeTextures[1])
            });
        nodes.push_back({
            Vector2f(fifthColX, 470), 2, false, false,
            Sprite(nodeTextures[2])
            });

        nodes.push_back({
            Vector2f(1000, 360), 0, false, false,
            Sprite(nodeTextures[0])
            });

        for (auto& node : nodes) {
            node.sprite.setScale(NODE_SCALE, NODE_SCALE);
            node.sprite.setPosition(node.position);
            node.sprite.setOrigin(node.sprite.getLocalBounds().width / 2,
                node.sprite.getLocalBounds().height / 2);
        }
    }

    void setupUI() {
        headerText.setFont(font);
        headerText.setString("MAGICKA");
        headerText.setCharacterSize(48);
        headerText.setPosition(640 - headerText.getLocalBounds().width / 2, 20);

        nodeInfoText.setFont(font);
        nodeInfoText.setCharacterSize(24);
        nodeInfoText.setPosition(20, 650);

        healthBox.setSize(Vector2f(200, 30));
        healthBox.setPosition(1060, 650);
        healthBox.setFillColor(Color(50, 50, 50, 200));

        healthText.setFont(font);
        healthText.setCharacterSize(24);
        healthText.setPosition(1070, 650);
        updateHealthDisplay();
    }

    void updateHealthDisplay() {
        float healthPercent = (float)player.getHP() / player.getMaxHP();

        if (healthPercent > 0.75f) {
            healthText.setFillColor(Color::Green);
        }
        else if (healthPercent > 0.25f) {
            healthText.setFillColor(Color(255, 165, 0));
        }
        else {
            healthText.setFillColor(Color::Red);
        }

        healthText.setString("HP: " + to_string(player.getHP()) + "/" + to_string(player.getMaxHP()));
    }

    void activateNextNodes(int chosenIndex) {
        if (currentNode >= 0 && currentNode < nodes.size()) {
            nodes[currentNode].visited = true;
        }

        for (auto& node : nodes) {
            node.active = false;
        }

        currentNode = chosenIndex;
        currentOptions.clear();

        switch (chosenIndex) {
        case 0:
            nodes[1].active = true;
            nodes[2].active = true;
            currentOptions = { 1, 2 };
            updateNodeText(2);
            break;
        case 1:
        case 2:
            nodes[3].active = true;
            currentOptions = { 3 };
            updateNodeText(1);
            break;
        case 3:
            nodes[4].active = true;
            nodes[5].active = true;
            currentOptions = { 4, 5 };
            updateNodeText(2);
            break;
        case 4:
        case 5:
            nodes[6].active = true;
            nodes[7].active = true;
            currentOptions = { 6, 7 };
            updateNodeText(2);
            break;
        case 6:
        case 7:
            nodes[8].active = true;
            currentOptions = { 8 };
            updateNodeText(1);
            break;
        default:
            nodes[0].active = true;
            currentOptions = { 0 };
            updateNodeText(1);
            break;
        }
    }

    void updateNodeText(int optionsCount) {
        if (optionsCount == 1) {
            string state;
            switch (nodes[currentOptions[0]].type) {
            case 0: state = "Battle"; break;
            case 1: state = "Shop"; break;
            case 2: state = "Refill Health"; break;
            }
            nodeInfoText.setString("Press ENTER to enter " + state);
        }
        else if (optionsCount == 2) {
            string state1, state2;
            switch (nodes[currentOptions[0]].type) {
            case 0: state1 = "Battle"; break;
            case 1: state1 = "Shop"; break;
            case 2: state1 = "Refill Health"; break;
            }
            switch (nodes[currentOptions[1]].type) {
            case 0: state2 = "Battle"; break;
            case 1: state2 = "Shop"; break;
            case 2: state2 = "Refill Health"; break;
            }
            nodeInfoText.setString("Press 1 for " + state1 + "\nPress 2 for " + state2);
        }
    }

    void handleNodeSelection(int optionIndex) {
        if (optionIndex < 0 || optionIndex >= currentOptions.size()) return;

        int nodeIndex = currentOptions[optionIndex];
        if (!nodes[nodeIndex].active) return;

        switch (nodes[nodeIndex].type) {
        case 0: {
            Battle battle(player, deck, currentNode, window);
            bool battleWon = battle.run();
            if (!player.isAlive()) {
                return; // Player died, handle in run()
            }
            if (battleWon) {
                activateNextNodes(nodeIndex);
            }
            break;
        }
        case 1: {
            Shop shop;
            shop.run(player, deck, window);
            activateNextNodes(nodeIndex);
            break;
        }
        case 2: {
            player.heal(player.getMaxHP() - player.getHP());
            activateNextNodes(nodeIndex);
            break;
        }
        }
        updateHealthDisplay();
    }

    void render() {
        window.clear();
        window.draw(background);

        for (auto& node : nodes) {
            if (node.visited) {
                node.sprite.setColor(VISITED_COLOR);
            }
            else if (node.active) {
                node.sprite.setColor(ACTIVE_COLOR);
            }
            else {
                node.sprite.setColor(INACTIVE_COLOR);
            }
            window.draw(node.sprite);
        }

        window.draw(headerText);
        window.draw(healthBox);
        window.draw(healthText);
        window.draw(nodeInfoText);

        window.display();
    }

public:
    Map(RenderWindow& w, Player& p, Deck& d) : window(w), player(p), deck(d), currentNode(-1) {
        if (!font.loadFromFile("Fonts/American Captain.ttf")) {
            cerr << "Critical: No fonts available!" << endl;
        }

        if (!nodeTextures[0].loadFromFile("Images/Map/iconbat.png") ||
            !nodeTextures[1].loadFromFile("Images/Map/iconshop.png") ||
            !nodeTextures[2].loadFromFile("Images/Map/health_refill.png") ||
            !bgTexture.loadFromFile("Images/Map/map_bg.png")) {
            cerr << "Failed to load map resources!" << endl;
        }
        background.setTexture(bgTexture);

        setupNodes();
        setupUI();
        activateNextNodes(-1);
    }

    ~Map() {}

    int getCurrentNode() const { return currentNode; }

    int run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return 0;
                }

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Enter && currentOptions.size() == 1) {
                        handleNodeSelection(0);
                        if (!player.isAlive()) {
                            return 2; // Defeat
                        }
                        if (currentNode == 8 && currentOptions.size() == 0) {
                            return 1; // Victory (Blue node battle won, no more options)
                        }
                    }
                    else if (event.key.code == Keyboard::Num1 && currentOptions.size() >= 1) {
                        handleNodeSelection(0);
                        if (!player.isAlive()) {
                            return 2; // Defeat
                        }
                        if (currentNode == 8 && currentOptions.size() == 0) {
                            return 1; // Victory
                        }
                    }
                    else if (event.key.code == Keyboard::Num2 && currentOptions.size() >= 2) {
                        handleNodeSelection(1);
                        if (!player.isAlive()) {
                            return 2; // Defeat
                        }
                        if (currentNode == 8 && currentOptions.size() == 0) {
                            return 1; // Victory
                        }
                    }
                }
            }

            render();
        }
        return 0;
    }
};

class Game {
private:
    RenderWindow window;
    Player player;
    Deck deck;
    Map* map;

    Font font;
    Text titleText;
    Text madeByText;
    Text pressEnterText;
    Text victoryText;
    Text victoryPromptText;
    Text defeatText;
    Text defeatOption1Text;

    enum GameState {
        TITLE,
        MAP,
        VICTORY,
        DEFEAT
    };
    GameState currentState;
    int lastNode;

    void setupUI() {
        if (!font.loadFromFile("Fonts/American Captain.ttf")) {
            cerr << "Critical: No fonts available!" << endl;
        }

        titleText.setFont(font);
        titleText.setString("MAGICKA: The Roguelike Deckbuilder Game");
        titleText.setCharacterSize(48);
        titleText.setFillColor(Color(255, 215, 0));
        titleText.setPosition(640 - titleText.getLocalBounds().width / 2, 360 - titleText.getLocalBounds().height / 2);

        madeByText.setFont(font);
        madeByText.setString("Made by: Talha Ahmad");
        madeByText.setCharacterSize(24);
        madeByText.setFillColor(Color::White);
        madeByText.setPosition(20, 20);

        pressEnterText.setFont(font);
        pressEnterText.setString("Press ENTER to continue");
        pressEnterText.setCharacterSize(24);
        pressEnterText.setFillColor(Color::White);
        pressEnterText.setPosition(640 - pressEnterText.getLocalBounds().width / 2, 420);

        victoryText.setFont(font);
        victoryText.setString("VICTORY");
        victoryText.setCharacterSize(72);
        victoryText.setFillColor(Color::Green);
        victoryText.setPosition(640 - victoryText.getLocalBounds().width / 2, 360 - victoryText.getLocalBounds().height / 2);

        victoryPromptText.setFont(font);
        victoryPromptText.setString("Press ENTER to Play Again");
        victoryPromptText.setCharacterSize(24);
        victoryPromptText.setFillColor(Color::White);
        victoryPromptText.setPosition(640 - victoryPromptText.getLocalBounds().width / 2, 450);

        defeatText.setFont(font);
        defeatText.setString("DEFEAT");
        defeatText.setCharacterSize(72);
        defeatText.setFillColor(Color::Red);
        defeatText.setPosition(640 - defeatText.getLocalBounds().width / 2, 360 - defeatText.getLocalBounds().height / 2);

        defeatOption1Text.setFont(font);
        defeatOption1Text.setString("Press 1 to Restart");
        defeatOption1Text.setCharacterSize(24);
        defeatOption1Text.setFillColor(Color::White);
        defeatOption1Text.setPosition(640 - defeatOption1Text.getLocalBounds().width / 2, 450);
    }

    void resetGame() {
        player = Player();
        deck = Deck();
        delete map;
        map = new Map(window, player, deck);
        lastNode = -1;
    }

    void render() {
        window.clear(Color::Black);

        switch (currentState) {
        case TITLE:
            window.draw(titleText);
            window.draw(madeByText);
            window.draw(pressEnterText);
            break;
        case VICTORY:
            window.draw(victoryText);
            window.draw(victoryPromptText);
            break;
        case DEFEAT:
            window.draw(defeatText);
            window.draw(defeatOption1Text);
            break;
        case MAP:
            break;
        }

        window.display();
    }

public:
    Game() : window(VideoMode(1280, 720), "Magicka - The Roguelike Deckbuilder"), currentState(TITLE), lastNode(-1) {
        window.setFramerateLimit(60);
        map = new Map(window, player, deck);
        setupUI();
    }

    ~Game() {
        delete map;
    }

    void run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return;
                }

                if (event.type == Event::KeyPressed) {
                    switch (currentState) {
                    case TITLE:
                        if (event.key.code == Keyboard::Enter) {
                            currentState = MAP;
                        }
                        break;
                    case MAP:
                        break;
                    case VICTORY:
                        if (event.key.code == Keyboard::Enter) {
                            resetGame();
                            currentState = TITLE;
                        }
                        break;
                    case DEFEAT:
                        if (event.key.code == Keyboard::Num1) {
                            player.heal(player.getMaxHP());
                            delete map;
                            map = new Map(window, player, deck);
                            for (int i = -1; i < lastNode; i++) {
                                map->run();
                            }
                            currentState = MAP;
                        }
                        break;
                    }
                }
            }

            switch (currentState) {
            case TITLE:
            case VICTORY:
            case DEFEAT:
                render();
                break;
            case MAP: {
                int status = map->run();
                if (status == 1) {
                    currentState = VICTORY;
                }
                else if (status == 2) {
                    lastNode = map->getCurrentNode();
                    currentState = DEFEAT;
                }
                else if (status == 0) {
                    return;
                }
                break;
            }
            }
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
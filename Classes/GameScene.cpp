#include "GameScene.h"
#include "audio/include/AudioEngine.h"
#include "ui/CocosGUI.h"
#include "GameMenu.h"
USING_NS_CC;

Scene* GameScene::createScene()
{
    Scene* scene = Scene::create();
    auto layerColor = LayerColor::create(Color4B(239, 239, 239, 255));
    auto layer = GameScene::create();
    scene->addChild(layerColor, 1);
    scene->addChild(layer, 2);

    return scene;
}

bool GameScene::init()
{
    if (!Layer::init())
    {
        return false;
    }
    auto listener2 = EventListenerTouchOneByOne::create();
    listener2->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    center_screen_x = origin.x + visibleSize.x / 2;
    center_screen_y = origin.y + visibleSize.y / 2;

    float start_x = origin.x + 20;
    float start_y = origin.y + 20;

    b.randomNow();

    int max_x = b.size.x;
    int max_y = b.size.y;

    Sprite* t = Sprite::create("cell-1-style1.png");
     scale = visibleSize.x / (b.size.x * 1.6) / t->getBoundingBox().size.width;
    t->setScale(scale);
    cellsize = t->getBoundingBox().size;

    Vec2 c00 = getPosByIndex(Vec2(0, 0));
    Vec2 c0m = getPosByIndex(Vec2(0, b.size.y - 1));
    Vec2 cm0 = getPosByIndex(Vec2(b.size.x - 1, 0));

    float diff_x = (cm0.x - c00.x) / 2 - center_screen_x;
    float diff_y = (c0m.y - c00.y) / 2 - center_screen_y;

    spriteSelectIndicator = Sprite::create("cell-select-style1.png");
    spriteSelectIndicator->runAction(RepeatForever::create(RotateBy::create(6, 360)));
    spriteSelectIndicator->setScale(scale);
    spriteSelectIndicator->setColor(Color3B(12, 12, 45));
    spriteSelectIndicator->setOpacity(0);
    this->addChild(spriteSelectIndicator, 2000);

    digButton = ui::Button::create("dig-button.png", "dig-button-pressed.png", "dig-button-disabled.png");
    digButton->setScale(0.6);
    digButton->addClickEventListener(CC_CALLBACK_0(GameScene::dig, this));
    digButton->setPosition(Vec2(origin.x + visibleSize.x - digButton->getBoundingBox().size.width/2  - 20, origin.y + visibleSize.y - digButton->getBoundingBox().size.height * 1));
    this->addChild(digButton);

    disButton = ui::Button::create("flag-button.png", "flag-button-pressed.png", "flag-button-disabled.png");
    disButton->setScale(0.6);
    disButton->addClickEventListener(CC_CALLBACK_0(GameScene::setFlag, this));
    disButton->setPosition(digButton->getPosition() - Vec2(digButton->getBoundingBox().size.width * 1.2, 0));
    this->addChild(disButton);
    flagLabel = Label::createWithTTF(std::to_string(b.flag_left), "fonts/Roboto-Bold.ttf", 22);
    flagLabel->setColor(Color3B(12, 12, 45));
    flagLabel->setPosition(disButton->getPosition() - Vec2(6, 0));
    this->addChild(flagLabel);

    timerBackground = Sprite::create("clock.png");
    timerBackground->setScale(0.6);
    timerBackground->setPosition(Vec2(origin.x + timerBackground->getBoundingBox().size.width / 2 + 20, disButton->getPosition().y));
    this->addChild(timerBackground);
    clockLabel = Label::createWithTTF("READY", "fonts/Roboto-Bold.ttf", 22);
    clockLabel->setColor(Color3B(12, 12, 45));
    clockLabel->setPosition(timerBackground->getPosition());
    this->addChild(clockLabel);

    for (int y = 0; y < max_y;y++) {
        for (int x = 0; x < max_x;x++) {
            if (y % 2 == 1 && x == max_x - 1) continue;
            flagMatrix[x][y] = nullptr;

            Sprite* cell = Sprite::create("cell-1-style2.png");
            cells.push_back(cell);
            cell->setTag(x * 100 + y);
            cellsMatrix[x][y] = cell;
            cell->setScale(scale);

            Vec2 originPos = getPosByIndex(Vec2(x, y));

            Vec2 cellPos = Vec2(originPos.x - diff_x, originPos.y - diff_y);

            cell->setPosition(cellPos);
            this->addChild(cell);

            if (debug) {
                if (b.boardData[x][y] > 0) {
                    Label* label = Label::createWithTTF(std::to_string(b.boardData[x][y]), "fonts/Roboto-Bold.ttf", 20);
                    label->setColor(Color3B(12, 12, 45));
                    this->addChild(label);
                    label->setPosition(cellPos + Vec2(0, 0));
                }

                if (b.boardData[x][y] == -1) {
                    Sprite* mine = Sprite::create("mine-style1.png");
                    mine->setScale(scale);
                    mine->setPosition(cellPos);
                    this->addChild(mine);
                }

                Label* label = Label::createWithTTF(std::to_string(x) + "-" + std::to_string(y), "fonts/Roboto-Bold.ttf", 12);
                label->setColor(Color3B(12, 12, 45));
                this->addChild(label);
                label->setPosition(cellPos - Vec2(0, 5));
            }
        }
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    
    return true;
}


void GameScene::clockTick() {
    second++;
    
    int minute = second / 60;
    int sec = second % 60;

    if (minute == 60 && sec == 0) {
        showYouLose();
        return;
    }

    clockLabel->setString((minute <= 9? "0" : "") + std::to_string(minute) + ":" + (sec <= 9 ? "0" : "") + std::to_string(sec % 60));
}

Vec2 GameScene::getPosByIndex(Vec2 index) {
    float pos_x = ((int)index.y % 2 == 0 ? 0 : cellsize.x * 0.79) + index.x * cellsize.x * 1.6;
    float pos_y = index.y * cellsize.y * 0.55;
    return Vec2(pos_x, pos_y);
}


void GameScene::loadSettings() {
    auto fs = FileUtils::getInstance();
    auto settingPath = fs->getWritablePath() + "/settings.txt";
}

void GameScene::dig() {
    if (touchAtSprite == nullptr) return;

    if (!start) {
        this->schedule(CC_CALLBACK_0(GameScene::clockTick, this), 1, 59 * 60, 0, "clockTick");
        start = true;
    }

    int x = touchAtSprite->getTag() / 100;
    int y = touchAtSprite->getTag() % 100;
    Vec2 sPosition = touchAtSprite->getPosition();

    if (b.hasFlagAt(x, y)) {
        flagMatrix[x][y]->removeFromParentAndCleanup(true);
        flagMatrix[x][y] = nullptr;
        b.removeFlag(x, y);
        updateStats();
        return;
    }

    touchAtSprite = nullptr;
    spriteSelectIndicator->setOpacity(0);
    if (b.hasMineAt(x, y)) {
        // end game
        CCLOG("End game");
        showYouLose();
        return;
    }

    vector<Vec2> al = b.touchAt(Vec2(x, y));
    for (int k = 0; k < al.size(); k++) {
        int tx = (int)al[k].x;
        int ty = (int)al[k].y;

        cellsMatrix[tx][ty]->setOpacity(100);

        if (b.boardData[tx][ty] > 0) {
            Label* label = Label::createWithTTF(std::to_string(b.boardData[tx][ty]), "fonts/Roboto-Bold.ttf", 27);
            label->setScale(scale);
            label->setColor(Color3B(12, 12, 45));
            this->addChild(label);
            label->setPosition(cellsMatrix[tx][ty]->getPosition());
        }

        if (flagMatrix[tx][ty] != nullptr) {
            flagMatrix[tx][ty]->removeFromParentAndCleanup(true);
            flagMatrix[tx][ty] = nullptr;
        }
    }
    checkEndgame();
    updateStats();
}

void GameScene::setFlag() {
    if (touchAtSprite == nullptr) return;
    if (!start) {
        this->schedule(CC_CALLBACK_0(GameScene::clockTick, this), 1, 59 * 60, 0, "clockTick");
        start = true;
    }

    int x = touchAtSprite->getTag() / 100;
    int y = touchAtSprite->getTag() % 100;

    if (b.setFlag(x, y)) {
        updateStats();

        Vec2 sPosition = touchAtSprite->getPosition();
        Sprite* flag = Sprite::create("flag.png");
        flag->setScale(scale);
        flag->setPosition(sPosition);
        this->addChild(flag);

        flagMatrix[x][y] = flag;
        checkEndgame();

    }
}

void GameScene::showYouLose() {
    CCLOG("YOU LOSE");
    b.endGame = true;
    b.youLose = false;
    spriteSelectIndicator->removeFromParentAndCleanup(true);

    digButton->removeFromParentAndCleanup(true);
    disButton->removeFromParentAndCleanup(true);
    flagLabel->removeFromParentAndCleanup(true);
    unschedule("clockTick");

    timerBackground->removeFromParentAndCleanup(true);
 
    Label* youLoseLabel = Label::createWithTTF("YOU LOSE", "fonts/Roboto-Bold.ttf", 22);
    youLoseLabel->setColor(Color3B(12,12,12));
    youLoseLabel->setPosition(center_screen_x, origin.y + visibleSize.y - youLoseLabel->getBoundingBox().size.height);
    this->addChild(youLoseLabel);
    clockLabel->setPositionX(youLoseLabel->getPositionX());
    //clockLabel->setPositionY(youLoseLabel->getPositionY() + youLoseLabel->getBoundingBox().size.height * 2.1);

    ui::Button * newgame = ui::Button::create("new-game.png", "new-game-pressed.png", "new-game.png");
    newgame->setPosition(Vec2(center_screen_x, origin.y + newgame->getBoundingBox().size.height * 0.7));
    newgame->addClickEventListener(CC_CALLBACK_0(GameScene::newgame,this));
    newgame->setScale(0.6);
    this->addChild(newgame);
    for (int x = 0;x < b.size.x;x++) {
        for (int y = 0;y < b.size.y;y++) {
            if (b.hasMineAt(x, y)) {
                cellsMatrix[x][y]->setColor(Color3B(255, 0, 0));
                Sprite* mine = Sprite::create("mine-style1.png");
                mine->setScale(scale);
                mine->setPosition(cellsMatrix[x][y]->getPosition());
                this->addChild(mine);
            }
        }
    }
}
void GameScene::newgame() {
    Director::getInstance()->replaceScene(GameScene::createScene());
}
void GameScene::checkEndgame() {
    if (b.isWin()) {
        CCLOG("YOU WIN");

        b.endGame = true;
        b.youLose = false;
        spriteSelectIndicator->removeFromParentAndCleanup(true);
        digButton->removeFromParentAndCleanup(true);
        disButton->removeFromParentAndCleanup(true);
        flagLabel->removeFromParentAndCleanup(true);
    
        unschedule("clockTick");

        timerBackground->removeFromParentAndCleanup(true);
        Label* endLabel = Label::createWithTTF("YOU WIN", "fonts/Roboto-Bold.ttf", 22);
        endLabel->setColor(Color3B(12, 12, 12));
        endLabel->setPosition(center_screen_x, origin.y + visibleSize.y - endLabel->getBoundingBox().size.height);
        this->addChild(endLabel);

        clockLabel->setPositionX(endLabel->getPositionX());
        //clockLabel->setPositionY(endLabel->getPositionY() + endLabel->getBoundingBox().size.height * 2.1);

        ui::Button* newgame = ui::Button::create("new-game.png", "new-game-pressed.png", "new-game.png");
        newgame->setPosition(Vec2(center_screen_x, origin.y + newgame->getBoundingBox().size.height * 0.7));
        newgame->addClickEventListener(CC_CALLBACK_0(GameScene::newgame, this));
        newgame->setScale(0.6);
        this->addChild(newgame);
    }
}

void GameScene::updateStats() {
    flagLabel->setString(std::to_string(b.flag_left));
}

bool GameScene::onTouchesBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    CCLOG("Touch At: %f, %f", touch->getLocation().x, touch->getLocation().y);
    if (b.endGame) return false;
    Vec2 location = touch->getLocation();
    int total = cells.size();
    touchAtSprite = nullptr;
    spriteSelectIndicator->setOpacity(0);
    for (int i = 0; i < total;i++) {
        Sprite* cell = cells.at(i);
        if (cell->getBoundingBox().containsPoint(location)) {
            int x = cell->getTag() / 100;
            int y = cell->getTag() % 100;

            if (b.isExpaned(x, y)) {
                break;
            }
            
            touchAtSprite = cell;
            break;
        }
    }

    if (touchAtSprite != nullptr) {

        if (!start) {
            this->schedule(CC_CALLBACK_0(GameScene::clockTick, this), 1, 59 * 60, 0, "clockTick");
            start = true;
        }

        spriteSelectIndicator->setOpacity(255);
        spriteSelectIndicator->setPosition(touchAtSprite->getPosition());
        return true;
    }

    return true;
}

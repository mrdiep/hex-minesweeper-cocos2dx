#ifndef __GAME_MAIN_SCENE_H__
#define __GAME_MAIN_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "BoardData.h"
USING_NS_CC;


class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    CREATE_FUNC(GameScene);
private:
    bool debug = false;
    float scale;
    Sprite* spriteSelectIndicator;
    Vec2 cellsize;
    Vec2 visibleSize, origin;
    float center_screen_x, center_screen_y;
    BoardData b;
    std::vector<Sprite*> cells;
    Sprite* touchAtSprite;
    Sprite* cellsMatrix[30][100];
    Sprite* flagMatrix[30][100];
    bool start = false;
    bool onTouchesBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void clockTick();
    Vec2 getPosByIndex(Vec2);
    void loadSettings();
    void dig();
    void setFlag();
    void updateStats();
    void checkEndgame();
    void showYouLose();
    void newgame();
    ui::Button* digButton, * disButton;
    int second = 0;
    Label* flagLabel, * clockLabel;
    Sprite* timerBackground;

};

#endif __GAME_MAIN_SCENE_H__

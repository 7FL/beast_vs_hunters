#ifndef __BEASTVSHUNTERS_SCENE1_H__
#define __BEASTVSHUNTERS_SCENE1_H__

#include "cocos2d.h"

class BeastVsHunters1 : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void menuCloseCallback(cocos2d::Ref* pSender);
    void FillMainArray(int Count, char c);
    void MoveHunters(cocos2d::Touch* touch, cocos2d::Event* event);
    void IncMoves();
    void RunGameOverCutscene();
    void JumpToTrap();
    bool BeastMove1();
    bool BeastMove1_Cant_See_Traps();
    bool BeastMove2();
    bool BeastMove3();
    cocos2d::Vec2 GetVec2XY(int x, int y);
    // implement the "static create()" method manually
    CREATE_FUNC(BeastVsHunters1);
};

#endif // __BEASTVSHUNTERS_SCENE1_H__

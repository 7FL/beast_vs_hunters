#ifndef __BEASTVSHUNTERS_MENU_H__
#define __BEASTVSHUNTERS_MENU_H__

#include "cocos2d.h"

class BeastVsHuntersMenu : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void Play(cocos2d::Ref* pSender);
    void menuCloseCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(BeastVsHuntersMenu);
};

#endif // __BEASTVSHUNTERS_MENU_H__

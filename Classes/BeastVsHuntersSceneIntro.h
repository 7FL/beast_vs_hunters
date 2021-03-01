#ifndef __BEASTVSHUNTERS_SCENE1_H__
#define __BEASTVSHUNTERS_SCENE1_H__

#include "cocos2d.h"

class BeastVsHunters1 : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(BeastVsHunters1);
};

#endif // __HELLOWORLD_SCENE_H__

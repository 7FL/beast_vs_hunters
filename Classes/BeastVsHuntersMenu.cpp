// �������� ������� - ���� "������ ����� � �������"
// ���� 2021 (C) 7FL (FL@MAIL.RU, github.com/7FL)

#include "BeastVsHuntersMenu.h"
#include "BeastVsHuntersSc1.h"

USING_NS_CC;

// ���������� ��������� � ����������
const int   BH_SPRITE_DIM = 128;        // ����������� ������ � ���� 128x128 �����
const float BH_MENU_SPRITE_SCALE = 0.5; // �������
const float BH_MENU_INT = 40;           // �������� ����� �������� � ����
const float BH_MENU_SPRITE_INT = 10;    // �������� ����� ��������� (������� ���� ����)
const int   BH_MENU_MARGIN_Y = 15;      // ����� ��������� � ������ ������� ����� ������ �� XML/JSON ��� �� ���������!

Scene* BeastVsHuntersMenu::createScene()
{
    return BeastVsHuntersMenu::create();
}

// Print useful error message instead of segfaulting when files are not there
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BeastVsHuntersSceneIntro.cpp\n");
}

// On "init" you need to initialize your instance
bool BeastVsHuntersMenu::init()
{
    // ����� �����������
    if (!Scene::init())     // super init first
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    // ---------------------------------------------------------------------------------------------------------

    // ������� ��������� ����
    auto Label1 = Label::createWithTTF(u8"���� \"������ ����� � �������\" ver 1.0", "fonts/arial.ttf", 26);
    float lb_x = origin.x + visibleSize.width / 2; // �������� ������
    float lb_y = origin.y + visibleSize.height;  // ������� ����� Label, ������� ���� � ��� ������� ������ ����
    if (Label1 == nullptr)
        problemLoading("'fonts'");
    else
    {
        lb_y -= BH_MENU_MARGIN_Y;
        Label1->setPosition(Vec2(lb_x, lb_y));
        Label1->setHorizontalAlignment(TextHAlignment::CENTER);
        Label1->setAnchorPoint(Vec2(0.5, 1));
        this->addChild(Label1, 1);
    }
    // ---------------------------------------------------------------------------------------------------------

    // ������� ������� ����
    auto MenuItem1 = MenuItemFont::create(u8"[ ������ ]", CC_CALLBACK_1(BeastVsHuntersMenu::Play, this));
    auto MenuItem2 = MenuItemFont::create(u8"[ ����� ]", CC_CALLBACK_1(BeastVsHuntersMenu::menuCloseCallback, this));

    lb_y -= MenuItem1->getContentSize().height + BH_MENU_INT;
    MenuItem1->setFontSize(30);
    MenuItem1->setPosition(Vec2(lb_x, lb_y));
    MenuItem1->setColor(Color3B::GREEN);
    lb_y -= MenuItem1->getContentSize().height + BH_MENU_INT;

    MenuItem2->setPosition(Vec2(lb_x, lb_y));
    MenuItem2->setColor(Color3B::GREEN);
    auto GameMenu = Menu::create(MenuItem1, MenuItem2, NULL);
    GameMenu->setPosition(Vec2::ZERO);
    this->addChild(GameMenu, 1);
    lb_y -= BH_MENU_SPRITE_INT;
    // ---------------------------------------------------------------------------------------------------------

    // ��������� ��� �������
    // ����� �������� ����������� � ���� ������, ����� ����� ����������, ����� ������ �� xml/json
    std::vector<std::string> spriteNames = { "beast128.png", "hunter128.png", "trap128.png", "wall128.png", "bush128.png", "flags128.png" };

    for (unsigned int n = 0; n < spriteNames.size(); n++)
    {
        auto sprite1 = Sprite::create(spriteNames.at(n));

        if (sprite1 == nullptr)
            problemLoading(spriteNames.at(n).c_str()); // char*
        else
        {
            lb_y -= sprite1->getContentSize().height * BH_MENU_SPRITE_SCALE + BH_MENU_SPRITE_INT;
            Vec2 v2 = Vec2(BH_SPRITE_DIM / 2, lb_y);
            sprite1->setPosition(v2);
            sprite1->setScale(BH_MENU_SPRITE_SCALE);    // ������ ������� ������� � ����
            this->addChild(sprite1, 0);
            auto Label2 = Label::createWithTTF(u8"-", "fonts/arial.ttf", 18);
            std::string L2Text;
            if (n == 0) 
                L2Text = u8"�����, �������� ���������� ���� ������� � �������,\n���� ������ ����������� ���������";
            if (n == 1)
                L2Text = u8"���� �� ���������, �������� �� ������ ��������\n�����, ������, ���� ��� �����";
            if (n == 2)
                L2Text = u8"�������, � ������� ����� ������� �����";
            if (n == 3)
                L2Text = u8"�����, ������������ �� ��� ���������, �� ��� �����";
            if (n == 4)
                L2Text = u8"�����, ������� ����� ��������� �����, �� �� ����� �������";
            if (n == 5)
                L2Text = u8"������, ������� ����� ��������� �������, �� �� ����� �����";
            if (Label2 != nullptr)
            {
                Label2->setString(L2Text);
                Label2->setAnchorPoint(Vec2(0, 0.5));
                Label2->setPosition(Vec2(v2.x + BH_SPRITE_DIM / 2, v2.y));
                //Label2->setHorizontalAlignment(TextHAlignment::LEFT);
                //Label2->setVerticalAlignment(TextVAlignment::CENTER);
                this->addChild(Label2, 1);
            }
        }
        
    }
    
    return true;
}

// ����� ����� ����, ���� ���� �����, �� ����� ������ �������� ���� ����� �������� (��� Director::getInstance()), ������ ����� �����������
void BeastVsHuntersMenu::Play(Ref* pSender)
{
    auto scene1 = BeastVsHunters1::createScene();
    //Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene1, Color3B(255, 255, 255)));
    Director::getInstance()->pushScene(TransitionFade::create(0.5, scene1, Color3B(255, 255, 255)));
}

void BeastVsHuntersMenu::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    // To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}

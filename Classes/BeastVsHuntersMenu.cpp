// Тестовое задание - игра "Загони зверя в ловушку"
// Март 2021 (C) 7FL (FL@MAIL.RU, github.com/7FL)

#include "BeastVsHuntersMenu.h"
#include "BeastVsHuntersSc1.h"

USING_NS_CC;

// Глобальные константы и переменные
const int   BH_SPRITE_DIM = 128;        // стандартные спайты в игре 128x128 точек
const float BH_MENU_SPRITE_SCALE = 0.5; // масштаб
const float BH_MENU_INT = 40;           // интервал между строками в меню
const float BH_MENU_SPRITE_INT = 10;    // интервал между спрайтами (которые ниже меню)
const int   BH_MENU_MARGIN_Y = 15;      // ОБЩИЕ НАСТРОЙКИ В РАЗНЫХ КЛАССАХ ПОТОМ ЧИТАТЬ ИЗ XML/JSON ИЛИ ИЗ СИНГЛТОНА!

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
    // Общая инициалиция
    if (!Scene::init())     // super init first
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    // ---------------------------------------------------------------------------------------------------------

    // Выводим заголовок игры
    auto Label1 = Label::createWithTTF(u8"Игра \"Загони зверя в ловушку\" ver 1.0", "fonts/arial.ttf", 26);
    float lb_x = origin.x + visibleSize.width / 2; // середина экрана
    float lb_y = origin.y + visibleSize.height;  // выводим текст Label, игровое меню и все спрайты сверху вниз
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

    // Выводим игровое меню
    auto MenuItem1 = MenuItemFont::create(u8"[ ИГРАТЬ ]", CC_CALLBACK_1(BeastVsHuntersMenu::Play, this));
    auto MenuItem2 = MenuItemFont::create(u8"[ ВЫХОД ]", CC_CALLBACK_1(BeastVsHuntersMenu::menuCloseCallback, this));

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

    // Добваляем все спрайты
    // Имена спрайтов назначаются в коде класса, затем можно переделать, чтобы читать из xml/json
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
            sprite1->setScale(BH_MENU_SPRITE_SCALE);    // задаем масштаб спрайта в меню
            this->addChild(sprite1, 0);
            auto Label2 = Label::createWithTTF(u8"-", "fonts/arial.ttf", 18);
            std::string L2Text;
            if (n == 0) 
                L2Text = u8"Зверь, которого необходимо либо загнать в ловушку,\nлибо лишить возможности двигаться";
            if (n == 1)
                L2Text = u8"Один из охотников, которого Вы можете пемещать\nвлево, вправо, вниз или вверх";
            if (n == 2)
                L2Text = u8"Ловушка, в которую можно загнать зверя";
            if (n == 3)
                L2Text = u8"Стена, непроходимая ни для охотников, ни для зверя";
            if (n == 4)
                L2Text = u8"Кусты, которые может проходить зверь, но не может охотник";
            if (n == 5)
                L2Text = u8"Флажки, которые может проходить охотник, но не может зверь";
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

// Выбор сцены игры, если сцен много, то нужно делать менеджер сцен через синглтон (как Director::getInstance()), только более продвинутый
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

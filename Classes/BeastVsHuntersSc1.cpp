// �������� ������� - ���� "������ ����� � �������"
// ���� 2021 (C) 7FL (FL@MAIL.RU, github.com/7FL)

#include "BeastVsHuntersSc1.h"

USING_NS_CC;

// ���������� ��������� � ����������
// ����� ����� �������� ����� ���������� ��� ���������� ��� ������ �� � ���. ����� "Settings" � ������� � �����

// =============== ������� ������ �� �������� ��������� ������� ===========================================
const int BH_FIELD_X = 21;                  // ����������� ���� �� �����������, ������ [0,0] ����� ������ ������� �������
const int BH_FIELD_Y = 15;                  // ����������� ���� �� ���������
const int BH_MAX_HUNTERS = 10;              // ���������� ���������
const int BH_MAX_TRAPS = 10;                // ���������� �������

const bool BH_BEAST_CAN_SEE_TRAPS = true;  // ����� �� ����� ������ �������, ��������� ���������� ������� �� ��������� �����!

const int BH_MAX_WALLS = 8;                 // ���������� ����
const int BH_MAX_FLAGS = 8;                 // ���������� �������
const int BH_MAX_BUSHES = 8;                // ���������� ������
// =============== ������� ������ �� �������� ��������� ������� ===========================================

const std::string bst_can_see = "hwf";  // ������� � ������� ������� �������, ������� ����� ����� ��� BH_BEAST_CAN_SEE_TRAPS = false
                                        // ��� ��� ������, ��� ��� ����� ���� ����� ����� ��� �� ������ ������!

const int BH_MARGIN_X = 15;             // ������ �� X �� ���� ������� ������� ��� ��������� �����
const int BH_MARGIN_Y = 15;             // ������ �� Y �� ���� ������� ������� ��� ��������� �����
const int BH_SPRITE_DIM = 128;          // ����������� ������ � ���� 128x128 �����
                                        // ����� ����� ������������ ������� ����������� �������
                                        // ��������� ������������ ������ �� ������� ������ ���������� � BH_FIELD_X/Y
float BH_CELL;                          // ��������� ������ 1 ������ �� ������, ��������� ��� ������ ����������, ��� � �������
float BH_SPRITE_SCALE;                  // ��������� ������� �������

//bool HuntersMove = true;              // ���������� ��� �������� ���� ���������/�����
bool GameOver;                          // true ��� ��������� �����, ��� ����� ���� � �������� �� ���� ���������
Vec2 touchTMPVec;                       // ��� ��������� ��������������� ������� (��������� ��� ��������� ��� ������������)

cocos2d::Sprite* pBeast_spr;            // ��������� �� ������ ����� ��� ��������� �� back-end ����
int b_x, b_y;                           // ���������� ���������� ����� � ������� �������, ������� �����

int TotalMove;                          // ������� ����� ����������, ������� ��� ������������ � ��������
std::string L1Text;                     // ������� �������� ���� � ���������� ����� ������ ������
cocos2d::Label* ptrLabel1;

Size visibleSize;                       // �������� ���������� - ������� �����
Vec2 origin;                            // �������� ���������� - 0,0

// ������� ������� ������, �������� �������������� ������������ ��������,
// . = ��������� ������, b = �����, h = �������, t = �������, w = ����� (����� �� ��������), 
// k = ���� (�������� �����), f = ������ (�������� �������), � �.�.
// main array [0][0] - ����� ������� ����� ������
char ma[BH_FIELD_X][BH_FIELD_Y];

/* �������� ������ (����������)
const char ma_const[BH_FIELD_Y][BH_FIELD_X] = {
{'.', 'h', '.', 'h', '.', '.', '.', 'h',},
{'h', '.', 'h', '.', '.', 'f', '.', '.',},
{'.', '.', '.', '.', '.', '.', '.', '.',},
{'.', '.', '.', '.', '.', '.', '.', '.',},
{'.', 't', '.', '.', 'w', 'b', '.', '.',},
{'.', 'k', '.', 'k', '.', '.', '.', '.',},
};*/

const std::string ma_allow_chars = "bhtwkf";          // ���������� ������� � ������� ������� ������� (��� ��������)
enum BMovesEnum { up, down, left, right };            // ��������� ���� ����� ��� ����������� ������� ��������� �����

//const std::string beast_allow_chars = ".k";         // ���������� ������� � ������� ��� ������ �����
//const std::string hunters_allow_chars = ".f";       // ���������� ������� � ������� ��� ������ ���������
// ����� ��� ���������� ������� ���� ���� ����� ��������� ������� �� "�����������" � "������������"
// ����� � ������������ ������� ������ ��������� ����� � ���������, � � ����������� - �������, �����, �����, ������
// ��� ����������� ����, ����� ������ �������� ����� ��� ��������
// ������� �� ���� �������� ����� ����������� ��� ���������� � �������

// ������ ����� (beast memory) ��� ���������, ����� ����� �� ����� �������, �����
// ������������ ��� ������ ���� �������� ��� �������, � ������� ��� ���, ��� ��� ��� ��� �������
char bm[BH_FIELD_X][BH_FIELD_Y]; 

Scene* BeastVsHunters1::createScene()
{
    return BeastVsHunters1::create();
}

// ������ ��������� �� ������, ������ ����� �� �������� � ��������
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BeastVsHuntersSceneIntro.cpp\n");
}

// ������������� �����
bool BeastVsHunters1::init()
{
    // ����� �����������
    if (!Scene::init())     // super init first
    {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    // ---------------------------------------------------------------------------------------------------------

    // ��� ���������� ���������� ������ ���� ���������������� ��� � Init, ����� ����������� �������� ����� ����!
    TotalMove = 0;
    GameOver = false;

    // ������������� �������� ������� � ������� ������ �����
    for (int x = 0; x < BH_FIELD_X; x++)
        for (int y = 0; y < BH_FIELD_Y; y++)
        {
            ma[x][y] = '.'; // ������ ������ �������� �������
            bm[x][y] = '?'; // ������������� ������ ������� ������ ����� ('+' = ���������)
        }

    FillMainArray(BH_MAX_HUNTERS, 'h');         // ������������� �������� ������� - ��������
    FillMainArray(1,              'b');         // ������������� �������� ������� - �����, �� ������ ����
    FillMainArray(BH_MAX_TRAPS,   't');         // ������������� �������� ������� - �������
    FillMainArray(BH_MAX_WALLS,   'w');         // ������������� �������� ������� - �����
    FillMainArray(BH_MAX_FLAGS,   'f');         // ������������� �������� ������� - ������
    FillMainArray(BH_MAX_BUSHES,  'k');         // ������������� �������� ������� - �����

    // ---------------------------------------------------------------------------------------------------------

    // ��������� ������ "�����" ��� ����
    auto closeItem = MenuItemImage::create("arrowLeft50.png", "arrowLeft50.png",
        CC_CALLBACK_1(BeastVsHunters1::menuCloseCallback, this));
    if (closeItem == nullptr || closeItem->getContentSize().width <= 0 || closeItem->getContentSize().height <= 0)
        problemLoading("'arrowLeft50.png'");
    else
    {
        float x = origin.x + /*visibleSize.width*/ + closeItem->getContentSize().width / 2;
        float y = origin.y + visibleSize.height - closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    // ---------------------------------------------------------------------------------------------------------
    
    
    // ��������� � Label1 ���������, ���. ������� utf - u8
    auto Label1 = Label::createWithTTF(u8"������� �����: 0", "fonts/arial.ttf", 26);
    ptrLabel1 = Label1;
    float lb_x = origin.x + visibleSize.width / 2; // �������� ������
    float lb_y = origin.y + visibleSize.height;  // ������� ����� Label, ������� ���� � ��� ������� ������ ����
    if (Label1 == nullptr)
        problemLoading("'fonts'");
    else
    {
        lb_y -= BH_MARGIN_Y;
        Label1->setPosition(Vec2(lb_x, lb_y));
        Label1->setHorizontalAlignment(TextHAlignment::CENTER);
        Label1->setAnchorPoint(Vec2(0.5, 1));
        this->addChild(Label1, 1);
        lb_y -= Label1->getContentSize().height;
    }
    // ---------------------------------------------------------------------------------------------------------

    // ��������� ������� ������� ��� �������� ����, ���������� lb_y ��������� �� �������� ���������
    float x_bottom = (BH_MARGIN_X)-2;
    float y_bottom = (BH_MARGIN_Y)-2;
    float x_top = (origin.y + visibleSize.width - BH_MARGIN_X) + 2;
    float y_top = (lb_y - BH_MARGIN_Y) + 2;

    // ��� ������� � ������� �������� ������� ������ ������ ����� ������������ ������� ���� �� ������� ����� ������
    /*auto drawFullSpritesField = DrawNode::create();
    Vec2 verticsFSF[4] = {Vec2(x_bottom, y_bottom),
                          Vec2(x_bottom, y_top   ),
                          Vec2(x_top   , y_top   ),
                          Vec2(x_top   , y_bottom)};
    drawFullSpritesField->drawPolygon(verticsFSF, 4, Color4F::BLACK, 1, Color4F::BLUE);
    this->addChild(drawFullSpritesField, -2);*/

    // ��������� ������ ������� ������ � ������� �������� ��� ����������� ������� ���������� ������
    // � ���������� x_bottom, y_bottom, x_top, y_top ������ � ��� ���������� ������� ��� ��������� ������ �������� ����
    // �������� ������ ������ (�������) � ������� ��������, ����� ���� ����������� ������ ����������
    float x_cell = ((x_top - x_bottom) / static_cast<float>(BH_FIELD_X));
    float y_cell = ((y_top - y_bottom) / static_cast<float>(BH_FIELD_Y));
    BH_CELL = x_cell < y_cell ? x_cell : y_cell; // ��� ������� ���������� ������ ����� ����������� ������, ����� ����� � �������
    BH_SPRITE_SCALE = (BH_CELL) / static_cast<float>(BH_SPRITE_DIM);  // ��������� ������� ������� ��� ������

    // ������ ����� - ���� �� ���������
    auto drawNodeGrid = DrawNode::create();
    for (int x = 0; x < BH_FIELD_X; x++) {
        for (int y = 0; y < BH_FIELD_Y; y++) {
            Vec2 vertics[4] = {
                Vec2(GetVec2XY(x,y).x - BH_CELL / 2, GetVec2XY(x,y).y - BH_CELL / 2),
                Vec2(GetVec2XY(x,y).x + BH_CELL / 2, GetVec2XY(x,y).y - BH_CELL / 2),
                Vec2(GetVec2XY(x,y).x + BH_CELL / 2, GetVec2XY(x,y).y + BH_CELL / 2),
                Vec2(GetVec2XY(x,y).x - BH_CELL / 2, GetVec2XY(x,y).y + BH_CELL / 2)};
            drawNodeGrid->drawPolygon(vertics, 4, Color4F::BLACK, 1, Color4F::GRAY);
        }
    }
    this->addChild(drawNodeGrid, -1);
    // ---------------------------------------------------------------------------------------------------------

    // ��������� ��� �������!
    // ���� ������� �� ������������� �������� �������, ��� �������, ���� ������ ����� �������� �� xml/json
    // std::vector<cocos2d::Sprite*> sprites_arr; // ������ ���� ��������, ����� ����������� � �������
    // ����� �������� ����������� � ���� ������, ����� ����� ����������, ����� ������ �� xml/json
    std::vector<std::string> spriteNames = { "beast128.png", "hunter128.png", "trap128.png", "wall128.png", "bush128.png", "flags128.png" };
    std::vector<char> spriteChars =        { 'b',            'h',             't',           'w',           'k',           'f'            };
    for (int x = 0; x < BH_FIELD_X; x++)
    {
        for (int y = 0; y < BH_FIELD_Y; y++)
        {
            if (ma[x][y] == '.')
                continue;
            if (ma_allow_chars.find(ma[x][y]) != std::string::npos) // ���������, ��� ������ �� ma � ����������� ��������
            {
                // � idx ��������� ������� ������� ma[x][y] �� �������� � ������� ������� � 0
                // ��� ������������� ������� �� ������� ������ � ������ ����� �������
                auto iter = std::find(spriteChars.begin(), spriteChars.end(), ma[x][y]);
                size_t idx;
                if (iter != spriteChars.end())
                    idx = iter - spriteChars.begin();
                else
                    continue; // ��� ������� �� ��������, ��������� ����������� if, �� �� ����� ��������� � ����� ������

                auto sprite1 = Sprite::create(spriteNames.at(idx));  // ����� ��� ����� ������� �� ������� � �������
                if (sprite1 == nullptr) // 
                    problemLoading(spriteNames.at(idx).c_str()); // char*
                else
                {
                    sprite1->setPosition(GetVec2XY(x, y)); // ������������� ����� ������� �� ������� �����
                    sprite1->setScale(BH_SPRITE_SCALE);    // ������ ������� �������
                    //sprite1->setAnchorPoint(Vec2(0.5, 0.5)); // "�������" ����� ������� - 0,0 - ����� �����
                    //sprite1->setTag(n); // ���� ����� ��� �����, ����� ����� ������������ ��

                    // ����� ��������� ��������� ������� ������, �� ������ ��� ����������� �������� ���������!
                    if (ma[x][y] == 'h') // h - �������
                    {
                        auto touchListener = EventListenerTouchOneByOne::create();
                        touchListener->setSwallowTouches(true); // ��� "�������������" �������, ����� ������� true � onTouchBegan()

                        // ������-������� - ������ �����������
                        touchListener->onTouchBegan = [=](Touch* touch, Event* event) -> bool {
                        auto bounds = event->getCurrentTarget()->getBoundingBox();
                        // ���� ������ ��� ��������� � ������ ��� ������� ������� (����� ����� �������� �� ����� ����� ������)
                        if (/*HuntersMove &&*/ !GameOver && bounds.containsPoint(touch->getLocation())) {
 
                            // ��� �������
                            //std::stringstream touchDetails;
                            //touchDetails << "Touched at OpenGL coordinates: " <<
                            //    touch->getLocation().x << "," << touch->getLocation().y << std::endl <<
                            //    "Touched at UI coordinate: " <<
                            //    touch->getLocationInView().x << "," << touch->getLocationInView().y << std::endl <<
                            //    "Touched at local coordinate:" <<
                            //    event->getCurrentTarget()->convertToNodeSpace(touch->getLocation()).x << "," <<
                            //    event->getCurrentTarget()->convertToNodeSpace(touch->getLocation()).y << std::endl <<
                            //    "Touch moved by:" << touch->getDelta().x << "," << touch->getDelta().y;
                            //cocos2d::log(touchDetails.str().c_str());
  
                            auto target = static_cast<Sprite*>(event->getCurrentTarget());
                            touchTMPVec = target->getPosition(); // ���������� �������������� ������� �������
                            return true; // ��������� ������ ������ ��� ����� �������
                        } else
                            return false; // ����� �� ��������� ������
                        };
 
                        // �� ����� �����������
                        touchListener->onTouchMoved = [](Touch* touch, Event* event) 
                        {
                            auto target = static_cast<Sprite*>(event->getCurrentTarget());
                            // ���������� ������ ��� ����������� �� ����� �������� ������ �� ������
                            target->setPosition(target->getPosition() + touch->getDelta());
                        };
                        
                        // ��������� �����������
                        touchListener->onTouchEnded = [=](Touch* touch, Event* event)
                        {
                            MoveHunters(touch, event); // �������� ��������
                        };
                        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, sprite1);
                    }

                    // sprites_arr.push_back(sprite1); // �� ������� ����������� ��������� ��� ������� � ����� ������
                    if (ma[x][y] == 'b') {
                        pBeast_spr = sprite1;   // ��� ������������� ��� ����� ���������� ������ � ptr, ������� �� ������ ������
                        b_x = x;                // � ����� ���������� ����� � ma[x][y]
                        b_y = y;
                    }
                    if (ma[x][y] == 'h')
                        this->addChild(sprite1, 2);     // ��������� ��������� ������ � ������ ����� Node
                    else
                        if (ma[x][y] == 'b')
                            this->addChild(sprite1, 1);
                        else
                            this->addChild(sprite1, -1);
                }
            }
        }
    }
    return true; // Init()
}

// ��������� ���������� �������� �������� ������� �������� � char � ����������� � int
// ��������� ������ ������� �� ������������� ���� �������� �������� � ��� ����� ������� ���������� �������
void BeastVsHunters1::FillMainArray(int Count, char c)
{
    unsigned int MaxLoopCount = BH_FIELD_X * BH_FIELD_X * 50; // ���������� ������ ��� ������ �� while, ���� ������� ������ ����� �������
    unsigned int LoopCount = 0;                               // c������ ����� ���������� ������ while, ����� ���� ����� �� ���� �����������
    int i = 0;                                       // ������� ����������, ��� ��������� � �������� Count
    srand((unsigned int)time(NULL));
    while (i < Count && LoopCount < MaxLoopCount)
    {
        int x = rand() % BH_FIELD_X;
        int y = rand() % BH_FIELD_Y;
        if (ma[x][y] == '.') // ���� �����, �� ���������
        {
            ma[x][y] = c;
            i++;  // ����������� ������� ������� ����������
        }
        LoopCount++;
    }
}

// ������������ �����, ���� ���� �����, �� ����� ������ �������� ���� ����� �������� (��� Director::getInstance()), ������ ����� �����������
void BeastVsHunters1::menuCloseCallback(Ref* pSender)
{
    // auto scene1 = BeastVsHuntersMenu::createScene();
    // Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene1, Color3B(255, 255, 255)));
    Director::getInstance()->popScene();
}

// ������� ������ ��� Label1 � ����������� ���������� ��������� �����
void BeastVsHunters1::IncMoves()
{
    L1Text = u8"������� �����: " + std::to_string(TotalMove++);
    if (ptrLabel1 != nullptr)
        ptrLabel1->setString(L1Text);
}

// �������� ������ �������� ������� � ����������� ���������� (� ������ ������� ������)
Vec2 BeastVsHunters1::GetVec2XY(int x, int y) 
{
    return Vec2(BH_MARGIN_X + BH_CELL / 2 + BH_CELL *                  (float)(x),
                BH_MARGIN_Y + BH_CELL / 2 + BH_CELL * (float)(BH_FIELD_Y - 1 - y)); // ������� �� y
}

// ������� �������� ��������� ��� ��������� ���� (�����������)
void BeastVsHunters1::MoveHunters(Touch* touch, Event* event)
{
    auto target = static_cast<Sprite*>(event->getCurrentTarget());
    Vec2 touchTMPVecNew = target->getPosition() + touch->getDelta(); // ������� ������� ����� �����������

    // ���������� ���������� ������� � ������������ ����� 2-� ������� ������� � ����� int 1
    int m_x_perv = static_cast<int>((touchTMPVec.x - BH_MARGIN_X) / BH_CELL);
    int m_y_perv = BH_FIELD_Y - 1 - static_cast<int>((touchTMPVec.y - BH_MARGIN_Y) / BH_CELL); // �������
    // ��������� ����� ������ 2-� ������� ������� (���� ����) ���� ������� ��� ������
    int m_x;
    if (touchTMPVecNew.x < BH_MARGIN_X || // ��������, ��� ������ �� ���������� �� ������� ������� �����
        static_cast<int>((touchTMPVecNew.x - BH_MARGIN_X) / BH_CELL) > BH_FIELD_X - 1)
    {
        m_x = 7777; /// 7777 - ��� ������ ��� ����, ����� ������������ ���� ���� �� ���������
    }
    else
    {
        // ������������� ������ �� X, ��� static_cast<int> ����� float, ������ ���������� ������� �����!
        m_x = static_cast<int>((touchTMPVecNew.x - BH_MARGIN_X) / BH_CELL);
    }
    int m_y;
    if (touchTMPVecNew.y < BH_MARGIN_Y || // ��������, ��� ������ �� ���������� �� ������� ������� �����
        static_cast<int>((touchTMPVecNew.y - BH_MARGIN_Y) / BH_CELL) > BH_FIELD_Y - 1)
    {
        m_y = 7777; /// 7777 - ��� ������ ��� ����, ����� ������������ ���� ���� �� ���������
    }
    else
    {
        // ������������� ������ �� X, ��� static_cast<int> ����� float, ������ ���������� ������� �����!
        m_y = BH_FIELD_Y - 1 - static_cast<int>((touchTMPVecNew.y - BH_MARGIN_Y) / BH_CELL); // ������������� ������ �� y
    }
    // ������� ���������, ��� �� �� ����� �������� �� ������� ����
    if (m_x == 7777 || m_y == 7777 ||
        // ����� ���������, ��� ������ ������ �����, ��� �� 1 ������ ������, �����, ���� ��� �����
        abs(m_x - m_x_perv) + abs(m_y - m_y_perv) > 1 ||
        // � �����, ��� �� �� ����� �� ����� � ��� �� �����
        (m_x == m_x_perv && m_y == m_y_perv) ||
        // � ������ ��������� ��� �� �� ������� �� ����� ������
        (ma[m_x][m_y] != '.' && ma[m_x][m_y] != 'f')  // ������ ������ ��� ������ 'f' - ����� ��� ����� ������ �������!
        )
    {
        target->setPosition(touchTMPVec); // ���������� ������ �������
    }
    else
    {
        // ������� ������ �� ������, ������ �� ������������� ������������� �� ������� ����� ������� ma[][]
        target->setPosition(GetVec2XY(m_x, m_y));
        if (ma[m_x_perv][m_y_perv] != 'f')
            ma[m_x_perv][m_y_perv] = '.'; // ���������� ����� �����������  ��������� � ������!!!
        if (ma[m_x][m_y] != 'f')
            ma[m_x][m_y] = 'h';           // � �� ����� ����� ������ ������� ��������
        // HuntersMove = false; // �������� ��� ����� // ���� �� ���������� ��� �����

        // ��� ������� ���������� (touch) � ������
        // std::string s = "Event point: " +
        //    std::to_string(touchTMPVecNew.x) + ", " + std::to_string(touchTMPVecNew.y) + "\n" +
        //   "Cell x = " + std::to_string(m_x) + "; y = " + std::to_string(m_y);
        //cocos2d::log(s.c_str());

        IncMoves(); // ����������� ������� ����� � ��������� ������� ������ � �� �����������

        // ����� �������� ����� ������ ��������� �������� ��� �����,
        // �������� ��������� ����� BeastMove1,2,3... � ����� �� ����� �������
        bool MoveIsDone;
        if (BH_BEAST_CAN_SEE_TRAPS)
            MoveIsDone = BeastMove1();
        else
            MoveIsDone = BeastMove1_Cant_See_Traps();
        
        if (!MoveIsDone)
        {
            // ���� ����� �� ���� ������� ���� ���, �� �� ��������
            GameOver = true; // ��������� ���������� ���� ������
            
            // ��������� ������� ��� �������� �� GameOver
            RunGameOverCutscene(); 
        }
    }
}

// ��������� ������� ����� ��� ������ ����� ����� ��� ������ ������
void BeastVsHunters1::RunGameOverCutscene()
{
    L1Text = u8"�� ��������!!! ������������ �����: " + std::to_string(TotalMove);
    ptrLabel1->setColor(Color3B::GREEN);
    if (ptrLabel1 != nullptr)
        ptrLabel1->setString(L1Text);
}

// ��������� ������ � �������, ���� �� ��������� ����� - �� � ������ ����������
void BeastVsHunters1::JumpToTrap()
{
    bool NeedToJump = false; // ���� �� �������
    Vec2 JumpVec; // ������ ���������� ������ � �������
    if (b_y > 0 && ma[b_x][b_y - 1] == 't') // ��������� ������� ������� ������ (�����, �� ������� �������)
    {
        JumpVec = Vec2(0.0, BH_CELL);
        NeedToJump = true;
    }
    if (b_x < BH_FIELD_X - 1 && ma[b_x + 1][b_y] == 't') // �������� ������
    {
        JumpVec = Vec2(BH_CELL, 0.0);
        NeedToJump = true;
    }
    if (b_y < BH_FIELD_Y - 1 && ma[b_x][b_y + 1] == 't') // �������� �����
    {
        JumpVec = Vec2(0.0, -BH_CELL);
        NeedToJump = true;
    }
    if (b_x > 0 && ma[b_x - 1][b_y] == 't') // �������� �����
    {
        JumpVec = Vec2(-BH_CELL, 0.0);
        NeedToJump = true;
    }

    if (NeedToJump) // ���� ���� ������� - ������� �������� � ������, ����� � ��������� Cocos2d-x
    {
        auto moveBy1 = MoveBy::create(1.5f, JumpVec);
        auto fadeOut1 = FadeOut::create(3.0f);            // �������� �������� � �������
        auto sequence1 = Sequence::create(moveBy1, fadeOut1, nullptr);
        pBeast_spr->runAction(sequence1);
    }
}

// ����� �������� ����� pBeast_spr � ������������ b_x, x_y
// ��������� �1 - ��������� �����������
// ����� ����������: true = ����� ���� ������� ���; false = ������������� ��� ����� � ������� = game over
bool BeastVsHunters1::BeastMove1()
{
    int b_x_prev = b_x; // ������ ���������� ���������� �����
    int b_y_prev = b_y;
    bool MoveIsDone = false;
    int StopCount = 0;
    const int MaxStopCount = 150; // ������ ����������� �������� � ����� ������� ������� �� ����������� �������������
    
    srand((unsigned int)time(NULL));
    // ���� ������������ ����� �������������� �������� ��������� ����� ��� "��������" (������� �������),
    // � �������� MaxStopCount ����������� ��� ���������� ��������� � �������,
    // �� �� ������� ������������ ������ ����� ���, ��� �������� �������� �/��� ����� ������������ ����� ���������
    // � ��������� ��������� ��� ������������ ������, ����� ������� � ����� �������� ����� �������� ��������� ����� - �����
    // ������������ ������������� ������� ��������� �����!
    while (!MoveIsDone && StopCount < MaxStopCount)
    {
        StopCount++;
        int r = rand() % 4;
        // ����� ��������� �� ����� �� ������� �������� �������, �� ����������� ������, � ����� ����� ������ ����� ���� 'k'!
        if (r == 0 && b_y > 0 && (ma[b_x][b_y - 1] == '.' || ma[b_x][b_y - 1] == 'k')) // ��� ����
        {
            b_y--;
            MoveIsDone = true;
        }
        if (r == 1 && b_x < BH_FIELD_X - 1 && (ma[b_x + 1][b_y] == '.' || ma[b_x + 1][b_y] == 'k')) // ��� ������
        {
            b_x++;
            MoveIsDone = true;
        }
        if (r == 2 && b_y < BH_FIELD_Y - 1 && (ma[b_x][b_y + 1] == '.' || ma[b_x][b_y + 1] == 'k')) // ��� �����
        {
            b_y++;
            MoveIsDone = true;
        }
        if (r == 3 && b_x > 0 && (ma[b_x - 1][b_y] == '.' || ma[b_x - 1][b_y] == 'k')) // ��� �����
        {
            b_x--;
            MoveIsDone = true;
        }

        if (MoveIsDone) // ���� ����� ������ ������� ���
        {
            pBeast_spr->setPosition(GetVec2XY(b_x, b_y)); //���������� ��� ������
            if (ma[b_x_prev][b_y_prev] != 'k')
                ma[b_x_prev][b_y_prev] = '.'; // ���������� ����� �����������, �� ��������� �����! ('k')
            if (ma[b_x][b_y] != 'k')
                ma[b_x][b_y] = 'b'; // � �� ����� ����� ������ ������� �����, ����� �� ���� ��� �� ����
        } 
    }
    if (!MoveIsDone)
        // ��������, ���� ����� �� ����� ������� ���, � ����� ���� ������� �� ������� ������� � ��
        JumpToTrap();             
    return MoveIsDone; // ���� ��� ������ �� �������, �� false �� ������
}

// ����� �������� ����� pBeast_spr � ������������ b_x, x_y
// ��������� �1 + ����� �� ����� �������
// � ������ � ������� ��������� ���������� ����� ����� ������ ����� �������� � ��������������� ��������
// ������ ����� ����� ����� "����������" ������, ��� ����� ��� ���, � ������ ��� �������.
// � ������ ���� ���� ���������� �������� ������� � �������, ���������� ���������� � ��������� �2, ��� �������
// ����� ��������� ��������� ��������� ������ ���� ����� ��������� � ������ �� ������������� ������� ����� �������
// ����� �� ��������� �������������� ��������� ����� �� ������ ����� �������� ���� - ��. ���� ��������� �2.
// ����� ����������: true = ����� ���� ������� ���; false = ������������� ��� ����� � ������� = game over
bool BeastVsHunters1::BeastMove1_Cant_See_Traps()
{
    int b_x_prev = b_x;                     // ������ ���������� ���������� �����
    int b_y_prev = b_y;
    bool BestMoveIsDone = false;            // ������ �� ��� � ����������� ����, � ������� ����� ��� ��� � ��� ��� �������
    bool MoveIsDone = false;                // true ���� ������������� ��� ����� � ������� => GameOver
    std::vector<BMovesEnum> BestMovesList;  // ������ ������ ����� �����
    std::vector<BMovesEnum> OtherMovesList; // ������ ������� ����� �����

    bm[b_x][b_y] = '+';                     // ������� ������ ���������

    // ������� ���������, ���� �� ������ ����� ������ � ������� ������ � ���������� � ma !��� ����� �� ����� �������!
    if (b_y > 0              && bm[b_x][b_y - 1] == '+' && bst_can_see.find(ma[b_x][b_y - 1]) == std::string::npos) // �����
        BestMovesList.push_back(down);
    if (b_x < BH_FIELD_X - 1 && bm[b_x + 1][b_y] == '+' && bst_can_see.find(ma[b_x + 1][b_y]) == std::string::npos) // ������
        BestMovesList.push_back(right);
    if (b_y < BH_FIELD_Y - 1 && bm[b_x][b_y + 1] == '+' && bst_can_see.find(ma[b_x][b_y + 1]) == std::string::npos) // ������
        BestMovesList.push_back(up);
    if (b_x > 0              && bm[b_x - 1][b_y] == '+' && bst_can_see.find(ma[b_x - 1][b_y]) == std::string::npos) // �����
        BestMovesList.push_back(left);

    if (!BestMovesList.empty()) // ���� ���� �������� ����������� ����
    {
        srand((unsigned int)time(NULL));
        int r = rand() % (BestMovesList.size()); // ����� ���� �������� ��� �� ���������
        if (BestMovesList.at(r) == down)  // ��� ����
            b_y--;
        if (BestMovesList.at(r) == right)
            b_x++;
        if (BestMovesList.at(r) == up)
            b_y++;
        if (BestMovesList.at(r) == left)
            b_x--;

        // ��� �� ������ �� ����� �������� �� �������, ����� ��� ��� ���, � ������� �����������
        pBeast_spr->setPosition(GetVec2XY(b_x, b_y)); // ���������� ������ �����
        if (ma[b_x_prev][b_y_prev] != 'k')
            ma[b_x_prev][b_y_prev] = '.'; // ���������� ����� �����������, �� ��������� �����! ('k')
        if (ma[b_x][b_y] != 'k')
            ma[b_x][b_y] = 'b'; // � �� ����� ����� ������ ������� �����, ����� �� ���� ��� �� ����

        MoveIsDone = true;
    }

    // ���� ������� ����� �� �������, �� ��� ������ ��������� ��� �� ��������� �����, �� �� ���� �������!
    if (!MoveIsDone)
    {
        if (b_y > 0              && bst_can_see.find(ma[b_x][b_y - 1]) == std::string::npos) // �����
            OtherMovesList.push_back(down);
        if (b_x < BH_FIELD_X - 1 && bst_can_see.find(ma[b_x + 1][b_y]) == std::string::npos) // ������
            OtherMovesList.push_back(right);
        if (b_y < BH_FIELD_Y - 1 && bst_can_see.find(ma[b_x][b_y + 1]) == std::string::npos) // ������
            OtherMovesList.push_back(up);
        if (b_x > 0              && bst_can_see.find(ma[b_x - 1][b_y]) == std::string::npos) // �����
            OtherMovesList.push_back(left);
    }

    if (!OtherMovesList.empty()) // ���� ���� �������� ����������� ����
    {
        srand((unsigned int)time(NULL));
        int r = rand() % (OtherMovesList.size()); // ����� ���� �������� ��� �� ���������
        if (OtherMovesList.at(r) == down)  // ��� ����
            b_y--;
        if (OtherMovesList.at(r) == right)
            b_x++;
        if (OtherMovesList.at(r) == up)
            b_y++;
        if (OtherMovesList.at(r) == left)
            b_x--;

        pBeast_spr->setPosition(GetVec2XY(b_x, b_y)); // ���������� ������ �����

        // � ��� ������� ��������� ������� � ������� ��� ���!!!
        if (ma[b_x][b_y] == 't')
        {
            // ���� ������� � ������� �� �������� - ��������� ������ ���
            auto effect = ParticleSmoke::create();
            effect->setPosition(GetVec2XY(b_x, b_y));
            effect->setEmissionRate(4.0f);
            addChild(effect, 10);
        }
        else
        {
            if (ma[b_x_prev][b_y_prev] != 'k')
                ma[b_x_prev][b_y_prev] = '.'; // ���������� ����� �����������, �� ��������� �����! ('k')
            if (ma[b_x][b_y] != 'k')
                ma[b_x][b_y] = 'b'; // � �� ����� ����� ������ ������� �����, ����� �� ���� ��� �� ����
            MoveIsDone = true;
        }

    }

    return MoveIsDone;
}

// ����� �������� ����� pBeast_spr � ������������ b_x, x_y
// ��������� �2 - ��� �������� ���� ������������ ����� ����� ���� ��������� (������������ ���������),
// ����� ����� ������ ��� ������ ���������� �������, ����� ���� �� ��������� ��������� �� ���������������
// ������� �������� ����, � ������ ���� ��� �� ����� ��������.
// �������� ����������� ��������� ��� ���������, ������� ������� ��������� �� ��������� ����� ����� ������� �����!
// ����� ����������: true = ����� ���� ������� ���; false = ������������� ��� ����� � ������� = game over
bool BeastVsHunters1::BeastMove2()
{
    return true;
}

// ����� �������� ����� pBeast_spr � ������������ b_x, x_y
// ��������� �3 - ������ ������ �����, ��� � ��������, � ���������� ��� (����������) ������� ����
// ����� ����������: true = ����� ���� ������� ���; false = ������������� ��� ����� � ������� = game over
bool BeastVsHunters1::BeastMove3()
{
    return true;
}
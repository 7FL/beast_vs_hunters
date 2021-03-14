// Тестовое задание - игра "Загони зверя в ловушку"
// Март 2021 (C) 7FL (FL@MAIL.RU, github.com/7FL)

#include "BeastVsHuntersSc1.h"

USING_NS_CC;

// Глобальные константы и переменные
// затем часть констант можно переделать под переменные для выбора их в доп. сцене "Settings" и хранить в файле

// =============== ВХОДНЫЕ ДАННЫЕ ПО УСЛОВИЯМ ТЕСТОВОГО ЗАДАНИЯ ===========================================
const int BH_FIELD_X = 21;                  // размерность поля по горизонтали, клетка [0,0] слева вверху игровой области
const int BH_FIELD_Y = 15;                  // размерность поля по вертикали
const int BH_MAX_HUNTERS = 10;              // количество охотников
const int BH_MAX_TRAPS = 10;                // количество ловушек

const bool BH_BEAST_CAN_SEE_TRAPS = true;  // может ли зверь видеть ловушки, оказывает глобальное влияние на стратегию зверя!

const int BH_MAX_WALLS = 8;                 // количество стен
const int BH_MAX_FLAGS = 8;                 // количество флажков
const int BH_MAX_BUSHES = 8;                // количество кустов
// =============== ВХОДНЫЕ ДАННЫЕ ПО УСЛОВИЯМ ТЕСТОВОГО ЗАДАНИЯ ===========================================

const std::string bst_can_see = "hwf";  // символы в главном игровом массиве, который видит зверь при BH_BEAST_CAN_SEE_TRAPS = false
                                        // тут нет кустов, так как зверь идет через кусты как по пустой клетке!

const int BH_MARGIN_X = 15;             // отступ по X от края видимой области для отрисовки сетки
const int BH_MARGIN_Y = 15;             // отступ по Y от края видимой области для отрисовки сетки
const int BH_SPRITE_DIM = 128;          // стандартные спайты в игре 128x128 точек
                                        // потом можно использовать функцию размерности спрайта
                                        // вычислять отображаемый размер из размера экрана устройства и BH_FIELD_X/Y
float BH_CELL;                          // вычисляем размер 1 клетки на экране, считается что клетка квадратная, как и спрайты
float BH_SPRITE_SCALE;                  // вычистяем масштаб спрайта

//bool HuntersMove = true;              // переменная для передачи хода охотникам/зверю
bool GameOver;                          // true при проигрыше зверя, для смены сцен и запретов на ходы охотников
Vec2 touchTMPVec;                       // для координат первоначального спрайта (требуется для возвратов при перемещениях)

cocos2d::Sprite* pBeast_spr;            // указатель на спрайт зверя для обращений из back-end кода
int b_x, b_y;                           // глобальные координаты зверя в игровом массиве, видимые везде

int TotalMove;                          // сделано ходов охотниками, считаем для соревновании и рейтинга
std::string L1Text;                     // выводим название игры и количество ходов вверху экрана
cocos2d::Label* ptrLabel1;

Size visibleSize;                       // экранные координаты - видимый экран
Vec2 origin;                            // экранные координаты - 0,0

// Главный игровой массив, задающий первоначальное расположение спрайтов,
// . = свободная клетка, b = зверь, h = охотник, t = ловушка, w = стена (никто не проходит), 
// k = куст (проходит зверь), f = флажки (проходит охотник), и т.п.
// main array [0][0] - самая верхняя левая клетка
char ma[BH_FIELD_X][BH_FIELD_Y];

/* Тестовый массив (зеркальный)
const char ma_const[BH_FIELD_Y][BH_FIELD_X] = {
{'.', 'h', '.', 'h', '.', '.', '.', 'h',},
{'h', '.', 'h', '.', '.', 'f', '.', '.',},
{'.', '.', '.', '.', '.', '.', '.', '.',},
{'.', '.', '.', '.', '.', '.', '.', '.',},
{'.', 't', '.', '.', 'w', 'b', '.', '.',},
{'.', 'k', '.', 'k', '.', '.', '.', '.',},
};*/

const std::string ma_allow_chars = "bhtwkf";          // допустимые символы в главном игровом массиве (для проверок)
enum BMovesEnum { up, down, left, right };            // возможные ходы зверя для составления списков возможных ходов

//const std::string beast_allow_chars = ".k";         // допустимые символы в массиве для захода зверя
//const std::string hunters_allow_chars = ".f";       // допустимые символы в массиве для захода охотников
// Также при усложнении условий игры есть смысл разделить массивы на "статический" и "динамический"
// чтобы в динамическом хранить только положение зверя и охотников, а в статическом - ловушки, кусты, стены, флажки
// для запоминания того, какой объект проходит зверь или охотники
// Переход на этот механизм можно реализовать для терепортов и прочего

// Память зверя (beast memory) для стратегии, когда зверь не видит ловушки, тогда
// предпочтение при выборе хода отдается тем клеткам, в которых уже был, так как там нет ловушек
char bm[BH_FIELD_X][BH_FIELD_Y]; 

Scene* BeastVsHunters1::createScene()
{
    return BeastVsHunters1::create();
}

// Печать сообщения об ошибке, какого файла не хвататет в ресурсах
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BeastVsHuntersSceneIntro.cpp\n");
}

// Инициализация сцены
bool BeastVsHunters1::init()
{
    // Общая инициалиция
    if (!Scene::init())     // super init first
    {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    // ---------------------------------------------------------------------------------------------------------

    // Все глабальные переменные класса надо инициализировать тут в Init, иначе некорректно работает смена сцен!
    TotalMove = 0;
    GameOver = false;

    // Инициализация игрового массива и массива памяти зверя
    for (int x = 0; x < BH_FIELD_X; x++)
        for (int y = 0; y < BH_FIELD_Y; y++)
        {
            ma[x][y] = '.'; // пустая клетка игрового массива
            bm[x][y] = '?'; // непроверенная клетка массива памяти зверя ('+' = проверено)
        }

    FillMainArray(BH_MAX_HUNTERS, 'h');         // инициализация игрового массива - Охотники
    FillMainArray(1,              'b');         // инициализация игрового массива - Зверь, он только один
    FillMainArray(BH_MAX_TRAPS,   't');         // инициализация игрового массива - Ловушки
    FillMainArray(BH_MAX_WALLS,   'w');         // инициализация игрового массива - Стены
    FillMainArray(BH_MAX_FLAGS,   'f');         // инициализация игрового массива - Флажки
    FillMainArray(BH_MAX_BUSHES,  'k');         // инициализация игрового массива - Кусты

    // ---------------------------------------------------------------------------------------------------------

    // Добавляем кнопку "выход" как меню
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
    
    
    // Добавляем в Label1 заголовок, исп. русский utf - u8
    auto Label1 = Label::createWithTTF(u8"Сделано ходов: 0", "fonts/arial.ttf", 26);
    ptrLabel1 = Label1;
    float lb_x = origin.x + visibleSize.width / 2; // середина экрана
    float lb_y = origin.y + visibleSize.height;  // выводим текст Label, игровое меню и все спрайты сверху вниз
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

    // Вычисляем границы области для игрового поля, переменная lb_y досталось от верхнего заголовка
    float x_bottom = (BH_MARGIN_X)-2;
    float y_bottom = (BH_MARGIN_Y)-2;
    float x_top = (origin.y + visibleSize.width - BH_MARGIN_X) + 2;
    float y_top = (lb_y - BH_MARGIN_Y) + 2;

    // Для отладки и поверки рассчета стороны клетки рисуем синим максимальное игровое поле на видимой части экрана
    /*auto drawFullSpritesField = DrawNode::create();
    Vec2 verticsFSF[4] = {Vec2(x_bottom, y_bottom),
                          Vec2(x_bottom, y_top   ),
                          Vec2(x_top   , y_top   ),
                          Vec2(x_top   , y_bottom)};
    drawFullSpritesField->drawPolygon(verticsFSF, 4, Color4F::BLACK, 1, Color4F::BLUE);
    this->addChild(drawFullSpritesField, -2);*/

    // Вычисляем размер игровой клетки и масштаб спрайтов для максимально полного заполнения экрана
    // в переменных x_bottom, y_bottom, x_top, y_top теперь у нас содержится область для отрисовки клеток игрового поля
    // вычислим размер клетки (квадрат) и масштаб спрайтов, чтобы было максимально полное заполнение
    float x_cell = ((x_top - x_bottom) / static_cast<float>(BH_FIELD_X));
    float y_cell = ((y_top - y_bottom) / static_cast<float>(BH_FIELD_Y));
    BH_CELL = x_cell < y_cell ? x_cell : y_cell; // для размера квадратной ячейки берем минимальный размер, чтобы вошло в область
    BH_SPRITE_SCALE = (BH_CELL) / static_cast<float>(BH_SPRITE_DIM);  // вычисляем масштаб спрайта под клетку

    // Рисуем сетку - поле из квадратов
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

    // Добваляем все спрайты!
    // Блок отделен от инициализации игрового массива, для случаев, если массив будет читаться из xml/json
    // std::vector<cocos2d::Sprite*> sprites_arr; // массив всех спрайтов, может пригодиться в будущем
    // Имена спрайтов назначаются в коде класса, затем можно переделать, чтобы читать из xml/json
    std::vector<std::string> spriteNames = { "beast128.png", "hunter128.png", "trap128.png", "wall128.png", "bush128.png", "flags128.png" };
    std::vector<char> spriteChars =        { 'b',            'h',             't',           'w',           'k',           'f'            };
    for (int x = 0; x < BH_FIELD_X; x++)
    {
        for (int y = 0; y < BH_FIELD_Y; y++)
        {
            if (ma[x][y] == '.')
                continue;
            if (ma_allow_chars.find(ma[x][y]) != std::string::npos) // проверяем, что символ из ma в разрешенных символах
            {
                // В idx вычисляем позицию символа ma[x][y] по значению в векторе начиная с 0
                // для сопостовлению символа из массива строке с именем файла спрайта
                auto iter = std::find(spriteChars.begin(), spriteChars.end(), ma[x][y]);
                size_t idx;
                if (iter != spriteChars.end())
                    idx = iter - spriteChars.begin();
                else
                    continue; // это никогда не случится, благодоря предыдущему if, но не лишне проверить в общем случае

                auto sprite1 = Sprite::create(spriteNames.at(idx));  // берем имя файла спрайта по позиции в векторе
                if (sprite1 == nullptr) // 
                    problemLoading(spriteNames.at(idx).c_str()); // char*
                else
                {
                    sprite1->setPosition(GetVec2XY(x, y)); // устанавливаем центр спрайта по игровой сетке
                    sprite1->setScale(BH_SPRITE_SCALE);    // задаем масштаб спрайта
                    //sprite1->setAnchorPoint(Vec2(0.5, 0.5)); // "средняя" точка спрайта - 0,0 - левая внизу
                    //sprite1->setTag(n); // пока можно без тагов, потом можно использовать их

                    // Далее добавляем обработку касания экрана, но только для перемещения спрайтов охотников!
                    if (ma[x][y] == 'h') // h - охотник
                    {
                        auto touchListener = EventListenerTouchOneByOne::create();
                        touchListener->setSwallowTouches(true); // для "проглатывания" касаний, нужно вернуть true в onTouchBegan()

                        // Лямбда-функция - начало перемещения
                        touchListener->onTouchBegan = [=](Touch* touch, Event* event) -> bool {
                        auto bounds = event->getCurrentTarget()->getBoundingBox();
                        // Если сейчас ход охотников и только для области спрайта (иначе можно схватить за любую точку экрана)
                        if (/*HuntersMove &&*/ !GameOver && bounds.containsPoint(touch->getLocation())) {
 
                            // для отладки
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
                            touchTMPVec = target->getPosition(); // запомниаем первоначальную позицию спрайта
                            return true; // позволяем тянуть только при таких условях
                        } else
                            return false; // иначе не позволяем тянуть
                        };
 
                        // Во время перемещения
                        touchListener->onTouchMoved = [](Touch* touch, Event* event) 
                        {
                            auto target = static_cast<Sprite*>(event->getCurrentTarget());
                            // перемещаем спрайт для наглядности во время движения пальца по экрану
                            target->setPosition(target->getPosition() + touch->getDelta());
                        };
                        
                        // Окончание перемещения
                        touchListener->onTouchEnded = [=](Touch* touch, Event* event)
                        {
                            MoveHunters(touch, event); // Движение охотника
                        };
                        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, sprite1);
                    }

                    // sprites_arr.push_back(sprite1); // на будущее возможность добавлять все спрайты в общий массив
                    if (ma[x][y] == 'b') {
                        pBeast_spr = sprite1;   // при инициализации для зверя запоминаем спрайт в ptr, видимый на уровне класса
                        b_x = x;                // а также координаты зверя в ma[x][y]
                        b_y = y;
                    }
                    if (ma[x][y] == 'h')
                        this->addChild(sprite1, 2);     // охотников добавляем правее в дереве узлов Node
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

// Случайное заполнение главного игрового массива символом в char и количеством в int
// заполнять данным методом не рекомендуется если критична скорость и при очень плотном заполнении массива
void BeastVsHunters1::FillMainArray(int Count, char c)
{
    unsigned int MaxLoopCount = BH_FIELD_X * BH_FIELD_X * 50; // количество циклов для выхода из while, если входные данные будут большие
    unsigned int LoopCount = 0;                               // cчитаем общее количество циклов while, чтобы цикл вдруг не стал бесконечным
    int i = 0;                                       // удачные заполнения, для сравнения с заданным Count
    srand((unsigned int)time(NULL));
    while (i < Count && LoopCount < MaxLoopCount)
    {
        int x = rand() % BH_FIELD_X;
        int y = rand() % BH_FIELD_Y;
        if (ma[x][y] == '.') // если пусто, то заполняем
        {
            ma[x][y] = c;
            i++;  // увеличиваем счетчик удачных заполнений
        }
        LoopCount++;
    }
}

// Возвращаемся назад, если сцен много, то нужно делать менеджер сцен через синглтон (как Director::getInstance()), только более продвинутый
void BeastVsHunters1::menuCloseCallback(Ref* pSender)
{
    // auto scene1 = BeastVsHuntersMenu::createScene();
    // Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene1, Color3B(255, 255, 255)));
    Director::getInstance()->popScene();
}

// Выводим строку для Label1 и увеличиваем количество сделанных ходов
void BeastVsHunters1::IncMoves()
{
    L1Text = u8"Сделано ходов: " + std::to_string(TotalMove++);
    if (ptrLabel1 != nullptr)
        ptrLabel1->setString(L1Text);
}

// Пересчет клетки игрового массива в графические координаты (в центры игровых клеток)
Vec2 BeastVsHunters1::GetVec2XY(int x, int y) 
{
    return Vec2(BH_MARGIN_X + BH_CELL / 2 + BH_CELL *                  (float)(x),
                BH_MARGIN_Y + BH_CELL / 2 + BH_CELL * (float)(BH_FIELD_Y - 1 - y)); // зеркало по y
}

// Перенос спрайтов охотников про окончании хода (перемещения)
void BeastVsHunters1::MoveHunters(Touch* touch, Event* event)
{
    auto target = static_cast<Sprite*>(event->getCurrentTarget());
    Vec2 touchTMPVecNew = target->getPosition() + touch->getDelta(); // позиция спрайта после перемещения

    // Предыдущие кооднинаты спрайта в координатной сетке 2-х мерного массива с шагом int 1
    int m_x_perv = static_cast<int>((touchTMPVec.x - BH_MARGIN_X) / BH_CELL);
    int m_y_perv = BH_FIELD_Y - 1 - static_cast<int>((touchTMPVec.y - BH_MARGIN_Y) / BH_CELL); // зеркало
    // Вычисляем номер ячейки 2-х мерного массива (поля игры) куда приехал наш спрайт
    int m_x;
    if (touchTMPVecNew.x < BH_MARGIN_X || // проверка, что спрайт не вытягиваем за границы игровой сетки
        static_cast<int>((touchTMPVecNew.x - BH_MARGIN_X) / BH_CELL) > BH_FIELD_X - 1)
    {
        m_x = 7777; /// 7777 - это попали вне поля, таких размерностей поля игра не допускает
    }
    else
    {
        // Целочисленная клетка по X, без static_cast<int> будет float, просто откидываем дробную часть!
        m_x = static_cast<int>((touchTMPVecNew.x - BH_MARGIN_X) / BH_CELL);
    }
    int m_y;
    if (touchTMPVecNew.y < BH_MARGIN_Y || // проверка, что спрайт не вытягиваем за границы игровой сетки
        static_cast<int>((touchTMPVecNew.y - BH_MARGIN_Y) / BH_CELL) > BH_FIELD_Y - 1)
    {
        m_y = 7777; /// 7777 - это попали вне поля, таких размерностей поля игра не допускает
    }
    else
    {
        // целочисленная клетка по X, без static_cast<int> будет float, просто откидываем дробную часть!
        m_y = BH_FIELD_Y - 1 - static_cast<int>((touchTMPVecNew.y - BH_MARGIN_Y) / BH_CELL); // целочисленная клетка по y
    }
    // Сначала проверяем, что мы не вышли спрайтом за границы поля
    if (m_x == 7777 || m_y == 7777 ||
        // Потом проверяем, что нельзя отойти более, чем на 1 клетку вправо, вверх, вниз или вверх
        abs(m_x - m_x_perv) + abs(m_y - m_y_perv) > 1 ||
        // А также, что мы не стоим на одном и том же месте
        (m_x == m_x_perv && m_y == m_y_perv) ||
        // И теперь проверяем что мы не заходим на чужие клетки
        (ma[m_x][m_y] != '.' && ma[m_x][m_y] != 'f')  // пустая клетка или флажки 'f' - через них может ходить охотник!
        )
    {
        target->setPosition(touchTMPVec); // возвращаем спрайт обратно
    }
    else
    {
        // Двигаем спрайт на экране, причем он автоматически выравнивается по игровой сетке массива ma[][]
        target->setPosition(GetVec2XY(m_x, m_y));
        if (ma[m_x_perv][m_y_perv] != 'f')
            ma[m_x_perv][m_y_perv] = '.'; // предыдущее место освобождаем  УЧИТЫВАЕМ И ФЛАЖКИ!!!
        if (ma[m_x][m_y] != 'f')
            ma[m_x][m_y] = 'h';           // а на новом месте ставим отметку охотника
        // HuntersMove = false; // Передаем ход зверю // пока не используем эту схему

        // Для отладки прикосаний (touch) к экрану
        // std::string s = "Event point: " +
        //    std::to_string(touchTMPVecNew.x) + ", " + std::to_string(touchTMPVecNew.y) + "\n" +
        //   "Cell x = " + std::to_string(m_x) + "; y = " + std::to_string(m_y);
        //cocos2d::log(s.c_str());

        IncMoves(); // Увеличиваем счетчик ходов и обновляем надпись вверху с их количеством

        // После успещной смены клетки охотником передаем ход зверю,
        // выбираем стратегию зверя BeastMove1,2,3... и видит ли зверь ловушки
        bool MoveIsDone;
        if (BH_BEAST_CAN_SEE_TRAPS)
            MoveIsDone = BeastMove1();
        else
            MoveIsDone = BeastMove1_Cant_See_Traps();
        
        if (!MoveIsDone)
        {
            // Если зверь не смог сделать свой ход, то он проиграл
            GameOver = true; // блокируем дальнейшие ходы игрока
            
            // Запускаем действе или катсцену по GameOver
            RunGameOverCutscene(); 
        }
    }
}

// Изменения текущей сцены или запуск новой сцены при победе игрока
void BeastVsHunters1::RunGameOverCutscene()
{
    L1Text = u8"ВЫ ПОБЕДИЛИ!!! ИСПОЛЬЗОВАНО ХОДОВ: " + std::to_string(TotalMove);
    ptrLabel1->setColor(Color3B::GREEN);
    if (ptrLabel1 != nullptr)
        ptrLabel1->setString(L1Text);
}

// Фигальный прыжок в ловушку, если их несколько рядом - то в первую попавшуюся
void BeastVsHunters1::JumpToTrap()
{
    bool NeedToJump = false; // Надо ли прыгать
    Vec2 JumpVec; // Вектор финального прыжка в ловушку
    if (b_y > 0 && ma[b_x][b_y - 1] == 't') // проверяем наличие ловушки вверху (далее, по часовой стрелке)
    {
        JumpVec = Vec2(0.0, BH_CELL);
        NeedToJump = true;
    }
    if (b_x < BH_FIELD_X - 1 && ma[b_x + 1][b_y] == 't') // проверка справа
    {
        JumpVec = Vec2(BH_CELL, 0.0);
        NeedToJump = true;
    }
    if (b_y < BH_FIELD_Y - 1 && ma[b_x][b_y + 1] == 't') // проверка внизу
    {
        JumpVec = Vec2(0.0, -BH_CELL);
        NeedToJump = true;
    }
    if (b_x > 0 && ma[b_x - 1][b_y] == 't') // проверка слева
    {
        JumpVec = Vec2(-BH_CELL, 0.0);
        NeedToJump = true;
    }

    if (NeedToJump) // если надо прыгать - прыгаем медленно и нехотя, можно с эффектами Cocos2d-x
    {
        auto moveBy1 = MoveBy::create(1.5f, JumpVec);
        auto fadeOut1 = FadeOut::create(3.0f);            // медленно исчезаем в ловушке
        auto sequence1 = Sequence::create(moveBy1, fadeOut1, nullptr);
        pBeast_spr->runAction(sequence1);
    }
}

// Ходим спрайтом зверя pBeast_spr с координатами b_x, x_y
// Стратегия №1 - случайное перемещение
// Метод возвращает: true = зверь смог сделать ход; false = заблокировали или попал в ловушку = game over
bool BeastVsHunters1::BeastMove1()
{
    int b_x_prev = b_x; // храним предыдущие координаты зверя
    int b_y_prev = b_y;
    bool MoveIsDone = false;
    int StopCount = 0;
    const int MaxStopCount = 150; // делаем ограничения счетчика с очень большим запасом по нормальному распределению
    
    srand((unsigned int)time(NULL));
    // Ниже используется метод множественного перебора возможных ходов при "неудачах" (занятых клетках),
    // с подобным MaxStopCount вероятность его успешности стремится к единице,
    // но не следует использовать данный метод там, где критична скорость и/или нужно обрабатывать много вариантов
    // в следующей стратегии уже используется другой, более быстрый и более надежный метод перебота возможных ходов - через
    // формирование динамического массива возможных ходов!
    while (!MoveIsDone && StopCount < MaxStopCount)
    {
        StopCount++;
        int r = rand() % 4;
        // Везде проверяем на выход за пределы игорвого массива, на незанятость клетки, и зверь может ходить через куст 'k'!
        if (r == 0 && b_y > 0 && (ma[b_x][b_y - 1] == '.' || ma[b_x][b_y - 1] == 'k')) // ход вниз
        {
            b_y--;
            MoveIsDone = true;
        }
        if (r == 1 && b_x < BH_FIELD_X - 1 && (ma[b_x + 1][b_y] == '.' || ma[b_x + 1][b_y] == 'k')) // ход вправо
        {
            b_x++;
            MoveIsDone = true;
        }
        if (r == 2 && b_y < BH_FIELD_Y - 1 && (ma[b_x][b_y + 1] == '.' || ma[b_x][b_y + 1] == 'k')) // ход вверх
        {
            b_y++;
            MoveIsDone = true;
        }
        if (r == 3 && b_x > 0 && (ma[b_x - 1][b_y] == '.' || ma[b_x - 1][b_y] == 'k')) // ход влево
        {
            b_x--;
            MoveIsDone = true;
        }

        if (MoveIsDone) // если зверю удаось сделать ход
        {
            pBeast_spr->setPosition(GetVec2XY(b_x, b_y)); //перемещаем его спрайт
            if (ma[b_x_prev][b_y_prev] != 'k')
                ma[b_x_prev][b_y_prev] = '.'; // предыдущее место освобождаем, но сохраняем кусты! ('k')
            if (ma[b_x][b_y] != 'k')
                ma[b_x][b_y] = 'b'; // а на новом месте ставим отметку зверя, опять же если там не куст
        } 
    }
    if (!MoveIsDone)
        // Проверка, если зверь не может сделать ход, а рядом есть ловушка то красиво прыгаем в неё
        JumpToTrap();             
    return MoveIsDone; // если ход делать не удалось, то false на выходе
}

// Ходим спрайтом зверя pBeast_spr с координатами b_x, x_y
// Стратегия №1 + зверь не видит ловушки
// в случае с простой случайной стратегией ходов зверя данный медов сводится к предварительной проверке
// зверем перед любым ходом "безопасных" клеток, где зверь уже был, а значит нет ловушек.
// В данном виде игра становится довольно скучная и простая, необходимо переходить к стратегии №2, при которой
// зверь обнаружив скопления охотников вокруг себя будет рисковать и ходить по непроверенным клеткам строя маршрут
// ухода от скоплений перемещающихся охотников рядом на другой конец игрового поля - см. ниже стратегию №2.
// Метод возвращает: true = зверь смог сделать ход; false = заблокировали или попал в ловушку = game over
bool BeastVsHunters1::BeastMove1_Cant_See_Traps()
{
    int b_x_prev = b_x;                     // храним предыдущие координаты зверя
    int b_y_prev = b_y;
    bool BestMoveIsDone = false;            // сделан ли ход в проверенное поле, в котором зверь уже был и где нет ловушек
    bool MoveIsDone = false;                // true если заблокировали или попал в ловушку => GameOver
    std::vector<BMovesEnum> BestMovesList;  // массив лучших ходов зверя
    std::vector<BMovesEnum> OtherMovesList; // массив обычный ходов зверя

    bm[b_x][b_y] = '+';                     // текущая клетка проверена

    // Сначала проверяем, есть ли вокруг зверя клетки с лучшими ходами и свободными в ma !ТУТ ЗВЕРЬ НЕ ВИДИТ ЛОВУШКУ!
    if (b_y > 0              && bm[b_x][b_y - 1] == '+' && bst_can_see.find(ma[b_x][b_y - 1]) == std::string::npos) // внизу
        BestMovesList.push_back(down);
    if (b_x < BH_FIELD_X - 1 && bm[b_x + 1][b_y] == '+' && bst_can_see.find(ma[b_x + 1][b_y]) == std::string::npos) // справа
        BestMovesList.push_back(right);
    if (b_y < BH_FIELD_Y - 1 && bm[b_x][b_y + 1] == '+' && bst_can_see.find(ma[b_x][b_y + 1]) == std::string::npos) // вверху
        BestMovesList.push_back(up);
    if (b_x > 0              && bm[b_x - 1][b_y] == '+' && bst_can_see.find(ma[b_x - 1][b_y]) == std::string::npos) // слева
        BestMovesList.push_back(left);

    if (!BestMovesList.empty()) // если есть варианты безопасного хода
    {
        srand((unsigned int)time(NULL));
        int r = rand() % (BestMovesList.size()); // берем один случайый ход из возможных
        if (BestMovesList.at(r) == down)  // ход вниз
            b_y--;
        if (BestMovesList.at(r) == right)
            b_x++;
        if (BestMovesList.at(r) == up)
            b_y++;
        if (BestMovesList.at(r) == left)
            b_x--;

        // Тут по логике не нужна проверка на ловушки, зверь тут уже был, а ловушки статические
        pBeast_spr->setPosition(GetVec2XY(b_x, b_y)); // перемещаем спрайт зверя
        if (ma[b_x_prev][b_y_prev] != 'k')
            ma[b_x_prev][b_y_prev] = '.'; // предыдущее место освобождаем, но сохраняем кусты! ('k')
        if (ma[b_x][b_y] != 'k')
            ma[b_x][b_y] = 'b'; // а на новом месте ставим отметку зверя, опять же если там не куст

        MoveIsDone = true;
    }

    // Если хороших ходов не сделано, то уже делаем случайный ход на незанятое место, но не видя ловушек!
    if (!MoveIsDone)
    {
        if (b_y > 0              && bst_can_see.find(ma[b_x][b_y - 1]) == std::string::npos) // внизу
            OtherMovesList.push_back(down);
        if (b_x < BH_FIELD_X - 1 && bst_can_see.find(ma[b_x + 1][b_y]) == std::string::npos) // справа
            OtherMovesList.push_back(right);
        if (b_y < BH_FIELD_Y - 1 && bst_can_see.find(ma[b_x][b_y + 1]) == std::string::npos) // вверху
            OtherMovesList.push_back(up);
        if (b_x > 0              && bst_can_see.find(ma[b_x - 1][b_y]) == std::string::npos) // слева
            OtherMovesList.push_back(left);
    }

    if (!OtherMovesList.empty()) // если есть варианты безопасного хода
    {
        srand((unsigned int)time(NULL));
        int r = rand() % (OtherMovesList.size()); // берем один случайый ход из возможных
        if (OtherMovesList.at(r) == down)  // ход вниз
            b_y--;
        if (OtherMovesList.at(r) == right)
            b_x++;
        if (OtherMovesList.at(r) == up)
            b_y++;
        if (OtherMovesList.at(r) == left)
            b_x--;

        pBeast_spr->setPosition(GetVec2XY(b_x, b_y)); // перемещаем спрайт зверя

        // А ВОТ ТЕПЕРТЬ ПРОВЕРЯЕМ ВЛЕТЕЛИ В ЛОВУШКУ ИЛИ НЕТ!!!
        if (ma[b_x][b_y] == 't')
        {
            // Если влетели в ловушку на скорости - запускаем оттуда дым
            auto effect = ParticleSmoke::create();
            effect->setPosition(GetVec2XY(b_x, b_y));
            effect->setEmissionRate(4.0f);
            addChild(effect, 10);
        }
        else
        {
            if (ma[b_x_prev][b_y_prev] != 'k')
                ma[b_x_prev][b_y_prev] = '.'; // предыдущее место освобождаем, но сохраняем кусты! ('k')
            if (ma[b_x][b_y] != 'k')
                ma[b_x][b_y] = 'b'; // а на новом месте ставим отметку зверя, опять же если там не куст
            MoveIsDone = true;
        }

    }

    return MoveIsDone;
}

// Ходим спрайтом зверя pBeast_spr с координатами b_x, x_y
// Стратегия №2 - для игрового поля рассчитываем центр массы всех охотников (отслеживание скопления),
// далее зверь каждый ход строит кратчайший маршрут, чтобы уйти от скоплений охотников на противоположную
// сторону игрового поля, и делает один ход по этому маршруту.
// Особенно отслеживаем скопления тех охотников, которые активно двигались за несколько ходов перед текущим ходом!
// Метод возвращает: true = зверь смог сделать ход; false = заблокировали или попал в ловушку = game over
bool BeastVsHunters1::BeastMove2()
{
    return true;
}

// Ходим спрайтом зверя pBeast_spr с координатами b_x, x_y
// Стратегия №3 - строим дерево ходов, как в шахматах, и определяем вес (полезность) каждого хода
// Метод возвращает: true = зверь смог сделать ход; false = заблокировали или попал в ловушку = game over
bool BeastVsHunters1::BeastMove3()
{
    return true;
}
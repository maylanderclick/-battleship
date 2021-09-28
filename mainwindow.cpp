#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1000,640);          //Задаем размеры окна
    this->setFixedSize(1000,640);    //Фиксируем размеры окна
    //_______________________________________________________________________________
    QPixmap jpg("C:\\morskoy_boy_2.jpg");                                                 //Установка фона
    jpg = jpg.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, jpg);
    this->setPalette(palette);
    ui->label->setStyleSheet("QLabel { background-color : ; color : cyan; }");              //Цвет заголовка
    ui->label_life_ikon1->setStyleSheet("QLabel { background-color : ; color : red; }");
    ui->label_life_ikon2->setStyleSheet("QLabel { background-color : ; color : red; }");
    ui->label_life1->setStyleSheet("QLabel { background-color : ; color : red; }");
    ui->label_life2->setStyleSheet("QLabel { background-color : ; color : red; }");
    ui->graphicsView->setStyleSheet("background-color: rgba(255, 255, 255, 0);");           //Прозрачное поле1
    ui->graphicsView_2->setStyleSheet("background-color: rgba(255, 255, 255, 0);");         //Прозрачное поле2
    //_______________________________________________________________________________
    scene_partner = new QGraphicsScene();                                                   //Инициализируем графическую сцену
    scene_me = new QGraphicsScene();

    ui->graphicsView->setScene(scene_partner);                                              //Связываем графическую сцену(scene_partner) с графическим окном(graphicsView)
    ui->graphicsView_2->setScene(scene_me);

    scene_partner->setSceneRect(0, 0, 400,400);                                             //Область графической сцены
    scene_me->setSceneRect(0, 0, 400,400);                                                  //Область графической сцены

    QPen pen_fields;                                                                        //Настройка кисти для разлиновки
    pen_fields.setWidth(4);                         //Задаем размер шрифта
    pen_fields.setColor(Qt::darkRed);               //Задаем цвет шрифта

    int i = 0;
    while(i <= 400)                                                                         //Предварительная прорисовка поля (Разлиновка)
    {
        scene_partner->addLine(i,0,i,400,pen_fields);                                       // Аргументы addLine (x0, y0, x, y, 'Настройки кисти')
        scene_partner->addLine(0,i,400,i,pen_fields);
        scene_me->addLine(i,0,i,400,pen_fields);
        scene_me->addLine(0,i,400,i,pen_fields);
        i+=40;                                                                              //Геометрический параметр клетки (длина)
    }
    ship_generation();                                                                      //Генерация поля противника
   // ships_test();                               //Тестовая расстановка (Автозаполнение)
}

MainWindow::~MainWindow()
{
    delete scene_partner;
    delete scene_me;
    delete ui;
}

bool MainWindow::hit_me()                                       //Есть ли корабль под прицелом?
{
    for(int i = 0; i < ships_partner.size(); i++)                                  //i - количество секций кораблей(суммарно) 4+3+3+2+2+2+1+1+1+1 = 20 (В тестовом режиме ==4)
        if(hit_x == ships_partner.value(i).value(0) && hit_y == ships_partner.value(i).value(1))
        {
            ships_partner.remove(i);
            //qDebug() << "mymasstor " << ": " << ships_partner;
            return true;
        }
    return false;
}

bool MainWindow::hit_partner()
{
    for(int i = 0; i < shots_partner.size(); i++)                                  //i - количество секций кораблей(суммарно) 4+3+3+2+2+2+1+1+1+1 = 20
    {
        if(shot_p_x == shots_partner.value(i).value(0) && shot_p_y == shots_partner.value(i).value(1))
            return false;
    }
    shots_partner.push_back({shot_p_x, shot_p_y});
    return true;
}

bool MainWindow::hit_partner_2()
{
    for(int i = 0; i < ships_my.size(); i++)                                  //i - количество секций кораблей(суммарно) 4+3+3+2+2+2+1+1+1+1 = 20
    {
        if(shot_p_x == ships_my.value(i).value(0) && shot_p_y == ships_my.value(i).value(1))
        {
            ships_my.remove(i);
            return true;
        }
    }
    return  false;
}



void MainWindow::shot_partner()
{
    bool flag = true;
    int x_rand, y_rand;
    QPen pen_ship, pen;                      //Настройка кисти для заголовка и секций кораблей
    pen_ship.setWidth(30);
    pen_ship.setColor(Qt::red);
    pen.setWidth(5);
    pen.setColor(Qt::red);
    while(flag == true)
    {
        x_rand = rand() % 10+13;
        y_rand = rand() % 10+1;

        shot_p_x = x_rand;
        shot_p_y = y_rand;
        //______________________________________________________________
        if(hit_partner())
        {
            scene_me->addLine((x_rand-1)*40-480,(y_rand-1)*40, x_rand*40-480, y_rand*40,pen);                  //Прорисовка метки удара
            scene_me->addLine(x_rand*40-480,(y_rand-1)*40, (x_rand-1)*40-480, y_rand*40,pen);
            flag = false;
            qDebug() << x_rand << " " << y_rand;
            if(hit_partner_2())                                         //Если есть попадание
            {
                flag = true;
                scene_me->addLine(x_rand*40-500, y_rand*40-20, x_rand*40-500, y_rand*40-20, pen_ship);
                ui->label_life2->setText(QVariant(ships_my.size()).toString());
//                shot_partner();
            }
        }
    }
}

void MainWindow::ship_generation()          //Генерация поля противника
{
    int mass[10][10] = {0};
    int massive[20][2];
    int x, y;
    int _x, _y;								//для запоминания значения; воспользуемся, если можно будет поставить корабль
                                            //вернем х и у начальные координаты(они изменяются в switch)
    int dir = 0;							// направление
    int number_of_deck;						//количество палуб
    int number_of_ships;
    int count = 0;							//общее число кораблей
    int row = 0;							//от 0 до 20 строки массива
    for (int i = 0; i < 4; i++)
    {
        number_of_ships = 0;
        number_of_deck = 4 - i;
        while (number_of_ships < (i + 1)) //расставляем корабли
        {
                //начальная позиция
                x = rand() % 10;
                y = rand() % 10;

                _x = x; _y = y;

                dir = rand() % 4;
                int flag = 1;		//для проверки возможности поставить корабль

                for(int j = 0; j < number_of_deck; j++)
                {
                    if((x<0)||(y<0)||(x>=10)||(y>=10))
                    {
                        //выходит за пределы массива
                        //не можем установить корабль
                        flag = 0;
                        break;
                    }

                    if ((x == 0) || (y == 0) || (x == 9) || (y == 9)) //проверяем для точек на границах (если рассматривать как матрицу)
                    {
                        if (mass[x][y] >= 1) { flag = 0; break; }
                        if ((x == 0) && (y < 9)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x][y + 1] == 1 || mass[x + 1][y + 1] == 1 || mass[x + 1][y] == 1 || mass[x + 1][y - 1] == 1) { flag = 0; break; } }
                        if ((x == 0) && (y < 9)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x][y + 1] == 1 || mass[x + 1][y + 1] == 1 || mass[x + 1][y] == 1 || mass[x + 1][y - 1] == 1) { flag = 0; break; } }
                        if ((y == 0) && (x < 9)) { if (mass[x][y] == 1 || mass[x][y + 1] == 1 || mass[x + 1][y + 1] == 1 || mass[x + 1][y] == 1 || mass[x - 1][y] == 1 || mass[x - 1][y + 1] == 1) { flag = 0; break; } }
                        if ((y == 0) && (x == 0)) { if (mass[x][y] == 1 || mass[x][y + 1] == 1 || mass[x + 1][y + 1] == 1 || mass[x + 1][y] == 1) { flag = 0; break; } }
                        if ((y == 9) && (x == 9)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x - 1][y] == 1 || mass[x - 1][y - 1] == 1) { flag = 0; break; } }
                        if ((y == 9) && (x == 0)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x + 1][y] == 1 || mass[x + 1][y - 1] == 1) { flag = 0; break; } }
                        if ((y == 0) && (x == 9)) { if (mass[x][y] == 1 || mass[x][y + 1] == 1 || mass[x - 1][y] == 1 || mass[x - 1][y - 1] == 1) { flag = 0; break; } }
                        if ((y == 9) && (x < 9) && (x > 0)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x + 1][y] == 1 || mass[x + 1][y - 1] == 1 || mass[x - 1][y] == 1 || mass[x - 1][y - 1] == 1) { flag = 0; break; } }
                        if ((y < 9) && (y > 0) && (x == 9)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x][y + 1] == 1 || mass[x - 1][y] == 1 || mass[x - 1][y + 1] == 1 || mass[x - 1][y - 1] == 1) { flag = 0; break; } }
                    }

                    //если на этом месте и рядом уже есть; проверяем для точек, которые не стоят на границах  //хз, как это написать
                    if((x>0)&&(x<9)&&(y>0)&&(y<9)) { if (mass[x][y] == 1 || mass[x][y - 1] == 1 || mass[x][y + 1] == 1 || mass[x + 1][y + 1] == 1 || mass[x + 1][y] == 1 || mass[x + 1][y - 1] == 1 || mass[x - 1][y] == 1 || mass[x - 1][y + 1] == 1 || mass[x - 1][y - 1] == 1 ) { flag = 0; break; } }

                    switch(dir)
                    {
                        case(0): x++; break;
                        case(1): y++; break;
                        case(2): x--; break;
                        case(3): y--; break;
                    }
                }

                if(flag == 1)	//установка корабля
                {
                    x = _x;
                    y = _y;

                    for(int j = 0; j < number_of_deck; j++)
                    {
                        mass[x][y] = 1;
                        massive[row][0] = { x };
                        massive[row][1] = { y };
                        row++;
                        switch(dir)
                        {
                            case(0): x++; break;
                            case(1): y++; break;
                            case(2): x--; break;
                            case(3): y--; break;
                        }
                    }
                    number_of_ships++;
                    count++;
                }

        }
    }
    for(int i = 0; i < 20; i++)                                  //Отправка координат в глобальный вектор
    {
        QVector<int> tempmasstor;
        for(int j = 0; j < 2; j++)
            tempmasstor.push_back(massive[i][j]+1);
        ships_partner.push_back(tempmasstor);
        //qDebug() << "mymasstor " << i << ": " << ships_partner.value(i);
    }
}

bool MainWindow::generate_ships_me()        //Проверка возможности расстановки
{
    if(count == 0 || count == 4 || count == 7 || count == 10 || count == 12 || count == 14 || count > 15)
    {
        for(int i = 0; i < ships_my.size(); i++)
            if((hit_x == ships_my.value(i).value(0) && hit_y == ships_my.value(i).value(1)) ||
               (hit_x+1 == ships_my.value(i).value(0) && hit_y == ships_my.value(i).value(1)) ||
               (hit_x-1 == ships_my.value(i).value(0) && hit_y == ships_my.value(i).value(1)) ||
               (hit_x == ships_my.value(i).value(0) && hit_y+1 == ships_my.value(i).value(1)) ||
               (hit_x == ships_my.value(i).value(0) && hit_y-1 == ships_my.value(i).value(1)) ||
               (hit_x+1 == ships_my.value(i).value(0) && hit_y+1 == ships_my.value(i).value(1)) ||
               (hit_x+1 == ships_my.value(i).value(0) && hit_y-1 == ships_my.value(i).value(1)) ||
               (hit_x-1 == ships_my.value(i).value(0) && hit_y+1 == ships_my.value(i).value(1)) ||
               (hit_x-1 == ships_my.value(i).value(0) && hit_y-1 == ships_my.value(i).value(1)))
                return false;
    }
    else
    {
        for(int i = 0; i < ships_my.size(); i++)
        {
            if(((hit_x == ships_my.value(i).value(0) && hit_y == ships_my.value(i).value(1)) ||     //Запрет на точку в точку и диагонали
               (hit_x+1 == ships_my.value(i).value(0) && hit_y+1 == ships_my.value(i).value(1)) ||
               (hit_x+1 == ships_my.value(i).value(0) && hit_y-1 == ships_my.value(i).value(1)) ||
               (hit_x-1 == ships_my.value(i).value(0) && hit_y+1 == ships_my.value(i).value(1)) ||
               (hit_x-1 == ships_my.value(i).value(0) && hit_y-1 == ships_my.value(i).value(1))))
                return false;
        }
        if((hit_x-1 == ships_my.value(count-1).value(0) && hit_y == ships_my.value(count-1).value(1)) ||
           (hit_x+1 == ships_my.value(count-1).value(0) && hit_y == ships_my.value(count-1).value(1)) ||
           (hit_y-1 == ships_my.value(count-1).value(1) && hit_x == ships_my.value(count-1).value(0)) ||
           (hit_y+1 == ships_my.value(count-1).value(1) && hit_x == ships_my.value(count-1).value(0)))
            return true;
        else return false;
    }
    return true;
}

/*void MainWindow::ships_test()               //Тестовая расстановка кораблей
{
    QPen pen_ship;                      //Настройка кисти для заголовка и секций кораблей
    pen_ship.setWidth(30);
    pen_ship.setColor(Qt::cyan);
    int massive[20][2] = {{13, 1}, {13,2}, {13,3}, {13,4},
                          {16,1}, {17,1}, {18,1},
                          {16,3}, {17,3}, {18,3},
                          {13,7}, {14,7}, {16,7}, {17,7}, {19,7} , {20, 7},
                          {18, 10}, {20, 10} , {22,10}, {15, 10} };
    for(int i = 0; i < 20; i++)                                  //Отправка координат 4х-палубника в глобальный вектор
    {
        QVector<int> tempmasstor;
        for(int j = 0; j < 2; j++)
        {
            tempmasstor.push_back(massive[i][j]);
            scene_me->addLine(massive[i][0]*40-500, massive[i][1]*40-20, massive[i][0]*40-500, massive[i][1]*40-20, pen_ship);
        }
        ships_my.push_back(tempmasstor);
        //qDebug() << "mymasstor " << i << ": " << ships_partner.value(i);
    }
    ui->label_life2->setText(QVariant(ships_my.size()).toString());
    count = 20;
}*/

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QPoint p = e->pos();                //Возврат координат клика
    QPen pen_ship;                      //Настройка кисти для заголовка и секций кораблей
    pen_ship.setWidth(30);
    pen_ship.setColor(Qt::cyan);
    //________________________________________________________________________________________________________________________
    //Поле игрока (545-943, 155-545)                                                        //Используется только для расстановки кораблей
    int coord_x_me = (p.rx()-25)/40, coord_y_me = (p.ry()-115)/40;                          //Исправление погрешности клика
    hit_x = coord_x_me;                                                                     //Присваивание глобальным координатам - координаты клика
    hit_y = coord_y_me;
    if(545 <= p.rx() && p.rx() <= 943 && 155 <= p.ry() && p.ry() <= 545 && count < 20 && generate_ships_me() == true)                                            //Пиксельные размеры поля
    {
        scene_me->addLine(coord_x_me*40-500, coord_y_me*40-20, coord_x_me*40-500, coord_y_me*40-20, pen_ship);
        ships_my.push_back({coord_x_me, coord_y_me});
        //qDebug() << "pos X: " << coord_x_me << "   pos Y: " << coord_y_me;
        ui->label_life2->setText(QVariant(ships_my.size()).toString());
        count++;
    }
    //________________________________________________________________________________________________________________________
    //Поле противника (55-453, 155-545)
    if(55 <= p.rx() && p.rx() <= 453 && 155 <= p.ry() && p.ry() <= 545 && count == 20)          //Пиксельные размеры поля
    {
        if(ships_partner.size() == 0 || ships_my.size() == 0) //Game over
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Game over");
            if(ships_partner.size() == 0)
            {
                msgBox.setText("Вы победили!");
                msgBox.exec();
            }
            else
            {
                msgBox.setText("Победа противника!");
                msgBox.exec();
            }
            exit(0);
        }
        int coord_x = (p.rx()-15)/40, coord_y = (p.ry()-115)/40;                 //Исправление погрешности клика
        hit_x = coord_x;                                                         //Присваивание глобальным координатам - координаты клика
        hit_y = coord_y;
        qDebug() << "pos X: " << p.rx() << "   pos Y: " << p.ry();
        qDebug() << "pos X: " << coord_x << "   pos Y: " << coord_y;
        if(hit_me())                                                            //Если есть попадание
        {
            scene_partner->addLine(coord_x*40-20, coord_y*40-20, coord_x*40-20, coord_y*40-20, pen_ship);
        }
        else
        {
            shot_partner();
        }
        QPen pen;                                                                                           //Настройка кисти удара
        pen.setWidth(5);
        pen.setColor(Qt::red);
        scene_partner->addLine((coord_x-1)*40,(coord_y-1)*40, coord_x*40, coord_y*40,pen);                  //Прорисовка метки удара
        scene_partner->addLine(coord_x*40,(coord_y-1)*40, (coord_x-1)*40, coord_y*40,pen);
        ui->label_life1->setText(QVariant(ships_partner.size()).toString());
    }
    update();
}

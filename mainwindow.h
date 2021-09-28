 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QGraphicsScene>
#include <QMessageBox>

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QThread>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool hit_me();
    bool hit_partner();
    bool hit_partner_2();
    void shot_partner();
    void ship_generation();
    bool generate_ships_me();
  //  void ships_test();
protected:
    void mousePressEvent(QMouseEvent *e) override;


private:
    QVector <QVector <int> > shots_partner;
    QVector <QVector <int> > ships_partner;
    QVector <QVector <int> > ships_my;
    int hit_x, hit_y;
    int shot_p_x, shot_p_y;
    int count = 0;                                  //Количество моих кораблей(активно при расстановке)
    Ui::MainWindow *ui;
    QGraphicsScene  *scene_partner;                     //Объявляем графическую сцену
    QGraphicsScene  *scene_me;

};

#endif // MAINWINDOW_H

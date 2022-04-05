#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<database.h>
#include<dialog.h>
#include <QTimer>
#include<QDateTime>
#include<QScreen>   //用于获取屏幕尺寸大小
#include<QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //当前的h和m
    int hour,minute,second;

   QTimer*timer;

private slots:

    void updateTime();

    void on_start_clicked();

    void on_suspend_clicked();

    void on_finish_clicked();

private:
    Ui::MainWindow *ui;
    Database db;

    int screenWidth,screenHeight;

    bool isSuspend=false;    //记录是否暂停
    bool isBigWindow=true;    //记录是大窗口还是小窗口
    int mouseLeaveWindowTime=0;    //记录鼠标停留在窗口内的时间

    void toSmallWindow();

    void toBigWindow();

    //重写鼠标进入事件
    void enterEvent(QEvent*);
    //重写鼠标离开事件
    void leaveEvent(QEvent*);
    //重写鼠标释放事件，实现小窗口到大窗口的转换
    void mouseReleaseEvent(QMouseEvent*);

    //重写鼠标事件，实现小窗口下的任意位置拖动
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);


    bool mouseInWindow=false;
    QPoint m_point;
    QPoint smallWindowPos;
    bool notMove=true;  //用于区分小窗口下的点击是用于移动，还是用于变换成大窗口
};
#endif // MAINWINDOW_H

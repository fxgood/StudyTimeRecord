#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    bool res=db.getConnection();
    if(!res){
        //警告框，提示数据库连接失败，请检查相关配置
        Dialog*d=new Dialog("数据库连接失败，请检查相关配置!",this,true);
        d->setParent(this);
        d->show();  //用了modal模态窗口
        qDebug()<<"数据库连接失败";
    }
    else{
        qDebug()<<"数据库连接成功";
    }
    hour=0,minute=0,second=0;
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::updateTime);   //连接信号与槽
    //获取屏幕尺寸
    QScreen *screen = QGuiApplication::primaryScreen ();
    QRect screenRect =  screen->availableVirtualGeometry();
    screenWidth=screenRect.width();
    screenHeight=screenRect.height();
    //隐藏两个按钮
    ui->suspend->hide();
    ui->finish->hide();
    //窗口置顶
    toBigWindow();

}

void MainWindow::toSmallWindow(){
    isBigWindow=false;
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    show(); //大坑，重复设置setWindowFlags会调用hide
    setFixedSize(100,40);
    ui->label->move(0,0);
    //move(screenWidth/2-width()/2,0);
    move(smallWindowPos);  //移动到上次记录的位置
}

void MainWindow::toBigWindow(){
    isBigWindow=true;
    setWindowFlags(Qt::WindowStaysOnTopHint);
    show(); //大坑，重复设置setWindowFlags会调用hide
    setFixedSize(250,160);
    //移动窗口位置
    move(screenWidth/2-width()/2,screenHeight/2-height()/2);
    //移动时间标签位置
    ui->label->move(width()/2-ui->label->width()/2,height()/2-ui->label->height()*1.5);
    //移动三个按钮的位置
    ui->start->move(width()/2-ui->start->width()/2,height()-ui->start->height()*1.5);
    ui->suspend->move(width()/2-ui->suspend->width()*1.1,height()-ui->suspend->height()*1.5);
    ui->finish->move(width()/2+ui->finish->width()*0.1,height()-ui->finish->height()*1.5);

}


QString transformIntToStrTime(int t){
    if(t>=10){
        return QString::number(t);
    }
    return "0"+QString::number(t);
}


void MainWindow::updateTime(){
    //处理时间变化
    if(second<59){
        second++;
    }else{
        second=0;
        if(minute<59)
            minute++;
        else{
            minute=0;
            hour++;
        }
    }
    ui->label->setText(transformIntToStrTime(hour)+":"+transformIntToStrTime(minute)+":"+transformIntToStrTime(second));
    //处理鼠标离开进入事件
    if(!isBigWindow)
        return;
    if(mouseInWindow)
        mouseLeaveWindowTime=0;
    else
        mouseLeaveWindowTime++;
    qDebug()<<"鼠标离开窗口的时间"<<mouseLeaveWindowTime;
    if(mouseLeaveWindowTime>=3){
        mouseLeaveWindowTime=0;
        isBigWindow=false;
        toSmallWindow();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_start_clicked()
{
    timer->start(1000);
    isSuspend=false;
    ui->suspend->setText("暂停");
    ui->start->hide();
    ui->suspend->setVisible(true);
    ui->finish->setVisible(true);

}

void MainWindow::on_suspend_clicked()
{
    if(!isSuspend){
        isSuspend=true;
        ui->suspend->setText("继续");
        timer->stop();
    }else{
        isSuspend=false;
        ui->suspend->setText("暂停");
        timer->start(1000);
    }

}

void MainWindow::on_finish_clicked()
{
    //暂停计时器
    timer->stop();
    //变更ui
    ui->suspend->hide();
    ui->finish->hide();
    ui->start->show();
    ui->label->setText("00:00:00");
    //将已经累计的学习时间计入数据库
    QSqlQuery sql;
    sql.prepare("select worktime from everydayworktime where date=:d");
    //获取系统当前的日期
    QDateTime curDateTime=QDateTime::currentDateTime();
    int year=curDateTime.date().year();
    int month=curDateTime.date().month();
    int day=curDateTime.date().day();
    QString str=QString::number(year)+"-"+QString::number(month)+"-"+QString::number(day);
    qDebug()<<str;
    sql.bindValue(":d",str);
    sql.exec();
    int t=-1;
    while(sql.next()){
        t=sql.value("worktime").toInt();
    }

    bool res;
    if(t==-1){
        sql.prepare("insert into everydayworktime values(:d,:t)");
        sql.bindValue(":d",str);
        sql.bindValue(":t",hour*60+minute);
        qDebug()<<"写入时间"<<hour*60+minute;
        res=sql.exec();
    }else{
        sql.prepare("update everydayworktime set worktime=:t where date=:d");
        sql.bindValue(":d",str);
        sql.bindValue(":t",hour*60+minute+t);
        qDebug()<<"写入时间"<<hour*60+minute+t;
        res=sql.exec();
    }
    if(res){
        qDebug()<<"更新成功";
    }else{
        qDebug()<<"更新失败";
    }
    //归零计时
    hour=0,minute=0,second=0;
}


void MainWindow::enterEvent(QEvent*){
    mouseInWindow=true;
    qDebug()<<"鼠标进入了窗口";
}

void MainWindow::leaveEvent(QEvent*){
    mouseInWindow=false;
    qDebug()<<"鼠标离开了窗口";
}

void MainWindow::mouseReleaseEvent(QMouseEvent*){

    if(!isBigWindow && /*并且不是在拖动之后进行释放*/ notMove){
        toBigWindow();
    }
}

void  MainWindow::mousePressEvent(QMouseEvent *e){
    notMove=true;
    if (e->button() == Qt::LeftButton) {
            m_point = e->globalPos() - pos();
            e->accept();
        }
}

void  MainWindow::mouseMoveEvent(QMouseEvent *e){
    notMove=false;
    if (e->buttons() & Qt::LeftButton) {
        smallWindowPos=e->globalPos() - m_point;
        move(e->globalPos() - m_point);

        e->accept();
     }
}

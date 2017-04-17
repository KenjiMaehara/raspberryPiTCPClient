#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QFont>


int main(int argc, char *argv[])
{

#if 0
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QApplication app(argc,argv);
    QLabel* label = new QLabel("<h2><i>Hello Qt!</i></h2>");
    label->show();
    //return a.exec();
#endif

#if 0
    QApplication app(argc, argv);
    QPushButton* button = new QPushButton("Hello Qt!");
    button->resize(200,50);
    button->move(100,50);
    button->show();
    button->setFont( QFont("Times",15,QFont::Bold,true));
    return app.exec();

#endif


#if 0
    QApplication app(argc,argv);
    QLabel* label = new QLabel("こんにちはQt");
    label->show();
    return app.exec();
#endif


    QApplication app(argc,argv);
    QPushButton* button = new QPushButton("Hello Qt!");
    QPushButton* button2 = new QPushButton("Goodbye");
    button->show();
    button2->show();
    return app.exec();







}

#pragma once
#include<QWidget>
class QVBoxLayout;
class TaskInfoWindow:public QWidget{
    Q_OBJECT;
private:
    QVBoxLayout* VBox;
public:
   explicit TaskInfoWindow(QWidget *parent = nullptr);

};

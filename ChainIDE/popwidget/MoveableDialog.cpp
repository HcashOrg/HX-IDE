#include "popwidget/MoveableDialog.h"

MoveableDialog::MoveableDialog(QWidget *parent)
    : QDialog(parent)
    ,mouse_press(false)
{

}

void MoveableDialog::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }

}

void MoveableDialog::mouseMoveEvent(QMouseEvent *event)
{

    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }

}

void MoveableDialog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

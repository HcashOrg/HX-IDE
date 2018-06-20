#ifndef SINGLECONTEXTWIDGET_H
#define SINGLECONTEXTWIDGET_H

#include <QDockWidget>

class SingleContextWidget : public QDockWidget
{
public:
    explicit SingleContextWidget(const QString &title = "", QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
};

#endif // SINGLECONTEXTWIDGET_H

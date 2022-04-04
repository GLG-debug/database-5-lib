#ifndef SPACE_FILTER_H
#define SPACE_FILTER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QDebug>

class SpaceFilter : public QObject
{
protected:
    virtual bool eventFilter(QObject*, QEvent*) override;

public:
    explicit SpaceFilter(QObject *parent = nullptr);
};

#endif // SPACE_FILTER_H

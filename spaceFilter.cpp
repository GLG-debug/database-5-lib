#include "spaceFilter.h"

SpaceFilter::SpaceFilter(QObject *parent)
    : QObject(parent)
{
}

bool SpaceFilter::eventFilter(QObject *, QEvent *pevent) {
    auto type = pevent->type();

    switch (type) {
    case QEvent::FocusIn:{
        return false;
    } break;
    case QEvent::KeyPress: {
        if(static_cast<QKeyEvent *>(pevent)->key() == Qt::Key_Space) {
            return true;
        }
    } break;
    default:
        return false;
    }
    return false;
}

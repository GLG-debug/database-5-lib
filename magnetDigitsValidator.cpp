#include "magnetDigitsValidator.h"

MagnetValidator::MagnetValidator(QObject *parent, QChar spaceSymbol)
    : QValidator(parent)
    , m_spaceSymbol(spaceSymbol)
{
}

QValidator::State MagnetValidator::validate(QString &str, int &pos) const {
    magnet(str, pos);
    return Acceptable;
}

bool MagnetValidator::magnet(QString &str, int &pos) const noexcept {
    int newPos = str.indexOf( m_spaceSymbol );
    int symbolPos = str.lastIndexOf( QRegExp("\\d") );
    if(newPos != -1 && newPos < symbolPos) {
        str[newPos] = str[symbolPos];
        str[symbolPos] = m_spaceSymbol;
        pos = newPos + 1;
        return true;
    }
    return false;
}

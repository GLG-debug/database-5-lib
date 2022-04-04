#ifndef DATE_VALIDATOR_H
#define DATE_VALIDATOR_H

#include <QValidator>
#include <QDate>
#include <QDebug>

#include "magnetDigitsValidator.h"

/*
 * There must be an input mask 00.00.0000
*/

class DateValidator
    : public MagnetValidator
{
    Q_OBJECT

public:
    explicit DateValidator(QObject *, QChar) noexcept;
    virtual State validate(QString &, int &) const override;

signals:
    void invalidDateHasBeenEntered() const;
    void intermediateDateHasBeenEntered() const;
    void acceptableDateHasBeenEntered(const QDate &) const;

protected:
    QChar m_splitSymbol;
};

#endif // DATE_VALIDATOR_H

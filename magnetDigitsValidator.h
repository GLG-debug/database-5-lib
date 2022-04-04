#ifndef MAGNET_DIGITS_VALIDATOR_H
#define MAGNET_DIGITS_VALIDATOR_H

#include <QValidator>
#include <QLineEdit>

#include <algorithm>

class MagnetValidator : public QValidator
{
public:
    explicit MagnetValidator(QObject *parent = nullptr, QChar spaceSymbol = QChar());
    virtual State validate(QString &, int &) const override;

protected:
    bool magnet(QString &, int &) const noexcept;

private:
    QChar m_spaceSymbol;
};

#endif // MAGNET_DIGITS_VALIDATOR_H

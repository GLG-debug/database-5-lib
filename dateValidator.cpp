#include "dateValidator.h"

DateValidator::DateValidator(QObject *parent, QChar splitSymbol) noexcept
    : MagnetValidator(parent, /*spacer*/'_')
    , m_splitSymbol(splitSymbol)
{
}

QValidator::State DateValidator::validate(QString &str, int &pos) const {
    enum : unsigned { Day, Month, Year };

    const QRegExp dayRegExp( R"(0[1-9]|[12]\d|3[01]|[0-3]*\_+)" );
    const QRegExp monthRegExp( R"(0[1-9]|1[0-2]|[01]*\_+)" );
    const QRegExp yearRegExp( R"(\d{4}|\d*\_+)" );
    const QString digit( R"(\d)" );
    const QString digitOrSpacer( R"([\d\_])" );

    /*
     * There must be an input mask 99[splitter]99[splitter]9999;_
    */

    magnet(str, pos);
    /*--- Additional variables ---*/
    QString bufFormat = QString( R"(%2{2}\%1%2{2}\%1%2{4})" ).arg(m_splitSymbol);
    int bufIndex =  str.indexOf( QRegExp( bufFormat.arg( digitOrSpacer ) ) );
    /*--- Checking for correct input ---*/
    if(bufIndex < 0) {
        qDebug() << str.indexOf( QRegExp( bufFormat.arg( digitOrSpacer ) ) );
        emit invalidDateHasBeenEntered();
        return QValidator::Invalid;
    }
    QString date = str.mid(bufIndex, 10);

    /*--- Numeric input check ---*/
    auto splitList = date.split('.');
    bool dmy[3] = {
        dayRegExp.exactMatch( splitList[Day] )
        , monthRegExp.exactMatch( splitList[Month] )
        , yearRegExp.exactMatch( splitList[Year] )
    };
    if( !dmy[Day] || !dmy[Month] || !dmy[Year] ) {
        emit invalidDateHasBeenEntered();
        return QValidator::Invalid;
    }
    /*--- Check for fullness ---*/
    bufIndex = date.indexOf( QRegExp( bufFormat.arg( digit ) ) );
    if( bufIndex < 0 ) {
        emit intermediateDateHasBeenEntered();
        return QValidator::Intermediate;
    }
    /*--- Getting the date ---*/
    bufFormat = QString("dd%1MM%1yyyy").arg(m_splitSymbol);
    QDate result = QDate::fromString(date, bufFormat);
    if(result.isValid() ) {
        emit acceptableDateHasBeenEntered( result );
        return QValidator::Acceptable;
    }
    emit invalidDateHasBeenEntered();
    return QValidator::Invalid;
}



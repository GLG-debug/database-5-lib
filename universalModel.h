#ifndef UNIVERSAL_MODEL_H
#define UNIVERSAL_MODEL_H

#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

#include <QDebug>

class UniversalModel
    : public QSqlTableModel
{
    Q_OBJECT
public:
    enum class TableType {
        Marketplaces
        , Customers
        , Rental
        , NotSet
    } m_currentTable = TableType::NotSet;

public:
    explicit UniversalModel( QObject *parent = nullptr, QSqlDatabase dataBase = QSqlDatabase() ) noexcept;

private:
    bool setHeaderNames() noexcept;

public:
    using QSqlTableModel::removeRow;
    bool selectTable(TableType) noexcept;
    void execute(const QSqlQuery &) noexcept;
    void refresh() noexcept;

protected:
    virtual QString selectStatement() const override;

signals:
    void tableHasBeenSet(TableType type) const;
};

#endif // UNIVERSAL_MODEL_H

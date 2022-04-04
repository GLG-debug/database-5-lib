#include "universalModel.h"

UniversalModel::UniversalModel(QObject *parent, QSqlDatabase dataBase) noexcept
    : QSqlTableModel(parent, dataBase)
{
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

bool UniversalModel::setHeaderNames() noexcept {
    bool result = true;
    auto renameField = [this, &result](QString key, QString name) ->void {
        result &= setHeaderData( fieldIndex(key), Qt::Horizontal, name, Qt::DisplayRole );
    };

    switch (m_currentTable) {
    case TableType::Marketplaces: {
        renameField( "floor",          tr("Этаж"                   ) );
        renameField( "airConditioner", tr("Наличие кондиционера"   ) );
        renameField( "area",           tr("Площадь"                ) );
        renameField( "costPerDay",     tr("Стоимость аренды в день") );
    } break;
    case TableType::Customers: {
        renameField( "phone",          tr("Телефон"        ) );
        renameField( "address",        tr("Адрес"          ) );
        renameField( "contactPerson",  tr("Контактное лицо") );
        renameField( "invoiceDetails", tr("Реквизиты"      ) );
        renameField( "name",           tr("Название"       ) );
    } break;
    case TableType::Rental: {
        renameField( "customer_id",    tr("Код клиента"       ) );
        renameField( "endDate",        tr("Дата окончания"    ) );
        renameField( "marketplace_id", tr("Код торговой точки") );
        renameField( "startDate",      tr("Дата начала"       ) );
    } break;
    case TableType::NotSet:
        return true;
    default:
        return false;
    }

    for (int rowIndex = 0, size = record().count(); rowIndex < size; ++rowIndex) {
        setHeaderData( rowIndex, Qt::Horizontal, record().fieldName(rowIndex), Qt::UserRole );
    }
    return result;
}

QString UniversalModel::selectStatement() const {
    QString statement("SELECT * FROM %1\n");
    if ( not filter().isEmpty() ) {
        statement.append( "WHERE " + filter() );
    }
    return statement.arg( tableName() );
}

bool UniversalModel::selectTable(TableType type) noexcept {
    switch (type) {
    case TableType::Marketplaces: {
        setTable("marketplaces");
    } break;
    case TableType::Customers: {
        setTable("customers");
    } break;
    case TableType::Rental: {
        setTable("rental");
    } break;
    case TableType::NotSet: {
        setTable( QString() );
    } break;
    default:
        return false;
    }
    bool result = lastError().type() == QSqlError::NoError;
    if(result) {
        m_currentTable = type;
        setHeaderNames();
        select();
        emit tableHasBeenSet( m_currentTable );
    }
    return result;
}

void UniversalModel::execute(const QSqlQuery &command) noexcept {
    setQuery( command );
}

void UniversalModel::refresh() noexcept {
    setHeaderNames();
    select();
    emit tableHasBeenSet( m_currentTable );
}

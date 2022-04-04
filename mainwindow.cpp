#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_model( nullptr )
{
    setupUi(this);
    initializationOfModel();
    installationOfValidators();
    settingSignals();

    m_cbAirConditioner->addItem("Отсутствует", false);
    m_cbAirConditioner->addItem("Присутствует", true);

    auto eventFilter = new SpaceFilter(this);
    foreach(auto lineEdit, findChildren<ColouredLineEdit *>()) {
        lineEdit->installEventFilter(eventFilter);
    }
}

void MainWindow::initializationOfModel() noexcept {
    m_db = QSqlDatabase::addDatabase("QODBC");
    /*--- Warning! QSqlQuery::lastInsertId() doesn't work for this driver! ---*/
    m_db.setDatabaseName(
        "DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};"
//        "DRIVER={Driver do Microsoft Access (*.mdb)};"
        "FIL={MS Access};"
        "DBQ=C:\\DataForDevelopment\\Database.accdb"
//        "DBQ=C:\\DataForDevelopment\\Database.mdb"
    );
    if(not m_db.open()){
        QMessageBox error(QMessageBox::Critical
                          , "Error"
                          , QString("The database hasn't opened!\nDetails: %1").arg(m_db.lastError().text())
                          , QMessageBox::Yes);
        error.exec();
        QMetaObject::invokeMethod( this, "close", Qt::QueuedConnection );
    }

    m_model = new UniversalModel(this, m_db);
    m_tableView->setModel( m_model );

    connect( m_cbChoicesTable, qOverload<int>(&QComboBox::activated), [this]( int index ){
        m_model->selectTable( static_cast<UniversalModel::TableType>( index ) );
    });
    connect( m_model, &UniversalModel::tableHasBeenSet, this, &MainWindow::tableHasBeenChosen);
    connect( m_tableView, &QTableView::activated, this, &MainWindow::rowHasBeenSelected);
    emit m_cbChoicesTable->activated( m_cbChoicesTable->currentIndex() );
}

QString MainWindow::fieldName(const QObject *pobj) noexcept {
    QString &&objName = pobj->objectName();
    for(int i = 0, size = m_model->columnCount(); i < size; ++i) {
        QString &&fieldName = m_model->record().fieldName(i);
        bool coincidence = objName.contains(fieldName, Qt::CaseInsensitive);
        if(coincidence) {
            return std::move(fieldName);
        }
    }
    return QString();
}

bool MainWindow::inputCheck() const noexcept {
    bool result = true;
    foreach( QWidget *current, currentInputWidgets() ) {
        switch ( getType(current) ) {
        case WidgetType::QComboBox: {
            result &= true;
        } break;
        case WidgetType::QDoubleSpinBox: {
            result &= true;
        } break;
        case WidgetType::QLineEdit: {
            result &= true;
        } break;
        case WidgetType::QSpinBox: {
            result &= true;
        } break;
        default:
            return false;
        }
    }
    return result;
}

MainWindow::WidgetType MainWindow::getType(const QWidget *widget) const noexcept {
    if (qobject_cast<const QComboBox *>(widget) != 0) {
        return WidgetType::QComboBox;
    }
    if (qobject_cast<const QDoubleSpinBox *>(widget) != 0) {
        return WidgetType::QDoubleSpinBox;
    }
    if (qobject_cast<const QSpinBox *>(widget) != 0) {
        return WidgetType::QSpinBox;
    }
    if (qobject_cast<const QLineEdit *>(widget) != 0) {
        return WidgetType::QLineEdit;
    }
    return WidgetType::OtherType;
}

QVariant MainWindow::getData(const QWidget *widget) const noexcept {
    switch ( getType(widget) ) {
    case WidgetType::QComboBox:
        return qobject_cast<const QComboBox *>(widget)->currentData();
    case WidgetType::QDoubleSpinBox:
        return qobject_cast<const QDoubleSpinBox *>(widget)->value();
    case WidgetType::QSpinBox:
        return qobject_cast<const QSpinBox *>(widget)->value();
    case WidgetType::QLineEdit:
        return qobject_cast<const QLineEdit *>(widget)->text();
    default:
        return QVariant();
    }
}

std::vector<QWidget *> MainWindow::currentInputWidgets() const noexcept {
    auto widgets = m_swInputsData->findChildren<QWidget *>();
    std::vector<QWidget *> currentWidgets;

    foreach (const auto current, widgets) {
        if ( /*visible check*/ current->isVisible()
            /*type check*/ && getType(current) != WidgetType::OtherType
            /*prefix check*/ && current->objectName().left(2) == "m_"
        ) {
            currentWidgets.push_back(current);
        }
    }
    return currentWidgets;
}

void MainWindow::clickedAdd() noexcept {
    inputCheck();
#if SQL_COMMAND /*--- SQL command ---*/
    auto addComma = [](const QStringList &list) -> QString {
        QString result( list.front() );
        auto it = list.begin();
        auto end = list.end();
        while(++it != end) {
            result += ", " + (*it);
        }
        return result;
    };

    QStringList columns;
    QStringList values;

    auto record = m_model->record();
    foreach( QWidget *current, currentInputWidgets() ) {
        columns.push_back( fieldName(current) );
        values.push_back( getData(current).toString() );
    }

    QSqlQuery command( /* --- I can't find the id --- */
        QString(
            "INSERT INTO %1 (%2) VALUES (%3)"
        ).arg( m_model->tableName() ).arg( addComma(columns) ).arg( addComma(values) )
    );

    m_model->execute( command );
    m_model->refresh();
#else /*--- The program method ---*/
    auto buffer = m_model->record();
    foreach( QWidget *current, currentInputWidgets() ) {
        QString &&fn = fieldName(current);
        buffer.setValue( fn, getData(current) );
    }

    auto id = 0;
    buffer.setValue( "id", id );
    m_model->insertRecord(-1, buffer);
    //m_model->submitAll();
#endif
}

void MainWindow::clickedEdit() noexcept {
    inputCheck();
}

void MainWindow::clickedDelete() noexcept {
    auto *select = m_tableView->selectionModel();
    if( not select->hasSelection() ) {
        return;
    }
    auto messageBox = new QMessageBox(
        QMessageBox::Warning
        , "Удаление записи"
        , "Вы действительно хотите удалить выбранную запись?"
        , QMessageBox::Yes | QMessageBox::No
    );
    messageBox->setDefaultButton(QMessageBox::No);
    if(messageBox->exec() == QMessageBox::No) {
        return;
    }
    auto indexRow = select->currentIndex().row();
#if SQL_COMMAND /*--- SQL command ---*/
    auto id = m_model->record(indexRow).field("id").value().toULongLong();
    QSqlQuery command(
        QString(
            "DELETE FROM %1 WHERE id = %2"
        ).arg( m_model->tableName() ).arg( id )
    );
    m_model->execute( command );
    m_model->refresh();
#else /*--- The program method ---*/
    m_model->removeRow(indexRow);
    m_model->submitAll();
#endif

}

void MainWindow::clickedClear() noexcept {
    foreach (auto lineEdit, findChildren<QLineEdit *>() ) {
        lineEdit->clear();
    }
}

void MainWindow::rowHasBeenSelected(const QModelIndex &/*index*/) noexcept {

}

void MainWindow::tableHasBeenChosen(UniversalModel::TableType /*type*/) noexcept {
    m_tableView->resizeColumnsToContents();
    m_tableView->hideColumn(0);
}

void MainWindow::installationOfValidators() noexcept {
    m_lePhone->setValidator(
        new MagnetValidator( m_lePhone, /*spacer*/ '_')
    );

    for ( auto dateLineEdit : { m_leStartDate, m_leEndDate } ) {
        auto dateValidator = new DateValidator(dateLineEdit, /*splitter*/ '.');
        dateLineEdit->setValidator( dateValidator );

        connect( dateValidator, &DateValidator::invalidDateHasBeenEntered, [this, dateLineEdit]() {
          dateLineEdit->slotRedNotice();
          m_statusBar->showMessage("Ввёденная дата не является корректной", 2000);
        } );
        connect( dateValidator, &DateValidator::acceptableDateHasBeenEntered, [this, dateLineEdit](const QDate &newDate){
            auto dateKey = dateLineEdit->objectName();
//            if(m_inputData[dateKey] != newDate) {
//                m_inputData[dateKey] = newDate;
//                dateLineEdit->slotGreenNotice();
//                m_statusBar->showMessage("Ввёденная дата является корректной", 2000);
//            }
        } );
    }
}

void MainWindow::initializationOfMaps() noexcept {
//    for(const auto &pair : keys) {
//        m_inputData.insert( std::make_pair(pair.first, QVariant() ) );
//        m_headers.insert( pair );
//    }
}

void MainWindow::settingSignals() const noexcept {
    connect(m_pbClear, &QPushButton::clicked, this, &MainWindow::clickedClear);
    connect(m_pbAdd, &QPushButton::clicked, this, &MainWindow::clickedAdd);
    connect(m_pbEdit, &QPushButton::clicked, this, &MainWindow::clickedEdit);
    connect(m_pbDelete, &QPushButton::clicked, this, &MainWindow::clickedDelete);
}

void MainWindow::lineEditEventFilter() const noexcept {

}

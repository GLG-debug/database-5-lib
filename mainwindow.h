#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define SQL_COMMAND true

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QtWidgets>

#include <memory>
#include <map>

#include "dateValidator.h"
#include "spaceFilter.h"
#include "universalModel.h"

#include "ui_mainwindow.h"

class MainWindow
    : public QMainWindow
    , public Ui::MainWindow
{
public:
    enum class WidgetType {
        QLineEdit
        , QComboBox
        , QSpinBox
        , QDoubleSpinBox
        , OtherType
    };

public:
    MainWindow(QWidget *parent = nullptr);

/* --- LOGIC --- */
private:
    void initializationOfModel() noexcept;
    void installationOfValidators() noexcept;
    void initializationOfMaps() noexcept;
    void settingSignals() const noexcept;
    void lineEditEventFilter() const noexcept;

/* --- TOOLS --- */
private:
    QString fieldName(const QObject *) noexcept;
    bool inputCheck() const noexcept;
    WidgetType getType(const QWidget *) const noexcept;
    QVariant getData(const QWidget *) const noexcept;
    std::vector<QWidget *> currentInputWidgets() const noexcept;

public slots:
    void clickedAdd() noexcept;
    void clickedEdit() noexcept;
    void clickedDelete() noexcept;
    void clickedClear() noexcept;
    void rowHasBeenSelected(const QModelIndex &) noexcept;
    void tableHasBeenChosen(UniversalModel::TableType type) noexcept;

private:
    QSqlDatabase m_db;
    UniversalModel *m_model;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <string>
#include "source/interface/i_ved_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    static std::unique_ptr<ved::i_ved_manager> uptr_ved_manager_;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void signalToTable();

private slots:

    void on_RunD_clicked();

    void on_Create_clicked();

    void on_Copy_clicked();

    void on_Mount_clicked();

    void CreateTable();

    void on_StopD_clicked();

    void on_Unmount_clicked();

    void on_Refresh_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:

    void update_table(void);

    Ui::MainWindow *ui;
    QMessageBox message_;

    QStandardItemModel *smodel{};


};
#endif // MAINWINDOW_H

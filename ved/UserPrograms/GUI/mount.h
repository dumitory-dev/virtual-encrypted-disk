#ifndef MOUNT_H
#define MOUNT_H

#include <QDialog>
#include <QRegExpValidator>
#include "mainwindow.h"
#include "source/classes/exceptions/exception.h"

namespace Ui {
class Mount;
}

class Mount : public QDialog
{
    Q_OBJECT

public:
    explicit Mount(QWidget *parent = nullptr);
    ~Mount();

signals:
    void signalToTable();

private slots:
    void on_PathButton_clicked();

    void on_Ok_clicked();

    void on_Cancel_clicked();

private:
    Ui::Mount *ui;
    QMessageBox message_;
};

#endif // MOUNT_H

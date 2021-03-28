#ifndef COPY_H
#define COPY_H

#include <QDialog>
#include "mainwindow.h"
#include "source/classes/exceptions/exception.h"

namespace Ui {
class Copy;
}

class Copy : public QDialog
{
    Q_OBJECT

public:
    explicit Copy(QWidget *parent = nullptr);
    ~Copy();

signals:
    void signalToTable();

private slots:
    void on_Ok_clicked();

    void on_Cancel_clicked();

    void on_PathButton_clicked();

    void on_PathButton_2_clicked();

private:
    Ui::Copy *ui;
    QMessageBox message_;

};

#endif // COPY_H

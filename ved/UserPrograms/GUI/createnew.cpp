#include "createnew.h"
#include "ui_createnew.h"
#include <QFileDialog>
#include "source/classes/exceptions/exception.h"
#include <QMessageBox>

CreateNew::CreateNew(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CreateNew)
{
  ui->setupUi(this);
  ui->Crypt->addItem("RC4");
  ui->Crypt->addItem("AEC");
  ui->label->setVisible(false);
}

CreateNew::~CreateNew()
{
  delete ui;
}

void CreateNew::on_Ok_clicked()
{
  try {


    if(ui->Path->text().isEmpty()||ui->Password->text().isEmpty()||this->ui->spinBox->text().toULongLong() < 10){
        ui->label->setVisible(true);
        return;
      }


    int mode = this->ui->Crypt->itemData(this->ui->Crypt->currentIndex()+1).toInt();



    MainWindow::uptr_ved_manager_->create_file(
          ui->Path->text().replace('/','\\').toStdWString(),
          this->ui->spinBox->text().toULongLong() * 1024 * 1024,
          ui->Password->text().toStdString(),
          static_cast<Crypt>(++mode)
          );

    this->message_.setText("Success!");
    this->message_.exec();

    close();


  }
  catch (const ved::c_win_api_exception & error) {


    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromWCharArray(error.GetMessageW().c_str()));


  }
  catch(const std::exception & error){


    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromUtf8(error.what()));

  }

}

void CreateNew::on_Cancel_clicked()
{
  close();
}

void CreateNew::on_PathButton_clicked()
{
  ui->Path->setText(QFileDialog::getSaveFileName(this, "Get Any File","C://"));
}

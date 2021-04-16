#include "copy.h"
#include "ui_copy.h"
#include <QFileDialog>

Copy::Copy(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Copy)
{
  ui->setupUi(this);
  ui->label->setVisible(false);
}

Copy::~Copy()
{
  delete ui;
}

void Copy::on_Ok_clicked()
{


  try {

    if(ui->Dir->text().isEmpty()||ui->File->text().isEmpty()){
        ui->label->setVisible(true);
        return;
      }

    this->ui->label->setText("Process...");
    this->ui->label->setVisible(true);

    MainWindow::uptr_ved_manager_->copy_image(
          this->ui->File->text().replace('/','\\').toStdWString(),
          this->ui->Dir->text().replace('/','\\').toStdWString());

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

void Copy::on_Cancel_clicked()
{
  close();
}

void Copy::on_PathButton_clicked()
{
  ui->File->setText(QFileDialog::getOpenFileName(this, "Get Any File","C://"));
}

void Copy::on_PathButton_2_clicked()
{
  ui->Dir->setText(QFileDialog::getSaveFileName(this, "Get Any File","C://"));
}

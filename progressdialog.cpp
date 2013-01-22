#include "progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::ProgressDialog)
{
    _ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
    delete _ui;
}

void ProgressDialog::show()
{
    if (this->parentWidget())
    {
        this->move((this->parentWidget()->width() - this->width()) / 2,
                   (this->parentWidget()->height() - this->height()) / 2);
    }
    QDialog::show();
    qApp->processEvents();
}

void ProgressDialog::setValue(int done, int total)
{
    _ui->progressBar->setMaximum(total);
    _ui->progressBar->setValue(done);
}

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include "ui_progressdialog.h"

class ProgressDialog: public QDialog
{
    Q_OBJECT
public:
    ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();

public slots:
    void setValue(int done, int total);
    void show();

private:
    Ui::ProgressDialog *_ui;
};

#endif // PROGRESSDIALOG_H

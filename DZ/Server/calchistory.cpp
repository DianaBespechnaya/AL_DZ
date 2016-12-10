#include "calchistory.h"
#include "ui_calchistory.h"
#include <sstream>
#include <iostream>

CalcHistory::CalcHistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalcHistory)
{
    ui->setupUi(this);
}

CalcHistory::~CalcHistory()
{
    delete ui;
}

void CalcHistory::AddNewEntry(char* r,QString& Number) {
    std::stringstream ss(r);
    ui->textEdit->insertPlainText(Number); //Для окна истории целесообразно использовать неизменяемый виджет
    while(ss){
        char* temp= new char[100]();
        temp[99] ='\0';
        ss.read(temp,98);
        ui->textEdit->insertPlainText(temp);
        delete[] temp;
    }
    ui->textEdit->insertPlainText("\n");
}

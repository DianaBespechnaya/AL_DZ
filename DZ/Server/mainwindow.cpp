#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/scope_exit.hpp>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    hist = new CalcHistory;
    connect(s.get_acc(),SIGNAL(rezultready()),this,SLOT(rezultwelldone()));		//хорошо бы отделить gui
    connect(s.get_acc(),SIGNAL(col_clientchanged(int)),this,SLOT(set_new_col_client(int)));
    connect(s.get_acc(),SIGNAL(PortOK(unsigned short)),this,SLOT(set_port(unsigned short)));
    connect(s.get_acc(), SIGNAL(UpdateProcInWin(float)), this, SLOT(set_proc(float)));
    connect(this, SIGNAL(GetNewResult(char*,QString&)), hist, SLOT(AddNewEntry(char*,QString&)));
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if (s.switchOn)
        s.Stop();
    delete hist;
    delete ui;
}

void MainWindow::set_port(unsigned short port){
    QString temp= QString::fromStdString(to_string(port));
    ui->lineEdit_2->setText(temp);
}

void MainWindow::set_new_col_client(int col){
    QString temp= QString::fromStdString(to_string(col));
    ui->lineEdit_3->setText(temp);
}

void MainWindow::rezultwelldone(){
    if (ui->textEdit->toPlainText().toStdString() ==""){
        mpz_class rez;
        char* r = new char[MAX_INT];
        mpz_init(rez.get_mpz_t());
        rez=1;
        for (int i=0;i<rezult.size();i++){
            rez=rez*rezult[i];
        }
        mpz_get_str(r,10,rez.get_mpz_t());

        std::stringstream ss(r);
        while(ss){
            char* temp= new char[100]();
            temp[99] ='\0';
            ss.read(temp,98);
            ui->textEdit->insertPlainText(temp);
            delete[] temp;
        }

        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Base,Qt::green);
        ui->textEdit->setPalette(*palette);

        string Number = ui->lineEdit->text().toStdString();
        if(ui->radioButton->isChecked())
            Number = Number + "! = ";
        if(ui->radioButton_2->isChecked())
            Number = "(" + Number + ") = ";
        QString temp = QString::fromStdString(Number);
        emit GetNewResult(r,temp);
        delete[] r;
        delete palette;
    }
}

void MainWindow::on_pushButton_clicked()
{ 
    control.clear();
    rezult.clear();
    try{
        if (!(ui->radioButton->isChecked()||ui->radioButton_2->isChecked()))
            throw "Не выбрана операция.";
        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Base,Qt::white);
        ui->textEdit->setPalette(*palette);
        ui->textEdit->clear();
        mpz_class N, K;
        mpz_init(N.get_mpz_t());
        mpz_init(K.get_mpz_t());
        string str=ui->lineEdit->text().toStdString();
        BOOST_SCOPE_EXIT((palette)){
            delete palette;
       } BOOST_SCOPE_EXIT_END
        if (ui->radioButton->isChecked()) {
            if (str==""){
                throw "Неверный формат ввода, введитe числo в формате число1"; //char* в исключении
            }
            if (str=="0"){
                ui->textEdit->setText("1");
                QPalette *palette = new QPalette();
                palette->setColor(QPalette::Base,Qt::green);
                ui->textEdit->setPalette(*palette);
                ui->lineEdit_4->setText("100.000");
                string Number = ui->lineEdit->text().toStdString();
                Number = Number + "! = " + "1";
                QString temp = QString::fromStdString(Number);
                emit GetNewResult("1",temp);
                return;
            }
                mpz_set_str(N.get_mpz_t(),str.c_str(),10);
                Factor(N);
        }

        if (ui->radioButton_2->isChecked()) {
        std::istringstream iss(str, std::istringstream::in);
        string n, k;
        iss >> n; iss >> k;
        if (n=="" || k==""){
            throw "Неверный формат ввода, введите два числа в формате число1_число2";
        }
        mpz_set_str(N.get_mpz_t(),n.c_str(),10);
        mpz_set_str(K.get_mpz_t(),k.c_str(),10);
        if (N < K){
            ui->textEdit->setText("0");
            QPalette *palette = new QPalette();
            palette->setColor(QPalette::Base,Qt::green);
            ui->textEdit->setPalette(*palette);
            ui->lineEdit_4->setText("100.000");
            string Number = ui->lineEdit->text().toStdString();
            Number = "(" + Number + ") = ";
            QString temp = QString::fromStdString(Number);
            emit GetNewResult("0",temp);
            return;
        }
        Binom(N, K);
    }
    for (int i =0; i<tasks.size();++i){
    rezult.push_back(0);
    control.push_back(0);
    }

    if (!s.switchOn){
    s.Start();
    s.switchOn = true;
    }
    set_proc(0);
    }
   catch(char const* message){
       QMessageBox::warning(this,"Warning", message);
   }
}

void MainWindow::set_proc(float proc){
    QString temp= QString::fromStdString(to_string(proc));
    ui->lineEdit_4->setText(temp);
}



void MainWindow::on_pushButton_2_clicked()
{
    hist->show();
}

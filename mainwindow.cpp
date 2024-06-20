#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->EditPin->setEchoMode(QLineEdit::Password);

    connect(ui->pushButtonLogin, &QPushButton::clicked, this, &MainWindow::authenticateUser);
    connect(ui->pushButtonOpenPromo, &QPushButton::clicked, this, &MainWindow::revealPromoCode);

    initializePromoCodes();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::authenticateUser() // Проверка PIN-кода
{
    const QString pin = "1234";

    if (ui->EditPin->text() == pin) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->label->setText("Invalid pin!");
        ui->label->setStyleSheet("color:red");
    }
}


void MainWindow::initializePromoCodes() // Инициализация карточек с промокодами
{
    QGridLayout *layout = new QGridLayout;
    ui->promoCardsPage->setLayout(layout);

    int numCards = QRandomGenerator::global()->bounded(1, 6); // от 1 до 5


    for (int i = 0; i < numCards; ++i) {
        appendNewPromoCode();
    }
}




QString MainWindow::createRandomCode() // Создание случайного промокода
{
    const int codeLength = 4;
    QString randomCode;

    for (int i = 0; i < codeLength; ++i) {
        int randomAscii;
        int charType = QRandomGenerator::global()->bounded(3);

        if (charType == 0) {
            randomAscii = QRandomGenerator::global()->bounded(48, 58); // Цифры
        } else if (charType == 1) {
            randomAscii = QRandomGenerator::global()->bounded(65, 91); // Заглавные буквы
        } else {
            randomAscii = QRandomGenerator::global()->bounded(97, 123); // Строчные буквы
        }

        randomCode.append(QChar(randomAscii));
    }

    return randomCode;
}



void MainWindow::appendNewPromoCode() // Добавление новой карточки
{
    QString newCode = createRandomCode();
    promoCodeArray.append(newCode);

    QPushButton *newPromoCard = new QPushButton("XXXX");
    newPromoCard->setFixedSize(200, 100);
    newPromoCard->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    int row = promoCard.size() / 4;
    int col = promoCard.size() % 4;
    QGridLayout *layout = qobject_cast<QGridLayout*>(ui->promoCardsPage->layout());
    if (layout) {
        layout->addWidget(newPromoCard, row, col);
    }

    promoCard.append(newPromoCard);

}



void MainWindow::revealPromoCode() // Показать промокод на карточке
{
    int randomIndex = QRandomGenerator::global()->bounded(promoCard.size());
    promoCard[randomIndex]->setText(promoCodeArray[randomIndex]);
    promoCard[randomIndex]->setStyleSheet("color:blue");

    appendNewPromoCode();
}




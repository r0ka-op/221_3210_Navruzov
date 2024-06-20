#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QGridLayout>

#include <openssl/sha.h>

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
        encryptionKey = generateKey(pin);
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




// Шифрование промокода
QByteArray MainWindow::encrypter(const QString &promoCode)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        // Обработка ошибки инициализации
        return QByteArray();
    }

    QByteArray encryptedCode;
    int len;
    int ciphertext_len;
    unsigned char iv[AES_BLOCK_SIZE];
    RAND_bytes(iv, AES_BLOCK_SIZE); // Генерируем случайный вектор инициализации

    encryptedCode.resize(promoCode.size() + AES_BLOCK_SIZE);

    // Инициализируем шифрование
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, reinterpret_cast<const unsigned char*>(encryptionKey.data()), iv)) {
        // Обработка ошибки инициализации
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Обновляем буфер с зашифрованными данными
    if (1 != EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(encryptedCode.data()), &len, reinterpret_cast<const unsigned char*>(promoCode.toUtf8().data()), promoCode.size())) {
        // Обработка ошибки обновления
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    ciphertext_len = len;

    // Завершаем шифрование
    if (1 != EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(encryptedCode.data()) + len, &len)) {
        // Обработка ошибки завершения
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    ciphertext_len += len;

    encryptedCode.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
    return encryptedCode;
}

QString MainWindow::decrypter(const QByteArray &encryptedCode)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        // Обработка ошибки инициализации
        return QString();
    }

    QByteArray decryptedCode;
    int len;
    int plaintext_len;
    unsigned char iv[AES_BLOCK_SIZE];
    RAND_bytes(iv, AES_BLOCK_SIZE); // Генерируем случайный вектор инициализации

    decryptedCode.resize(encryptedCode.size());

    // Инициализируем дешифрование
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, reinterpret_cast<const unsigned char*>(encryptionKey.data()), iv)) {
        // Обработка ошибки инициализации
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }

    // Обновляем буфер с расшифрованными данными
    if (1 != EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decryptedCode.data()), &len, reinterpret_cast<const unsigned char*>(encryptedCode.data()), encryptedCode.size())) {
        // Обработка ошибки обновления
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }
    plaintext_len = len;

    // Завершаем дешифрование
    if (1 != EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decryptedCode.data()) + len, &len)) {
        // Обработка ошибки завершения
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }
    plaintext_len += len;

    decryptedCode.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return QString::fromUtf8(decryptedCode);
}

// Функция для генерации ключа шифрования
QByteArray MainWindow::generateKey(const QString &pin)
{
    QByteArray key(SHA256_DIGEST_LENGTH, 0);
    SHA256(reinterpret_cast<const unsigned char*>(pin.toUtf8().data()), pin.size(), reinterpret_cast<unsigned char*>(key.data()));
    key.resize(AES_BLOCK_SIZE);
    return key;
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

    QByteArray encryptedText = encrypter(newCode);
    encryptedCodes.append(encryptedText);

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

    QString decryptedText = decrypter(encryptedCodes[randomIndex]);
    promoCard[randomIndex]->setText(decryptedText);
    promoCard[randomIndex]->setStyleSheet("color:blue");

    appendNewPromoCode();
}




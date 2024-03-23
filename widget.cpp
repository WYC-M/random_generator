#include "widget.h"
#include "ui_widget.h"

#include <QString>
#include <QPushButton>
#include <QTextBrowser>
#include <QRandomGenerator>
#include <QTextCharFormat>
#include <QColor>
#include <QMessageBox>
#include <QTextOption>

#include <windows.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->minBox->setPrefix("最小值: ");
    ui->minBox->setRange(0, 1024);
    ui->minBox->setValue(0);
    ui->maxBox->setPrefix("最大值: ");
    ui->maxBox->setRange(0, 1024);
    ui->maxBox->setValue(10);

    QTextCharFormat initializeFormat;
    initializeFormat.setFontPointSize(80);
    initializeFormat.setForeground(QColor("red"));
    initializeFormat.setFontWeight(800);
    ui->textBrowser->setCurrentCharFormat(initializeFormat);

    QTextOption option;
    option.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->textBrowser->document()->setDefaultTextOption(option);

    setWindowTitle("Random Generator");

    int random=0;
    int min=0;
    int max=0;

    // connect(ui->minBox,&QSpinBox::valueChanged,[=]()mutable{min = ui->minBox->value();}); // 获取最小值
    // connect(ui->maxBox,&QSpinBox::valueChanged,[=]()mutable{max = ui->maxBox->value();}); // 获取最大值

    // 按下按钮生成随机数
    connect(ui->startBtn,&QPushButton::clicked,[=]()mutable{
        // 读取最大值最小值
        min=ui->minBox->value();
        max=ui->maxBox->value();

        // 当最大值小于最小值时报错
        if(min>=max){
            QMessageBox::warning(this,"警告","最大值不得小于等于最小值");
            return;
        }

        // 生成随机数
        random=QRandomGenerator::global()->bounded(min,max+1);

        // 清屏
        ui->textBrowser->clear();

        // 输出随机数
        ui->textBrowser->insertPlainText(QString::number(random));
    });
}

Widget::~Widget()
{
    delete ui;
}

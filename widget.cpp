#include "widget.h"
#include "ui_widget.h"

#include <QString>
#include <QPushButton>
#include <QTextBrowser>
#include <QRandomGenerator>
#include <QTextCharFormat>
#include <QMessageBox>
#include <QDebug>
#include <QList>
#include <QFile>
#include <QIODevice>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QStringList>

// #include <QColor>
// #include <QTextOption>
// #include <windows.h>

#define FONT_SIZE 16 // 默认作业

Widget::Widget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 设置最小值文本，范围
    ui->minBox->setPrefix("最小值:");
    ui->minBox->setRange(0, 1024);
    ui->minBox->setValue(0);

    // 最大值
    ui->maxBox->setPrefix("最大值:");
    ui->maxBox->setRange(0, 1024);
    ui->maxBox->setValue(10);

    // 生成数量
    ui->genNum->setValue(1);
    ui->genNum->setRange(1, 256);

    // 字号
    ui->setSize->setValue(FONT_SIZE);
    ui->setSize->setRange(5, 50);

    // 设置输出日志默认勾选
    ui->isLog->setChecked(1);

    QTextCharFormat initializeFormat;
    initializeFormat.setFontPointSize(FONT_SIZE);
    // initializeFormat.setForeground(QColor("red"));
    // initializeFormat.setFontWeight(800);
    ui->textBrowser->setCurrentCharFormat(initializeFormat);

    // QTextOption option;
    // option.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // ui->textBrowser->document()->setDefaultTextOption(option);

    setWindowTitle("随机数生成器"); // 设置标题

    int random = 0;    // 随机数
    int subRandom = 0; // 随机下标
    int min = 0;       // 最大值
    int max = 0;       // 最小值
    int i_genNum = 0;  // 生成数量

    bool b_isLog = 0;    // 是否生成日志
    bool b_useFile = 0;  // 是否使用list.txt
    bool b_isRepeat = 0; // 是否重复

    // connect(ui->minBox,&QSpinBox::valueChanged,[=]()mutable{min = ui->minBox->value();}); // 获取最小值
    // connect(ui->maxBox,&QSpinBox::valueChanged,[=]()mutable{max = ui->maxBox->value();}); // 获取最大值

    // 按下按钮生成随机数
    connect(ui->startBtn, &QPushButton::clicked, [=]() mutable
        {
            // 检测是否勾选某些 checkBox
            if (ui->isLog->isChecked())b_isLog = 1;
            if (ui->isRepeat->isChecked())b_isRepeat = 1;
            // if (ui->useFile->isChecked())b_useFile = 1;

            // 读取最大值，最小值，生成数量
            min = ui->minBox->value();
            max = ui->maxBox->value();
            i_genNum = ui->genNum->value();

            // 写入文件所需对象
            // 输出日志
            QFile fLog("data/log.txt");
            QTextStream sLog(&fLog);

            if (b_isLog == 1) // 输出日志（日期部分）
            {
                QDateTime time = QDateTime::currentDateTime(); // 获取当前时间

                // 当文件夹不存在时创建
                QDir dir(QDir::currentPath());
                if (!dir.exists("data"))dir.mkdir("data");

                fLog.open(QIODevice::Append); // 打开文件
                sLog << "[" << time.toString("yyyy.MM.dd hh:mm:ss zzz ddd") << "] "; // 写入文件
            }
            // qDebug()<<i_genNum;

            // 当最大值小于最小值时报错
            if (min >= max)
            {
                QMessageBox::warning(this, "警告", "最大值不得小于等于最小值");
                if (b_isLog == 1)sLog << "error" << "\n"; // 输出日志
                return;
            }

            if (b_isRepeat == 1 && b_useFile == 0) // 重复，不使用 list.txt
            {
                ui->textBrowser->clear(); // 清屏
                for (int i = 0;i < (i_genNum - 1);i++)
                {
                    random = QRandomGenerator::global()->bounded(min, max + 1);       // 生成随机数
                    ui->textBrowser->insertPlainText(QString::number(random) + ", "); // 输出随机数

                    if (b_isLog == 1)sLog << random << ", "; // 输出日志
                }
                random = QRandomGenerator::global()->bounded(min, max + 1); // 生成随机数
                ui->textBrowser->insertPlainText(QString::number(random));  // 输出随机数

                if (b_isLog == 1)sLog << random << "\n"; // 输出日志
                fLog.close();
            }

            if (b_isRepeat == 0 && b_useFile == 0) // 不重复，不使用 list.txt
            {
                if (i_genNum > max - min + 1)
                {
                    QMessageBox::warning(this, "警告", "在不可重复模式下，生成数量不得大于 (最大值-最小值+1)");
                    if (b_isLog == 1)sLog << "error" << "\n"; // 输出日志
                    return;
                }

                ui->textBrowser->clear(); // 清屏

                QList<int> randomList;
                for (int i = 0;i < max - min + 1;i++)randomList.append(min + i);

                // qDebug()<<randomList;

                for (int i = 0;i < (i_genNum - 1);i++)
                {
                    subRandom = QRandomGenerator::global()->bounded(0, randomList.count()); // 生成随机下标
                    // random=randomList[subRandom];
                    random = randomList.takeAt(subRandom); // 随机数等于数组随机下标所对应的，删除下标对应的数字
                    ui->textBrowser->insertPlainText(QString::number(random) + ", "); // 输出随机数

                    if (b_isLog == 1)sLog << random << ", "; // 当输出日志时
                }
                subRandom = QRandomGenerator::global()->bounded(0, randomList.count()); // 生成随机下标
                // random=randomList[subRandom];
                random = randomList.takeAt(subRandom); // 随机数等于数组随机下标所对应的，删除下标对应的数字
                ui->textBrowser->insertPlainText(QString::number(random)); // 输出随机数

                if (b_isLog == 1)sLog << random << "\n"; // 当输出日志时
                fLog.close();
            }
        });

    connect(ui->setSize, &QSpinBox::valueChanged, [=]()
        { // 当字号变化时
            int size = ui->setSize->value(); // 获取 setSize 当前值

            // 调节字号
            QTextCharFormat format;
            format.setFontPointSize(size);
            ui->textBrowser->setCurrentCharFormat(format);
        });


    connect(ui->delLog, &QPushButton::clicked, []()
        { // 清除日志
            QDir dir(QDir::currentPath());
            dir.remove("data/log.txt");
        });

    connect(ui->listBtn, &QPushButton::clicked, [=]() mutable
        {
            if (ui->isLog->isChecked())b_isLog = 1;

            // 写入文件所需对象
            // 输出日志
            QFile fLog("data/log.txt");
            QTextStream sLog(&fLog);

            if (b_isLog == 1) // 输出日志（日期部分）
            {
                QDateTime time = QDateTime::currentDateTime(); // 获取当前时间

                // 当文件夹不存在时创建
                QDir dir(QDir::currentPath());
                if (!dir.exists("data"))dir.mkdir("data");

                fLog.open(QIODevice::Append); // 打开文件
                sLog << "[" << time.toString("yyyy.MM.dd hh:mm:ss zzz ddd") << "] "; // 写入文件
            }

            // 读取list.txt
            QFile fList("data/list.txt");
            QTextStream sList(&fList);

            QStringList strList = {};

            QFileInfo fInfo("data/list.txt");
            if (!fInfo.exists()) // 当文件不存在时
            {
                QMessageBox::warning(this, "警告", "data/list.txt 不存在");
                if (b_isLog == 1)sLog << "error" << "\n"; // 输出日志
                return;
            }

            ui->textBrowser->clear(); // 清屏

            fList.open(QIODevice::ReadOnly | QIODevice::Text); // 打开文件

            QString listStr = sList.readAll(); // 读取文件
            fList.close(); // 关闭文件

            if (listStr.isEmpty()) // 当文件为空时
            {
                QMessageBox::warning(this, "警告", "data/list.txt 不得为空");
                if (b_isLog == 1)sLog << "error" << "\n"; // 输出日志
                return;
            }

            strList = listStr.trimmed().replace(" ", "").replace("\n", "").split(','); // 分割listStr

            std::random_shuffle(strList.begin(), strList.end());  // 打乱数组
            ui->textBrowser->insertPlainText(strList.join(", ")); // 输出随机数
            if (b_isLog == 1)sLog << strList.join(", ") << "\n";  // 输出日志
        });
}

Widget::~Widget()
{
    delete ui;
}

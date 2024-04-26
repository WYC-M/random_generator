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
#include <QFont>

// #include <algorithm>
// #include <random>
// #include <iostream>
// #include <QColor>
// #include <QTextOption>
// #include <windows.h>

#define FONT_SIZE 16            // 默认字号
#define LOG_PATH "data/log.txt" // 日志路径
#define LIST_PATH "data/list.txt"
#define DIR_DATA "data"         // 数据文件夹

Widget::Widget(QWidget* parent)
	: QWidget(parent), ui(new Ui::Widget)
{
	ui->setupUi(this);

	// 设置文本，范围
	// 最小值
	ui->minBox->setPrefix("最小值:");
	ui->minBox->setRange(0, 1024);
	ui->minBox->setValue(0);
	// 最大值
	ui->maxBox->setPrefix("最大值:");
	ui->maxBox->setRange(0, 1024);
	ui->maxBox->setValue(10);

	// 设置生成数量
	ui->genNum->setValue(1);
	ui->genNum->setRange(1, 256);

	// 设置字号
	ui->setSize->setValue(FONT_SIZE);
	ui->setSize->setRange(5, 50);

	// 设置输出日志默认勾选
	ui->isLog->setChecked(1);
	// 默认不粗体
	ui->isBold->setChecked(0);

	QTextCharFormat initializeFormat;
	initializeFormat.setFontPointSize(FONT_SIZE);
	// initializeFormat.setForeground(QColor("red"));
	initializeFormat.setFontWeight(QFont::Normal);
	ui->textBrowser->setCurrentCharFormat(initializeFormat);

	// QTextOption option;
	// option.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	// ui->textBrowser->document()->setDefaultTextOption(option);

	setWindowTitle("随机数生成器"); // 设置标题

	// int random = 0;    // 随机数
	// int subRandom = 0; // 随机下标
	int min = 0;       // 最大值
	int max = 0;       // 最小值
	int i_genNum = 0;  // 生成数量

	bool b_isLog = 0;    // 是否生成日志
	// bool b_useFile = 0;  // 是否使用list.txt
	bool b_isRepeat = 0; // 是否重复
	bool b_isBold = 0;   // 是否粗体

	// connect(ui->minBox,&QSpinBox::valueChanged,[=]()mutable{min = ui->minBox->value();}); // 获取最小值
	// connect(ui->maxBox,&QSpinBox::valueChanged,[=]()mutable{max = ui->maxBox->value();}); // 获取最大值

	// 按下按钮生成随机数
	connect(ui->startBtn, &QPushButton::clicked, [=]() mutable
		{
			// 检测是否勾选某些 checkBox
			if (ui->isLog->isChecked()) { b_isLog = 1; }
			if (ui->isRepeat->isChecked()) { b_isRepeat = 1; }
			// if (ui->useFile->isChecked())b_useFile = 1;

			// 读取最大值，最小值，生成数量
			min = ui->minBox->value();
			max = ui->maxBox->value();
			i_genNum = ui->genNum->value();

			// 写入文件所需对象
			// 输出日志
			QFile fLog(LOG_PATH);
			QTextStream sLog(&fLog);

			if (b_isLog) // 输出日志（日期部分）
			{
				// 当文件夹不存在时创建
				QDir dir(QDir::currentPath());
				if (!dir.exists(DIR_DATA)) { dir.mkdir(DIR_DATA); }

				QDateTime time = QDateTime::currentDateTime(); // 获取当前时间
				fLog.open(QIODevice::Append); // 打开文件
				sLog << "[" << time.toString("yyyy.MM.dd hh:mm:ss zzz ddd") << "] "; // 写入文件
			}
			// qDebug()<<i_genNum;

			// 当最大值小于最小值时报错
			if (min >= max)
			{
				QMessageBox::warning(this, "警告", "最大值不得小于等于最小值");
				if (b_isLog) { sLog << "error" << "\n"; } // 输出日志
				return;
			}

			if (b_isRepeat) // 重复，不使用 list.txt
			{
				ui->textBrowser->clear(); // 清屏

				QString outputStr = "";

				for (int i = 0; i < (i_genNum - 1); i++)
				{
					outputStr = outputStr + QString::number(QRandomGenerator::global()->bounded(min, max + 1)) + ", ";
					// random = QRandomGenerator::global()->bounded(min, max + 1);       // 生成随机数
					// ui->textBrowser->insertPlainText(QString::number(random) + ", "); // 输出随机数

					// if (b_isLog) { sLog << random << ", "; } // 输出日志
				}
				outputStr = outputStr + QString::number(QRandomGenerator::global()->bounded(min, max + 1));
				// random = QRandomGenerator::global()->bounded(min, max + 1); // 生成随机数
				// ui->textBrowser->insertPlainText(QString::number(random));  // 输出随机数
				ui->textBrowser->insertPlainText(outputStr); // 输出随机数
				if (b_isLog) { sLog << outputStr << "\n"; }  // 输出日志
				// if (b_isLog) { sLog << random << "\n"; }  // 输出日志

				fLog.close();
			}

			if (!b_isRepeat) // 不重复，不使用 list.txt
			{
				if (i_genNum > max - min + 1)
				{
					QMessageBox::warning(this, "警告", "在不可重复模式下，生成数量不得大于 (最大值-最小值+1)");
					if (b_isLog) { sLog << "error" << "\n"; } // 输出日志
					return;
				}

				ui->textBrowser->clear(); // 清屏

				QList<int> randomList; // 初始化列表
				for (int i = 0; i < max - min + 1; i++) { randomList.append(min + i); }

				// qDebug()<<randomList;

				QString outputStr = {};
				for (int i = 0; i < (i_genNum - 1); i++)
				{
					outputStr = outputStr
						+ QString::number(randomList.takeAt(QRandomGenerator::global()->bounded(0, randomList.count())))
						+ ", ";
					// subRandom = QRandomGenerator::global()->bounded(0, randomList.count()); // 生成随机下标
					// random=randomList[subRandom];
					// random = randomList.takeAt(subRandom); // 随机数等于数组随机下标所对应的，删除下标对应的数字
					// ui->textBrowser->insertPlainText(QString::number(random) + ", "); // 输出随机数

					// if (b_isLog)
					//     sLog << random << ", "; // 当输出日志时
				}
				outputStr = outputStr + QString::number(randomList.takeAt(QRandomGenerator::global()->bounded(0, randomList.count())));
				// subRandom = QRandomGenerator::global()->bounded(0, randomList.count()); // 生成随机下标
				// random=randomList[subRandom];
				// random = randomList.takeAt(subRandom); // 随机数等于数组随机下标所对应的，删除下标对应的数字
				// ui->textBrowser->insertPlainText(QString::number(random)); // 输出随机数
				ui->textBrowser->insertPlainText(outputStr); // 输出随机数

				if (b_isLog) { sLog << outputStr << "\n"; } // 当输出日志时
				fLog.close();
			}
		});

	// 当字号变化时
	connect(ui->setSize, &QSpinBox::valueChanged, [=]()
		{
			int size = ui->setSize->value(); // 获取 setSize 当前值

			// 调节字号
			QTextCharFormat format;
			format.setFontPointSize(size);
			if (b_isBold) { format.setFontWeight(QFont::Black); }   // 黑体
			if (!b_isBold) { format.setFontWeight(QFont::Normal); } // 正常
			ui->textBrowser->setCurrentCharFormat(format);
		});

	// 清除日志
	connect(ui->delLog, &QPushButton::clicked, []()
		{
			QDir dir(QDir::currentPath());
			dir.remove(LOG_PATH);
		});

	// 打乱list.txt
	connect(ui->listBtn, &QPushButton::clicked, [=]() mutable
		{
			if (ui->isLog->isChecked()) { b_isLog = 1; }

			// 写入文件所需对象
			// 输出日志
			QFile fLog(LOG_PATH);
			QTextStream sLog(&fLog);

			if (b_isLog) // 输出日志（日期部分）
			{
				QDateTime time = QDateTime::currentDateTime(); // 获取当前时间

				// 当文件夹不存在时创建
				QDir dir(QDir::currentPath());
				if (!dir.exists(DIR_DATA)) { dir.mkdir(DIR_DATA); }

				fLog.open(QIODevice::Append); // 打开文件
				sLog << "[" << time.toString("yyyy.MM.dd hh:mm:ss zzz ddd") << "] "; // 写入文件（时间）
			}

			// 读取list.txt
			QFile fList(LIST_PATH);
			QTextStream sList(&fList);

			QFileInfo fInfo(LIST_PATH);
			if (!fInfo.exists()) // 当文件不存在时
			{
				QMessageBox::warning(this, "警告", "data/list.txt 不存在");
				if (b_isLog) { sLog << "error" << "\n"; } // 输出日志
				return;
			}

			fList.open(QIODevice::ReadOnly | QIODevice::Text); // 打开文件
			QString listStr = sList.readAll(); // 读取文件
			fList.close(); // 关闭文件

			QStringList strList = {}; // 存放元素的列表

			if (listStr.isEmpty()) // 当文件为空时
			{
				QMessageBox::warning(this, "警告", "data/list.txt 不得为空");
				if (b_isLog) { sLog << "error" << "\n"; } // 输出日志
				return;
			}

			ui->textBrowser->clear(); // 清屏

			strList = listStr.trimmed().replace(" ", "").replace("\n", "").split(','); // 分割listStr

			// 打乱数组
			// std::random_shuffle(strList.begin(), strList.end());  
			int j = strList.count();
			QString outputStr = "";
			for (int i = 0; i < j - 1; i++)
			{
				outputStr = outputStr
					+ strList.takeAt(QRandomGenerator::global()->bounded(0, strList.count()))
					+ ", ";
			}
			// subRandom = QRandomGenerator::global()->bounded(0, strList.count()); // 生成随机下标
			outputStr = outputStr + strList.takeAt(QRandomGenerator::global()->bounded(0, strList.count()));

			ui->textBrowser->insertPlainText(outputStr);     // 输出随机数
			if (b_isLog) { sLog << outputStr << "\n"; } // 输出日志
		});

	// 加粗
	connect(ui->isBold, &QCheckBox::stateChanged, [=]()mutable
		{
			// 转换粗体状态
			if (b_isBold) { b_isBold = 0; }
			else if (!b_isBold) { b_isBold = 1; }

			// qDebug() << b_isBold;

			int fontSize = ui->setSize->value(); // 获取 setSize 当前值

			// 转换黑体正常
			QTextCharFormat changeBoldFormat;
			changeBoldFormat.setFontPointSize(fontSize);
			if (b_isBold) { changeBoldFormat.setFontWeight(QFont::Black); }   // 黑体
			if (!b_isBold) { changeBoldFormat.setFontWeight(QFont::Normal); } // 正常
			ui->textBrowser->setCurrentCharFormat(changeBoldFormat);
		});
}

Widget::~Widget()
{
	delete ui;
}

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

#define DEFAULT_FONT_SIZE 18                // 默认字号
#define LOG_NAME "log.txt"                  // 日志文件名
#define LIST_PATH "data/list.txt"           // 列表路径
#define DATA_PATH "data/"                   // 数据文件夹
#define WINDOW_TITILE "随机数生成器"          // 窗口标题

#define ERR_MAX_LESS_MIN "ERR_MAX_LESS_MIN" // 报错：最大值不得小于等于最小值
#define ERR_GEN_NUM "ERR_GEN_NUM"           // 报错：在不可重复模式下，生成数量不得大于 (最大值-最小值+1)
#define ERR_NO_EXIST "ERR_NO_EXIST"         // 报错：data/list.txt 不存在
#define ERR_EMPTY "ERR_EMPTY"               // 报错：data/list.txt 不得为空

#define IS_LOG ui->isLog->isChecked()       // 是否生成日志
#define IS_REPEAT ui->isRepeat->isChecked() // 是否重复
#define IS_BOLD ui->isBold->isChecked()     // 是否黑体
#define GEN_NUM ui->genNum->value()         // 生成数量
#define MIN ui->minBox->value()             // 最大值
#define MAX ui->maxBox->value()             // 最小值
#define FONT_SIZE ui->setSize->value()      // 字号

Widget::Widget(QWidget* parent)
	: QWidget(parent), ui(new Ui::Widget)
{
	ui->setupUi(this);

	// 最小值
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
	ui->setSize->setValue(DEFAULT_FONT_SIZE);
	ui->setSize->setRange(5, 50);
	// 输出日志默认勾选
	ui->isLog->setChecked(1);
	// 默认不粗体
	ui->isBold->setChecked(0);
	// 字号字重
	QTextCharFormat initializeFormat;
	initializeFormat.setFontPointSize(DEFAULT_FONT_SIZE); // 默认字号
	initializeFormat.setFontWeight(QFont::Normal);        // 默认字重
	ui->textBrowser->setCurrentCharFormat(initializeFormat);
	// 标题
	setWindowTitle(WINDOW_TITILE);

	// 按下按钮生成随机数
	connect(ui->startBtn, &QPushButton::clicked, [=]() mutable
		{
			// 当最大值小于最小值时报错
			if (MIN >= MAX)
			{
				QMessageBox::warning(this, "警告", "最大值不得小于等于最小值");
				if (IS_LOG) { this->outputLog(LOG_NAME, DATA_PATH, ERR_MAX_LESS_MIN); }
				return;
			}

			if (IS_REPEAT) // 重复，不使用 list.txt
			{
				ui->textBrowser->clear(); // 清屏

				QString outputStr = "";

				for (int i = 0; i < (GEN_NUM - 1); i++)
				{
					outputStr = outputStr +
						QString::number(QRandomGenerator::global()->bounded(MIN, MAX + 1)) + ", ";
				}

				outputStr = outputStr
					+ QString::number(QRandomGenerator::global()->bounded(MIN, MAX + 1)); // 生成随机数并追加到outputStr
				ui->textBrowser->insertPlainText(outputStr);                              // 输出随机数
				if (IS_LOG) { this->outputLog(LOG_NAME, DATA_PATH, outputStr); }          // 输出日志
			}

			else // 不重复，不使用 list.txt
			{
				if (GEN_NUM > MAX - MIN + 1)
				{
					QMessageBox::warning(this, "警告", "在不可重复模式下，生成数量不得大于 (最大值-最小值+1)");
					if (IS_LOG) { this->outputLog(LOG_NAME, DATA_PATH, ERR_GEN_NUM); }
					return;
				}

				ui->textBrowser->clear(); // 清屏

				QList<int> randomList;
				for (int i = 0; i < MAX - MIN + 1; i++) { randomList.append(MIN + i); }

				QString outputStr = {};
				for (int i = 0; i < (GEN_NUM - 1); i++)
				{
					outputStr = outputStr
						+ QString::number(randomList.takeAt(QRandomGenerator::global()->bounded(0, randomList.count())))
						+ ", ";
				}

				outputStr = outputStr
					+ QString::number(randomList.takeAt(QRandomGenerator::global()->bounded(0, randomList.count())));
				ui->textBrowser->insertPlainText(outputStr); // 输出随机数
				if (IS_LOG) { this->outputLog(LOG_NAME, DATA_PATH, outputStr); }
			}
		});

	// 清除日志
	connect(ui->delLog, &QPushButton::clicked, []()
		{
			QDir dir(QDir::currentPath());
			dir.remove(QString(DATA_PATH) + QString(LOG_NAME));
		});

	// 打乱list.txt
	connect(ui->listBtn, &QPushButton::clicked, [=]() mutable
		{
			// 读取list.txt
			QFile f(LIST_PATH);
			QTextStream s(&f);

			QFileInfo fInfo(LIST_PATH);
			if (!fInfo.exists()) // 当文件不存在时
			{
				QMessageBox::warning(this, "警告", "data/list.txt 不存在");
				if (IS_LOG) { this->outputLog(LOG_NAME, DATA_PATH, ERR_NO_EXIST); } // 输出日志
				return;
			}

			QStringList strList = {}; // 存放元素的列表

			f.open(QIODevice::ReadOnly | QIODevice::Text); // 打开文件

			strList = s.readAll()
				.trimmed()
				.replace(" ", "")
				.replace("\n", "")
				.replace("\r", "")
				.split(','); // 分割文件内容

			if (strList.isEmpty()) // 当文件为空时
			{
				QMessageBox::warning(this, "警告", "data/list.txt 不得为空");
				if (IS_LOG) { outputLog(LOG_NAME, DATA_PATH, ERR_EMPTY); } // 输出日志
				return;
			}

			// 打乱数组
			int j = strList.count();
			QString outputStr = "";
			for (int i = 0; i < j - 1; i++)
			{
				outputStr = outputStr
					+ strList.takeAt(QRandomGenerator::global()->bounded(0, strList.count()))
					+ ", ";
			}
			outputStr = outputStr
				+ strList.takeAt(QRandomGenerator::global()->bounded(0, strList.count()));

			ui->textBrowser->clear(); // 清屏
			ui->textBrowser->insertPlainText(outputStr); // 输出随机数
			if (IS_LOG) { this->outputLog(LOG_NAME, DATA_PATH, outputStr); } // 输出日志
		});

	connect(ui->isBold, &QCheckBox::stateChanged, this, &Widget::changeSizeAndBold); // 当字重变化时
	connect(ui->setSize, &QSpinBox::valueChanged, this, &Widget::changeSizeAndBold); // 当字号变化时
}

// 输出日志
void Widget::outputLog(QString fileName, QString dataPath, QString outputStr)
{
	// 实例化输出日志所需对象
	QFile f(dataPath + fileName);
	QTextStream s(&f);

	// 当文件夹不存在时创建
	QDir dir(QDir::currentPath());
	if (!dir.exists(dataPath)) { dir.mkdir(dataPath); }

	QDateTime time = QDateTime::currentDateTime(); // 获取当前时间
	f.open(QIODevice::Append); // 打开文件
	s << "[" << time.toString("yyyy.MM.dd hh:mm:ss zzz ddd") << "] " << outputStr << "\n"; // 写入文件（时间+内容+换行）
	f.close();
}

// 调节字号和粗体
void Widget::changeSizeAndBold()
{
	QTextCharFormat format;

	format.setFontPointSize(FONT_SIZE); // 调节字号

	// 调节字重
	if (IS_BOLD) { format.setFontWeight(QFont::Black); }   // 黑体
	if (!IS_BOLD) { format.setFontWeight(QFont::Normal); } // 正常

	ui->textBrowser->setCurrentCharFormat(format); // 应用样式
}

Widget::~Widget()
{
	delete ui;
}

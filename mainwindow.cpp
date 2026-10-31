#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 剪切、复制：选中文字才可用
    connect(ui->plainTextEdit, &QPlainTextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
    connect(ui->plainTextEdit, &QPlainTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    // 粘贴：剪贴板有文本才可用
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [=](){
        QString txt = QApplication::clipboard()->text();
        ui->actionPaste->setEnabled(!txt.isEmpty());
    });
    // 初始状态：未选中时剪切复制禁用
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开文件","", "文本文件(*.txt);;所有文件(*.*)");
    if(fileName.isEmpty()) return;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) return;
    QTextStream in(&file);
    ui->plainTextEdit->setPlainText(in.readAll());
    file.close();
    this->setWindowTitle(fileName + " - 文本编辑器");
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,"保存文件","untitled.txt","文本文件(*.txt)");
    if(fileName.isEmpty()) return;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) return;
    QTextStream out(&file);
    out << ui->plainTextEdit->toPlainText();
    file.close();
    this->setWindowTitle(fileName + " - 文本编辑器");
    // 开启菜单显示图标
    this->setWindowIcon(QIcon(":/icons/new.png"));
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
}

//撤销
void MainWindow::on_actionUndo_triggered()
{
    ui->plainTextEdit->undo();
}
//恢复
void MainWindow::on_actionRedo_triggered()
{
    ui->plainTextEdit->redo();
}
//剪切
void MainWindow::on_actionCut_triggered()
{
    ui->plainTextEdit->cut();
}
//复制
void MainWindow::on_actionCopy_triggered()
{
    ui->plainTextEdit->copy();
}
//粘贴
void MainWindow::on_actionPaste_triggered()
{
    ui->plainTextEdit->paste();
}

void MainWindow::on_actionNew_triggered()
{
    ui->plainTextEdit->clear();        //清空编辑区
    this->setWindowTitle("无标题 - 文本编辑器");
}

void MainWindow::on_actionFind_triggered()
{
    QDialog dlg(this);
    dlg.setWindowTitle("查找");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QHBoxLayout *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("查找目标："));
    QLineEdit *editFind = new QLineEdit();
    row1->addWidget(editFind);
    layout->addLayout(row1);
    QCheckBox *checkCase = new QCheckBox("区分大小写");
    layout->addWidget(checkCase);
    QHBoxLayout *rowDir = new QHBoxLayout;
    rowDir->addWidget(new QLabel("方向"));
    QRadioButton *radioUp = new QRadioButton("向上(U)");
    QRadioButton *radioDown = new QRadioButton("向下(D)");
    radioDown->setChecked(true);
    rowDir->addWidget(radioUp);
    rowDir->addWidget(radioDown);
    layout->addLayout(rowDir);
    QHBoxLayout *rowBtn = new QHBoxLayout;
    QPushButton *btnFindNext = new QPushButton("查找下一个(F)");
    QPushButton *btnCancel = new QPushButton("取消");
    rowBtn->addWidget(btnFindNext);
    rowBtn->addWidget(btnCancel);
    layout->addLayout(rowBtn);
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnFindNext, &QPushButton::clicked, this, [&](){
        QString key = editFind->text();
        if(key.isEmpty()) return;
        QTextDocument::FindFlags flag;
        if(checkCase->isChecked())
            flag |= QTextDocument::FindCaseSensitively;
        if(radioUp->isChecked())
            flag |= QTextDocument::FindBackward;
        bool found = ui->plainTextEdit->find(key, flag);
        if(!found)
        {
            QMessageBox::information(this, "提示", "未找到");
        }
    });
    dlg.exec();
}

void MainWindow::on_actionReplace_triggered()
{
    QDialog dlg(this);
    dlg.setWindowTitle("替换");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addWidget(new QLabel("查找目标："));
    QLineEdit *editFind = new QLineEdit();
    layout->addWidget(editFind);
    layout->addWidget(new QLabel("替换为："));
    QLineEdit *editReplace = new QLineEdit();
    layout->addWidget(editReplace);
    QCheckBox *checkCase = new QCheckBox("区分大小写");
    layout->addWidget(checkCase);
    QHBoxLayout *rowDir = new QHBoxLayout;
    QRadioButton *radioUp = new QRadioButton("向上(U)");
    QRadioButton *radioDown = new QRadioButton("向下(D)");
    radioDown->setChecked(true);
    rowDir->addWidget(radioUp);
    rowDir->addWidget(radioDown);
    layout->addLayout(rowDir);
    QPushButton *btnFindNext = new QPushButton("查找下一个(F)");
    QPushButton *btnReplace = new QPushButton("替换(R)");
    QPushButton *btnReplaceAll = new QPushButton("全部替换(A)");
    QPushButton *btnCancel = new QPushButton("取消");
    layout->addWidget(btnFindNext);
    layout->addWidget(btnReplace);
    layout->addWidget(btnReplaceAll);
    layout->addWidget(btnCancel);
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnFindNext, &QPushButton::clicked, this, [&](){
        QString key = editFind->text();
        if(key.isEmpty()) return;
        QTextDocument::FindFlags flag;
        if(checkCase->isChecked()) flag |= QTextDocument::FindCaseSensitively;
        if(radioUp->isChecked()) flag |= QTextDocument::FindBackward;
        bool ok = ui->plainTextEdit->find(key, flag);
        if(!ok) QMessageBox::information(this,"提示","未找到");
    });
    connect(btnReplace, &QPushButton::clicked, this, [&](){
        QString key = editFind->text();
        QString repText = editReplace->text();
        if(key.isEmpty()) return;
        if(ui->plainTextEdit->textCursor().selectedText() == key)
        {
            ui->plainTextEdit->insertPlainText(repText);
        }
        QTextDocument::FindFlags flag;
        if(checkCase->isChecked()) flag |= QTextDocument::FindCaseSensitively;
        if(radioUp->isChecked()) flag |= QTextDocument::FindBackward;
        bool ok = ui->plainTextEdit->find(key, flag);
        if(!ok) QMessageBox::information(this,"提示","未找到");
    });
    connect(btnReplaceAll, &QPushButton::clicked, this, [&](){
        QString key = editFind->text();
        QString repText = editReplace->text();
        if(key.isEmpty()) return;
        int count = 0;
        ui->plainTextEdit->moveCursor(QTextCursor::Start);
        QTextDocument::FindFlags flag;
        if(checkCase->isChecked()) flag |= QTextDocument::FindCaseSensitively;
        while(ui->plainTextEdit->find(key, flag))
        {
            ui->plainTextEdit->insertPlainText(repText);
            count++;
        }
        QMessageBox::information(this, "提示", QString("一共替换 %1 处").arg(count));
    });
    dlg.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Dialog");
    dlg.resize(420,320);
    QVBoxLayout *vLayout = new QVBoxLayout(&dlg);
    QLabel *titleLabel = new QLabel("小牛文本编辑器");
    titleLabel->setStyleSheet("font-size:36px;font-weight:bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(titleLabel);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap("D:/qt/images/1.png").scaled(120,120,Qt::KeepAspectRatio));
    iconLabel->setFixedSize(120,120);
    hLayout->addWidget(iconLabel);
    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(new QLabel("开发者：周意畅"));
    infoLayout->addWidget(new QLabel("学号：2024414290143"));
    infoLayout->addWidget(new QLabel("email：1559706439@qq.com"));
    hLayout->addLayout(infoLayout);
    vLayout->addLayout(hLayout);
    QLabel *verLabel = new QLabel("V1.0 build 202611");
    verLabel->setAlignment(Qt::AlignLeft);
    vLayout->addWidget(verLabel);
    QPushButton *btnOk = new QPushButton("OK");
    btnOk->setFixedWidth(120);
    connect(btnOk,&QPushButton::clicked,&dlg,&QDialog::accept);
    vLayout->addWidget(btnOk,0,Qt::AlignCenter);
    dlg.exec();
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
    "另存为",
    "untitled.txt",
    "文本文件 (*.txt);;所有文件 (*.*)");
    if(fileName.isEmpty())
        return; // 用户点了取消，直接返回
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "错误", "文件打开失败！");
        return;
    }
    QTextStream out(&file);
    out << ui->plainTextEdit->toPlainText();
    file.close();
    // 更新当前文件名和窗口标题
    curFileName = fileName;
    this->setWindowTitle(fileName + " - 文本编辑器");
}

void MainWindow::on_actionFont_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->plainTextEdit->font(), this, "选择字体");
    if (ok)
    {
        ui->plainTextEdit->setFont(font);
    }
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(ui->plainTextEdit->palette().color(QPalette::Text), this, "选择文字颜色");
    if (color.isValid())
    {
        QPalette pal = ui->plainTextEdit->palette();
        pal.setColor(QPalette::Text, color);
        ui->plainTextEdit->setPalette(pal);
    }
}

void MainWindow::on_actionFontBgColor_triggered()
{
    QColor color = QColorDialog::getColor(ui->plainTextEdit->palette().base().color(), this, "选择背景颜色");
    if (color.isValid())
    {
        QPalette pal = ui->plainTextEdit->palette();
        pal.setColor(QPalette::Base, color);
        ui->plainTextEdit->setPalette(pal);
    }
}

void MainWindow::on_actionAutoWrap_triggered(bool checked)
{
    if(checked)
    {
        ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth); //自动换行
    }
    else
    {
        ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap); //不换行，横向滚动
    }
}

// 显示/隐藏工具栏
void MainWindow::on_actionShowToolBar_toggled(bool checked)
{
    ui->mainToolBar->setVisible(checked);
}
// 显示/隐藏状态栏
void MainWindow::on_actionShowStatusBar_toggled(bool checked)
{
    statusBar()->setVisible(checked);
}

// 文本变化，更新状态栏信息
void MainWindow::on_plainTextEdit_textChanged()
{
    int count = ui->plainTextEdit->toPlainText().length();
    QString info = QString("字符数：%1 | 当前文件：%2").arg(count).arg(curFileName);
    statusBar()->showMessage(info);
}

void MainWindow::on_actionSelectAll_triggered()
{
    ui->plainTextEdit->selectAll();
}

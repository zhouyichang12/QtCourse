#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_plainTextEdit_textChanged();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();


    void on_actionNew_triggered();

    void on_actionFind_triggered();

    void on_actionAbout_triggered();

    void on_actionReplace_triggered();

   void closeEvent(QCloseEvent *event) override;

    void on_actionSaveAs_triggered();

    void on_actionFont_triggered();

    void on_actionFontColor_triggered();

    void on_actionFontBgColor_triggered();

    void on_actionAutoWrap_triggered(bool checked);

    void on_actionShowToolBar_toggled(bool checked);
    void on_actionShowStatusBar_toggled(bool checked);



    void on_actionSelectAll_triggered();



private:
    Ui::MainWindow *ui;
    QString curFileName;
    QString m_curFilePath;  // 记录当前打开的文件路径
    bool m_isModified = false; // 文本是否被修改


};


#endif // MAINWINDOW_H

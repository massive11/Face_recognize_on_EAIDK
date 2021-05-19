/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QLabel *img_label;
    QLabel *img_label_2;
    QLabel *title;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QRadioButton *rb_face_track;
    QRadioButton *face_attr_2;
    QRadioButton *face_rec_2;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout;
    QPushButton *ok;
    QLineEdit *lineEdit;
    QPushButton *cancel;
    QLabel *regist_info;
    QLabel *name;
    QPushButton *regist;
    QPushButton *cancel_2;
    QPushButton *ok_2;
    QLabel *label_diku;
    QLabel *label_diku_2;
    QTextEdit *textEdit_info;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1249, 728);
        MainWindow->setMaximumSize(QSize(16777215, 16777215));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(290, 140, 640, 490));
        groupBox->setStyleSheet(QStringLiteral(""));
        groupBox->setAlignment(Qt::AlignCenter);
        img_label = new QLabel(groupBox);
        img_label->setObjectName(QStringLiteral("img_label"));
        img_label->setEnabled(true);
        img_label->setGeometry(QRect(0, 20, 641, 471));
        img_label->setAlignment(Qt::AlignCenter);
        img_label_2 = new QLabel(groupBox);
        img_label_2->setObjectName(QStringLiteral("img_label_2"));
        img_label_2->setEnabled(true);
        img_label_2->setGeometry(QRect(0, 20, 641, 471));
        img_label_2->setAlignment(Qt::AlignCenter);
        title = new QLabel(centralWidget);
        title->setObjectName(QStringLiteral("title"));
        title->setGeometry(QRect(380, 60, 461, 41));
        QFont font;
        font.setPointSize(16);
        title->setFont(font);
        title->setAlignment(Qt::AlignCenter);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(70, 160, 181, 182));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        rb_face_track = new QRadioButton(layoutWidget);
        rb_face_track->setObjectName(QStringLiteral("rb_face_track"));
        QFont font1;
        font1.setFamily(QStringLiteral("Source Han Sans CN"));
        font1.setPointSize(12);
        rb_face_track->setFont(font1);

        verticalLayout->addWidget(rb_face_track);

        face_attr_2 = new QRadioButton(layoutWidget);
        face_attr_2->setObjectName(QStringLiteral("face_attr_2"));
        QFont font2;
        font2.setPointSize(12);
        face_attr_2->setFont(font2);

        verticalLayout->addWidget(face_attr_2);

        face_rec_2 = new QRadioButton(layoutWidget);
        face_rec_2->setObjectName(QStringLiteral("face_rec_2"));
        face_rec_2->setFont(font2);

        verticalLayout->addWidget(face_rec_2);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(62, 425, 187, 179));
        gridLayout = new QGridLayout(layoutWidget1);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        ok = new QPushButton(layoutWidget1);
        ok->setObjectName(QStringLiteral("ok"));
        ok->setFont(font2);

        gridLayout->addWidget(ok, 4, 2, 1, 1);

        lineEdit = new QLineEdit(layoutWidget1);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout->addWidget(lineEdit, 3, 1, 1, 2);

        cancel = new QPushButton(layoutWidget1);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setFont(font2);

        gridLayout->addWidget(cancel, 4, 0, 1, 2);

        regist_info = new QLabel(layoutWidget1);
        regist_info->setObjectName(QStringLiteral("regist_info"));
        regist_info->setFont(font2);
        regist_info->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(regist_info, 1, 0, 1, 3);

        name = new QLabel(layoutWidget1);
        name->setObjectName(QStringLiteral("name"));
        name->setFont(font2);

        gridLayout->addWidget(name, 3, 0, 1, 1);

        regist = new QPushButton(layoutWidget1);
        regist->setObjectName(QStringLiteral("regist"));
        regist->setFont(font2);

        gridLayout->addWidget(regist, 0, 0, 1, 3);

        cancel_2 = new QPushButton(layoutWidget1);
        cancel_2->setObjectName(QStringLiteral("cancel_2"));
        cancel_2->setFont(font2);

        gridLayout->addWidget(cancel_2, 2, 0, 1, 1);

        ok_2 = new QPushButton(layoutWidget1);
        ok_2->setObjectName(QStringLiteral("ok_2"));
        ok_2->setFont(font2);

        gridLayout->addWidget(ok_2, 2, 1, 1, 2);

        label_diku = new QLabel(centralWidget);
        label_diku->setObjectName(QStringLiteral("label_diku"));
        label_diku->setGeometry(QRect(940, 170, 111, 151));
        label_diku_2 = new QLabel(centralWidget);
        label_diku_2->setObjectName(QStringLiteral("label_diku_2"));
        label_diku_2->setGeometry(QRect(1100, 170, 111, 151));
        textEdit_info = new QTextEdit(centralWidget);
        textEdit_info->setObjectName(QStringLiteral("textEdit_info"));
        textEdit_info->setGeometry(QRect(950, 340, 281, 291));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1249, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        groupBox->setTitle(QString());
        img_label->setText(QApplication::translate("MainWindow", "\346\255\244\345\244\204\346\234\211\350\247\206\351\242\221", nullptr));
        img_label_2->setText(QApplication::translate("MainWindow", "\346\255\244\345\244\204\346\234\211\350\247\206\351\242\221", nullptr));
        title->setText(QApplication::translate("MainWindow", "\344\272\272\350\204\270\350\257\206\345\210\253", nullptr));
        rb_face_track->setText(QApplication::translate("MainWindow", "\344\272\272\350\204\270\350\267\237\350\270\252", nullptr));
        face_attr_2->setText(QApplication::translate("MainWindow", "\344\272\272\350\204\270\345\261\236\346\200\247", nullptr));
        face_rec_2->setText(QApplication::translate("MainWindow", "\344\272\272\350\204\270\350\257\206\345\210\253", nullptr));
        ok->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        cancel->setText(QApplication::translate("MainWindow", "\345\217\226\346\266\210", nullptr));
        regist_info->setText(QApplication::translate("MainWindow", "\350\257\267\345\257\271\345\207\206\351\225\234\345\244\264", nullptr));
        name->setText(QApplication::translate("MainWindow", "\346\263\250\345\206\214\345\247\223\345\220\215", nullptr));
        regist->setText(QApplication::translate("MainWindow", "\346\263\250\345\206\214", nullptr));
        cancel_2->setText(QApplication::translate("MainWindow", "\345\217\226\346\266\210", nullptr));
        ok_2->setText(QApplication::translate("MainWindow", "\347\241\256\345\256\232", nullptr));
        label_diku->setText(QString());
        label_diku_2->setText(QString());
        textEdit_info->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

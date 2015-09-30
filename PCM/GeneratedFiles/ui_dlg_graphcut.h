/********************************************************************************
** Form generated from reading UI file 'dlg_graphcut.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_GRAPHCUT_H
#define UI_DLG_GRAPHCUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Expansion
{
public:
    QPushButton *pushButton;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *nLabels_;
    QLineEdit *nCur_;
    QLineEdit *Expansion_;
    QLineEdit *nSwap_;
    QLineEdit *nSmooth_;
    QLabel *label_7;
    QLineEdit *lineEdit;
    QLabel *label_8;
    QLineEdit *lineEdit_2;
    QLabel *label_9;
    QLineEdit *lineEdit_3;
    QLabel *label_10;
    QLineEdit *nCenterF;
    QSlider *nGraph_;
    QLabel *label_11;

    void setupUi(QWidget *Expansion)
    {
        if (Expansion->objectName().isEmpty())
            Expansion->setObjectName(QString::fromUtf8("Expansion"));
        Expansion->resize(463, 465);
        pushButton = new QPushButton(Expansion);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(360, 410, 75, 23));
        label = new QLabel(Expansion);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(50, 30, 61, 16));
        label_2 = new QLabel(Expansion);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(50, 260, 71, 20));
        label_3 = new QLabel(Expansion);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(50, 70, 101, 16));
        label_4 = new QLabel(Expansion);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(50, 110, 61, 21));
        label_5 = new QLabel(Expansion);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(50, 150, 71, 21));
        label_6 = new QLabel(Expansion);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(50, 200, 61, 21));
        nLabels_ = new QLineEdit(Expansion);
        nLabels_->setObjectName(QString::fromUtf8("nLabels_"));
        nLabels_->setGeometry(QRect(170, 30, 61, 20));
        nCur_ = new QLineEdit(Expansion);
        nCur_->setObjectName(QString::fromUtf8("nCur_"));
        nCur_->setGeometry(QRect(170, 70, 61, 20));
        Expansion_ = new QLineEdit(Expansion);
        Expansion_->setObjectName(QString::fromUtf8("Expansion_"));
        Expansion_->setGeometry(QRect(170, 110, 61, 20));
        nSwap_ = new QLineEdit(Expansion);
        nSwap_->setObjectName(QString::fromUtf8("nSwap_"));
        nSwap_->setGeometry(QRect(170, 150, 61, 20));
        nSmooth_ = new QLineEdit(Expansion);
        nSmooth_->setObjectName(QString::fromUtf8("nSmooth_"));
        nSmooth_->setGeometry(QRect(170, 200, 61, 21));
        label_7 = new QLabel(Expansion);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(260, 30, 54, 12));
        lineEdit = new QLineEdit(Expansion);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(310, 30, 61, 20));
        label_8 = new QLabel(Expansion);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(260, 70, 41, 16));
        lineEdit_2 = new QLineEdit(Expansion);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(310, 70, 61, 20));
        label_9 = new QLabel(Expansion);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(260, 110, 41, 16));
        lineEdit_3 = new QLineEdit(Expansion);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(310, 110, 61, 20));
        label_10 = new QLabel(Expansion);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(260, 150, 41, 16));
        nCenterF = new QLineEdit(Expansion);
        nCenterF->setObjectName(QString::fromUtf8("nCenterF"));
        nCenterF->setGeometry(QRect(310, 150, 61, 20));
        nGraph_ = new QSlider(Expansion);
        nGraph_->setObjectName(QString::fromUtf8("nGraph_"));
        nGraph_->setGeometry(QRect(140, 260, 221, 19));
        nGraph_->setMaximum(100);
        nGraph_->setPageStep(1);
        nGraph_->setOrientation(Qt::Horizontal);
        label_11 = new QLabel(Expansion);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(390, 260, 31, 20));

        retranslateUi(Expansion);

        QMetaObject::connectSlotsByName(Expansion);
    } // setupUi

    void retranslateUi(QWidget *Expansion)
    {
        Expansion->setWindowTitle(QApplication::translate("Expansion", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Expansion", "Refine", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Expansion", "\345\210\235\345\247\213\347\261\273\344\270\252\346\225\260", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Expansion", "Graph \351\202\273\345\237\237", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Expansion", "Curvature \351\202\273\345\237\237", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Expansion", "Expansion", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Expansion", "Swap", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Expansion", "\345\271\263\346\273\221\351\241\271\346\235\203\351\207\215", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Expansion", "CSigma", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Expansion", "DSigma", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Expansion", "DiffuR", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Expansion", "\346\277\200\346\264\273\345\270\247", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Expansion", "10", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Expansion: public Ui_Expansion {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_GRAPHCUT_H

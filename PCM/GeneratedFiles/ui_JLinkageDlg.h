/********************************************************************************
** Form generated from reading UI file 'JLinkageDlg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JLINKAGEDLG_H
#define UI_JLINKAGEDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_JLinkageDlg
{
public:
    QLineEdit *TrajLen;
    QLineEdit *Resolution;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *pushButton;
    QLabel *label_5;
    QLineEdit *modelT;
    QLabel *label_6;
    QLineEdit *smallLife;
    QCheckBox *rigid;
    QCheckBox *equal;
    QSlider *neigbhorNum;
    QLabel *outputNeigNum;
    QLabel *label_11;
    QSlider *Lambda;
    QSlider *Threshold;
    QLabel *LambdaValue;
    QLabel *ThresholdValue;
    QLabel *label_7;
    QLabel *CenterFrame;

    void setupUi(QWidget *JLinkageDlg)
    {
        if (JLinkageDlg->objectName().isEmpty())
            JLinkageDlg->setObjectName(QString::fromUtf8("JLinkageDlg"));
        JLinkageDlg->setEnabled(true);
        JLinkageDlg->resize(418, 413);
        TrajLen = new QLineEdit(JLinkageDlg);
        TrajLen->setObjectName(QString::fromUtf8("TrajLen"));
        TrajLen->setGeometry(QRect(80, 30, 51, 20));
        Resolution = new QLineEdit(JLinkageDlg);
        Resolution->setObjectName(QString::fromUtf8("Resolution"));
        Resolution->setGeometry(QRect(80, 70, 51, 20));
        label = new QLabel(JLinkageDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 30, 54, 12));
        label_2 = new QLabel(JLinkageDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 70, 61, 16));
        label_3 = new QLabel(JLinkageDlg);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 160, 54, 12));
        label_4 = new QLabel(JLinkageDlg);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 200, 54, 20));
        pushButton = new QPushButton(JLinkageDlg);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(320, 370, 75, 23));
        label_5 = new QLabel(JLinkageDlg);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(160, 70, 54, 12));
        modelT = new QLineEdit(JLinkageDlg);
        modelT->setObjectName(QString::fromUtf8("modelT"));
        modelT->setGeometry(QRect(230, 70, 51, 20));
        label_6 = new QLabel(JLinkageDlg);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(160, 30, 54, 20));
        smallLife = new QLineEdit(JLinkageDlg);
        smallLife->setObjectName(QString::fromUtf8("smallLife"));
        smallLife->setGeometry(QRect(230, 30, 51, 20));
        rigid = new QCheckBox(JLinkageDlg);
        rigid->setObjectName(QString::fromUtf8("rigid"));
        rigid->setGeometry(QRect(10, 250, 81, 21));
        equal = new QCheckBox(JLinkageDlg);
        equal->setObjectName(QString::fromUtf8("equal"));
        equal->setGeometry(QRect(10, 290, 81, 21));
        neigbhorNum = new QSlider(JLinkageDlg);
        neigbhorNum->setObjectName(QString::fromUtf8("neigbhorNum"));
        neigbhorNum->setGeometry(QRect(120, 120, 231, 19));
        neigbhorNum->setMinimum(10);
        neigbhorNum->setMaximum(100);
        neigbhorNum->setPageStep(5);
        neigbhorNum->setOrientation(Qt::Horizontal);
        outputNeigNum = new QLabel(JLinkageDlg);
        outputNeigNum->setObjectName(QString::fromUtf8("outputNeigNum"));
        outputNeigNum->setGeometry(QRect(370, 120, 41, 20));
        label_11 = new QLabel(JLinkageDlg);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 120, 101, 20));
        Lambda = new QSlider(JLinkageDlg);
        Lambda->setObjectName(QString::fromUtf8("Lambda"));
        Lambda->setGeometry(QRect(120, 160, 231, 19));
        Lambda->setMinimum(0);
        Lambda->setMaximum(100);
        Lambda->setPageStep(1);
        Lambda->setValue(0);
        Lambda->setSliderPosition(0);
        Lambda->setOrientation(Qt::Horizontal);
        Threshold = new QSlider(JLinkageDlg);
        Threshold->setObjectName(QString::fromUtf8("Threshold"));
        Threshold->setGeometry(QRect(120, 200, 231, 19));
        Threshold->setMaximum(100);
        Threshold->setPageStep(1);
        Threshold->setOrientation(Qt::Horizontal);
        LambdaValue = new QLabel(JLinkageDlg);
        LambdaValue->setObjectName(QString::fromUtf8("LambdaValue"));
        LambdaValue->setGeometry(QRect(370, 160, 41, 20));
        ThresholdValue = new QLabel(JLinkageDlg);
        ThresholdValue->setObjectName(QString::fromUtf8("ThresholdValue"));
        ThresholdValue->setGeometry(QRect(370, 200, 41, 20));
        label_7 = new QLabel(JLinkageDlg);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(300, 30, 71, 20));
        CenterFrame = new QLabel(JLinkageDlg);
        CenterFrame->setObjectName(QString::fromUtf8("CenterFrame"));
        CenterFrame->setGeometry(QRect(380, 30, 31, 20));

        retranslateUi(JLinkageDlg);

        QMetaObject::connectSlotsByName(JLinkageDlg);
    } // setupUi

    void retranslateUi(QWidget *JLinkageDlg)
    {
        JLinkageDlg->setWindowTitle(QApplication::translate("JLinkageDlg", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("JLinkageDlg", "TrajLen", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("JLinkageDlg", "Resolution", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("JLinkageDlg", "Lambda", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("JLinkageDlg", "Threshold", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("JLinkageDlg", "Compute", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("JLinkageDlg", "ModelT", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("JLinkageDlg", "SmallLife", 0, QApplication::UnicodeUTF8));
        rigid->setText(QApplication::translate("JLinkageDlg", "Rigid", 0, QApplication::UnicodeUTF8));
        equal->setText(QApplication::translate("JLinkageDlg", "Equal", 0, QApplication::UnicodeUTF8));
        outputNeigNum->setText(QApplication::translate("JLinkageDlg", "10", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("JLinkageDlg", "Neighbor Number", 0, QApplication::UnicodeUTF8));
        LambdaValue->setText(QApplication::translate("JLinkageDlg", "0", 0, QApplication::UnicodeUTF8));
        ThresholdValue->setText(QApplication::translate("JLinkageDlg", "0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("JLinkageDlg", "CenterFrame", 0, QApplication::UnicodeUTF8));
        CenterFrame->setText(QApplication::translate("JLinkageDlg", "10", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class JLinkageDlg: public Ui_JLinkageDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JLINKAGEDLG_H

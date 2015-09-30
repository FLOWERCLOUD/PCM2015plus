/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_main_windowClass
{
public:
    QAction *actionImportFiles;
    QAction *actionSet_Visible;
    QAction *actionSet_Invisible;
    QAction *actionScene_Mode;
    QAction *actionSelect_Mode;
    QAction *actionClustering;
    QAction *actionObject_Color;
    QAction *actionVertex_Color;
    QAction *actionLabel_Color;
    QAction *actionOriginal_Location;
    QAction *actionShow_Tracjectory;
    QAction *actionDont_Trace;
    QAction *actionRegister;
    QAction *actionSpectral_Cluster;
    QAction *actionGraphCut;
    QAction *actionCalculateNorm;
    QAction *actionClusterAll;
    QAction *actionVisDistortion;
    QAction *actionGCopti;
    QAction *actionPlanFit;
    QAction *actionShow_Graph_WrapBox;
    QAction *actionShow_EdgeVertexs;
    QAction *actionButtonback;
    QAction *actionButton2stop;
    QAction *actionButtonRunOrPause;
    QAction *actionButtonadvance;
    QAction *actionPropagate;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFiles;
    QMenu *menuPaint;
    QMenu *menuSelect;
    QMenu *menuAlgorithm;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QTreeWidget *treeWidget;
    QSpinBox *LayerSpinBox;

    void setupUi(QMainWindow *main_windowClass)
    {
        if (main_windowClass->objectName().isEmpty())
            main_windowClass->setObjectName(QString::fromUtf8("main_windowClass"));
        main_windowClass->resize(1122, 592);
        actionImportFiles = new QAction(main_windowClass);
        actionImportFiles->setObjectName(QString::fromUtf8("actionImportFiles"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("Resources/openFile.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImportFiles->setIcon(icon);
        actionSet_Visible = new QAction(main_windowClass);
        actionSet_Visible->setObjectName(QString::fromUtf8("actionSet_Visible"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("Resources/visible.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSet_Visible->setIcon(icon1);
        actionSet_Invisible = new QAction(main_windowClass);
        actionSet_Invisible->setObjectName(QString::fromUtf8("actionSet_Invisible"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("Resources/invisible.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSet_Invisible->setIcon(icon2);
        actionScene_Mode = new QAction(main_windowClass);
        actionScene_Mode->setObjectName(QString::fromUtf8("actionScene_Mode"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("Resources/scene.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionScene_Mode->setIcon(icon3);
        actionSelect_Mode = new QAction(main_windowClass);
        actionSelect_Mode->setObjectName(QString::fromUtf8("actionSelect_Mode"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("Resources/select.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelect_Mode->setIcon(icon4);
        actionClustering = new QAction(main_windowClass);
        actionClustering->setObjectName(QString::fromUtf8("actionClustering"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8("Resources/categorize.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClustering->setIcon(icon5);
        actionObject_Color = new QAction(main_windowClass);
        actionObject_Color->setObjectName(QString::fromUtf8("actionObject_Color"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8("Resources/tree.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionObject_Color->setIcon(icon6);
        actionVertex_Color = new QAction(main_windowClass);
        actionVertex_Color->setObjectName(QString::fromUtf8("actionVertex_Color"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8("Resources/leaf.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionVertex_Color->setIcon(icon7);
        actionLabel_Color = new QAction(main_windowClass);
        actionLabel_Color->setObjectName(QString::fromUtf8("actionLabel_Color"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8("Resources/label.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLabel_Color->setIcon(icon8);
        actionOriginal_Location = new QAction(main_windowClass);
        actionOriginal_Location->setObjectName(QString::fromUtf8("actionOriginal_Location"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8("Resources/tree2.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOriginal_Location->setIcon(icon9);
        actionShow_Tracjectory = new QAction(main_windowClass);
        actionShow_Tracjectory->setObjectName(QString::fromUtf8("actionShow_Tracjectory"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8("Resources/show_trace.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShow_Tracjectory->setIcon(icon10);
        actionDont_Trace = new QAction(main_windowClass);
        actionDont_Trace->setObjectName(QString::fromUtf8("actionDont_Trace"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8("Resources/dont_trace.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDont_Trace->setIcon(icon11);
        actionRegister = new QAction(main_windowClass);
        actionRegister->setObjectName(QString::fromUtf8("actionRegister"));
        actionSpectral_Cluster = new QAction(main_windowClass);
        actionSpectral_Cluster->setObjectName(QString::fromUtf8("actionSpectral_Cluster"));
        actionGraphCut = new QAction(main_windowClass);
        actionGraphCut->setObjectName(QString::fromUtf8("actionGraphCut"));
        actionCalculateNorm = new QAction(main_windowClass);
        actionCalculateNorm->setObjectName(QString::fromUtf8("actionCalculateNorm"));
        actionClusterAll = new QAction(main_windowClass);
        actionClusterAll->setObjectName(QString::fromUtf8("actionClusterAll"));
        actionVisDistortion = new QAction(main_windowClass);
        actionVisDistortion->setObjectName(QString::fromUtf8("actionVisDistortion"));
        actionGCopti = new QAction(main_windowClass);
        actionGCopti->setObjectName(QString::fromUtf8("actionGCopti"));
        actionPlanFit = new QAction(main_windowClass);
        actionPlanFit->setObjectName(QString::fromUtf8("actionPlanFit"));
        actionShow_Graph_WrapBox = new QAction(main_windowClass);
        actionShow_Graph_WrapBox->setObjectName(QString::fromUtf8("actionShow_Graph_WrapBox"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8("Resources/nnolinkNode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShow_Graph_WrapBox->setIcon(icon12);
        actionShow_EdgeVertexs = new QAction(main_windowClass);
        actionShow_EdgeVertexs->setObjectName(QString::fromUtf8("actionShow_EdgeVertexs"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8("Resources/NoedgeVertexs.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShow_EdgeVertexs->setIcon(icon13);
        actionButtonback = new QAction(main_windowClass);
        actionButtonback->setObjectName(QString::fromUtf8("actionButtonback"));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8("Resources/buttonback.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionButtonback->setIcon(icon14);
        actionButton2stop = new QAction(main_windowClass);
        actionButton2stop->setObjectName(QString::fromUtf8("actionButton2stop"));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8("Resources/button2stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionButton2stop->setIcon(icon15);
        actionButtonRunOrPause = new QAction(main_windowClass);
        actionButtonRunOrPause->setObjectName(QString::fromUtf8("actionButtonRunOrPause"));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8("Resources/buttonstop2run.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionButtonRunOrPause->setIcon(icon16);
        actionButtonadvance = new QAction(main_windowClass);
        actionButtonadvance->setObjectName(QString::fromUtf8("actionButtonadvance"));
        QIcon icon17;
        icon17.addFile(QString::fromUtf8("Resources/buttonadvance.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionButtonadvance->setIcon(icon17);
        actionPropagate = new QAction(main_windowClass);
        actionPropagate->setObjectName(QString::fromUtf8("actionPropagate"));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8("Resources/propagate.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPropagate->setIcon(icon18);
        centralWidget = new QWidget(main_windowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setEnabled(true);
        main_windowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(main_windowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1122, 23));
        menuFiles = new QMenu(menuBar);
        menuFiles->setObjectName(QString::fromUtf8("menuFiles"));
        menuPaint = new QMenu(menuBar);
        menuPaint->setObjectName(QString::fromUtf8("menuPaint"));
        menuSelect = new QMenu(menuBar);
        menuSelect->setObjectName(QString::fromUtf8("menuSelect"));
        menuAlgorithm = new QMenu(menuBar);
        menuAlgorithm->setObjectName(QString::fromUtf8("menuAlgorithm"));
        main_windowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(main_windowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        main_windowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(main_windowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        main_windowClass->setStatusBar(statusBar);
        dockWidget = new QDockWidget(main_windowClass);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setMinimumSize(QSize(200, 38));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        treeWidget = new QTreeWidget(dockWidgetContents);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(0, 0, 300, 431));
        treeWidget->setMinimumSize(QSize(300, 0));
        LayerSpinBox = new QSpinBox(dockWidgetContents);
        LayerSpinBox->setObjectName(QString::fromUtf8("LayerSpinBox"));
        LayerSpinBox->setGeometry(QRect(0, 430, 301, 61));
        dockWidget->setWidget(dockWidgetContents);
        main_windowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);

        menuBar->addAction(menuFiles->menuAction());
        menuBar->addAction(menuPaint->menuAction());
        menuBar->addAction(menuSelect->menuAction());
        menuBar->addAction(menuAlgorithm->menuAction());
        menuFiles->addAction(actionImportFiles);
        menuPaint->addAction(actionSet_Visible);
        menuPaint->addAction(actionSet_Invisible);
        menuPaint->addAction(actionObject_Color);
        menuPaint->addAction(actionVertex_Color);
        menuPaint->addAction(actionLabel_Color);
        menuPaint->addAction(actionOriginal_Location);
        menuPaint->addAction(actionShow_Tracjectory);
        menuPaint->addAction(actionDont_Trace);
        menuPaint->addAction(actionShow_Graph_WrapBox);
        menuPaint->addAction(actionShow_EdgeVertexs);
        menuSelect->addAction(actionScene_Mode);
        menuSelect->addAction(actionSelect_Mode);
        menuAlgorithm->addAction(actionClustering);
        menuAlgorithm->addAction(actionRegister);
        menuAlgorithm->addAction(actionSpectral_Cluster);
        menuAlgorithm->addAction(actionGraphCut);
        menuAlgorithm->addAction(actionCalculateNorm);
        menuAlgorithm->addAction(actionClusterAll);
        menuAlgorithm->addAction(actionVisDistortion);
        menuAlgorithm->addAction(actionGCopti);
        menuAlgorithm->addAction(actionPlanFit);
        menuAlgorithm->addAction(actionPropagate);
        mainToolBar->addAction(actionImportFiles);
        mainToolBar->addAction(actionSet_Visible);
        mainToolBar->addAction(actionSet_Invisible);
        mainToolBar->addAction(actionShow_Tracjectory);
        mainToolBar->addAction(actionDont_Trace);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionObject_Color);
        mainToolBar->addAction(actionVertex_Color);
        mainToolBar->addAction(actionLabel_Color);
        mainToolBar->addAction(actionOriginal_Location);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSelect_Mode);
        mainToolBar->addAction(actionScene_Mode);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionClustering);
        mainToolBar->addAction(actionPropagate);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionShow_Graph_WrapBox);
        mainToolBar->addAction(actionShow_EdgeVertexs);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionButtonback);
        mainToolBar->addAction(actionButton2stop);
        mainToolBar->addAction(actionButtonRunOrPause);
        mainToolBar->addAction(actionButtonadvance);

        retranslateUi(main_windowClass);

        QMetaObject::connectSlotsByName(main_windowClass);
    } // setupUi

    void retranslateUi(QMainWindow *main_windowClass)
    {
        main_windowClass->setWindowTitle(QApplication::translate("main_windowClass", "main_window", 0, QApplication::UnicodeUTF8));
        actionImportFiles->setText(QApplication::translate("main_windowClass", "ImportFiles", 0, QApplication::UnicodeUTF8));
        actionSet_Visible->setText(QApplication::translate("main_windowClass", "Set Visible", 0, QApplication::UnicodeUTF8));
        actionSet_Invisible->setText(QApplication::translate("main_windowClass", "Set Invisible", 0, QApplication::UnicodeUTF8));
        actionScene_Mode->setText(QApplication::translate("main_windowClass", "Scene Mode", 0, QApplication::UnicodeUTF8));
        actionSelect_Mode->setText(QApplication::translate("main_windowClass", "Select Mode", 0, QApplication::UnicodeUTF8));
        actionClustering->setText(QApplication::translate("main_windowClass", "Clustering", 0, QApplication::UnicodeUTF8));
        actionObject_Color->setText(QApplication::translate("main_windowClass", "Object Color", 0, QApplication::UnicodeUTF8));
        actionVertex_Color->setText(QApplication::translate("main_windowClass", "Vertex Color", 0, QApplication::UnicodeUTF8));
        actionLabel_Color->setText(QApplication::translate("main_windowClass", "Label Color", 0, QApplication::UnicodeUTF8));
        actionOriginal_Location->setText(QApplication::translate("main_windowClass", "Original Location", 0, QApplication::UnicodeUTF8));
        actionShow_Tracjectory->setText(QApplication::translate("main_windowClass", "Show Tracjectory", 0, QApplication::UnicodeUTF8));
        actionDont_Trace->setText(QApplication::translate("main_windowClass", "Dont Trace", 0, QApplication::UnicodeUTF8));
        actionRegister->setText(QApplication::translate("main_windowClass", "Register", 0, QApplication::UnicodeUTF8));
        actionSpectral_Cluster->setText(QApplication::translate("main_windowClass", "Spectral Cluster", 0, QApplication::UnicodeUTF8));
        actionGraphCut->setText(QApplication::translate("main_windowClass", "GraphCut", 0, QApplication::UnicodeUTF8));
        actionCalculateNorm->setText(QApplication::translate("main_windowClass", "CalculateNorm", 0, QApplication::UnicodeUTF8));
        actionClusterAll->setText(QApplication::translate("main_windowClass", "ClusterAll", 0, QApplication::UnicodeUTF8));
        actionVisDistortion->setText(QApplication::translate("main_windowClass", "VisDistortion", 0, QApplication::UnicodeUTF8));
        actionGCopti->setText(QApplication::translate("main_windowClass", "GCopti", 0, QApplication::UnicodeUTF8));
        actionPlanFit->setText(QApplication::translate("main_windowClass", "PlanFit", 0, QApplication::UnicodeUTF8));
        actionShow_Graph_WrapBox->setText(QApplication::translate("main_windowClass", "Show Graph WrapBox", 0, QApplication::UnicodeUTF8));
        actionShow_EdgeVertexs->setText(QApplication::translate("main_windowClass", "Show EdgeVertexs", 0, QApplication::UnicodeUTF8));
        actionButtonback->setText(QApplication::translate("main_windowClass", "buttonback", 0, QApplication::UnicodeUTF8));
        actionButton2stop->setText(QApplication::translate("main_windowClass", "button2stop", 0, QApplication::UnicodeUTF8));
        actionButtonRunOrPause->setText(QApplication::translate("main_windowClass", "buttonstopORrun", 0, QApplication::UnicodeUTF8));
        actionButtonadvance->setText(QApplication::translate("main_windowClass", "buttonadvance", 0, QApplication::UnicodeUTF8));
        actionPropagate->setText(QApplication::translate("main_windowClass", "propagate", 0, QApplication::UnicodeUTF8));
        menuFiles->setTitle(QApplication::translate("main_windowClass", "Files", 0, QApplication::UnicodeUTF8));
        menuPaint->setTitle(QApplication::translate("main_windowClass", "View", 0, QApplication::UnicodeUTF8));
        menuSelect->setTitle(QApplication::translate("main_windowClass", "Select", 0, QApplication::UnicodeUTF8));
        menuAlgorithm->setTitle(QApplication::translate("main_windowClass", "Algorithm", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class main_windowClass: public Ui_main_windowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H

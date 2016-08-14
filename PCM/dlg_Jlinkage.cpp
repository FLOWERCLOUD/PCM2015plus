#include "dlg_Jlinkage.h"




void JLinkageUI::init()
{
	ui.CenterFrame->setText(QString::number(selectedSmpIdx));
	ui.rigid->setChecked(isRigid);
	ui.equal->setChecked(isEqual);
	ui.TrajLen->setText(QString::number(trajLen));
	ui.Resolution->setText(QString::number(resolution));
	ui.Lambda->setValue(lambda*100);
	ui.LambdaValue->setText(QString::number(lambda) );
	ui.Threshold->setValue(threshold*100);
	ui.ThresholdValue->setText(QString::number(threshold) );
	ui.modelT->setText(QString::number(modelT));
	ui.smallLife->setText(QString::number(smallLife));
	ui.neigbhorNum->setValue(neigNum);
	ui.outputNeigNum->setText(QString::number(neigNum) );
	connect( ui.TrajLen, SIGNAL( textEdited(const QString&) ), this, SLOT( getTrajLen(const QString&) ) );

	connect( ui.Resolution, SIGNAL( textEdited(const QString&) ), this, SLOT( getResolution(const QString&) ) );

	connect( ui.Lambda, SIGNAL( valueChanged(int) ), this, SLOT( updateLambda(int) ) );

	connect( ui.Threshold, SIGNAL( valueChanged(int) ), this, SLOT( updateThreshold(int) ) );

	connect( ui.modelT,SIGNAL( textEdited(const QString&)) , this,SLOT(getModelT(const QString&) ) );

	connect( ui.smallLife, SIGNAL (textEdited(const QString&)), this, SLOT(getSmallLife(const QString&) ) );


	connect(ui.neigbhorNum,SIGNAL(valueChanged(int)) ,this, SLOT(updateNeigbhorNum(int)) );

	connect( ui.pushButton, SIGNAL(clicked(bool)), this, SLOT(run(bool)) );
}

JLinkageUI::JLinkageUI(IndexType _selectedSmpIdx) :selectedSmpIdx(_selectedSmpIdx)
{
	ui.setupUi(this);

	trajLen = 2;
	resolution = 32;
	lambda = 0.6;
	threshold = 0.7;
	modelT = 1;
	smallLife = 2;
	isEqual = true;
	isRigid = false;
	neigNum = 30;
}

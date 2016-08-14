#ifndef _DLG_JLINKAGE_H
#define _DLG_JLINKAGE_H

#include <QtGui/QDialog>
#include "basic_types.h"
#include"trajectory_classifier.h"
//#include "traj_clustering.h"
#include "ui_JLinkageDlg.h"

class JLinkageUI : public QDialog
{
	Q_OBJECT
public:
	JLinkageUI(IndexType _selectedSmpIdx);

	void init();

	void setCurrentSmp(IndexType _curSmp)
	{
		selectedSmpIdx = _curSmp;
		ui.CenterFrame->setText(QString::number(selectedSmpIdx) );
	}

	public slots:

		void run(bool b)
		{
			//TrajClusteringThread* cluster = new TrajClusteringThread(selectedSmpIdx);

			isRigid = ui.rigid->isChecked();

			isEqual = ui.equal->isChecked();

			TrajectoryClassifier* cluster = new TrajectoryClassifier(selectedSmpIdx);
			
			cluster->setNeigNum(neigNum);

			cluster->setParamter(trajLen,resolution,lambda,threshold,modelT,smallLife,isEqual,isRigid);

			connect( cluster, SIGNAL(finished()), cluster, SLOT(deleteLater()) );

			cluster->start();
		}

		void getTrajLen(const QString& txt)
		{
			trajLen = txt.toInt();
		}

		void getResolution(const QString& txt)
		{
			resolution = txt.toInt();
		}

		void updateLambda(int _lambda)
		{
			lambda = (0.01)*_lambda;
			ui.LambdaValue->setText(QString::number(lambda) );
		}

		void updateThreshold(int _thereshold)
		{
			threshold = (0.01) *_thereshold;
			ui.ThresholdValue->setText(QString::number(threshold));
		}

		void getModelT(const QString& txt)
		{
			modelT = txt.toInt();
		}

		void getSmallLife(const QString& txt)
		{
			smallLife = txt.toInt();
		}

		void updateNeigbhorNum(int _neigNum)
		{
			neigNum = _neigNum;
			ui.outputNeigNum->setText(QString::number(neigNum) );
		}


private:
	IndexType selectedSmpIdx;
	IndexType trajLen;
	IndexType resolution;
	ScalarType lambda;
	ScalarType threshold;
	IndexType modelT;
	IndexType smallLife;
	Ui::JLinkageDlg ui;

	bool isEqual;

	bool isRigid;

	IndexType neigNum;
};

#endif
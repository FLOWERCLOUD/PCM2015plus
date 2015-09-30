#ifndef _DLG_GRAPH_CUTS_H
#define _DLG_GRAPH_CUTS_H

#include <QtGui/QDialog>
#include "basic_types.h"
//#include "ui_GraphCutsDlg.h" //dialog form error
#include "GCop.h"

#include "ui_dlg_graphcut.h"

class GraphCutUI :public QDialog
{
	Q_OBJECT
public:
// 	GraphCutUI :public QDialog();
// 	~GraphCutUI :public QDialog();

	GraphCutUI(IndexType _selectedSmpId):selectedSmpid(_selectedSmpId)
	{
		ui.setupUi(this);
		
	}

	void init()
	{
		connect( ui.nLabels_ ,SIGNAL( textEdited(const QString&) ), this,SLOT(getInitalNumber(const QString&)));

		connect( ui.Expansion_,SIGNAL( textEdited(const QString&) ), this,SLOT(getExpansionNumber(const QString&)) );

		connect( ui.nSwap_,SIGNAL( textEdited(const QString&) ), this,SLOT(getSwapNumber(const QString&)) );

		connect( ui.nGraph_, SIGNAL( valueChanged(int) ), this,SLOT(updateGraphNeig(int)) );

		connect( ui.nCur_,SIGNAL( textEdited(const QString&) ), this,SLOT(getCuvNeig(const QString&)) ); 

		connect( ui.nSmooth_,SIGNAL( textEdited(const QString&) ), this,SLOT(getTradeOff(const QString&)) );

		connect( ui.lineEdit,SIGNAL( textEdited(const QString&) ), this,SLOT(getCSigma(const QString&)) );

		connect( ui.lineEdit_2,SIGNAL( textEdited(const QString&) ), this,SLOT(getDSigma(const QString&)) );

		connect( ui.lineEdit_3,SIGNAL( textEdited(const QString&) ), this,SLOT(getDiffuR(const QString&)) );

		connect(ui.nCenterF,SIGNAL (textEdited(const QString&) ),this, SLOT(getCenterF(const QString&))   );

		connect( ui.pushButton,SIGNAL(clicked(bool)), this, SLOT(run(bool)) );


	}

	void setCurrentSmp(IndexType _curSmp)
	{
		selectedSmpid = _curSmp;
	}

public slots:

	void run(bool b)
	{
		GCop * graphCut = new GCop;
		graphCut->setParamter(m_nLabels,m_nExpansion,m_nSwap,m_nGraphNeig,m_nCurNeig,m_nTradeOff,m_cSigma,m_dSigma,m_nDiffu,m_centerF);

		connect( graphCut, SIGNAL(finished()), graphCut, SLOT(deleteLater()) );

		graphCut->start();

	}

	void getInitalNumber(const QString& txt)
	{
		m_nLabels = txt.toInt();
	}

	void getExpansionNumber(const QString& txt)
	{
		m_nExpansion = txt.toInt();
	}

	void getSwapNumber(const QString& txt)
	{
		m_nSwap = txt.toInt();
	}

	void updateGraphNeig(int _nNeigNum)
	{
		m_nGraphNeig = _nNeigNum;
		ui.label_11->setText(QString::number(m_nGraphNeig) );
	}

	void getCuvNeig(const QString& txt)
	{
		m_nCurNeig = txt.toInt();
	}

	void getTradeOff(const QString& txt)
	{
		m_nTradeOff = txt.toFloat();
	}

	void getCSigma(const QString& txt)
	{
		m_cSigma = txt.toFloat();
	}

	void getDSigma(const QString& txt)
	{
		m_dSigma = txt.toFloat();
	}

	void getDiffuR(const QString& txt)
	{
		m_nDiffu = txt.toFloat();
	}

	void getCenterF(const QString& txt)
	{
		m_centerF = txt.toInt();
	}

private:
	IndexType  selectedSmpid;
	IndexType m_nLabels;
	IndexType m_nExpansion;
	IndexType m_nSwap;
	IndexType m_nGraphNeig;
	IndexType m_nCurNeig;
	ScalarType m_nTradeOff;
	ScalarType m_cSigma;
	ScalarType m_dSigma;
	ScalarType m_nDiffu;
	IndexType m_centerF;
	Ui::Expansion ui;
};

// GraphCutUI :public QDialog::GraphCutUI :public QDialog()
// {
// }
// 
// GraphCutUI :public QDialog::~GraphCutUI :public QDialog()
// {
// }

#endif  
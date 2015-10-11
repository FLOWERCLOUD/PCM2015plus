// 
// 
// #ifndef _DLG_FITPLAN_H
// #define _DLG_FITPLAN_H
// 
// #include "ui_dlg_planfit.h"
// #include "basic_types.h"
// #include <QtWidgets/QDialog>
// #include "plans_fit.h"
// 
// class PlanFitUI : public QDialog
// {
// 	//Q_OBJECT
// 
// public:
//     PlanFitUI(IndexType _selectSmpId ):selectSmpId(_selectSmpId)
// 	{
// 		ui.setupUi(this);
// 	}
// 
// 	void init()
// 	{
// 		connect(ui.lineEdit, SIGNAL( textEdited( const QString&)), this, SLOT(getInitalValue(const QString&)) );
// 		
// 		connect(ui.pushButton, SIGNAL(clicked (bool) ),this, SLOT(runT(bool)) );
// 	
// 	}
// 
// public slots:
// public:
// 	void runT(bool b)
// 	{
// 		PlanClassifier* pFit = new PlanClassifier(selectSmpId);
// 
// 		pFit->setParem(selectSmpId,m_tradeOff);
// 
// 		connect(pFit,SIGNAL(finished()) ,pFit, SLOT(deleteLater() ));
// 
// 		pFit->start();
// 	}
// 
// 	void getInitalValue(const QString& txt)
// 	{
// 		m_tradeOff = txt.toFloat();
// 	}
// 
// private:
// 	ScalarType m_tradeOff;
// 
// 	IndexType selectSmpId;
// 
// 	Ui::PlanForm ui;
// 
// };
// #endif
// 
// 
// 

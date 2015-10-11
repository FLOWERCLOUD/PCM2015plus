#pragma  once

#include <QDialog>
#include <QString>
#include "basic_types.h"

#include "ui_papagate_dialog.h"
#include "propagate_withThread.h"

class PropagateUI : public QDialog
{
	Q_OBJECT

public:
	PropagateUI(IndexType _selectedSmpIdx): selectedSmpIdx_( _selectedSmpIdx)
	{
		ui_.setupUi( this);

	}
	void init();
	void setCurrentSmp(IndexType _curSmp)
	{
		selectedSmpIdx_ = _curSmp;
		ui_.center_frame_text->setText( QString::number(selectedSmpIdx_ ));
	}


	
	public slots:

	void run( bool b);
	void update_propagatefront_state( int _in)
	{
		is_propagate_front_ = _in;
	}
	void update_propagateback_state( int _in)
	{
		is_propagate_back_ = _in;
	}
	void update_centerframe( const QString& _in)
	{
		centerframe_ = _in.toInt();
	}
	void update_frontendframe( const QString& _in)
	{
		front_endframe_ = _in.toInt();

	}
	void update_backendframe( const QString& _in)
	{
		back_endframe_ = _in.toInt();
	}

private:
	IndexType selectedSmpIdx_;

	IndexType centerframe_;
	IndexType front_endframe_;
	IndexType back_endframe_;
	bool	is_propagate_front_;
	bool    is_propagate_back_;
	Ui::propagate_compute_dialog ui_;
	

};
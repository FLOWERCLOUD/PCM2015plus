#include "dlg_propagate.h"




void PropagateUI::init()
{
	Logger<< "propagateui init"<<std::endl;
	//Q_ASSERT(connect( ui_.propagate_front ,SIGNAL( stateChanged(int))  , this ,SLOT( update_propagatefront_state(int ) ) ) );
	//Q_ASSERT(connect(  ui_.propagate_front ,SIGNAL( clicked(bool)) , this , SLOT(run(bool)) ) );
	connect( ui_.propagate_front, SIGNAL( stateChanged(int)) , this , SLOT( update_propagatefront_state(int )) ) ;
	connect( ui_.propagate_back, SIGNAL( stateChanged(int)) , this , SLOT( update_propagateback_state(int )) ) ;
	connect( ui_.center_frame_text ,SIGNAL( textEdited(const QString& )) , this , SLOT(update_centerframe(const QString&)) ) ;
	connect( ui_.front_endframe_text ,SIGNAL( textEdited(const QString&)), this , SLOT(update_frontendframe( const QString&))) ;
	connect( ui_.back_endframe_text , SIGNAL( textEdited(const QString&)),this ,SLOT( update_backendframe( const QString&)) ) ;
	connect( ui_.propagate_compute ,SIGNAL( clicked(bool)) , this ,SLOT(run(bool)) ) ;
}

void PropagateUI::run(bool b)
{
	Logger<<"propagateui run"<<std::endl;
	is_propagate_front_ = ui_.propagate_front->isChecked();
	is_propagate_back_ = ui_.propagate_back->isChecked();

}

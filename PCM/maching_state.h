#pragma  once

#include "basic_types.h"
#include <QObject>
#include <QMainWindow>
#include <QTimer>
namespace ANIMATION{
	enum clickedId{ BACKID, STOPID, LABEL_COLOR ,RUNID ,PAUSEID, NEXTID};
	enum AState{STOPSTATE ,RUNSTATE, PAUSESTATE ,ERRORSTATE , BUFFERINGSTATE};
	class AbstractState{

		virtual bool clicked(  AState _prevstate, clickedId _clicked_id)
		{

		}

	};

	class StopState:public AbstractState{



		virtual bool clicked(AState _prevstate, clickedId _clicked_id)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	};
	class RunSate: public AbstractState{




		virtual bool clicked(AState _prevstate, clickedId _clicked_id)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	};

	class PauseState: public AbstractState{




		virtual bool clicked(AState _prevstate, clickedId _clicked_id)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	};

	class StateManager :public QObject
	{
		Q_OBJECT
	public:
		static  StateManager& getInstance(){

			static StateManager _instance;
			return _instance;
		}
		bool increaseInterval();

		bool decreaseInterval();
		bool pause();
		bool run();

		bool stop();
		void setState( AbstractState* _state)
		{
			if( NULL == _state )return ;
			if( cstate_ == NULL){
				cstate_ = _state;
			}else if( cstate_ != _state){
				delete cstate_;
				cstate_ = _state;

			}else if( cstate_ == _state){
				// 不作处理
			}

		}
		void clicked(  clickedId _clicked_id )
		{
			//if( newstate )

		}
		AbstractState* getState()
		{
			return cstate_;
		}
		AState state()
		{
		
			return cenumstate_;
		}
		void setWindowrefer(QMainWindow* _in)
		{
			qwindow_ = _in;
		}
	signals:
		void stateChanged(ANIMATION::AState x,ANIMATION::AState y);


	private:
		StateManager(){
			cenumstate_  = STOPSTATE;
			cstate_ = NULL;
			interval_= 50;
			step_ = 10;
		}

		AbstractState* cstate_;
		AbstractState* newstate;
		AState	cenumstate_;
		QTimer * frameTimer;   //added by huayun
		QMainWindow* qwindow_;
		IndexType		interval_;
		IndexType		step_;

	};


}

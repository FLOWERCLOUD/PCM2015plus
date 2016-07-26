#include "maching_state.h"




bool ANIMATION::StateManager::increaseInterval()
{
	interval_ = 300;
	//interval_ += step_;
	//if( interval_ > 1000){
	//	interval_ =  1000;
	//}
	if(  frameTimer->isActive()){
		frameTimer->start(interval_);

	}
	return false;
}

bool ANIMATION::StateManager::decreaseInterval()
{
	interval_ = 20;
	//interval_ -=step_;
	//if( interval_ < 10){
	//	interval_ = 10;
	//}
	if(  frameTimer->isActive()){
		frameTimer->start(interval_);

	}
	return false;
}

bool ANIMATION::StateManager::pause()
{
	AState oldState = cenumstate_;
	cenumstate_ = ANIMATION::PAUSESTATE;
	if( NULL != frameTimer){
		if( frameTimer->isActive()){

			frameTimer->stop();
		}

	}
	emit stateChanged(cenumstate_ , oldState);
	return false;
}

bool ANIMATION::StateManager::stop()
{
	AState oldState = cenumstate_;
	cenumstate_ = ANIMATION::STOPSTATE;
	if( NULL != frameTimer){
		if( frameTimer->isActive()){

			frameTimer->stop();
		}

	}

	emit stateChanged(cenumstate_ , oldState);
	return false;
}

bool ANIMATION::StateManager::run()
{
	bool i;
	AState oldState = cenumstate_;
	cenumstate_ = ANIMATION::RUNSTATE;
	//创建定时器
	if( NULL == frameTimer ){
		frameTimer = new QTimer(this);

		i =this->connect( frameTimer, SIGNAL(timeout()), qwindow_, SLOT( excFrameAnimation()) );
		frameTimer->start(interval_);
	}else if( ! frameTimer->isActive() ){

		frameTimer->start(interval_);
	}
	//Logger<<"曹"<<i<<std::endl;
	DualwayPropagation::get_instance().startframeAnimation(frameTimer);

	emit stateChanged(cenumstate_ , oldState);
	return false;
}

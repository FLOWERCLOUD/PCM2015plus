#ifndef _POINT2PLAN_DISTANCE
#define _POINT2PLAN_DISTANCE
#include "basic_types.h"

struct PlanModel
{
	PlanModel(){}
	NormalType pNorm;
	PointType  pCoor;
};

class Point2PlanDisFunc
{
public:
	typedef double DistanceType;

public:
	Point2PlanDisFunc(){}
	Point2PlanDisFunc(DistanceType l):lamda(l){}
	~Point2PlanDisFunc(){};

	DistanceType operator()(PointType pCo,PlanModel Plan)
	{	
	  return abs(Plan.pNorm.transpose() * (pCo - Plan.pCoor) );
	}

public:
	DistanceType lamda;
};



#endif // !_POINT2PLAN_DISTANCE

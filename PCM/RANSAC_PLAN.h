#ifndef _RANSAC_PLAN_H
#define _RANSAC_PLAN_H

#include <vector>
#include "basic_types.h"
using namespace std;

class Ransac_Plan
{

public:

	Ransac_Plan(){}

	Ransac_Plan(vector<PointType> & _points,vector<bool> _inliers,
		ScalarType _noise,IndexType _num,IndexType _iterNum):Points(_points),
		inliers(_inliers),noiseR(_noise),initPotNum(_num),iterNum(_iterNum)
	{
		center.setZero();
		pNorm.setZero();
		inlierNUm = 0;
	}

	Ransac_Plan(Matrix3X & _points,ScalarType _noise,IndexType _num,IndexType _iterNum):oriCoor(_points),
		noiseR(_noise),initPotNum(_num),iterNum(_iterNum)
	{
		inliers.resize(_points.cols(),false);

		center.setZero();
		pNorm.setZero();
		inlierNUm = 0;
	}

	~Ransac_Plan(){}


public:

	bool compute()
	{
		assert(noiseR <= 1.0 && noiseR > 0.);
        assert(iterNum >=1 && oriCoor.cols() > 3 && initPotNum >3);

		inliers.resize(oriCoor.cols(),false);

		initPlan();

		while (iterNum-- > 0)
		{
			dist2Plan();
			
			Matrix3X smpCoor;	
			
			getSmpCoor(smpCoor);

			calculateCenterNorm(smpCoor);
		}


		return true;
	}
	
	void initPlan()
	{
		Matrix3X startCoor;
		
		startCoor.setZero(3,initPotNum);

		IndexType i = 0;
		while (initPotNum-- > 0)
		{
			IndexType num = rand()%oriCoor.cols();
			startCoor.col(i) = oriCoor.col(num);
			inliers[num] = true;
			i ++;
		}

		calculateCenterNorm(startCoor);
	}

	void calculateCenterNorm(Matrix3X& pCoor)
	{
		center =  pCoor.rowwise().mean();

		for (IndexType i = 0; i<pCoor.cols();i++)
		{
			pCoor.col(i) -= center;
		}

		Matrix33 sigma = pCoor * pCoor.transpose();

		Eigen::EigenSolver<Matrix33> eigen_solver(sigma, (bool)(Eigen::ComputeFullU | Eigen::ComputeFullV) );

		auto ev = eigen_solver.eigenvectors();
		auto eval = eigen_solver.eigenvalues();
		ScalarType tmp[3] = { eval(0).real(),eval(1).real(),eval(2).real() };
		IndexType min_idx = std::min_element(tmp,tmp+3) - tmp;
		pNorm(0) = (ev.col(min_idx))(0).real();
		pNorm(1) = (ev.col(min_idx))(1).real();
		pNorm(2) = (ev.col(min_idx))(2).real();

		pNorm.normalize();

	}

	void dist2Plan()
	{
		
		vector<ScalarType> distRecord;

		distRecord.resize(oriCoor.cols(),0.);

		for (IndexType i = 0; i < oriCoor.cols(); i ++)
		{
			PointType coor = oriCoor.col(i);

			ScalarType dist = pNorm.transpose() * (coor - center);

			distRecord[i] = abs(dist);
		}

		vector<ScalarType> distCopy;

		distCopy = distRecord;

		sort(distRecord.begin(),distRecord.begin() + distRecord.size());

		ScalarType markDist = distRecord [(noiseR * distRecord.size())];

		inlierNUm = 0;

		for (auto it = inliers.begin(); it != inliers.end(); it++)
		{
              (*it) = false;    
		}

		for (IndexType i = 0; i < oriCoor.cols(); i ++)
		{
			if (distCopy[i] <= markDist )
			{
				inliers[i] = true;
				inlierNUm ++;
			}
		}
	}

	void getSmpCoor(Matrix3X& smpCoor)
	{
		smpCoor.setZero(3,inlierNUm);

		IndexType j = 0;
		for (IndexType i = 0; i < oriCoor.cols(); i ++)
		{
			if (inliers[i])
			{
				smpCoor.col(j) = oriCoor.col(i);
				j ++;
			}
		}
	}

	PointType& getCenter()
	{
		return center;
	}

	NormalType& getNormal()
	{
		return pNorm;
	}

	vector<bool>& getInliers()
	{
		return inliers;
	}
private:
	vector<PointType> Points;
	Matrix3X    oriCoor;
	vector<bool> inliers;
	PointType  center;
	NormalType pNorm;
	ScalarType noiseR;
	IndexType  initPotNum;
	IndexType  iterNum; 
	IndexType inlierNUm;
};
#endif
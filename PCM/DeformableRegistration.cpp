#include "DeformaleRegistration.h"
#include "globals.h"
#include "basic_types.h"
#include "sample.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include "map"
#include "list"
#include "ICP/J_linkage.h"
#include"sample_properity.h"
#include "sample_set.h"
#include "vertex.h"

//#define  PI 3.141592653;
//#include "ICP.h"

DeformableRegistration::DeformableRegistration():
	m_smpSet(SampleSet::get_instance())
{
	m_neigNum = 30;
	//Logger<<"Local ICP using neighbor K ="<<m_neigNum<<endl;
}

void DeformableRegistration::setNeigNum(IndexType _neigN)
{
	this->m_neigNum = _neigN;
}


void DeformableRegistration::run()
{
	Logger<<"Begin Register.\n";
 	Sample & srGraph = m_smpSet[0];
 	Sample & tgGraph = m_smpSet[1];
// 	Sample & srCloud = m_smpSet[2];
// 	Sample & tgCloud = m_smpSet[3];

 	IndexType	num_vtx	= srGraph.num_vertices();
	//IndexType   srcloud_num_vtx = srCloud.num_vertices();


	//updateCooByTPS(srCloud,orisrGraphCoor,srGraph,tgGraph);


 	MatrixXXi vtx_cor;
 	Matrix3X  restPos;
	Matrix3X  orisrGraphCoor;
	MatrixXX  isDeformable;
 	vtx_cor.resize(1,num_vtx);
 	restPos.resize(3,num_vtx);
	orisrGraphCoor.resize(3,num_vtx);
	isDeformable.setZero(1,num_vtx);
	//global  ICP
	SICP::Parameters pa(false,2.0,10,1.2,1e5,100,100,1,1e-5);
	SICP::point_to_point(srGraph.vertices_matrix(),tgGraph.vertices_matrix(),vtx_cor,pa);
	srGraph.update();

	//init distance field by srGraph coordinate
//      	orisrGraphCoor = srGraph.vertices_matrix();
//      	IndexType iterMax  = 1;
//       	while (iterMax-->0)
//       	{     
//       		////using loacl ICP
//   			///loaclICP(srGraph,tgGraph,vtx_cor,restPos);
//   			///optimizeCoor(srGraph,tgGraph,vtx_cor);
//       		///combinationCoor(0.95,srGraph,tgGraph,restPos,vtx_cor);
//  
//  			//// using threshold to combinate coordinate----2014-9-9
//  			localICP(srGraph,tgGraph,vtx_cor,restPos,isDeformable);
//  			optimizeCoor(srGraph,tgGraph,vtx_cor);
//  			combinationCoor(0.95,srGraph,tgGraph,restPos,vtx_cor,isDeformable);
//  
//       	}
	// update srCloud coordinte by TPS
	//updateCooByTPS(srCloud,orisrGraphCoor,srGraph,tgGraph);


	//test-9-10
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	ScalarType * neigDis = new ScalarType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);
	MatrixXXi loaclCor;
	//SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	loaclCor.setZero(1,m_neigNum);
	VecX vtxNeigCoor;
	vtxNeigCoor.setZero(m_neigNum,1);

	srGraph.neighbours(8000,m_neigNum,neigSrGraph,neigDis);
	tgGraph.neighbours(vtx_cor(0,8000),m_neigNum,neigTgGraph);
// 	cout<<"ICP 8000 ==="<<vtx_cor(0,8000)<<endl;
// 	for (IndexType v_it = 0;v_it < m_neigNum;v_it++)
// 	{
// 		vtxNeigCoor(v_it,0) = neigSrGraph[v_it];//转化成eigen
// 	}


// 	cout<<"sr neig"<<endl;
// 	for (IndexType v_it = 0;v_it < m_neigNum;v_it++)
// 	{
// 		cout<<neigSrGraph[v_it]<<endl;
// 	}

// 	cout<<"tg neig"<<endl;
//  	for (IndexType v_it = 0;v_it < m_neigNum;v_it++)
//  	{
//  		cout<<neigTgGraph[v_it]<<endl;
//  	}

	getConstantNeigCoorByIndex(srGraph,neigSrGraph,neigSrCoor);
	getConstantNeigCoorByIndex(tgGraph,neigTgGraph,neigTgCoor);

// 	Logger<<"befor ICP source.\n";
// 	Logger<<neigSrCoor<<endl;
// 
// 	Logger<<"befor ICP target.\n";
// 	Logger<<neigTgCoor<<endl;

	VecX oriNeigDis;
	VecX resNeigDis;
	resNeigDis.setZero(m_neigNum,1);
	oriNeigDis.setZero(m_neigNum,1);
	calculateNeigDis(neigSrCoor,oriNeigDis);
// 	cout<<"ori dist"<<endl;
// 	Logger<<oriNeigDis<<endl;

	SICP::point_w_point(neigSrCoor,neigTgCoor,loaclCor,pa);
// 	cout<<"local cor"<<endl;
// 	for (IndexType v_it = 0;v_it < m_neigNum;v_it++)
// 	{
// 		cout<<neigSrGraph[v_it]<<"对应着"<<vtx_cor(0,neigSrGraph[v_it]);
// 		cout<<"local"<<neigTgGraph[loaclCor(0,v_it)]<<endl;
// 	}


// 	Logger<<"after ICP.\n";
// 	Logger<<neigSrCoor<<endl;

	calculateNeigDis(neigTgCoor,resNeigDis);
// 	cout<<"target dis"<<endl;
// 	Logger<<resNeigDis<<endl;

	ScalarType dmeasure = deformableValue(oriNeigDis,resNeigDis);
	cout<<"distor = "<<dmeasure<<endl;
	updateLocalCoordinate(srGraph,neigSrCoor,vtxNeigCoor);
	// draw trajectory
	Tracer& cor = Tracer::get_instance();
	cor.clear_records();
	cor.add_percentCor(0,1,vtx_cor,20,num_vtx);


	Logger<<"end register \n";

}

void DeformableRegistration::estimateCorrespondence(Sample &srGraph,Sample & tgGraph,MatrixXXi & res_cor)
{
	IndexType	num_vtx	= srGraph.num_vertices();
// 	res_cor.resize(1,num_vtx);
// 	res_cor.setZero();
	Matrix3X    distance(3,num_vtx);
	ScalarType  total_dis = 0.0;

	//find a closest point in tgGraph and calculate distance
	for (IndexType v_idx = 0; v_idx < num_vtx;v_idx ++)
	{
		unsigned int mp = tgGraph.closest_vtx(srGraph.vertices_matrix().col(v_idx));
		res_cor(0,v_idx) = mp;
		distance.col(v_idx) = tgGraph.vertices_matrix().col(mp) - srGraph.vertices_matrix().col(v_idx);
	}

// 	Tracer& tracer = Tracer::get_instance();
// 	tracer.clear_records();
// 	tracer.add_percentCor(0,1,res_cor,1,num_vtx);
//  	 	     			std::fstream op("E:\\distance_1.txt",std::ios::app);
//  	 	     			if (op.is_open())
//  	 	     			{
// 							for (IndexType rowIndex = 0; rowIndex < num_vtx; rowIndex++)
// 							{
// 								op<<distance.col(rowIndex).transpose()<<endl;
// 							}
//  	 	                     //op<<distance<<endl;
//  	 	     				op.close();
//  	 	     			}
//  	 	     			else
//  	 	     			{
//  	 	     				cout<<"open file error！"<<endl;
//  	 	     				return ;
//  	 	     			}

	//calculate total distance

     total_dis = totalDistance(srGraph,m_neigNum,distance);
   	
   	//update correspondence
   
   	IndexType *tg_neig = new IndexType[m_neigNum];
   	PointType srPoint;
   	IndexType vtx_cor = 0;
   	Vec3 srMean;
   	srPoint.setZero();
   	srMean.setZero();
   	IndexType newCor = 0;
   	ScalarType criterion;
   	IndexType iterMax = 10;
	Matrix3X temp;
	temp.setZero(3,num_vtx);
   	while (iterMax-- > 0)
   	{
   		criterion = total_dis;
   		for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
   		{
   			srPoint = srGraph.vertices_matrix().col(v_idx);
   			vtx_cor = res_cor(0,v_idx);
   			tgGraph.neighbours(vtx_cor,m_neigNum,tg_neig);             //find neighbor of vtx_cor in tgGraph
   			avgDistancePoint(v_idx,srGraph,m_neigNum,distance,srMean); 
   			res_cor(0,v_idx) = findMinCorr(srPoint,srMean,tg_neig,tgGraph);//update correspondence
   			//distance.col(v_idx) = tgGraph.vertices_matrix().col(newCor) - srPoint;//update distance matrix		
			temp.col(v_idx) =  tgGraph.vertices_matrix().col(newCor) - srPoint;
   		}
		distance = temp;
    		total_dis = totalDistance(srGraph,m_neigNum,distance);
    		if (total_dis > criterion) break;	//???	
    }


 // Logger<<"iter number = "<<iterMax<<endl;
// 	Tracer& tracer = Tracer::get_instance();
// 	tracer.clear_records();
// 	tracer.add_percentCor(0,1,res_cor,1,num_vtx);

	// draw correspondence
// 	for (IndexType corInd = 0; corInd < 10; corInd++)
// 	{
// 		Logger<<res_cor(0,corInd)<<endl;
// 	}

}
void DeformableRegistration::calculateRestPos(Sample& srGraph,Sample& tgGraph,MatrixXXi & vtx_cor,Matrix3X & restPos)
{
	IndexType	num_vtx	= srGraph.num_vertices();
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);
	PointType srPoint;
	for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
	{
		neigSrCoor.setZero();neigTgCoor.setZero();srPoint.setZero();
		srPoint = srGraph.vertices_matrix().col(v_idx);
		srGraph.neighbours(v_idx,m_neigNum,neigSrGraph);
		tgGraph.neighbours(vtx_cor(0,v_idx),m_neigNum,neigTgGraph);
		//getCorNeigIndex(neigSrGraph,neigTgGraph,vtx_cor);

		getConstantNeigCoorByIndex(srGraph,neigSrGraph,neigSrCoor);
		getConstantNeigCoorByIndex(tgGraph,neigTgGraph,neigTgCoor);
		updateCoor(srPoint,neigSrCoor,neigTgCoor);
		restPos.col(v_idx) = srPoint;
	}
}
 void DeformableRegistration::getCorNeigIndex(IndexType * srNeigIndex,IndexType * tgNeigIndex,MatrixXXi & corMat)
 {
 	for (IndexType v_it = 0; v_it < m_neigNum;v_it ++)
 	{
 		tgNeigIndex[v_it] = corMat(0,srNeigIndex[v_it]);
 	}
 }
IndexType DeformableRegistration::findMinCorr(PointType & srPoint,Vec3& mean, int * neigIndex,Sample & smp)
{
	PointType tgPoint;
	IndexType minIndex = 0;
	ScalarType diff = 1e7;
	srPoint += mean;
	for (IndexType neig_it = 0; neig_it < m_neigNum;neig_it++)
	{
		tgPoint = smp.vertices_matrix().col(neigIndex[neig_it]);
		tgPoint -= srPoint;
		if (tgPoint.squaredNorm() < diff)
		{
			diff = tgPoint.squaredNorm();
			minIndex = neigIndex[neig_it];
		}
	}

	return minIndex;
}
ScalarType DeformableRegistration::avgDistancePoint(IndexType pointIndex,Sample & smp,IndexType neigNum,Matrix3X & dst,Vec3 & mean)
{
	assert(neigNum > 0);
	ScalarType dis = 0.0;
	IndexType *sr_neig = new IndexType[neigNum];
	Vec3 ori_point;
	ori_point.setZero();
    mean.setZero();

	ori_point = smp.vertices_matrix().col(pointIndex);
	smp.neighbours(pointIndex,neigNum,sr_neig);

	for (IndexType neigIndex = 0; neigIndex < neigNum; neigIndex++)
	{
		mean += dst.col(sr_neig[neigIndex]);
	}
	mean /= neigNum;

  return (ori_point - mean).squaredNorm();
}
ScalarType DeformableRegistration::totalDistance(Sample &smp,IndexType neigNum,Matrix3X & dst)
{
	ScalarType tot_dis = 0.0;
	IndexType vtx_num = dst.cols();
	Vec3 mean_;
	for (IndexType v_idx = 0; v_idx < vtx_num; v_idx++)
	{
		tot_dis += avgDistancePoint(v_idx,smp,neigNum,dst,mean_);
	}
	return tot_dis;
}
void DeformableRegistration::getNeigCoorByIndex(Sample & smp,vector<IndexType> & neigIndex,Matrix3X & res_coor)
{
	IndexType neigsize = neigIndex.size();
	for (IndexType v_it = 0; v_it < neigsize; v_it++)
	{
		 res_coor.col(v_it) = smp.vertices_matrix().col(neigIndex[v_it]);
	}
}
void DeformableRegistration::getConstantNeigCoorByIndex(Sample & smp,IndexType * neigIndex,Matrix3X & res_coor)
{
	for (IndexType v_it = 0; v_it < m_neigNum;v_it++)
	{
		res_coor.col(v_it) = smp.vertices_matrix().col(neigIndex[v_it]);
	}
}

void DeformableRegistration::updateCoor(PointType& srpoint,Matrix3X & srNeigCoor,Matrix3X & tgNeigCoor)
{
 	Vec3 X_mean,Y_mean; 
 	Matrix33 rotMat;
 	Vec3 trans;
 	ScalarType scl = 1.0;
 	rotMat.setIdentity();
 	trans.setZero();
 	for (IndexType axisIndex = 0; axisIndex < 3; axisIndex ++)
 	{
 		X_mean(axisIndex) = srNeigCoor.row(axisIndex).sum()/srNeigCoor.cols();
 		Y_mean(axisIndex) = tgNeigCoor.row(axisIndex).sum()/tgNeigCoor.cols();
 	}
 	srNeigCoor.colwise() -= X_mean;
 	tgNeigCoor.colwise() -= Y_mean;
 
 	Matrix33 sigma = srNeigCoor * tgNeigCoor.transpose();
 	Eigen::JacobiSVD<Eigen::Matrix3f> svd(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
 	if(svd.matrixU().determinant()*svd.matrixV().determinant() < 0.0)//contains reflection
 	{
 		Eigen::Vector3f S = Eigen::Vector3f::Ones(); S(2) = -1.0;
 		 rotMat = svd.matrixV()*S.asDiagonal()*svd.matrixU().transpose();
 	} else 
 	{
 		rotMat = svd.matrixV()*svd.matrixU().transpose();//calculate rotate matrix
 	}
 
 	//ScalarType molecule = srNeigCoor.colwise().squaredNorm().sum();
 	//ScalarType denominator = tgNeigCoor.colwise().squaredNorm().sum();
    // assert( denominator > 1e-5);
 	//scl = sqrt(molecule/denominator);
 
 	srpoint = /*scl * */(rotMat* (srpoint - X_mean))  + Y_mean; 
}
void DeformableRegistration::combinationCoor(ScalarType stiffCoff,Sample & srGraph,Sample & trGraph,Matrix3X & restPos,MatrixXXi& vtx_cor)
{
	IndexType	num_vtx	= srGraph.num_vertices();
	PointType temp ;
	temp.setZero();
	for (IndexType v_it = 0; v_it < num_vtx; v_it++)
	{
		temp = stiffCoff * restPos.col(v_it) + (1- stiffCoff) * trGraph.vertices_matrix().col(vtx_cor(0,v_it));
		srGraph.vertices_matrix().col(v_it) = temp;
	}
}

void DeformableRegistration::combinationCoor(ScalarType stiffCoff,Sample & srGraph,Sample & trGraph,
											 Matrix3X & restPos,MatrixXXi& vtx_cor,MatrixXX& isDeformable,std::set<IndexType> & distLrgIndex)
{
	ScalarType threshold = 0.2;
	IndexType	num_vtx	= srGraph.num_vertices();
	//IndexType recordDistortionNum05 = 0;
	PointType temp ;
	temp.setZero();
// 	 	std::fstream op("E:\\distor.txt",std::ios::app);
// 		if (op.is_open())
// 		{
// 		}
// 		else
// 		{
// 			cout<<"open file error！"<<endl;
// 			return ;
// 		}
	for (IndexType v_it = 0; v_it < num_vtx; v_it++)
	{
		if (isDeformable(0,v_it) < threshold)
		{
		   temp = stiffCoff * restPos.col(v_it) + (1- stiffCoff) * trGraph.vertices_matrix().col(vtx_cor(0,v_it));
		   srGraph.vertices_matrix().col(v_it) = temp;
		}else
		{
          //recordDistortionNum05++;
		 // op<<v_it<<endl;
          srGraph.vertices_matrix().col(v_it) = restPos.col(v_it);
		  distLrgIndex.insert(v_it);
		}

	}
// 	op<<"this iter is  over  wait the next time !!!!!!.\n";
// 	op.close();
//	Logger<<"out of distortion number = "<<recordDistortionNum05<<endl;
}
void DeformableRegistration::updateCooByTPS(Sample & srCloud,Matrix3X & oriCoor, Sample & srGraph,Sample& tgGraph)
{
	IndexType num_vtx = srGraph.num_vertices();
	Matrix3X  mapFunctionCoff;
	mapFunctionCoff.setZero(3,(num_vtx + 4));
	//calculateMapFunction(oriCoor,srGraph,0,mapFunctionCoff); // X direction
	//calculateMapFunction(oriCoor,srGraph,1,mapFunctionCoff); // Y direction
	//calculateMapFunction(oriCoor,srGraph,2,mapFunctionCoff); // Z direction

	//interpolateCoordinate(srCloud,mapFunctionCoff,tgGraph);

	srCloud.update();//update srCloud coordinate
}
void DeformableRegistration::deformationUsingTPS(Sample& srCloud,Sample& tgCloud,IndexType feaNum)
{
	Matrix3X  srGraph,tgGraph;
	Matrix3X  mapFunctionCoff;
	vector<bool> isSample;
	mapFunctionCoff.setZero(3,(feaNum + 4));

	//getSamplePoints(srCloud,srGraph,feaNum);
	//getSamplePoints(tgCloud,tgGraph,feaNum);

	getSamplePointsAll(srCloud,srGraph,tgCloud,tgGraph,feaNum,isSample);

	calculateMapFunction(srGraph,tgGraph,0,mapFunctionCoff); // X direction
	calculateMapFunction(srGraph,tgGraph,1,mapFunctionCoff); // Y direction
	calculateMapFunction(srGraph,tgGraph,2,mapFunctionCoff); // Z direction

	interpolateCoordinate(srCloud,mapFunctionCoff,tgGraph,isSample);
	srCloud.update();//update srCloud coordinate

}
void DeformableRegistration::calculateMapFunction(Matrix3X& srGraphCoor,Matrix3X& tgGraphCoor,IndexType axisIndex, Matrix3X & res_mapFunc)
{
	IndexType num_vtx = srGraphCoor.cols();
	MatrixXX  Left_mat;
	MatrixXX  Right_mat;
	MatrixXX  Upper_mat;
	MatrixXX  BottomMat;
	Left_mat.setZero(num_vtx + 4,num_vtx +4);
	Right_mat.setZero(num_vtx + 4,1);
	Upper_mat.setZero(num_vtx,num_vtx);
	BottomMat.setZero(4,4);
	Left_mat.col(num_vtx).setOnes();
	Left_mat.row(num_vtx).setOnes();

	//std::fstream op("E:\\22leftmat100.txt",std::ios::app);
	constructUpperMat(Upper_mat,srGraphCoor,tgGraphCoor,axisIndex);

	//assignment 
	Left_mat.block(0,0,num_vtx,num_vtx) = Upper_mat.block(0,0,num_vtx,num_vtx);
	Left_mat.block(0,num_vtx + 1,num_vtx,3) = (srGraphCoor.transpose()).block(0,0,num_vtx,3);
	Left_mat.block(num_vtx + 1,0,3,num_vtx) = srGraphCoor.block(0,0,3,num_vtx);//block
	Left_mat.block(num_vtx ,num_vtx ,4,4) = BottomMat.block(0,0,4,4);


	//Logger<<"Left_mat.\n";
	//Logger<<Left_mat<<endl;//have infinite element
	  // if (op.is_open())
 		//{  
			//op<<Left_mat<<endl;
 		//	//op.close();
 		//}
 		//else
 		//{
 		//	cout<<"open file error！"<<endl;
 		//	return ;
 		//}

	//assignment
	Right_mat.block(0,0,num_vtx,1) = tgGraphCoor.row(axisIndex).transpose().block(0,0,num_vtx,1);//block
	Right_mat.block(num_vtx,0,4,1) = BottomMat.block(0,0,4,1);

	//op<<Right_mat<<endl;
	//op.close();

	sloveLinearSysByLU(Left_mat,Right_mat,res_mapFunc,axisIndex);

}
void DeformableRegistration::sloveLinearSysByLU(MatrixXX & LeftMat,MatrixXX & RightMat,Matrix3X& res_mapFunc,IndexType axisIndex)
{
	//A = L * U; L * y = b; U * x = y;//must Lu decompostion
	//MatrixXX L,U;
	//Eigen::FullPivLU<MatrixXX> lu(LeftMat);
	//L = lu.matrixLU().triangularView<Eigen::StrictlyLower>();
	//U = lu.matrixLU().triangularView<Eigen::Upper>();

	//Logger<<"lower = \n";
	//Logger<<L<<endl;
	//Logger<<"L inverse\n";
	//Logger<<L.inverse()<<endl;

	//Logger<<"upper = \n";
	//Logger<<U<<endl;
	//Logger<<"U inverse\n";
	//Logger<<U.inverse()<<endl;

	//MatrixXX temp = LeftMat.inverse();
	//MatrixXX res = temp * RightMat;
	//res_mapFunc.row(axisIndex) = res.transpose();	

	//std::fstream op("E:\\LUMat.txt",std::ios::app);
	//res_mapFunc.row(axisIndex) = LeftMat.lu().solve(RightMat).transpose();



	//X = (A'A).inverse() * A'*b;
	MatrixXX temp = (LeftMat.transpose() * LeftMat);
	MatrixXX tpInverse = temp.inverse();
	res_mapFunc.row(axisIndex) = (tpInverse * LeftMat.transpose() * RightMat).transpose();
	  // if (op.is_open())
 		//{  
			//op<<res_mapFunc.row(axisIndex).transpose()<<endl;
 		//	op.close();
 		//}
 		//else
 		//{
 		//	cout<<"open file error！"<<endl;
 		//	return ;
 		//}

}
void DeformableRegistration::constructUpperMat(MatrixXX & upperMat,Matrix3X & srCoor,Matrix3X& tgCoor,IndexType axisIndex)
{
	IndexType num_vtx = srCoor.cols();
	for (IndexType rowIdx = 0; rowIdx < num_vtx; rowIdx++)
	{
		for (IndexType colIdx = 0; colIdx <= rowIdx; colIdx++)
		{
			PointType ori = srCoor.col(rowIdx);
			PointType cur = tgCoor.col(colIdx);//////?2014-10-21
			ScalarType len = (cur - ori)[axisIndex];
			ScalarType dis = tpsRBF(abs(len));
			upperMat(rowIdx,colIdx) = dis;
			upperMat(colIdx,rowIdx) = dis;
		}
	}
}
void DeformableRegistration::interpolateCoordinate(Sample & srCloud,Matrix3X & mapFunctionCoff,Matrix3X& tgGraphCoor,vector<bool>& isSample)
{
	IndexType sr_num_vtx = srCloud.num_vertices();
	IndexType contr_num_vtx = tgGraphCoor.cols();
	MatrixXX  rbfCoff;

	PointType sPoint;
	PointType cPoint;
	ScalarType dis = 0.0;
	ScalarType uCoor = 0.0;
	IndexType  smpIter = 0;
	for (IndexType v_it = 0; v_it < sr_num_vtx; v_it++)
	{
		if(isSample[v_it])
		{
			srCloud.vertices_matrix().col(v_it) = tgGraphCoor.col(smpIter);
			smpIter++;
		}
		else
		{
			sPoint = srCloud.vertices_matrix().col(v_it);
			rbfCoff.setZero(contr_num_vtx + 4,1);
			rbfCoff(contr_num_vtx,0) = 1.0;
			rbfCoff.block(contr_num_vtx + 1,0,3,1) = sPoint.block(0,0,3,1);
			for (IndexType axisIndex = 0; axisIndex < 3; axisIndex++)
			{
				for (IndexType contr_v_it = 0; contr_v_it < contr_num_vtx; contr_v_it ++)
				{
					cPoint = tgGraphCoor.col(contr_v_it);
					dis = cPoint(axisIndex,0) - sPoint(axisIndex,0);
					rbfCoff(contr_v_it,0)  = tpsRBF(abs(dis));
				}
				Logger<<"functions coff.\n";
				Logger<<mapFunctionCoff.row(axisIndex)<<endl;
				Logger<<"coor"<<endl;
				Logger<<rbfCoff.transpose()<<endl;

				uCoor = mapFunctionCoff.row(axisIndex) * rbfCoff.col(0);
				Logger<<uCoor<<endl;
				srCloud.vertices_matrix().col(v_it)[axisIndex] = uCoor;
			}
		}//end if

	}

}
ScalarType DeformableRegistration::tpsRBF(ScalarType radius)
{
	if( 0.0 == radius) return 0.0;
	return radius * radius * log(radius);
	//return radius * radius * radius;
}

void DeformableRegistration::getSamplePointsAll(Sample&srCloud,Matrix3X& sampleSrPoints,
		                                        Sample&tgCloud,Matrix3X& sampleTgPoints,
							                    IndexType pointNum,vector<bool>& isSample)
{
	IndexType totPoints = srCloud.num_vertices();
	IndexType sampleValue = 0;

	sampleSrPoints.setZero(3,pointNum);
	sampleTgPoints.setZero(3,pointNum);
	isSample.resize(totPoints,false);

	for(IndexType randIter = 0; randIter < pointNum; randIter ++)
	{
		sampleValue = rand() % totPoints;
		Logger<<"rand number = "<<sampleValue<<endl;
		sampleSrPoints.col(randIter) = srCloud.vertices_matrix().col(sampleValue);
		sampleTgPoints.col(randIter) = tgCloud.vertices_matrix().col(sampleValue);
		isSample[sampleValue] = true;
	}
}
void DeformableRegistration::loaclICP(Sample & srGraph,Sample& tgGraph,MatrixXXi & vtx_cor,Matrix3X& restPos)
{

	IndexType num_vtx = srGraph.num_vertices();
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);

	//loacl ICP 
	//SICP::Parameters pa;
	SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	MatrixXXi loaclCor;
	loaclCor.setZero(1,m_neigNum);

	//find a closest point in tgGraph to init correspondence
	for (IndexType v_idx = 0; v_idx < num_vtx;v_idx ++)
	{
		unsigned int mp = tgGraph.closest_vtx(srGraph.vertices_matrix().col(v_idx));
		vtx_cor(0,v_idx) = mp;		
	}


 	for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
 	{
 		neigSrCoor.setZero();
 		neigTgCoor.setZero();
 		srGraph.neighbours(v_idx,m_neigNum,neigSrGraph);
 		tgGraph.neighbours(vtx_cor(0,v_idx),m_neigNum,neigTgGraph);
 
 		getConstantNeigCoorByIndex(srGraph,neigSrGraph,neigSrCoor);
 		getConstantNeigCoorByIndex(tgGraph,neigTgGraph,neigTgCoor);
 
 		SICP::point_to_point(neigSrCoor,neigTgCoor,loaclCor,pa);
		restPos.col(v_idx) = neigSrCoor.col(0);

 		vtx_cor(0,v_idx) = neigTgGraph[loaclCor(0,0)];
 	}

// 	Tracer& cor = Tracer::get_instance();
// 	cor.clear_records();
// 	cor.add_percentCor(0,1,vtx_cor,30,num_vtx);

}

void DeformableRegistration::localICP(Sample & srGraph,Sample& tgGraph,MatrixXXi & vtx_cor, Matrix3X& restPos,
									  MatrixXX& isDeformable)
{
	IndexType num_vtx = srGraph.num_vertices();
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);

	//ScalarType * neigDis = new ScalarType[m_neigNum];
	VecX neigDis;     //record ori distance
	VecX resNeigDis;  // record target distances 
	neigDis.setZero(m_neigNum,1);
	resNeigDis.setZero(m_neigNum,1);

	//loacl ICP 
	SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	MatrixXXi loaclCor;
	loaclCor.setZero(1,m_neigNum);

	 	//std::fstream op("E:\\isdeformable.txt",std::ios::app);


	//find a closest point in tgGraph to init correspondence//test----using global correspondence

// 	for (IndexType v_idx = 0; v_idx < num_vtx;v_idx ++)
// 	{
// 		unsigned int mp = tgGraph.closest_vtx(srGraph.vertices_matrix().col(v_idx));
// 		vtx_cor(0,v_idx) = mp;		
// 	}


	for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
	{
		neigSrCoor.setZero();
		neigTgCoor.setZero();

		srGraph.neighbours(v_idx,m_neigNum,neigSrGraph);
		tgGraph.neighbours(vtx_cor(0,v_idx),m_neigNum,neigTgGraph);// in order to local ICP

		getConstantNeigCoorByIndex(srGraph,neigSrGraph,neigSrCoor);
		getConstantNeigCoorByIndex(tgGraph,neigTgGraph,neigTgCoor);// in order to local ICP
		calculateNeigDis(neigSrCoor,neigDis);//calculate the ori distance around the v_idx 为了计算扭曲量用到的距离

		//isDeformable(0,v_idx) = deformableTotal(neigSrCoor,neigTgCoor);

		SICP::point_w_point(neigSrCoor,neigTgCoor,loaclCor,pa);

		restPos.col(v_idx) = neigSrCoor.col(0);//record the res position(rigid ICP)
		vtx_cor(0,v_idx) = neigTgGraph[loaclCor(0,0)];//update the correspondence of v_idx in tgGraph



//计算中心点到邻域边
  		tgGraph.neighbours(vtx_cor(0,v_idx),m_neigNum,neigTgGraph);// in order calculate the distance near the corr vertex;
  		getConstantNeigCoorByIndex(tgGraph,neigTgGraph,neigTgCoor);// the same above
  		calculateNeigDis(neigTgCoor,resNeigDis);
  
  		isDeformable(0,v_idx) = deformableValue(neigDis,resNeigDis);//


	}

	delete []neigSrGraph;
	delete []neigTgGraph;	
}
//----------------------------
void DeformableRegistration::localICPDisto(Sample & srGraph,Sample& tgGraph,MatrixXXi & vtx_cor, Matrix3X& restPos,MatrixXX& isDeformable)
{
	IndexType num_vtx = srGraph.num_vertices();
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);
	Matrix3X OrineigSrCoor;

	VecX neigDis;     //record ori distance
	VecX resNeigDis;  // record target distances 
	neigDis.setZero(m_neigNum,1);
	resNeigDis.setZero(m_neigNum,1);

	//loacl ICP 
	SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	MatrixXXi localCor;
	localCor.setZero(1,m_neigNum);


	for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
	{
		neigSrCoor.setZero();
		neigTgCoor.setZero();

		srGraph.neighbours(v_idx,m_neigNum,neigSrGraph);
		tgGraph.neighbours(vtx_cor(0,v_idx),m_neigNum,neigTgGraph);// in order to local ICP

		getConstantNeigCoorByIndex(srGraph,neigSrGraph,neigSrCoor);
		getConstantNeigCoorByIndex(tgGraph,neigTgGraph,neigTgCoor);// in order to local ICP
		calculateNeigDis(neigSrCoor,neigDis);//calculate the ori distance around the v_idx 为了计算扭曲量用到的距离

		OrineigSrCoor = neigSrCoor;

		SICP::point_w_point(neigSrCoor,neigTgCoor,localCor,pa);

		restPos.col(v_idx) = neigSrCoor.col(0);//record the res position(rigid ICP)
		vtx_cor(0,v_idx) = neigTgGraph[localCor(0,0)];//update the correspondence of v_idx in tgGraph



		//计算中心点到邻域边
		alignTargetCoor(neigTgCoor,localCor);
		//calculateNeigDis(neigTgCoor,resNeigDis);
		//isDeformable(0,v_idx) = deformableValue(neigDis,resNeigDis);//

		isDeformable(0,v_idx) = deformableTotal(OrineigSrCoor,neigTgCoor);
	}

	delete []neigSrGraph;
	delete []neigTgGraph;	
}
//----------------------------
void DeformableRegistration::optimizeCoor(Sample&srGraph,Sample&tgGraph,MatrixXXi & res_cor)
{
	IndexType	num_vtx	= srGraph.num_vertices();
	Matrix3X    distance(3,num_vtx);
	ScalarType  total_dis = 0.0;

	for (IndexType v_idx = 0; v_idx < num_vtx;v_idx ++)
	{
		unsigned int mp = res_cor(0,v_idx);
		distance.col(v_idx) = tgGraph.vertices_matrix().col(mp) - srGraph.vertices_matrix().col(v_idx);
	}

	//calculate total distance
 	total_dis = totalDistance(srGraph,m_neigNum,distance);
 
 	//update correspondence
 
 	IndexType *tg_neig = new IndexType[m_neigNum];
 	PointType srPoint;
 	IndexType vtx_cor = 0;
 	Vec3 srMean;
 	srPoint.setZero();
 	srMean.setZero();
 	IndexType newCor = 0;
 	ScalarType criterion;
 	IndexType iterMax = 2;
 	Matrix3X temp;
 	temp.setZero(3,num_vtx);
 	while (iterMax-- > 0)
 	{
 		criterion = total_dis;
 		for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
 		{
 			srPoint = srGraph.vertices_matrix().col(v_idx);
 			vtx_cor = res_cor(0,v_idx);
 			tgGraph.neighbours(vtx_cor,m_neigNum,tg_neig);             //find neighbor of vtx_cor in tgGraph
 			avgDistancePoint(v_idx,srGraph,m_neigNum,distance,srMean); 
 			res_cor(0,v_idx) = findMinCorr(srPoint,srMean,tg_neig,tgGraph);//update correspondence		
 			temp.col(v_idx) = tgGraph.vertices_matrix().col(newCor) - srPoint;
 		}
 		distance = temp;
 		total_dis = totalDistance(srGraph,m_neigNum,distance);
 		if (total_dis > criterion) break;	//???	
 	}
}
 void DeformableRegistration::point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat)
 {
 	Eigen::Vector3f X_mean, Y_mean;
 	for(int i=0; i<3; ++i) //计算两点云的均值
 	{
 		X_mean(i) = srCloud.row(i).sum()/srCloud.cols();
 		Y_mean(i) = tgCloud.row(i).sum()/tgCloud.cols();
 	}
 
 	srCloud.colwise() -= X_mean;
 	tgCloud.colwise() -= Y_mean;
 
 	/// Compute transformation
 	Eigen::Affine3f transformation;
 	Eigen::Matrix3f sigma = srCloud * tgCloud.transpose();
 	Eigen::JacobiSVD<Eigen::Matrix3f> svd(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
 	if(svd.matrixU().determinant()*svd.matrixV().determinant() < 0.0)//contains reflection
 	{
 		Eigen::Vector3f S = Eigen::Vector3f::Ones(); S(2) = -1.0;
 		transformation.linear().noalias() = svd.matrixV()*S.asDiagonal()*svd.matrixU().transpose();
 	} else 
 	{
 		transformation.linear().noalias() = svd.matrixV()*svd.matrixU().transpose();//计算旋转矩阵
 	}
 	//transformation.translation().noalias() = Y_mean - transformation.linear()*X_mean;//计算平移向量
 	rotMat = transformation.linear() ;

 	srCloud.colwise() += X_mean;
 	tgCloud.colwise() += Y_mean;
 }

void DeformableRegistration::calculateTrajFrature(MatrixXX & featureMat)
{
 	Logger << "Begin Calculate trajectory featureMat.\n";
 
 	IndexType	num_sample = m_smpSet.size();
 	IndexType	num_vtx	= m_smpSet[0].num_vertices();// default trajectory number
 	int col_ele = (num_sample - 1) * 9;
	//int col_ele = (num_sample - 1) * 12;
	//int col_ele = (num_sample - 1) * 3;
 	featureMat.setZero(num_vtx,col_ele);
 
 	Matrix3X  local_srCloud;
 	Matrix3X  local_tgCloud;
 	MatrixXXi global_vtx_map,local_vtx_map;
	Matrix33 rotateMat;
	Matrix33 rotateMatBack;
	MatrixXXi frontVtxMap;
    MatrixXXi afterVtxMap;
	IndexType *srLocalNeig = new IndexType[m_neigNum];
	IndexType *tgLoaclNeig = new IndexType[m_neigNum];
	IndexType mp = 0;
	vector<IndexType> srIntersection;
	vector<IndexType> tgIntersection;
	IndexType interSize = 0;
	IndexType trajBegin = 0;
	IndexType trajEnd   = 0;
	SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	MatrixXXi local_cor;
	Matrix3X temp_localCoor;
	PointType vtxPos;
	MatrixXXi trajMat;
	trajMat.setZero(num_sample ,num_vtx);

	// record distorted and add trajectories number 9-19
	MatrixXX isDeformable;
	std::set<IndexType> corIndex;
	std::set<IndexType> distoredLargeSet;

 	initFrontVtxMap(frontVtxMap,num_vtx);//init the begin of trajectory
	trajMat.row(0) = frontVtxMap;
	//std::fstream op("E:\\DisIndexDistui.txt",std::ios::app);

  	for (IndexType frameIndex = 1; frameIndex < num_sample;frameIndex++)
  	{
  		// frameIndex - 1 and frameIndex do non-rigid registration
 		Sample& srGraph = m_smpSet[frameIndex - 1];
 		Sample& tgGraph = m_smpSet[frameIndex];
        non_rigidRegister(srGraph,tgGraph,global_vtx_map,isDeformable,distoredLargeSet);
		srGraph.update();

		IndexType corNum = recordCorIndex(global_vtx_map,corIndex);

		Logger<<"frame "<<frameIndex -1<<"Large dist Number = "<<distoredLargeSet.size()<<endl;
		Logger<<"frame id = "<<frameIndex<<endl;
		Logger<<"glb correspondences size = "<<corNum<<".\n";
		Logger<<" produce new trajectory number = "<<tgGraph.num_vertices() - corNum<<".\n";

// 		if (op.is_open())
// 		{   op<<" 0000000000000000000000000000000000000000000000first global cor.\n";
// 		    //op<<global_vtx_map.transpose()<<endl;
// 		    op<<distoredLargeSet.size()<<endl;
// 		    op.close();
// 		}
// 		else
// 		{
// 			cout<<"open file error！"<<endl;
// 			return ;
// 		}

		// iter trajectory otherwise vertexes
		for (IndexType traj_Index = 0; traj_Index < num_vtx; traj_Index ++)
		{
			trajBegin = frontVtxMap(0,traj_Index);// traj_index 's trajectory begin
			vtxPos = srGraph.vertices_matrix().col(trajBegin);
			srGraph.neighbours(trajBegin,m_neigNum,srLocalNeig);

			calculateTrajEndIndex(frontVtxMap,global_vtx_map,afterVtxMap);//

			trajEnd = afterVtxMap(0,traj_Index);
			tgGraph.neighbours(trajEnd,m_neigNum,tgLoaclNeig);


 			srIntersection.clear();
 			tgIntersection.clear();
 
  			intersectionCorrespondenceInverse(srLocalNeig,tgLoaclNeig,global_vtx_map,srIntersection,tgIntersection);
   			interSize = srIntersection.size();
  			rotateMat.setIdentity();
			rotateMatBack.setIdentity();
  			if ( interSize > 3)
  			{
   			   local_srCloud.setZero(3,interSize);
   			   local_tgCloud.setZero(3,interSize); 
   			  getNeigCoorByIndex(srGraph,srIntersection,local_srCloud);
   			  getNeigCoorByIndex(tgGraph,tgIntersection,local_tgCloud);
  
  // 			  local_srCloud.setZero(3,m_neigNum);
  // 			  local_tgCloud.setZero(3,m_neigNum);
  // 			  getConstantNeigCoorByIndex(srGraph,srLocalNeig,local_srCloud);
  // 			  getConstantNeigCoorByIndex(tgGraph,tgLoaclNeig,local_tgCloud);
  
  
  
  // 			  local_cor.setZero(1,interSize);
  // 			  temp_localCoor = local_srCloud;
  // 			  SICP::point_to_point(temp_localCoor,local_tgCloud,local_cor,pa);//9-12
  // 			  alignTargetCoor(local_tgCloud,local_cor);
  		      point2point(local_srCloud,local_tgCloud,rotateMat);
			  point2point(local_tgCloud,local_srCloud,rotateMatBack);
  			}

//   			featureMat.block(traj_Index,(frameIndex - 1) * 18,    1,3) = rotateMat.block(0,0,1,3);
//   			featureMat.block(traj_Index,(frameIndex - 1) * 18 + 3,1,3) = rotateMat.block(1,0,1,3);
//   			featureMat.block(traj_Index,(frameIndex - 1) * 18 + 6,1,3) = rotateMat.block(2,0,1,3);
// 
// 			featureMat.block(traj_Index,(frameIndex - 1) * 18 + 9, 1,3) = rotateMatBack.block(0,0,1,3);
// 			featureMat.block(traj_Index,(frameIndex - 1) * 18 + 12,1,3) = rotateMatBack.block(1,0,1,3);
// 			featureMat.block(traj_Index,(frameIndex - 1) * 18 + 15,1,3) = rotateMatBack.block(2,0,1,3);

 			featureMat.block(traj_Index,(frameIndex - 1) * 9,    1,3) = rotateMat.block(0,0,1,3);
 			featureMat.block(traj_Index,(frameIndex - 1) * 9 + 3,1,3) = rotateMat.block(1,0,1,3);
 			featureMat.block(traj_Index,(frameIndex - 1) * 9 + 6,1,3) = rotateMat.block(2,0,1,3);

//  			featureMat.block(traj_Index,(frameIndex - 1) * 12,    1,3) = rotateMat.block(0,0,1,3);
//  			featureMat.block(traj_Index,(frameIndex - 1) * 12 + 3,1,3) = rotateMat.block(1,0,1,3);
//  			featureMat.block(traj_Index,(frameIndex - 1) * 12 + 6,1,3) = rotateMat.block(2,0,1,3);
//  			featureMat.block(traj_Index,(frameIndex - 1) * 12 + 9,1,3) = 0.0001 * vtxPos.transpose().block(0,0,1,3);


		}
		frontVtxMap = afterVtxMap;// end point become  begin 
		trajMat.row(frameIndex) = frontVtxMap;
  	}

     Tracer& cor = Tracer::get_instance();
  	MatrixXXi start;
 	MatrixXXi end;
//   	for (IndexType frameIndex = 1; frameIndex < num_sample;frameIndex ++)
//   	{
//           //cor.add_percentCor(frameIndex - 1,frameIndex,global_vtx_map,40,num_vtx);
//   		start = trajMat.row(frameIndex - 1);
//  		end   = trajMat.row(frameIndex);
//   		cor.add_percentCor_SE(frameIndex - 1,frameIndex,start,end,10,num_vtx);
//   	}

	cor.add_percentCor(8,9,global_vtx_map,4,m_smpSet[8].num_vertices());


// 	std::fstream op("E:\\trajactoryMatDraw.txt",std::ios::app);
// 	if (op.is_open())
// 	{
// 		op<<trajMat.transpose()<<endl;
// 		op.close();
// 	}
// 	else
// 	{
// 		cout<<"open file error！"<<endl;
// 		return ;
// 	}

	Logger << "End Calculate trajectory featureMat.\n";
}

//traj have the same size
void DeformableRegistration::calculateTrajFeature(vector<PCloudTraj>& oriTraj,IndexType trajLen,MatrixXX& featureMat)
{
	IndexType trajSize = oriTraj.size();
	assert(trajSize > 0);
	//featureMat.setZero(trajSize,21 * trajLen);//add coordinates
	featureMat.setZero(trajSize,18 * trajLen);

	
	PCloudModel resModel;
	PCloudTraj sampleTraj;
	Matrix33 rotMat;
	MatrixXX rowVector;
	PointType center;

	rowVector.setZero(1,21 * trajLen);
	for (IndexType trajIdx = 0; trajIdx < trajSize; trajIdx ++)
	{
		sampleTraj = oriTraj[trajIdx];
		resModel = constructRigidMotionByNeig(sampleTraj);
		for(IndexType frameIdx = 0; frameIdx < trajLen; frameIdx ++)
		{
			rotMat = resModel.fNode[frameIdx];//front rotmate
			rowVector.block(0,18/*21*/ * frameIdx    ,1,3) = rotMat.block(0,0,1,3);
			rowVector.block(0,18/*21*/ * frameIdx + 3,1,3) = rotMat.block(1,0,1,3);
			rowVector.block(0,18/*21*/ * frameIdx + 6,1,3) = rotMat.block(2,0,1,3);

			rotMat = resModel.bNode[frameIdx];//back rotmate
			rowVector.block(0,18/*21*/ * frameIdx + 9,1,3) =  rotMat.block(0,0,1,3);
			rowVector.block(0,18/*21*/ * frameIdx + 12,1,3) = rotMat.block(1,0,1,3);
			rowVector.block(0,18/*21*/ * frameIdx + 15,1,3) = rotMat.block(2,0,1,3);

			//center = resModel.Centers.col(frameIdx);//centers
			//rowVector.block(0,21 * frameIdx + 18,1,3) = center.transpose().block(0,0,1,3);
		}
		featureMat.row(trajIdx) = rowVector;
	}
}
void DeformableRegistration::calculateLifeSpansTraj(vector<PCloudTraj> & totalTraj,IndexType LifeMin)
{
	Logger << "Begin Calculate  Life - spans trajectory .\n";

	IndexType	num_sample = m_smpSet.size();
	IndexType	num_vtx	= 0;//m_smpSet[0].num_vertices();// default trajectory number
	MatrixXXi   global_vtx_map;
	MatrixXX    isDeformable;
	std::set<IndexType> distoredLargeSet;
	ScalarType defValue = 0.0;
	ScalarType thresholdDef = 0.2;

	//
	set<IndexType> validEnd;//valid end index
	set<IndexType> nextValidEnd;//product new valid end index 
	vector<PCloudTraj> willGoOnTraj;
	vector<PCloudTraj> nextwillGoOnTraj;
	set<IndexType>::iterator isInValidIter;
	//9-27
	std::set<IndexType> corIndex;
	IndexType proFaildTraj = 0;

	//
	IndexType vtx_cor = 0;
	IndexType willGoSetSize = 0;
	PCloudTraj tempTraj;
	validEnd.clear();
	Matrix3X srVtxMatrix;
	vector<bool> onceLabel;

	//std::fstream op("E:\\1013Deformable.txt",std::ios::app);
	//std::fstream optCor("E:\\correspondence.txt",std::ios::app);

	for (IndexType frameIndex = 1; frameIndex < num_sample;frameIndex++)
	{
		// frameIndex - 1 and frameIndex do non-rigid registration  //test  multimap  9-28
		Sample& srGraph = m_smpSet[frameIndex - 1];
		Sample& tgGraph = m_smpSet[frameIndex];
		srVtxMatrix = srGraph.vertices_matrix();

		non_rigidRegister(srGraph,tgGraph,global_vtx_map,isDeformable,distoredLargeSet);

		srGraph.vertices_matrix() = srVtxMatrix; 
		srGraph.update();//back to original position after registeration

		num_vtx = m_smpSet[frameIndex - 1].num_vertices();
		onceLabel.resize(num_vtx,false);// add a label  to determine  using once on  a veterx-line 905 and 911 

		for (IndexType vtx_index = 0; vtx_index < num_vtx; vtx_index ++)
		{
			vtx_cor = global_vtx_map(0,vtx_index);
			isInValidIter = validEnd.find(vtx_index);
			defValue = isDeformable(0,vtx_index);
			if (isInValidIter != validEnd.end()) // Go on --must search a traj--在有效集合里面//validEnd
			{
               vector<PCloudTraj>::iterator v_iter =willGoOnTraj.begin();
				for (; v_iter != willGoOnTraj.end();)//需要用查找-修改-删除-插入操作
				{
					IndexType trajLife = (*v_iter).trajNode.size();					
					IndexType trajEndIndex = (*v_iter).trajNode[trajLife - 1];
					if (vtx_index == trajEndIndex)
					{
						if (defValue < thresholdDef  /*&& onceLabel[vtx_index] == false*/ )//可以继续延伸
						{							               
							(*v_iter).trajLifeSpan.end += 1;//Ti.end = Ti.end + 1;
							(*v_iter).trajNode.push_back(vtx_cor);//轨迹的最终顶点号码追加为对应号码
							nextwillGoOnTraj.push_back(*v_iter);// record traj which cross frameIndex 
							nextValidEnd.insert(vtx_cor);
							//onceLabel[vtx_index] = true;
							v_iter = willGoOnTraj.erase(v_iter); //discard
						} 
						else
						{
							if (((*v_iter).trajLifeSpan.end - (*v_iter).trajLifeSpan.start) > LifeMin)//加入轨迹长度判断
							{
								totalTraj.push_back(*v_iter);        
							}
							v_iter = willGoOnTraj.erase(v_iter); //discard
						}
					}else
					{
						v_iter ++;
					}
				}//end for willgoset

			}else //create a new trajectory
			{
				 if (defValue < thresholdDef)
				 {
					   tempTraj.trajLifeSpan.start = frameIndex - 1;
					   tempTraj.trajLifeSpan.end   = frameIndex;
					   tempTraj.trajNode.clear();
					   tempTraj.trajNode.push_back(vtx_index);
					   tempTraj.trajNode.push_back(vtx_cor);
				   
					   nextwillGoOnTraj.push_back(tempTraj);//willGoOnTraj.push_back(tempTraj);//插入到一颗r-tree中，需要查找--插入--删除;[4,8] [5,12]
					   nextValidEnd.insert(vtx_cor);    // next frame vtx index,标记为可能可以继续伸展的轨迹
				 }else //可以优化-省去下面三行代码
				 {
					 proFaildTraj ++; //discard
				 }             
			}//end if is in valid set
		}//end for vtx -- ortherwise each traj 

		Logger<<frameIndex -1 <<" frame begin is end, never stay this world  = " <<proFaildTraj<<endl;
		validEnd.clear();
		validEnd = nextValidEnd;

		//Logger<<"before clear traj size = "<<willGoOnTraj.size()<<"should size  = 0.\n";
		willGoOnTraj.clear();
		willGoOnTraj  = nextwillGoOnTraj;

		if ((num_sample - 1) == frameIndex)
		{
			//vector<PCloudTraj>::iterator totEnd = totalTraj.end();//也要加入长度的判断			
			vector<PCloudTraj>::iterator willStart = willGoOnTraj.begin();
			vector<PCloudTraj>::iterator willEnd = willGoOnTraj.end();
			for (; willStart != willEnd; willStart ++)
			{
				if (((*willStart).trajLifeSpan.end - (*willStart).trajLifeSpan.start) > LifeMin)
				{
					totalTraj.push_back(*willStart);
				}
			}           
          //totalTraj.insert(totEnd,willStart,willEnd); //全部轨迹添加到后面
		}

		proFaildTraj = 0;
		nextValidEnd.clear();
		nextwillGoOnTraj.clear();

	}//end for(frame)
	
	//draw life-spans  trajectories
	drawLifespansTraj(totalTraj);

	Logger<<"End calculate traj-spans.\n";
}

//计算采样后的不等长轨迹
void DeformableRegistration::calculateDownSmpLifeSpanTraj(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap,
														  IndexType length,IndexType octreeRes ,ScalarType dfThreshold, IndexType LifeMin)
{
	Logger<<"start Calculate  down sample life-span Traj.\n";

	SampleSet& smp_set = SampleSet::get_instance();
	IndexType start_idx = smp_idx - length/2 ;
	IndexType end_idx = smp_idx + length/2;

	if( length%2!=0 )
		++end_idx;
	if( start_idx<0 )
	{
		start_idx = 0;
		end_idx = length;
	}
	if( end_idx > smp_set.size() - 1  )
	{
		end_idx = smp_set.size() - 1;
		start_idx = end_idx - length;
	}

	vector<Sample*> p_subsmp_set( end_idx - start_idx + 1);
	vector< vector<IndexType> > vtx_map(end_idx - start_idx + 1);

	for (int i = 0; i < end_idx - start_idx+1; i++)
	{
		p_subsmp_set[i] = new Sample;
		SampleManipulation::SubSampleSolution solv( smp_set[start_idx+i], *p_subsmp_set[i],octreeRes);
		vtx_map[i] = solv.compute();
	}

	centerVtxMap = vtx_map[smp_idx - start_idx];

	set<IndexType> validEnd;//valid end index
	set<IndexType> nextValidEnd;//product new valid end index 
	vector<PCloudTraj> willGoOnTraj;
	vector<PCloudTraj> nextwillGoOnTraj;
	set<IndexType>::iterator isInValidIter;

	IndexType vtx_cor = 0;
	IndexType willGoSetSize = 0;
	PCloudTraj tempTraj;
	validEnd.clear();
	Matrix3X srVtxMatrix;
	vector<bool> onceLabel;

	IndexType invalidTraj = 0;

	for ( int i = 0; i < end_idx - start_idx; i++ )
	{
		Sample* srGraph = p_subsmp_set[i]; //对应原始的 ( start_idx + i) 帧
		Sample* tgGraph = p_subsmp_set[i + 1];//对应原始的 ( start_idx + i + 1) 帧
		Matrix3X srVtxMatrix = srGraph->vertices_matrix();
		MatrixXXi   global_vtx_map;
		MatrixXX    isDeformable;
		std::set<IndexType> distoredLargeSet;

		non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);

		srGraph->vertices_matrix() = srVtxMatrix; 
		srGraph->update();//back to original position after registeration

		IndexType srGraphVtxN = srGraph->num_vertices();

		for ( int j = 0; j < srGraphVtxN; j++)
		{
			//j号点为i帧中的实际顶点号码为: vtx_map[i][j]
			//j 点在下一帧的对应点为 vtx_map[i + 1][global_vtx_map(0,j)]
			vtx_cor = vtx_map[i + 1][global_vtx_map(0,j)];
			isInValidIter = validEnd.find(vtx_map[i][j]);

			ScalarType defValue = isDeformable(0,j);


			if (isInValidIter != validEnd.end())
			{
				vector<PCloudTraj>::iterator v_iter =willGoOnTraj.begin();
				for (; v_iter != willGoOnTraj.end();)
				{
					IndexType trajLife = (*v_iter).trajNode.size();	
					IndexType trajEndIndex = (*v_iter).trajNode[trajLife - 1];

					if (trajEndIndex == vtx_map[i][j])
					{
						if (defValue <= dfThreshold)
						{
							(*v_iter).trajLifeSpan.end += 1;//Ti.end = Ti.end + 1;
							(*v_iter).trajNode.push_back(vtx_cor);//轨迹的最终顶点号码追加为对应号码
							nextwillGoOnTraj.push_back(*v_iter);// 没有断,继续走下去
							nextValidEnd.insert(vtx_cor);
							v_iter = willGoOnTraj.erase(v_iter); //discard

						}else
						{
							if (((*v_iter).trajLifeSpan.end - (*v_iter).trajLifeSpan.start) >= LifeMin)//加入轨迹长度判断
							{
								trajs.push_back(*v_iter);//保存轨迹        
							}
							v_iter = willGoOnTraj.erase(v_iter); //discard

						}

					}else
					{
						v_iter ++;
					}
				}

			}else
			{
				if (defValue <= dfThreshold)
				{
					tempTraj.trajLifeSpan.start = (start_idx + i);
					tempTraj.trajLifeSpan.end = (start_idx + i + 1);
					tempTraj.trajNode.clear();

					tempTraj.trajNode.push_back(vtx_map[i][j]);

					tempTraj.trajNode.push_back(vtx_cor);

					nextwillGoOnTraj.push_back(tempTraj);

					nextValidEnd.insert(vtx_cor);

				}else
				{
					invalidTraj++;
				}
			}
		}//end for very vtx in current frame

		Logger<<i <<" frame begin is end, never stay this world  = " <<invalidTraj<<endl;
		validEnd.clear();
		validEnd = nextValidEnd;
		nextValidEnd.clear();

		willGoOnTraj.clear();
		willGoOnTraj  = nextwillGoOnTraj;
		nextwillGoOnTraj.clear();

		invalidTraj = 0;

		if ((end_idx - 1) == (i + start_idx) )
		{		
			vector<PCloudTraj>::iterator willStart = willGoOnTraj.begin();
			vector<PCloudTraj>::iterator willEnd = willGoOnTraj.end();
			for (; willStart != willEnd; willStart ++)
			{
				if (((*willStart).trajLifeSpan.end - (*willStart).trajLifeSpan.start) >= LifeMin)
				{
					trajs.push_back(*willStart);
				}
			}           
		}

	}//end for sequences

	//draw life-spans  trajectories
	drawLifespansTraj(trajs);

	Logger<<"End Calculate  down sample life-span Traj.\n";
}

//0728
//------------------------------------------从中间开始,向前后帧传播, 生成不等长轨迹
void DeformableRegistration::calculateDownSmpLifeSpanTrajCenter(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap, IndexType length,IndexType octreeRes ,
																ScalarType dfThreshold, IndexType LifeMin)
{
	Logger<<"start Calculate  down sample life-span Traj from  Center!.\n";

	SampleSet& smp_set = SampleSet::get_instance();
	IndexType start_idx = smp_idx - length/2 ;
	IndexType end_idx = smp_idx + length/2;

	if( length%2!=0 )
		++end_idx;
	if( start_idx<0 )
	{
		start_idx = 0;
		end_idx = length;
	}
	if( end_idx > smp_set.size() - 1  )
	{
		end_idx = smp_set.size() - 1;
		start_idx = end_idx - length;
	}

	vector<Sample*> p_subsmp_set( end_idx - start_idx + 1);
	vector< vector<IndexType> > vtx_map(end_idx - start_idx + 1);

	for (int i = 0; i < end_idx - start_idx+1; i++)
	{
		p_subsmp_set[i] = new Sample;
		SampleManipulation::SubSampleSolution solv( smp_set[start_idx+i], *p_subsmp_set[i],octreeRes);
		vtx_map[i] = solv.compute();
	}

	centerVtxMap = vtx_map[smp_idx - start_idx];

	set<IndexType> validEnd;//valid end index
	set<IndexType> nextValidEnd;//product new valid end index 
	vector<PCloudTraj> willGoOnTraj;
	vector<PCloudTraj> nextwillGoOnTraj;
	set<IndexType>::iterator isInValidIter;

	IndexType vtx_cor = 0;
	IndexType willGoSetSize = 0;
	PCloudTraj tempTraj;
	validEnd.clear();
	Matrix3X srVtxMatrix;
	vector<bool> onceLabel;

	IndexType invalidTraj = 0;


	// forward trace! life spans trajectories  ------>>

	for ( int i = (smp_idx - start_idx); i < (end_idx - start_idx); i++ )
	{

		Sample* srGraph = p_subsmp_set[i]; //对应原始的 ( start_idx + i) 帧

		Sample* tgGraph = p_subsmp_set[i + 1];//对应原始的 ( start_idx + i + 1) 帧

		Matrix3X srVtxMatrix = srGraph->vertices_matrix();

		MatrixXXi   global_vtx_map;

		MatrixXX    isDeformable;

		std::set<IndexType> distoredLargeSet;


		non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);


		srGraph->vertices_matrix() = srVtxMatrix; 

		srGraph->update();//back to original position after registeration

	    IndexType srGraphVtxN = srGraph->num_vertices();

		for ( int j = 0; j < srGraphVtxN; j++)
		{
			//j号点为i帧中的实际顶点号码为: vtx_map[i][j]
			//j 点在下一帧的对应点为 vtx_map[i + 1][global_vtx_map(0,j)]
			vtx_cor = vtx_map[i + 1][global_vtx_map(0,j)];
			isInValidIter = validEnd.find(vtx_map[i][j]);

			ScalarType defValue = isDeformable(0,j);

			if (isInValidIter != validEnd.end())
			{

				vector<PCloudTraj>::iterator v_iter =willGoOnTraj.begin();
				for (; v_iter != willGoOnTraj.end();)
				{
					IndexType trajLife = (*v_iter).trajNode.size();	
					IndexType trajEndIndex = (*v_iter).trajNode[trajLife - 1];
					IndexType endF = (*v_iter).trajLifeSpan.end;

					if (endF == (start_idx + i) && trajEndIndex == vtx_map[i][j])
					{

						if (defValue <= dfThreshold)
						{

							(*v_iter).trajLifeSpan.end += 1;//Ti.end = Ti.end + 1;
							(*v_iter).trajNode.push_back(vtx_cor);//轨迹的最终顶点号码追加为对应号码
							nextwillGoOnTraj.push_back(*v_iter);// 没有断,继续走下去
							nextValidEnd.insert(vtx_cor);
							v_iter = willGoOnTraj.erase(v_iter); //discard

						}else
						{
							if ( (*v_iter).trajLifeSpan.start == smp_idx )//若起点是中心帧,则保留,为后向传播做准备
							{

								v_iter ++;

							}else
							{

								if (((*v_iter).trajLifeSpan.end - (*v_iter).trajLifeSpan.start) >= LifeMin)//加入轨迹长度判断
								{
									trajs.push_back(*v_iter);//保存轨迹        
								}

								v_iter = willGoOnTraj.erase(v_iter); //前向不能延续,直接抛弃该轨迹!

							}

						}

					}else
					{

						v_iter ++;

					}
				}

			}else
			{
				if (defValue <= dfThreshold)
				{
					tempTraj.trajLifeSpan.start = (start_idx + i);
					tempTraj.trajLifeSpan.end = (start_idx + i + 1);
					tempTraj.trajNode.clear();

					tempTraj.trajNode.push_back(vtx_map[i][j]);

					tempTraj.trajNode.push_back(vtx_cor);

					nextwillGoOnTraj.push_back(tempTraj);

					nextValidEnd.insert(vtx_cor);

				}else
				{
					invalidTraj++;
				}
			}
		}//end for very vtx in current frame

		Logger<<i <<"Forward! frame begin is end, never stay this world num size = " <<invalidTraj<<endl;
		validEnd.clear();
		validEnd = nextValidEnd;
		nextValidEnd.clear();

		willGoOnTraj.clear();
		willGoOnTraj  = nextwillGoOnTraj;
		nextwillGoOnTraj.clear();

		invalidTraj = 0;
// save willGOTraj for backward trace!
// 		if ((end_idx - 1) == (i + start_idx) )
// 		{		
// 			vector<PCloudTraj>::iterator willStart = willGoOnTraj.begin();
// 			vector<PCloudTraj>::iterator willEnd = willGoOnTraj.end();
// 			for (; willStart != willEnd; willStart ++)
// 			{
// 				if (((*willStart).trajLifeSpan.end - (*willStart).trajLifeSpan.start) >= LifeMin)
// 				{
// 					trajs.push_back(*willStart);
// 				}
// 			}           
// 		}

	}


	// Backward !!!!!!!      <<-----------------------------------

	validEnd.clear();

	vector<PCloudTraj>::iterator vw_iter =willGoOnTraj.begin();

	for (; vw_iter != willGoOnTraj.end(); vw_iter ++) //保存仍然需要延续轨迹起始的顶点索引,使得在遍历中心帧时需不要重新生成新的轨迹
	{
		if ((*vw_iter).trajLifeSpan.start == smp_idx)
		{
			validEnd.insert((*vw_iter).trajNode[0]);
		}
	}

	for ( int i = (smp_idx - start_idx); i > 0; i --)
	{
		Sample* srGraph = p_subsmp_set[i]; //对应原始的 ( start_idx + i) 帧

		Sample* tgGraph = p_subsmp_set[i - 1];//对应原始的 ( start_idx + i + 1) 帧

		Matrix3X srVtxMatrix = srGraph->vertices_matrix();

		MatrixXXi   global_vtx_map;

		MatrixXX    isDeformable;

		std::set<IndexType> distoredLargeSet;


		non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);


		srGraph->vertices_matrix() = srVtxMatrix; 

		srGraph->update();//back to original position after registeration

		IndexType srGraphVtxN = srGraph->num_vertices();

		for ( int j = 0; j < srGraphVtxN; j++)
		{
			//j号点为i帧中的实际顶点号码为: vtx_map[i][j]
			//j 点在下一帧的对应点为 vtx_map[i + 1][global_vtx_map(0,j)]
			vtx_cor = vtx_map[i - 1][global_vtx_map(0,j)];
			isInValidIter = validEnd.find(vtx_map[i][j]);

			ScalarType defValue = isDeformable(0,j);


			if (isInValidIter != validEnd.end())
			{
				vector<PCloudTraj>::iterator v_iter =willGoOnTraj.begin();
				for (; v_iter != willGoOnTraj.end();)
				{
// 					IndexType trajLife = (*v_iter).trajNode.size();	
// 					IndexType trajEndIndex = (*v_iter).trajNode[trajLife - 1];

					IndexType trajStartIndex = (*v_iter).trajNode[0]; //获得轨迹的起点顶点索引
					IndexType strF = (*v_iter).trajLifeSpan.start;

					if ( strF == (start_idx + i)  && trajStartIndex == vtx_map[i][j])
					{

						if (defValue <= dfThreshold)
						{

							(*v_iter).trajLifeSpan.start -= 1;//向前推进一帧

							vector<IndexType>::iterator itB = (*v_iter).trajNode.begin();

							(*v_iter).trajNode.insert(itB,vtx_cor);//在起始位置插入对应点索引

							nextwillGoOnTraj.push_back(*v_iter);// 没有断,继续走下去

							nextValidEnd.insert(vtx_cor);

							v_iter = willGoOnTraj.erase(v_iter); //discard

						}else
						{

							if (((*v_iter).trajLifeSpan.end - (*v_iter).trajLifeSpan.start) >= LifeMin)//加入轨迹长度判断
							{
								trajs.push_back(*v_iter);//保存轨迹        
							}

							v_iter = willGoOnTraj.erase(v_iter); //前向不能延续,直接抛弃该轨迹!
						}

					}else //该轨迹不以该点为结束且不以(start+ i)为起始帧,"那么它永远不会续约下去,进行判断,要么放入最终的轨迹中,要么抛弃." 上面已经抛弃了一次!
					{
						v_iter ++;
					}

				}// end for

			}else //生成一条新的轨迹
			{
				if (defValue <= dfThreshold)
				{
					tempTraj.trajLifeSpan.start = (start_idx + i - 1);
					tempTraj.trajLifeSpan.end = (start_idx + i);
					tempTraj.trajNode.clear();

					tempTraj.trajNode.push_back(vtx_cor);

					tempTraj.trajNode.push_back(vtx_map[i][j]);

					nextwillGoOnTraj.push_back(tempTraj);

					nextValidEnd.insert(vtx_cor);

				}else
				{
					invalidTraj++;
				}
			}
		}//end for very vtx in current frame

		Logger<<i <<"Backward! frame begin is end, never stay this world num size = " <<invalidTraj<<endl;
		validEnd.clear();
		validEnd = nextValidEnd;
		nextValidEnd.clear();

		willGoOnTraj.clear();
		willGoOnTraj  = nextwillGoOnTraj;
		nextwillGoOnTraj.clear();


		if ( 1 == i ) //???
		{		
			vector<PCloudTraj>::iterator willStart = willGoOnTraj.begin();
			vector<PCloudTraj>::iterator willEnd = willGoOnTraj.end();
			for (; willStart != willEnd; willStart ++)
			{
				if (((*willStart).trajLifeSpan.end - (*willStart).trajLifeSpan.start) >= LifeMin)
				{
					trajs.push_back(*willStart);
				}
			}           
		}

		invalidTraj = 0;

	}

	//draw life-spans  trajectories
	drawLifespansTraj(trajs);

	Logger<<"End Calculate  down sample life-span Traj.\n";

}
//------------------------------------------
void DeformableRegistration::alignTargetCoor(Matrix3X & tgCloud,MatrixXXi & vertexMap)
{
	MatrixXX temp = tgCloud;
	int verN = temp.cols();
	for (int i = 0; i < verN; i++)
	{
		tgCloud.col(i) = temp.col(vertexMap(0,i));
	}
}
//
void DeformableRegistration::non_rigidRegister(Sample& srGraph,Sample& tgGraph,MatrixXXi & vtxMap,
											   MatrixXX &isDeformable,std::set<IndexType> & distLrgIndex)
{
	IndexType num_vtx = srGraph.num_vertices();
	Matrix3X resPosition;
	isDeformable.setZero(1,num_vtx);
	resPosition.setZero(3,num_vtx);
	vtxMap.setZero(1,num_vtx);

	//do  global ICP
	SICP::Parameters pa(false,2.0,10,1.2,1e5,100,100,1,1e-5); //release
	//SICP::Parameters pa(false,2.0,10,1.2,1e5,20,20,1,1e-5);     //debug  
	SICP::point_to_point(srGraph.vertices_matrix(),tgGraph.vertices_matrix(),vtxMap,pa);
	srGraph.update();//为了查找点的邻域，需要更新

	//local ICP and mix coordinate with rest  and correspondence position
 	ScalarType rigidRate = 0.95;
 	IndexType iterMax = 1;
 
 	while (iterMax-- > 0)
 	{
 		//localICP(srGraph,tgGraph,vtxMap,resPosition,isDeformable);
		localICPDisto(srGraph,tgGraph,vtxMap,resPosition,isDeformable);//用对应点的距离比值计算扭曲量
 		optimizeCoor(srGraph,tgGraph,vtxMap);
 		combinationCoor(rigidRate,srGraph,tgGraph,resPosition,vtxMap,isDeformable,distLrgIndex);
 	}
}
void DeformableRegistration::intersectionCorrespondence(IndexType * srLoaclNeig,IndexType* tgLoaclNeig,MatrixXXi & vtx_map,
														vector<IndexType> & srInterRes,vector<IndexType>& tgInterRes)
{
	std::set<IndexType> tgNeigSet(tgLoaclNeig,tgLoaclNeig + m_neigNum);
	std::set<IndexType>::iterator tgLoaclNeigIter;
	IndexType cor = 0;

	for (IndexType vtxIndex = 0; vtxIndex < m_neigNum; vtxIndex++)
	{
		cor = vtx_map(0,srLoaclNeig[vtxIndex]);
		tgLoaclNeigIter = tgNeigSet.find(cor);//find intersection elements in target neighbour
		if (tgLoaclNeigIter != tgNeigSet.end())
		{
			srInterRes.push_back(srLoaclNeig[vtxIndex]);
			tgInterRes.push_back(cor);
		}
	}

	

	if (srInterRes.size() < 3)
	{
		Logger<<"intersection is  small.\n";
		//return ;
	}

}
void DeformableRegistration::intersectionCorrespondenceInverse(IndexType * srLoaclNeig,IndexType * tgLoaclNeig,MatrixXXi & vtx_map,
															   vector<IndexType> & srInterRes,vector<IndexType> & tgInterRes)
{
	std::set<IndexType> srCorrespondenceSet;
	std::set<IndexType>::iterator srCorrespondenceSetIter;
	IndexType cor = 0;

	//construct set
	for (IndexType v_it = 0; v_it < m_neigNum; v_it++)
	{
		cor = vtx_map(0,srLoaclNeig[v_it]);
		srCorrespondenceSet.insert(cor);
	}

	for (IndexType vtxIndex = 0; vtxIndex < m_neigNum; vtxIndex++)
	{
		cor = tgLoaclNeig[vtxIndex];
		srCorrespondenceSetIter = srCorrespondenceSet.find(cor);//find intersection elements in target neighbour
		if (srCorrespondenceSetIter != srCorrespondenceSet.end())
		{
			srInterRes.push_back(srLoaclNeig[vtxIndex]);
			tgInterRes.push_back(cor);
			//tgInterRes.push_back(vtx_map(0,srLoaclNeig[vtxIndex]));
		}
	}

	if (srInterRes.size() < 3)
	{
		Logger<<"intersection is  small.\n";
		//return ;
	}
}
void DeformableRegistration::calculateNeigDis(Matrix3X & neigCoor,VecX& resDis)
{
	IndexType vtx_num = neigCoor.cols();
	PointType ori = neigCoor.col(0);
	PointType diff;
	diff.setZero();
	for (IndexType vtx_iter = 1; vtx_iter < vtx_num;vtx_iter ++)
	{
		diff = neigCoor.col(vtx_iter) - ori;
		resDis(vtx_iter,0) = diff.norm();
	}
}
 ScalarType DeformableRegistration::deformableValue(VecX& srNeigDis,VecX & resNeigDis)
 {
 	ScalarType totle = 0.0;
 	ScalarType molecule = 0.0;
 	ScalarType denominator = 1.0;
 	IndexType neigNum = resNeigDis.rows();
 	for (IndexType v_iter  = 1;v_iter < neigNum; v_iter++)
 	{
 		denominator = srNeigDis(v_iter,0);
 		molecule = abs(resNeigDis(v_iter,0) - denominator);
 		assert(denominator > 1e-7);
 		totle += molecule/denominator;
 	}
 	return totle/(neigNum - 1);
 }
void DeformableRegistration::calculateTrajEndIndex(MatrixXXi & TrajBeginIndex,MatrixXXi& globalCor2Frame,MatrixXXi& trajEndIndex)
{
	IndexType trajNum = TrajBeginIndex.cols();
	trajEndIndex.setZero(1,trajNum);
	IndexType trajBegin = 0;
	for (IndexType index = 0; index < trajNum;index ++)
	{
		trajBegin = TrajBeginIndex(0,index);
		trajEndIndex(0,index) = globalCor2Frame(0,trajBegin);
	}
}
void DeformableRegistration::initFrontVtxMap(MatrixXXi& frontVtxMap,IndexType trajNum)
{
	frontVtxMap.setZero(1,trajNum);
	for (IndexType v_idx = 0; v_idx < trajNum; v_idx++)
	{
		frontVtxMap(0,v_idx) = v_idx;
	}
}
void DeformableRegistration::visNeigVertex(Sample& oriCloud,VecX& vtxIndex)
{

}
void DeformableRegistration::updateLocalCoordinate(Sample& oriCloud,Matrix3X& vtxCoor,VecX& vtxIndex)
{
	IndexType vtxNum = vtxCoor.cols();
	for (IndexType v_it = 0; v_it < vtxNum; v_it++)
	{
		oriCloud.vertices_matrix().col(vtxIndex(v_it,0)) = vtxCoor.col(v_it);
	}
	oriCloud.update();
}
IndexType DeformableRegistration::recordCorIndex(MatrixXXi & glb_cor,std::set<IndexType> recordCorSet)
{
	IndexType glbSize = glb_cor.cols();
	for (IndexType v_tx = 0; v_tx < glbSize; v_tx ++ )
	{
		recordCorSet.insert(glb_cor(0,v_tx));
	}
	return recordCorSet.size();
}
void DeformableRegistration::drawLifespansTraj(vector<PCloudTraj> & lifeSpans)
{
	Tracer& cor = Tracer::get_instance();
	vector<PCloudTraj>::iterator traj_iter = lifeSpans.begin();
	IndexType traj_start = 0;
	IndexType traj_end = 0;

// 	for (; traj_iter != lifeSpans.end(); traj_iter += 10)
// 	{
// 		traj_start = (*traj_iter).start;
// 		traj_end   = (*traj_iter).end;
// 		IndexType vecIter = 0;
// 		for (IndexType trajNodeIndex = traj_start; trajNodeIndex < traj_end; trajNodeIndex++,vecIter ++)
// 		{
//            cor.add_record(trajNodeIndex,(*traj_iter).trajNode[vecIter],
// 			              trajNodeIndex + 1,(*traj_iter).trajNode[vecIter + 1]);
// 		}
// 		
// 	}

	// using vector size
	IndexType totsize =lifeSpans.size();
	PCloudTraj temp;
	//IndexType trajNum = 0;
	for (IndexType trajIndex = 0; trajIndex < totsize; trajIndex += 30)
	{
		temp = lifeSpans[trajIndex];
		traj_start = temp.trajLifeSpan.start;
		traj_end   = temp.trajLifeSpan.end;
		IndexType vecIter = 0;
		//if ((traj_end - traj_start) < 7 )
		//{
			for (IndexType trajNodeIndex = traj_start; trajNodeIndex < traj_end; trajNodeIndex++,vecIter ++)
			{
				cor.add_record(trajNodeIndex,temp.trajNode[vecIter],
					trajNodeIndex + 1,temp.trajNode[vecIter + 1]);
			}
			//trajNum++;
		//}

	}
	Logger<<"this length number are =  "<<totsize<<endl;
}
void DeformableRegistration::makeCorrespondenceOne2One(MatrixXXi& globalMap,MatrixXX& deformableValue,IndexType tgVtxNum)
{
	struct distId
	{
		ScalarType disto;
		IndexType  vtxId;
	};
	multimap<IndexType,distId> CorpDis;
 	typedef  multimap<IndexType,distId>::const_iterator CIT;
 	typedef pair<CIT,CIT> Range;
	distId inputData;

	IndexType vtx_size = globalMap.cols();
 	for (IndexType vtxIter = 0; vtxIter < vtx_size; vtxIter ++)
 	{
		inputData.disto = deformableValue(0,vtxIter);
		inputData.vtxId = vtxIter;
		CorpDis.insert(make_pair(globalMap(0,vtxIter),inputData));
 	}

 	for (IndexType tgVtxIter = 0; tgVtxIter < tgVtxNum; tgVtxIter ++)
 	{
 		Range range = CorpDis.equal_range(tgVtxIter);//找出了对应点都是tgVtxIter的点
		if(1/*range.second - range.first < 2*/)// <=1
		{

		}else
		{
 			for (CIT iter = range.first; iter != range.second; iter ++)
 			{
 				//Logger<<"value = "<<iter->second<<endl;//continue 9-29 ---must do it
 			}
		}

 
 	}
	//sort-

	//IndexType vtx_size = globalMap.cols();
	//struct CorDis 
	//{
	//	IndexType   cor;
	//	ScalarType  disValue;
	//};
	//vector<list<CorDis > *> tgPlist;
	//tgPlist.resize(tgVtxNum);
	//IndexType vtx_cor = 0;
	//CorDis sTemp;
	//list<CorDis>::iterator lBegin; 
	//for (IndexType vtxIter = 0; vtxIter < vtx_size; vtxIter ++)
	//{
	//	list<CorDis> * lTemp = new list<CorDis>;
	//	vtx_cor = globalMap(0,vtxIter);
	//	sTemp.cor = vtx_cor;
	//	sTemp.disValue = deformableValue(0,vtxIter);

	//	if (true)
	//	{
	//		lBegin = lTemp->begin();
	//	}
	//	lTemp->insert(lBegin,sTemp);
	//	tgPlist[vtx_cor] = lTemp;
	//}

// 	multimap<IndexType,ScalarType>::iterator keyStart;
// 	multimap<IndexType,ScalarType>::iterator keyEnd;
// 	keyStart = CorpDis.lower_bound(3);
// 	keyEnd   = CorpDis.upper_bound(3);
// 
// 	multimap<IndexType,ScalarType>::iterator iterIter = keyStart;
// 	for (;iterIter != keyEnd; iterIter ++)
// 	{
// 		(*iterIter).first;
// 		(*iterIter).second;
// 	}

}
//9-30 the follow codes are not test!
bool DeformableRegistration::sample3validTraj(vector<PCloudTraj> & totTraj,vector<PCloudTraj>& resTraj,LifeSpan & lifesp)
{
	//rand()

	IndexType firstTraj = rand()%totTraj.size();
	IndexType secondTraj = rand()%totTraj.size();
	IndexType thirdTraj = rand()%totTraj.size();

	resTraj.push_back(totTraj[firstTraj]);
	resTraj.push_back(totTraj[secondTraj]);
	resTraj.push_back(totTraj[thirdTraj]);
	//
	//Logger<<"first sample traj is "<<firstTraj<<endl;
	//Logger<<"second sample traj is "<<secondTraj<<endl;
	//Logger<<"third sample traj is "<<thirdTraj<<endl;
	//
	return getCommonTime(resTraj,lifesp);

}

//---------------------------
bool DeformableRegistration::sampleValidTraj(vector<PCloudTraj> & totTraj,vector<PCloudTraj>& resTraj,LifeSpan & lifesp,IndexType num)
{
	assert(num > 3);

	for (IndexType i = 0; i < num; i ++)
	{
		IndexType indexTraj = rand()%totTraj.size();
		resTraj.push_back(totTraj[indexTraj]);
	}

	return getCommonTime(resTraj,lifesp);

}
 PCloudModel DeformableRegistration::constrcutRigidMotionMode(vector<PCloudTraj> & oriTraj,LifeSpan & comTime)
 {
 	//oriTraj size is 3,and  begin and end  of those trajectories even known
	assert(comTime.end > comTime.start);

	IndexType totFrame = comTime.end - comTime.start + 1;
 	PCloudModel pModel;
	pModel.modeLifeSpan = comTime;
	pModel.Centers.setZero(3,totFrame);

	Eigen::Vector3f mean;
 	Matrix33 f_rotmate;
	Matrix33 b_rotmate;
	Matrix3X srPoints;
	Matrix3X tgPoints;    

 	//calculate rotmate matrix  between frame (_start and _start + 1)
	// rotmate * srPos = tgPos  ----- [3×3] * [3×3] = [3×3];column-major storage vertex position
	getMultiTrajNodeCoordinate(oriTraj,comTime.start,srPoints);

	IndexType centerId = 0;
	calculateCenter(srPoints,mean);
	pModel.Centers.col(centerId) = mean;

	for (IndexType frameIndex = comTime.start; frameIndex < comTime.end; frameIndex ++)//nodes  number = (end - start)
 	{
        getMultiTrajNodeCoordinate(oriTraj,frameIndex + 1,tgPoints);	
		// calculate center 2014-10-24
		centerId ++;
		mean.setZero();
		calculateCenter(tgPoints,mean);
		pModel.Centers.col(centerId) = mean;
		//
		f_rotmate.setIdentity();
		b_rotmate.setIdentity();

		//affine-9-30
		//calculateTransformation2PClouds(srPoints,tgPoints,f_rotmate);//calculate  front rotmate --least squares
		//calculateTransformation2PClouds(tgPoints,srPoints,b_rotmate);//calculate  back  rotmate --least squares

		//rigid-10-8--svd-factorization
		point2point(srPoints,tgPoints,f_rotmate);
		point2point(tgPoints,srPoints,b_rotmate);

		pModel.fNode.push_back(f_rotmate);
		pModel.bNode.push_back(b_rotmate);

		srPoints = tgPoints;
 	}
    return pModel; 
 }

 //affine model 7-30
 PCloudAffModel DeformableRegistration::constrcutAffineMotionMode(vector<PCloudTraj> & oriTraj,LifeSpan & comTime)
 {
	 //oriTraj size is 3,and  begin and end  of those trajectories even known
	 assert(comTime.end > comTime.start);

	 IndexType totFrame = comTime.end - comTime.start + 1;
	 PCloudAffModel pModel;
	 pModel.modelLifeSpan = comTime;


	 Eigen::Vector3f mean;
	 Matrix34 f_transf;
	 Matrix34 b_transf;
	 Matrix3X srPoints;
	 Matrix3X tgPoints;    

	 //calculate rotmate matrix  between frame (_start and _start + 1)
	 // rotmate * srPos = tgPos  ----- [3×3] * [3×3] = [3×3];column-major storage vertex position
	 getMultiTrajNodeCoordinate(oriTraj,comTime.start,srPoints);


	 for (IndexType frameIndex = comTime.start; frameIndex < comTime.end; frameIndex ++)//nodes  number = (end - start)
	 {
		 getMultiTrajNodeCoordinate(oriTraj,frameIndex + 1,tgPoints);

		 f_transf.setZero();
		 b_transf.setZero();

		 calculateAffineTrans(srPoints,tgPoints,f_transf);

		 pModel.fNode.push_back(f_transf);

		 calculateAffineTrans(tgPoints,srPoints,b_transf);

		 pModel.bNode.push_back(b_transf);

		 //affine-2015-7-30--ldlt

		 srPoints = tgPoints;
	 }
	 return pModel; 
 }

 //---------------------------
 void DeformableRegistration::calculateAffineTrans(Matrix3X& srCoor, Matrix3X& tgCoor,Matrix34& affineMat)
 {
	 //

// 	 Logger<<"sr coor = "<<endl;
// 	 Logger<<srCoor<<endl;
// 
// 	 Logger<<"tg coor = "<<endl;
// 	 Logger<<tgCoor<<endl;

	 MatrixXX srOriCoor = srCoor;

	 MatrixXX tgOriCoor = tgCoor;

	 IndexType pSzie = srCoor.cols();
	 MatrixXX rA;

	 rA.setOnes(4,pSzie);

	 rA.block(0,0,3,pSzie) = srOriCoor.block(0,0,3,pSzie);

	 MatrixXX rAT = rA.transpose();

// 	 Logger<<"rAT = "<<endl;
// 	 Logger<<rAT<<endl;

	 //MatrixXX res = rAT.ldlt().solve(tgOriCoor.transpose() );
	 MatrixXX res = rAT.fullPivHouseholderQr().solve(tgOriCoor.transpose() );

	 affineMat = res.transpose();

// 	 Logger<<" result transformation = "<<endl;
// 	 Logger<<affineMat<<endl;
 }
 //---------------------------

 void DeformableRegistration::getTrajNodeCoordinate(PCloudTraj & traj,IndexType frameIndex,PointType & resCoor)
 {
	 IndexType vtxId = 0;
	 assert(traj.trajLifeSpan.end > traj.trajLifeSpan.start);
	 if (frameIndex <= traj.trajLifeSpan.end && frameIndex >= traj.trajLifeSpan.start)
	 {
		 vtxId = traj.trajNode[frameIndex - traj.trajLifeSpan.start];
		 resCoor = m_smpSet[frameIndex].vertices_matrix().col(vtxId);
	 }else
	 {
		 Logger<<" index out of range.\n";
	 }
 }
 void DeformableRegistration::getMultiTrajNodeCoordinate(vector<PCloudTraj>& multiTraj,IndexType frameId,Matrix3X &resCoorMat)
 {//获取不同轨迹在同一帧的顶点坐标<假设它们有共同的life-time>
   
   //assert(each trajectory have the common instance of frameId)

   IndexType trajNum = multiTraj.size();
   resCoorMat.setZero(3,trajNum);
   PointType tempPoint;
   for (IndexType trajIndex = 0; trajIndex < trajNum; trajIndex ++)
   {
	   getTrajNodeCoordinate(multiTraj[trajIndex],frameId,tempPoint);
	   resCoorMat.col(trajIndex) = tempPoint;
   }
 }
 void DeformableRegistration::calculateTransformation2PClouds(Matrix3X & srPCoud,Matrix3X & tgPCloud,Matrix33 & transMat)
 {
	 //x * a = b
	 MatrixXX srTrsP  = srPCoud.transpose(); //a'
	 Matrix33 srprdsr = srPCoud * srTrsP;   //a*a'
	 Matrix33 srInvs  = srprdsr.inverse();  // 1/(a * a')
	 transMat = (tgPCloud * srTrsP) * srInvs;
 }
 void DeformableRegistration::testingcalPmodel()
 {
	 //vector<PCloudTraj> totTraj;
	 //vector<PCloudTraj> resTraj;
	 //vector<PCloudModel> totModel;
	 //vector<IndexType> labels;
	 //PCloudModel resModel;
	 //calculateLifeSpansTraj(totTraj,3);
	 //Logger<<"total traj size = "<<totTraj.size()<<endl;

	 //IndexType modelNum = 250;
	 //LifeSpan commonLifeSp;
	 //while (modelNum-- > 0) 
	 //{
		// while (true)
		// {
		//	 if (sample3validTraj(totTraj,resTraj,commonLifeSp))
		//	 {
		//	   Logger<<"sample success!.\n";
		//	   break;
		//	 }
		//	 resTraj.clear();
		//	 commonLifeSp.start = 0;
		//	 commonLifeSp.end   = 0;
		// }
	 //    resModel = constrcutRigidMotionMode(resTraj,commonLifeSp);
		// totModel.push_back(resModel);
	 //}
	 //Logger<<"sucess calculate model.\n";

	 ////J_LinkageAdapter<PCloudTraj,PCloudModel,>
	 //Logger<<"End calculate dis between traj and model.\n";

	 Logger<<"Begin test TPS.\n";

	 Sample& srCloud = m_smpSet[0];
	 Sample& tgCloud = m_smpSet[1];
	 //deformationUsingTPS(srCloud,tgCloud,100);

	 //calculateNorm();

	 /////---2014-10-28 test non-rigid data  robot
	 MatrixXXi vtxMap;
	 MatrixXX  isDeformable;
	 std::set<IndexType> disLarge;
	 non_rigidRegister(srCloud,tgCloud,vtxMap,isDeformable,disLarge);
	 srCloud.update();

	 // draw trajectory
	 Tracer& cor = Tracer::get_instance();
	 cor.clear_records();
	 cor.add_percentCor(0,1,vtxMap,20,srCloud.num_vertices());

	 Logger<<"End test Non.\n";

 }
 bool DeformableRegistration::getCommonTime(vector<PCloudTraj> & eleTraj,LifeSpan & lifesp)
 {
	 IndexType lifeStart = 0;
	 IndexType lifeEnd   = 1e5;
	 IndexType trajSize = eleTraj.size();
	 PCloudTraj temp;
	 for (IndexType trajId = 0; trajId < trajSize; trajId ++)
	 {
		 temp = eleTraj[trajId];
		 if (temp.trajLifeSpan.start > lifeStart)
		 {
			 lifeStart = temp.trajLifeSpan.start;
		 }

		 if (temp.trajLifeSpan.end < lifeEnd)
		 {
			 lifeEnd = temp.trajLifeSpan.end;
		 }
	 }

	 if ( lifeEnd > lifeStart)
	 {
		 lifesp.start = lifeStart;
		 lifesp.end   = lifeEnd;
		 return true;
	 }else
	 {
		 return false;
	 }
 }
 ScalarType DeformableRegistration::traj2ModlDistance(PCloudTraj& traj, PCloudModel& model)
 {
	 ScalarType dis = 0.0;

	 //1--common time 
	 LifeSpan commonTime = traj.trajLifeSpan; //traj and model common life-span
	 if (commonTime.start < model.modeLifeSpan.start) // start get large value
	 {
		 commonTime.start = model.modeLifeSpan.start;
	 }

	 if (commonTime.end > model.modeLifeSpan.end)//end get small value
	 {
		 commonTime.end = model.modeLifeSpan.end;
	 }

	 if(commonTime.end > commonTime.start)
	 {
		 //2-- produce a new trajectory from traj have  common time with Model
		 PCloudTraj comTraj;
		 intersectionTrajWithComTime(traj,commonTime,comTraj);

		 //3--
		 PCloudModel comModel;
		 intersectionModelWithComTime(model,commonTime,comModel);

		 // 4--
		 return minTraj2ModelDis(comTraj,comModel,commonTime);
	 }else // no  common time
	 {
		 return 1e5;
	 }

 }
 void DeformableRegistration::intersectionTrajWithComTime(PCloudTraj & oriTraj,LifeSpan & comTime,PCloudTraj & newTraj)
 {
	 IndexType spans = 0;
	 IndexType sPos = 0;   //start node index
	 spans = comTime.end - comTime.start + 1;
	 sPos = comTime.start - oriTraj.trajLifeSpan.start;
	 newTraj.trajLifeSpan = comTime;
	 //vector must resize before assign
	 for (IndexType nodeId = 0; nodeId < spans; nodeId ++,sPos ++)
	 {
		 newTraj.trajNode.push_back(oriTraj.trajNode[sPos]);
	 }
 }
 void DeformableRegistration::intersectionModelWithComTime(PCloudModel & oriModel,LifeSpan & comTime,PCloudModel & newModel)
 {
	 IndexType spans = 0;
	 IndexType sPos = 0;   //start node index
	 spans = comTime.end - comTime.start;
	 sPos = comTime.start - oriModel.modeLifeSpan.start;
	 newModel.modeLifeSpan = comTime;
	 newModel.Centers.setZero(3,spans + 1);
	 for (IndexType nodeId = 0; nodeId < spans; nodeId ++,sPos ++)
	 {
		 newModel.fNode.push_back(oriModel.fNode[sPos]);
		 newModel.bNode.push_back(oriModel.bNode[sPos]);
	 }

	 sPos = comTime.start - oriModel.modeLifeSpan.start;
	 for (IndexType nodeId = 0; nodeId <= spans; nodeId ++,sPos ++)
	 {
		 newModel.Centers.col(nodeId) = oriModel.Centers.col(sPos);
	 }
 }
 ScalarType DeformableRegistration::minTraj2ModelDis(PCloudTraj& traj, PCloudModel& model,LifeSpan & comTime)
 {
	 assert(traj.trajLifeSpan.start == comTime.start && traj.trajLifeSpan.end == comTime.end);
	 assert(model.modeLifeSpan.start == comTime.start && model.modeLifeSpan.end == comTime.end);

	 //start end ={3,7}
	 //node      ={21, 34, 87,45,76}
	 //frame     =[3   4   5  6  7]
	 ScalarType dis = 1e6;
	 IndexType vtxIndex = 0;
	 IndexType startFrame = comTime.start;// = 3
	 IndexType totFrame = comTime.end - comTime.start + 1;// 7 - 3 + 1
	 PointType trajNode;

	 Matrix3X TrajCoordinate;
	 TrajCoordinate.setZero(3,totFrame);

	 //20150103-normal
	 MatrixXX TrajNormal;
	 TrajNormal.setZero(3,totFrame);
	 //

	 MatrixXX rowTrajCoordinate; //(x1,y1,z1,---,xn,yn,zn) in order to calculate distances between two trajectories
	 rowTrajCoordinate.setZero(1,3 * totFrame);
	 
	 ///20150103
	 ScalarType N_x,N_y,N_z;
	 MatrixXX rowTrajNormal;
	 rowTrajNormal.setZero(1,3 * totFrame);
	 //////////////////////////////////////////////////////////////////////////



	 //get the traj's coordinate  of  each  node   ////get the traj's normal of each node--20150103
	 for (IndexType nodeId = 0; nodeId < totFrame; nodeId ++,startFrame ++)
	 {
		 vtxIndex = traj.trajNode[nodeId];
		 trajNode = m_smpSet[startFrame].vertices_matrix().col(vtxIndex);
		 TrajCoordinate.col(nodeId) = trajNode;
		 rowTrajCoordinate.block(0, 3 * nodeId,1,3) = trajNode.transpose().block(0,0,1,3);

		 //normal
		 N_x = m_smpSet[startFrame][vtxIndex].nx();
		 N_y = m_smpSet[startFrame][vtxIndex].ny();
		 N_z = m_smpSet[startFrame][vtxIndex].nz();
		 NormalType tNormal(N_x,N_y,N_z);

		 //rowTrajNormal(0,3*nodeId)     = N_x;
		 //rowTrajNormal(0,3*nodeId + 1) = N_y;
		 //rowTrajNormal(0,3*nodeId + 2) = N_z;
		 TrajNormal.col(nodeId) = tNormal;
		 rowTrajNormal.block(0, 3 * nodeId,1,3) = tNormal.transpose().block(0,0,1,3);
	 }
	 
	 


	 // produce a  new traj under motion mode [model]
	 ScalarType tempdis = 0.0;
	 MatrixXX tempTraj;
	 tempTraj.setZero(1,3 * totFrame);

	 //
	 MatrixXX tempTrajNormal;
	 tempTrajNormal.setZero(1,3*totFrame);
	 //

	 for (IndexType nodeId = 0; nodeId < totFrame; nodeId ++)
	 {
		 tempTraj.setZero();
		 calNewTrajUnderModel(nodeId,model,TrajCoordinate.col(nodeId),tempTraj);
		 tempdis = ((tempTraj.row(0) - rowTrajCoordinate.row(0)).norm()) / totFrame;
		 //
		 calNewTrajNormalUnderModel(nodeId,model,TrajNormal.col(nodeId),tempTrajNormal);
		 //ScalarType n_dot = (tempTrajNormal.row(0)-rowTrajNormal.row(0)).norm()/totFrame;
		 ScalarType n_dot = calcualteInnerProduct(rowTrajNormal,tempTrajNormal);
		 //

		 tempdis += n_dot;

		 if(tempdis < dis)
		 {
			 dis = tempdis;
		 }
	 }
	 return dis;
 }

 //2015-1-3
 void DeformableRegistration::calNewTrajNormalUnderModel(IndexType nodeId,PCloudModel & model,NormalType oriNodePos,MatrixXX & rowNewTrajNorm)
 {
	 //aim to fill the row vector

	 // frame -- 2  3   4   5   6   7
	 //vtxId    45  78  92  39  53  16
	 //nodeId   0   1   [2] 3   4   5
	 //FRotate  0   1   2   3   4   
	 //BRotate      0   1   2   3   4  
	 //start = 2
	 //end   = 7
	 //totFrame = 5

	 IndexType start = model.modeLifeSpan.start;
	 IndexType end   = model.modeLifeSpan.end;
	 IndexType totFrame = end - start ;

	 //nodeId  ∈ [ 0-1-2--nodeId--(totFrame)]
	 rowNewTrajNorm.block(0, 3 * nodeId,1,3) = oriNodePos.transpose().block(0,0,1,3);

	 //0  <= nodeId <=  totFrame 
	 //(Front)---->[nodeId,nodeId + 1,---,totFrame] 2014-10-2
	 //(Back)<-----[0,---,nodeId - 1,nodeId]

	 IndexType fNum = 0;
	 IndexType bNum = 0;
	 PointType transPoint;
	 Eigen::Vector3f x_mean,y_mean;
	 Matrix33 rotmate;
	 rotmate.setIdentity();
	 x_mean.setZero();
	 y_mean.setZero();

	 if(nodeId > 0 && nodeId < totFrame)
	 {
		 fNum = totFrame - nodeId;
		 bNum = nodeId;
		 transPoint = oriNodePos;

		 for (IndexType fId = 0; fId < fNum; fId ++)
		 {
			 rotmate = model.fNode[nodeId + fId];
			 //x_mean = model.Centers.col(nodeId + fId);
			 //y_mean = model.Centers.col(nodeId + fId + 1);
			 /*transPoint = rotmate * (transPoint - x_mean) + y_mean;*/
			 transPoint = rotmate * transPoint;
			 rowNewTrajNorm.block(0, 3 * (nodeId + fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
		 }

		 transPoint = oriNodePos;
		 for(IndexType fId = bNum - 1 ; fId >= 0; fId --)
		 {
			 rotmate = model.bNode[fId];
			 //x_mean  = model.Centers.col(fId + 1);
			 //y_mean  = model.Centers.col(fId);

			 //transPoint = rotmate * (transPoint - x_mean) + y_mean;
			 transPoint = rotmate* transPoint;
			 rowNewTrajNorm.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

		 }
	 }else
	 {
		 if (0 == nodeId)//
		 {
			 transPoint = oriNodePos;
			 for (IndexType fId = 0; fId < totFrame; fId ++)
			 {
				 rotmate = model.fNode[fId];
				 x_mean = model.Centers.col(fId);
				 y_mean = model.Centers.col(fId + 1);

				 transPoint = rotmate * (transPoint - x_mean) + y_mean;

				 PointType transPoint_ = rotmate* oriNodePos;
				 rowNewTrajNorm.block(0, 3 * (fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
			 }
		 }else// totFrame == nodeId 
		 {			 
			 transPoint = oriNodePos;
			 for(IndexType fId = totFrame - 1; fId >= 0;fId --)
			 {
				 rotmate = model.bNode[fId];
				 //x_mean = model.Centers.col(fId + 1);
				 //y_mean = model.Centers.col(fId);

				 //transPoint = rotmate * (transPoint - x_mean) + y_mean; 
				 transPoint = rotmate* transPoint;
				 rowNewTrajNorm.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

			 }
		 }
	 }
 }
ScalarType DeformableRegistration::calcualteInnerProduct(MatrixXX & oriNorm,MatrixXX& newNorm)
{
	ScalarType res = 0.0;
	IndexType tot = oriNorm.cols();
	for (IndexType n_id = 0; n_id < tot; n_id +=3)
	{
		NormalType sNorm(oriNorm(0,n_id),oriNorm(0,n_id + 1),oriNorm(0,n_id + 2));
		NormalType tNorm(newNorm(0,n_id),newNorm(0,n_id + 1),newNorm(0,n_id + 2));
		ScalarType temp = 1. - abs(sNorm.dot(tNorm));
		res += temp;
	}
	return 3*res/tot;
}
 void DeformableRegistration::calNewTrajUnderModel(IndexType nodeId,PCloudModel & model,PointType oriNodePos,MatrixXX & rowNewTraj)
 {
	 //aim to fill the row vector

	 // frame -- 2  3   4   5   6   7
	 //vtxId    45  78  92  39  53  16
     //nodeId   0   1   [2] 3   4   5
	 //FRotate  0   1   2   3   4   
	 //BRotate      0   1   2   3   4  
	 //start = 2
	 //end   = 7
	 //totFrame = 5

	 IndexType start = model.modeLifeSpan.start;
	 IndexType end   = model.modeLifeSpan.end;
	 IndexType totFrame = end - start ;

	 //nodeId  ∈ [ 0-1-2--nodeId--(totFrame)]
	 rowNewTraj.block(0, 3 * nodeId,1,3) = oriNodePos.transpose().block(0,0,1,3);

	 //0  <= nodeId <=  totFrame 
	 //(Front)---->[nodeId,nodeId + 1,---,totFrame] 2014-10-2
	 //(Back)<-----[0,---,nodeId - 1,nodeId]

	 IndexType fNum = 0;
	 IndexType bNum = 0;
	 PointType transPoint;
	 Eigen::Vector3f x_mean,y_mean;
	 Matrix33 rotmate;
	 rotmate.setIdentity();
	 x_mean.setZero();
	 y_mean.setZero();

	 if(nodeId > 0 && nodeId < totFrame)
	 {
		 fNum = totFrame - nodeId;
		 bNum = nodeId;
         transPoint = oriNodePos;

		 for (IndexType fId = 0; fId < fNum; fId ++)
		 {
			 rotmate = model.fNode[nodeId + fId];
			 x_mean = model.Centers.col(nodeId + fId);
			 y_mean = model.Centers.col(nodeId + fId + 1);

			 transPoint = rotmate * (transPoint - x_mean) + y_mean;
			 rowNewTraj.block(0, 3 * (nodeId + fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
		 }

		 transPoint = oriNodePos;
		 for(IndexType fId = bNum - 1 ; fId >= 0; fId --)
		 {
			 rotmate = model.bNode[fId];
			 x_mean  = model.Centers.col(fId + 1);
			 y_mean  = model.Centers.col(fId);

			 transPoint = rotmate * (transPoint - x_mean) + y_mean;
			 rowNewTraj.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

		 }
	 }else
	 {
		 if (0 == nodeId)//
		 {
			 transPoint = oriNodePos;
			 for (IndexType fId = 0; fId < totFrame; fId ++)
			 {
				 rotmate = model.fNode[fId];
				 x_mean = model.Centers.col(fId);
				 y_mean = model.Centers.col(fId + 1);

				 transPoint = rotmate * (transPoint - x_mean) + y_mean;
				 rowNewTraj.block(0, 3 * (fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
			 }
		 }else// totFrame == nodeId 
		 {			 
			 transPoint = oriNodePos;
			 for(IndexType fId = totFrame - 1; fId >= 0;fId --)
			 {
				 rotmate = model.bNode[fId];
				 x_mean = model.Centers.col(fId + 1);
				 y_mean = model.Centers.col(fId);

				 transPoint = rotmate * (transPoint - x_mean) + y_mean; 
				 rowNewTraj.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

			 }
		 }
	 }
 }
 bool DeformableRegistration::sampleModel(vector<PCloudTraj>& oriTraj,vector<PCloudModel>& smpOriModel,IndexType modelNum)
 {
	   LifeSpan commonLifeSp;
	   vector<PCloudTraj> resTraj;
	   PCloudModel resModel;

   		while (modelNum-- > 0) 
		{
			while (true)
			{
				if (sample3validTraj(oriTraj,resTraj,commonLifeSp))
				{
					//Logger<<"sample success!.\n";
					break;
				}
				resTraj.clear();
				commonLifeSp.start = 0;
				commonLifeSp.end   = 0;
			}
			resModel = constrcutRigidMotionMode(resTraj,commonLifeSp);
			smpOriModel.push_back(resModel);
			resTraj.clear();
			commonLifeSp.start = 0;
			commonLifeSp.end   = 0;
		}
		// 
		Logger<<"sample mode size = "<<smpOriModel.size()<<endl;
		return true;
 }

 //2015-07-30
 //---------------------------Affine transformation
 bool DeformableRegistration::sampleAffineModel(vector<PCloudTraj>& oriTraj,vector<PCloudAffModel>& smpOriModel,IndexType modelNum)
 {
	 LifeSpan commonLifeSp;
	 vector<PCloudTraj> resTraj;
	 PCloudAffModel resModel;

	 while (modelNum-- > 0) 
	 {
		 while (true)
		 {
			 if (sampleValidTraj(oriTraj,resTraj,commonLifeSp,4) )
			 {
				 //Logger<<"sample success!.\n";
				 break;
			 }
			 resTraj.clear();
			 commonLifeSp.start = 0;
			 commonLifeSp.end   = 0;
		 }
		 /*resModel = constrcutRigidMotionMode(resTraj,commonLifeSp);*/
		 resModel = constrcutAffineMotionMode(resTraj,commonLifeSp);
		 smpOriModel.push_back(resModel);
		 resTraj.clear();
		 commonLifeSp.start = 0;
		 commonLifeSp.end   = 0;
	 }
	 // 
	 Logger<<"sample mode size = "<<smpOriModel.size()<<endl;
	 return true;
 }
 //---------------------------
 bool DeformableRegistration::sampleModelByNeig(vector<PCloudTraj>& oriTraj,vector<PCloudModel>& smpOriModel,IndexType modelNum)
 {
	   LifeSpan commonLifeSp;
	   PCloudModel resModel;
	   PCloudTraj sampleTraj;
	   IndexType smpId = 0;
   		while (modelNum-- > 0) 
		{
			smpId = rand() % oriTraj.size();
			sampleTraj = oriTraj[smpId];
			resModel = constructRigidMotionByNeig(sampleTraj);
			smpOriModel.push_back(resModel);
		}
		// 
		Logger<<"sample models success .\n";
		return true;
 }

 PCloudModel DeformableRegistration::constructRigidMotionByNeig(PCloudTraj & oriTraj)
 {
	 IndexType totFrame = oriTraj.trajLifeSpan.end - oriTraj.trajLifeSpan.start + 1;
	 assert(totFrame > 1);
 	PCloudModel pModel;
	pModel.modeLifeSpan = oriTraj.trajLifeSpan;
	pModel.Centers.setZero(3,totFrame);
 	Matrix33 f_rotmate;
	Matrix33 b_rotmate;
	Eigen::Vector3f sr_mean,tg_mean;
	sr_mean.setZero();
	tg_mean.setZero();
	//save the neig indexs
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];


	//save those neig coordinates
	Matrix3X srPoints;
	Matrix3X tgPoints;    
	Matrix3X srTempPoints;
	Matrix3X tgTempPoints;
	Matrix3X srSecondTemp;


 	//calculate rotmate matrix  between frame (_start and _start + 1)
	IndexType frameStart = oriTraj.trajLifeSpan.start;
	IndexType frameEnd   = oriTraj.trajLifeSpan.end;
	vector<IndexType> & vtxIndex = oriTraj.trajNode;
	IndexType nodeIndex = 0;

	//loacl ICP 
	SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	MatrixXXi localCor;
	localCor.setZero(1,m_neigNum);

	for (IndexType frameIndex = frameStart; frameIndex < frameEnd; frameIndex ++,nodeIndex ++)//nodes  number = (end - start)
 	{
	    Sample & srGraph = m_smpSet[frameIndex];		
		Sample & tgGraph = m_smpSet[frameIndex + 1];

		srGraph.neighbours(vtxIndex[nodeIndex],m_neigNum,neigSrGraph);
		tgGraph.neighbours(vtxIndex[nodeIndex + 1],m_neigNum,neigTgGraph);

		srPoints.setZero(3,m_neigNum);
		tgPoints.setZero(3,m_neigNum);

		getConstantNeigCoorByIndex(srGraph,neigSrGraph,srPoints);
		srTempPoints = srPoints;
		srSecondTemp = srPoints;


		getConstantNeigCoorByIndex(tgGraph,neigTgGraph,tgPoints);
		tgTempPoints = tgPoints;

		f_rotmate.setIdentity();
		b_rotmate.setIdentity();

		SICP::point_w_point(srPoints,tgPoints,localCor,pa);//front rotmate
		alignTargetCoor(srTempPoints,localCor);
		point2point(srTempPoints,tgPoints,f_rotmate);

		calculateCenter(srTempPoints,sr_mean);
		calculateCenter(tgPoints,tg_mean);

		pModel.Centers.col(nodeIndex) = sr_mean;
		pModel.Centers.col(nodeIndex + 1) = tg_mean;

		///
		localCor.setZero();
		SICP::point_w_point(tgPoints,srSecondTemp,localCor,pa);//back rotmate
		alignTargetCoor(tgTempPoints,localCor);
		point2point(tgTempPoints,srSecondTemp,b_rotmate);

		//rigid-10-8
		pModel.fNode.push_back(f_rotmate);
		pModel.bNode.push_back(b_rotmate);
 	}

	delete []neigSrGraph;
	delete []neigTgGraph;	

    return pModel; 
 }

 void DeformableRegistration::staticDistance(vector<PCloudTraj>& oritrj,vector<PCloudModel>& oriModel)
 {
	 Logger<<"Start static distance,output .txt file .\n";

	 IndexType trajSize = oritrj.size();
	 IndexType modelSize = oriModel.size();
	 assert(trajSize > 0 && modelSize > 0);

	 MatrixXX disMat;
	 disMat.setZero(oritrj.size(),oriModel.size());
	 ScalarType dis = 0.0;
	 std::fstream op("E:\\1114LabWalk.txt",std::ios::app);

	 Logger<<"traj size = "<<trajSize<<endl;
	 Logger<<"Model size = "<<modelSize<<endl;

	 for (IndexType testTraj = 0; testTraj < trajSize; testTraj ++)
	 {
		 for (IndexType testModel = 0; testModel < modelSize; testModel ++)
		 {			 
			 dis =  traj2ModlDistance(oritrj[testTraj],oriModel[testModel]);
			 //if (dis < 1000.0)
			 //{
                disMat(testTraj,testModel)  = dis;
			 //}else
			 //{
				//disMat(testTraj,testModel) = 380;
			 //}
		 }
	 }

	  	if (op.is_open())
 		{  
			op<<disMat<<endl;
 			op.close();
 		}
 		else
 		{
 			cout<<"open file error！"<<endl;
 			return ;
 		}
		Logger<<"End static distance.\n";
 }
 void DeformableRegistration::produceDreamTraj(vector<PCloudTraj>& resTraj)
 {
	 Logger<<"Begin calculate dream traj.\n";

 	IndexType	num_sample = m_smpSet.size();
 	IndexType	num_vtx	= m_smpSet[0].num_vertices();// default trajectory number

	PCloudTraj tempTraj;
	tempTraj.trajLifeSpan.start = 0;
	tempTraj.trajLifeSpan.end   = num_sample - 1;
	tempTraj.trajNode.resize(num_sample);

	for(IndexType vtxId = 0; vtxId < num_vtx; vtxId += 4)
	{
		vector<IndexType> trajNode(num_sample,vtxId);
		tempTraj.trajNode = trajNode;
		resTraj.push_back(tempTraj);
	}

	drawLifespansTraj(resTraj);
	Logger<<"End calculate dream traj.\n";
 }
 void DeformableRegistration::produceDreamTraj(vector<PCloudTraj>& resTraj,vector<IndexType>& sampleCenter)
 {
	 Logger<<"Begin calculate dream traj.\n";

	 IndexType	num_sample = m_smpSet.size();
	 IndexType	num_vtx	= m_smpSet[0].num_vertices();// default trajectory number

	 PCloudTraj tempTraj;
	 tempTraj.trajLifeSpan.start = 0;
	 tempTraj.trajLifeSpan.end   = num_sample - 1;
	 tempTraj.trajNode.resize(num_sample);

	 //calculateNorm();
	 //0107
	 sampleCenter.resize(num_vtx);
	 //IndexType pId = 0;
	 for(IndexType vtxId = 0; vtxId < num_vtx; vtxId ++/*=5,pId ++*/)
	 {
		 vector<IndexType> trajNode(num_sample,vtxId);
		 tempTraj.trajNode = trajNode;
		 resTraj.push_back(tempTraj);
		 sampleCenter[vtxId] = vtxId;
		 //sampleCenter[pId] = vtxId;
	 }

	 drawLifespansTraj(resTraj);
	 Logger<<"End calculate dream traj.\n";
 }
 void DeformableRegistration::calculateCenter(Matrix3X& oriCoordinate,Eigen::Vector3f& mean)
 {
	 assert(oriCoordinate.cols() > 0);
	for(int i=0; i<3; ++i) //calculate center
 	{
		mean(i) = oriCoordinate.row(i).sum()/oriCoordinate.cols();
 	}
 }

  void DeformableRegistration::calculateFixedLengthTraj( vector<PCloudTraj>& trajs, IndexType smp_idx,IndexType length )
 {
	 SampleSet& smp_set = SampleSet::get_instance();
	 IndexType start_idx = smp_idx - length/2 ;
	 IndexType end_idx = smp_idx + length/2;

	 if( length%2!=0 )
		 ++end_idx;
	 if( start_idx<0 )
	 {
		 start_idx = 0;
		 end_idx = length;
	 }
	 if( end_idx > smp_set.size() - 1  )
	 {
		 end_idx = smp_set.size() - 1;
		 start_idx = end_idx - length;
	 }

	 trajs.clear();
	 trajs.resize( smp_set[smp_idx].num_vertices() );
	 for ( int i=0; i<trajs.size();i++ )
	 {
		 trajs[i].trajLifeSpan.start = start_idx;
		 trajs[i].trajLifeSpan.end = end_idx;
		 trajs[i].trajNode.resize( end_idx-start_idx+1 );
	 }

	 for ( int j=start_idx; j<=end_idx; j++ )
	 {
		 if ( j==smp_idx )
		 {
			 for ( int i=0; i<trajs.size();i++ )
			 {
				 trajs[i].trajNode[j-start_idx] = i;
			 }
			 continue;
		 }

		 Sample& srGraph = smp_set[smp_idx];
		 Sample& tgGraph = smp_set[j];
		 Matrix3X srVtxMatrix = srGraph.vertices_matrix();
		 MatrixXXi   global_vtx_map;
		 MatrixXX    isDeformable;
		 std::set<IndexType> distoredLargeSet;

		 non_rigidRegister(srGraph,tgGraph,global_vtx_map,isDeformable,distoredLargeSet);

		 srGraph.vertices_matrix() = srVtxMatrix; 
		 srGraph.update();//back to original position after registeration

		 for ( int i=0; i<trajs.size();i++ )
		 {
			trajs[i].trajNode[j-start_idx] = global_vtx_map(0,i);
		 }
	 }

	 //draw traj
	 drawLifespansTraj(trajs);
 }

 //   void DeformableRegistration::calculateFixedLengthTraj( vector<PCloudTraj>& trajs, IndexType smp_idx,IndexType length )
 //{
	// SampleSet& smp_set = SampleSet::get_instance();
	// IndexType start_idx = smp_idx - length/2 ;
	// IndexType end_idx = smp_idx + length/2;
	// vector<Sample*> p_subsmp_set( end_idx - start_idx + 1);
	// vector< vector<IndexType> > vtx_map(end_idx - start_idx + 1);
	// for (int i = 0; i < end_idx - start_idx+1; i++)
	// {
	//	 p_subsmp_set[i] = new Sample;
	//	 SampleManipulation::SubSampleSolution solv( smp_set[start_idx+i], *p_subsmp_set[i],32);
	//	 vtx_map[i] = solv.compute();
	// }

	// if( length%2!=0 )
	//	 ++end_idx;
	// if( start_idx<0 )
	// {
	//	 start_idx = 0;
	//	 end_idx = length;
	// }
	// if( end_idx > smp_set.size() - 1  )
	// {
	//	 end_idx = smp_set.size() - 1;
	//	 start_idx = end_idx - length;
	// }

	// trajs.clear();
	// trajs.resize( p_subsmp_set[smp_idx]->num_vertices() );
	// for ( int i=0; i<trajs.size();i++ )
	// {
	//	 trajs[i].trajLifeSpan.start = start_idx;
	//	 trajs[i].trajLifeSpan.end = end_idx;
	//	 trajs[i].trajNode.resize( end_idx-start_idx+1 );
	// }
	// for ( int j=start_idx; j<=end_idx; j++ )
	// {
	//	 if ( j==smp_idx )
	//	 {
	//		 for ( int i=0; i<trajs.size();i++ )
	//		 {
	//			 trajs[i].trajNode[j-start_idx] = vtx_map[j-start_idx][i];
	//		 }
	//		 continue;
	//	 }

	//	 Sample* srGraph = p_subsmp_set[smp_idx- start_idx];
	//	 Sample* tgGraph = p_subsmp_set[j - start_idx];
	//	 Matrix3X srVtxMatrix = srGraph->vertices_matrix();
	//	 MatrixXXi   global_vtx_map;
	//	 MatrixXX    isDeformable;
	//	 std::set<IndexType> distoredLargeSet;

	//	 non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);

	//	 srGraph->vertices_matrix() = srVtxMatrix; 
	//	 srGraph->update();//back to original position after registeration

	//	 for ( int i=0; i<trajs.size();i++ )
	//	 {
	//		trajs[i].trajNode[j-start_idx] = vtx_map[j-start_idx][global_vtx_map(0,i)];
	//	 }
	// }
	// //draw traj
	// drawLifespansTraj(trajs);
 //}
	void DeformableRegistration::calculateFixedLengthTraj(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap, IndexType length,IndexType octreeRes )
	{
		Logger<<"start Calculate Traj.\n";

		//calculateNorm();

		 SampleSet& smp_set = SampleSet::get_instance();
		 IndexType start_idx = smp_idx - length/2 ;
		 IndexType end_idx = smp_idx + length/2;

		 if( length%2!=0 )
			 ++end_idx;
		 if( start_idx<0 )
		 {
			 start_idx = 0;
			 end_idx = length;
		 }
		 if( end_idx > smp_set.size() - 1  )
		 {
			 end_idx = smp_set.size() - 1;
			 start_idx = end_idx - length;
		 }

		 vector<Sample*> p_subsmp_set( end_idx - start_idx + 1);
		 vector< vector<IndexType> > vtx_map(end_idx - start_idx + 1);

		 for (int i = 0; i < end_idx - start_idx+1; i++)
		 {
			 p_subsmp_set[i] = new Sample;
			 SampleManipulation::SubSampleSolution solv( smp_set[start_idx+i], *p_subsmp_set[i],octreeRes);
			 vtx_map[i] = solv.compute();
		 }

		 centerVtxMap = vtx_map[smp_idx - start_idx];

		 trajs.clear();
		 trajs.resize( p_subsmp_set[smp_idx - start_idx]->num_vertices() );
		 for ( int i=0; i<trajs.size();i++ )
		 {
			 trajs[i].trajLifeSpan.start = start_idx;
			 trajs[i].trajLifeSpan.end = end_idx;
			 trajs[i].trajNode.resize( end_idx-start_idx+1 );
		 }
		 for ( int j=start_idx; j<=end_idx; j++ )
		 {
			 if ( j==smp_idx )
			 {
				 for ( int i=0; i<trajs.size();i++ )
				 {
					 trajs[i].trajNode[j-start_idx] = vtx_map[j-start_idx][i];
				 }
				 continue;
			 }

			 Sample* srGraph = p_subsmp_set[smp_idx- start_idx];
			 Sample* tgGraph = p_subsmp_set[j - start_idx];
			 Matrix3X srVtxMatrix = srGraph->vertices_matrix();
			 MatrixXXi   global_vtx_map;
			 MatrixXX    isDeformable;
			 std::set<IndexType> distoredLargeSet;

			 non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);

			 srGraph->vertices_matrix() = srVtxMatrix; 
			 srGraph->update();//back to original position after registeration

			 for ( int i=0; i<trajs.size();i++ )
			 {
				trajs[i].trajNode[j-start_idx] = vtx_map[j-start_idx][global_vtx_map(0,i)];
			 }
		 }

		 //draw traj
		 drawLifespansTraj(trajs);
		 Logger<<"End calculate traj.\n";

	}
	void DeformableRegistration::downSampleTraj(vector<PCloudTraj>& oriTraj,vector<PCloudTraj>&sampleTraj,Sample&centerFrame,vector<IndexType>&sampleVtxId)
	{ 
		Sample* p_subsmp_set = new Sample;
		SampleManipulation::SubSampleSolution solv( centerFrame, *p_subsmp_set,32);
		sampleVtxId = solv.compute();

		PCloudTraj tempTraj;
		for (int i = 0; i < sampleVtxId.size(); i++)
		{
			sampleTraj.push_back(oriTraj[sampleVtxId[i]]);
		}
	}
	void DeformableRegistration::calculateFixedLengthTrajBySampleTraj( vector<PCloudTraj>& trajs, IndexType smp_idx,IndexType length ,vector<IndexType>& sampleVtxMap)
	{
	 SampleSet& smp_set = SampleSet::get_instance();
	 IndexType start_idx = smp_idx - length/2 ;
	 IndexType end_idx = smp_idx + length/2;
	 vector<PCloudTraj> tempTraj;

	 if( length%2!=0 )
		 ++end_idx;
	 if( start_idx<0 )
	 {
		 start_idx = 0;
		 end_idx = length;
	 }
	 if( end_idx > smp_set.size() - 1  )
	 {
		 end_idx = smp_set.size() - 1;
		 start_idx = end_idx - length;
	 }

	 tempTraj.clear();
	 tempTraj.resize( smp_set[smp_idx].num_vertices() );

	 Sample& centerFrame = smp_set[smp_idx];
	 for ( int i=0; i<tempTraj.size();i++ )
	 {
		 tempTraj[i].trajLifeSpan.start = start_idx;
		 tempTraj[i].trajLifeSpan.end = end_idx;
		 tempTraj[i].trajNode.resize( end_idx-start_idx+1 );
	 }

	 for ( int j=start_idx; j<=end_idx; j++ )
	 {
		 if ( j==smp_idx )
		 {
			 for ( int i=0; i<tempTraj.size();i++ )
			 {
				 tempTraj[i].trajNode[j-start_idx] = i;
			 }
			 continue;
		 }

		 Sample& srGraph = smp_set[smp_idx];
		 Sample& tgGraph = smp_set[j];
		 Matrix3X srVtxMatrix = srGraph.vertices_matrix();
		 MatrixXXi   global_vtx_map;
		 MatrixXX    isDeformable;
		 std::set<IndexType> distoredLargeSet;

		 non_rigidRegister(srGraph,tgGraph,global_vtx_map,isDeformable,distoredLargeSet);

		 srGraph.vertices_matrix() = srVtxMatrix; 
		 srGraph.update();//back to original position after registeration

		 for ( int i=0; i<tempTraj.size();i++ )
		 {
			tempTraj[i].trajNode[j-start_idx] = global_vtx_map(0,i);
		 }
	 }


	 //draw traj
	 drawLifespansTraj(tempTraj);
	 downSampleTraj(tempTraj,trajs,centerFrame,sampleVtxMap);
	 tempTraj.clear();
	}

void DeformableRegistration::calculateFixedLengthTrajWithTracingAlong(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap,IndexType length, IndexType octreeRes )
	{

		Logger<<"start Calculate Traj,using concenstive rigid.\n";



		SampleSet& smp_set = SampleSet::get_instance();

		IndexType start_idx = smp_idx - length/2 ;

		IndexType end_idx = smp_idx + length/2;



		if( length%2!=0 )

			++end_idx;

		if( start_idx<0 )

		{

			start_idx = 0;

			end_idx = length;

		}

		if( end_idx > smp_set.size() - 1  )

		{

			end_idx = smp_set.size() - 1;

			start_idx = end_idx - length;

		}



		vector<Sample*> p_subsmp_set( end_idx - start_idx + 1);

		vector< vector<IndexType> > vtx_map(end_idx - start_idx + 1);



		for (int i = 0; i < end_idx - start_idx+1; i++)

		{

			p_subsmp_set[i] = new Sample;

			SampleManipulation::SubSampleSolution solv( smp_set[start_idx+i], *p_subsmp_set[i],octreeRes);

			vtx_map[i] = solv.compute();

		}



		centerVtxMap = vtx_map[smp_idx - start_idx];



		trajs.clear();

		trajs.resize( p_subsmp_set[smp_idx - start_idx]->num_vertices() );

		for ( int i=0; i<trajs.size();i++ )

		{

			trajs[i].trajLifeSpan.start = start_idx;

			trajs[i].trajLifeSpan.end = end_idx;

			trajs[i].trajNode.resize( end_idx-start_idx+1 );

		}



		for ( int i=0; i<trajs.size();i++ )

		{

			trajs[i].trajNode[smp_idx-start_idx] = vtx_map[smp_idx-start_idx][i];

		}





		vector<int> pre_idx_map(trajs.size());

		for ( int i=0; i<trajs.size(); ++i )

		{

			pre_idx_map[i] = i;

		}

		//backward trace

		for ( int j = smp_idx; j>start_idx; --j )

		{

			Sample* srGraph = p_subsmp_set[j- start_idx];

			Sample* tgGraph = p_subsmp_set[j - start_idx - 1];

			Matrix3X srVtxMatrix = srGraph->vertices_matrix();

			MatrixXXi   global_vtx_map;

			MatrixXX    isDeformable;

			std::set<IndexType> distoredLargeSet;



			non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);



			srGraph->vertices_matrix() = srVtxMatrix; 

			srGraph->update();//back to original position after registeration

			for ( int i=0; i<trajs.size();i++ )

			{

				trajs[i].trajNode[j-start_idx-1] = vtx_map[j-start_idx-1][global_vtx_map(0,pre_idx_map[i])];

				pre_idx_map[i] = global_vtx_map(0,pre_idx_map[i]);

			}



		}



		for ( int i=0; i<trajs.size(); ++i )

		{
			pre_idx_map[i] = i;
		}

		//forward trace

		for ( int j = smp_idx+1; j<=end_idx; ++j )

		{

			Sample* srGraph = p_subsmp_set[j- start_idx-1];

			Sample* tgGraph = p_subsmp_set[j - start_idx];

			Matrix3X srVtxMatrix = srGraph->vertices_matrix();

			MatrixXXi   global_vtx_map;

			MatrixXX    isDeformable;

			std::set<IndexType> distoredLargeSet;



			non_rigidRegister(*srGraph,*tgGraph,global_vtx_map,isDeformable,distoredLargeSet);



			srGraph->vertices_matrix() = srVtxMatrix; 

			srGraph->update();//back to original position after registeration

			for ( int i=0; i<trajs.size();i++ )

			{

				trajs[i].trajNode[j-start_idx] = vtx_map[j-start_idx][global_vtx_map(0,pre_idx_map[i])];

				pre_idx_map[i] = global_vtx_map(0,pre_idx_map[i]);

			}



		}

		//draw traj

		drawLifespansTraj(trajs);

	}

//sampleVtxId were oridered!
void DeformableRegistration::propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori)
{
	IndexType nCluster = -1;
	for (int i = 0; i < label_smp.size(); i++)
	{
		if(nCluster < label_smp[i])
		{
			nCluster = label_smp[i];
		}
	}
	Logger<<"The number of segments are "<<nCluster + 1<<endl;

	map<IndexType,IndexType> smpLabel;
	map<IndexType,IndexType>::iterator IsValidIter;
	for (int i = 0; i < label_smp.size(); i++)
	{
		smpLabel.insert(make_pair(sampleVtxId[i],label_smp[i]));
	}

	const IndexType k = 300;
	IndexType neighbours[k];
	ScalarType dist[k];

	IndexType vtx_num = oriPC.num_vertices();
	IndexType result_label;


	for(IndexType vtx_id = 0; vtx_id < vtx_num; vtx_id ++)
	{
	    vector<IndexType> recordLabelTime(nCluster + 1,0);
		result_label = -1;
		oriPC.neighbours(vtx_id, k, neighbours, dist);
		for(IndexType neig_id = 0; neig_id < k; neig_id ++)
		{
			IsValidIter = smpLabel.find(neighbours[neig_id]);
			if(IsValidIter != smpLabel.end())
			{
				recordLabelTime[IsValidIter->second] += 1;
			}
		}
		for (int i = 0; i <= nCluster; i++)
		{
			if(result_label < recordLabelTime[i])
			{
				label_ori[vtx_id] = i;
				result_label = recordLabelTime[i];
			}
		}

	}
	Logger<<"k-neig's k = "<<k<<endl;
	Logger<<"End Propagate!"<<endl;
}

void DeformableRegistration::smoothSampleLabel(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_smooth)
{
	IndexType nCluster = -1;
	for (int i = 0; i < label_smp.size(); i++)
	{
		if(nCluster < label_smp[i])
		{
			nCluster = label_smp[i];
		}
	}
	Logger<<"The number of segments are "<<nCluster + 1<<endl;

	map<IndexType,IndexType> smpLabel;
	map<IndexType,IndexType>::iterator IsValidIter;
	for (int i = 0; i < label_smp.size(); i++)
	{
		smpLabel.insert(make_pair(sampleVtxId[i],label_smp[i]));
	}

	const IndexType k = 300;
	IndexType neighbours[k];
	ScalarType dist[k];

    IndexType result_label;
	IndexType s_id = 0;
	for(auto vec_iter =sampleVtxId.begin(); vec_iter != sampleVtxId.end(); vec_iter++,s_id ++)
	{
		IndexType smpId = *vec_iter;

	    vector<IndexType> recordLabelTime(nCluster + 1,0);
		result_label = -1;
		oriPC.neighbours(smpId, k, neighbours, dist);
		for(IndexType neig_id = 0; neig_id < k; neig_id ++)
		{
			IsValidIter = smpLabel.find(neighbours[neig_id]);
			if(IsValidIter != smpLabel.end())
			{
				recordLabelTime[IsValidIter->second] += 1;
			}
		}
		for (int i = 0; i <= nCluster; i++)
		{
			if(result_label < recordLabelTime[i])
			{
				label_smooth[s_id] = i;
				result_label = recordLabelTime[i];
			}
		}
	}
	Logger<<"k-neig's k = "<<k<<endl;
	Logger<<"End  Sample Propagate!"<<endl;
	



}

//------------------------
void DeformableRegistration::smoothSmapleLabel_KDTree(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_smooth)
{

	Sample* downSmp = new Sample;

	for (auto iter = sampleVtxId.begin(); iter != sampleVtxId.end(); iter++)
	{
		Vertex& vtx = oriPC[*iter];

		PointType v( vtx.x(), vtx.y(), vtx.z() );
		ColorType cv(vtx.r(), vtx.g(), vtx.b(), vtx.alpha());
		NormalType nv(vtx.nx(), vtx.ny(), vtx.nz());

		downSmp->add_vertex(v,nv,cv);
	}

	downSmp->build_kdtree();


	IndexType nCluster = -1;

	for (int i = 0; i < label_smp.size(); i++)
	{
		if(nCluster < label_smp[i])
		{
			nCluster = label_smp[i];
		}
	}

	Logger<<"The number of segments are "<<nCluster + 1<<endl;


	const IndexType k = 10;

	IndexType neighbours[k];

	ScalarType dist[k];

	IndexType result_label;

	IndexType s_id = 0;
	

	for (IndexType i = 0; i < sampleVtxId.size(); i ++)
	{
		vector<IndexType> recordLabelTime(nCluster + 1,0);

		result_label = -1;

		downSmp->neighbours(i, k, neighbours, dist);

		for(IndexType neig_id = 0; neig_id < k; neig_id ++)
		{
			recordLabelTime[ label_smp [neighbours[neig_id] ] ] += 1;
		}

		for (int j = 0; j <= nCluster; j++)
		{
			if(result_label < recordLabelTime[j])
			{
				label_smooth[i] = j;

				result_label = recordLabelTime[j];
			}
		}
	}

	Logger<<"k-neig's k = "<<k<<endl;
	Logger<<"End  Sample Propagate!"<<endl;


	delete downSmp;

}


void DeformableRegistration::readSampleLabelFile(char *filename,vector<IndexType>&labels,vector<IndexType>&label_smooth,vector<IndexType>&sampleCenter,IndexType & centerFrame)
{
	FILE *in_file = fopen(filename,"r");
	if (in_file==NULL)
	{
		return;
	}

	while (true)
	{
		int frame,label,label_s,index;
		int status = fscanf(in_file,"%d %d %d %d\n",&centerFrame,&label,&label_s,&index);
		if(status==EOF)break;
		labels.push_back(label);
		label_smooth.push_back(label_s);
		sampleCenter.push_back(index);
	}
}
void DeformableRegistration::calculateNorm()
{
	Logger<<"Start calculate normal .\n";

	NormalType viewpoint(0,0,-1.);
	//NormalType viewpoint(-1,0,0);//horse

	for (IndexType frameId = 0; frameId<m_smpSet.size(); frameId++)
	{
		SampleManipulation::compute_normal(frameId,viewpoint);
	}
    Logger<<"End calculate normal.\n";
}
void DeformableRegistration::calculateNormalRotMat(NormalType& sN,PointType& tN,Matrix33& rotMat)
{
	ScalarType sL,sA,tL,tA;
	//ScalarType PI = 3.141592653;

	sL = acos(sN.z());
	tL = acos(tN.z());

	//assert(sN.x() != 0 && tN.x() != 0);
	if(sN.x() > -1e-5 && sN.x() < 1e-5 ) 
	{
		if (sN.y() > 1e-5)
		{
			sA = 0.5 *PI;
		}
		sA = 1.5*PI;
	}
	else
	{
	    sA = atan(sN.y()/sN.x());
	}


	if(tN.x() > -1e-5 && tN.x() < 1e-5 ) 
	{
		if (tN.y() > 1e-5)
		{
			tA = 0.5 *PI;
		}
		tA = 1.5*PI;
	}
	else
	{
		tA = atan(tN.y()/tN.x());
	}


	tA -= sA;
	tL -= sL;
	//rotate with A axis
	Matrix33 rotZ;

	rotZ(0,0) = cos(2*PI + tA);rotZ(0,1) =-sin(2*PI + tA);rotZ(0,2) =0; 
	rotZ(1,0) = sin(2*PI + tA);rotZ(1,1) =cos(2*PI + tA) ;rotZ(1,2) =0; 
	rotZ(2,0) = 0;             rotZ(2,1) =0 ;            rotZ(2,2) =1; 

	//Logger<<"Z axis = "<<rotZ<<endl;
	Matrix33 rotX;

	rotX(0,0) = 1;rotX(0,1) = 0;rotX(0,2) =0; 
	rotX(1,0) = 0;rotX(1,1) =cos(2*PI + tL) ;rotX(1,2) =-sin(2*PI +tL); 
	rotX(2,0) = 0;rotX(2,1) =sin(2*PI +tL) ;rotX(2,2) =cos(2*PI + tL); 

	//Logger<<"X axis = "<<rotX<<endl;
	rotMat = rotZ * rotX;
}

void DeformableRegistration::sampleModel_norm(vector<PCloudTraj>& oriTraj,vector<PCloudModel>& smpOriModel,IndexType modelNum)
{
	PCloudModel resModel;
	resModel.modeLifeSpan.start = 18;
	resModel.modeLifeSpan.end = 22;
	while (modelNum-->0)
	{

	  IndexType firstTraj = rand()%oriTraj.size();
	  PCloudTraj smpT = oriTraj[firstTraj];
	  resModel = constrcutRigidMotionMode(smpT); 
	  smpOriModel.push_back(resModel);
	}


}

PCloudModel DeformableRegistration::constrcutRigidMotionMode(PCloudTraj& traj)
{
	/*NormalType sNorm,tNorm;*/
	ScalarType Nx,Ny,Nz;
	IndexType i = 0;
	PCloudModel tempModel;
	tempModel.modeLifeSpan.start = 18;
	tempModel.modeLifeSpan.end = 22;

	for (IndexType startF = traj.trajLifeSpan.start; startF < traj.trajLifeSpan.end; startF ++,i ++)
	{
		Nx = m_smpSet[startF][traj.trajNode[i]].nx();
		Ny = m_smpSet[startF][traj.trajNode[i]].ny();
		Nz = m_smpSet[startF][traj.trajNode[i]].nz();
		NormalType sNorm(Nx,Ny,Nz);

		Nx = m_smpSet[startF + 1][traj.trajNode[i + 1]].nx();
		Ny = m_smpSet[startF + 1][traj.trajNode[i + 1]].ny();
		Nz = m_smpSet[startF + 1][traj.trajNode[i + 1]].nz();
		NormalType tNorm(Nx,Ny,Nz);

// 		if (sNorm(0,0) != tNorm(0,0) ||sNorm(1,0) != tNorm(1,0) ||sNorm(2,0) != tNorm(2,0) )
// 		{
//             Logger<<"debug.\n";
// 		}

		Matrix33 rotMat;
		calculateNormalRotMat(sNorm,tNorm,rotMat);
		tempModel.fNode.push_back(rotMat);

		calculateNormalRotMat(tNorm,sNorm,rotMat);
		tempModel.bNode.push_back(rotMat);
	}
	return tempModel;
}

void DeformableRegistration::sampleLifeSpanTraj(vector<PCloudTraj> & oriTraj,vector<PCloudTraj>& resTraj,IndexType & cFrame,ScalarType& thresH)
{
   IndexType trajSize = oriTraj.size();
   assert(trajSize > 0);
   IndexType tLen = 2;
   resTraj.clear();

   for (IndexType trajId = 0; trajId < trajSize; trajId++)
   {
       PCloudTraj tempT;
	   tempT = constructNewLifeTraj(oriTraj[trajId],cFrame,thresH);

	   if ( (tempT.trajLifeSpan.end - tempT.trajLifeSpan.start) > tLen)
	   {
		   resTraj.push_back(tempT);
	   }
   }
   
   Logger<<"traj length morn than  "<<tLen<<endl;
   drawLifespansTraj(resTraj);
}

PCloudTraj DeformableRegistration::constructNewLifeTraj(PCloudTraj & oriTraj,IndexType& cFrame,ScalarType& thresH)
{
	//start end = {3,7}
	//node      = {21, 34, 87,45,76}
	//frame     = [3   4   5  6  7]
	//index     = [0   1   2  3  4]
	//cFrame    = 5

	IndexType cNodeId = cFrame - oriTraj.trajLifeSpan.start;// =2
	IndexType cNode = oriTraj.trajNode[cNodeId];//=87
	IndexType len = oriTraj.trajLifeSpan.end - oriTraj.trajLifeSpan.start + 1;//5
	vector<bool> isDst;
	isDst.resize(len,false);

	for (IndexType nodeId = 0; nodeId < len; nodeId ++)
	{
		if ( nodeId == cNodeId)
		{
			isDst[nodeId] = true;
			continue;
		}

		IndexType tFrame = oriTraj.trajLifeSpan.start + nodeId;
		IndexType tNode = oriTraj.trajNode[nodeId];
         
		ScalarType dVal = measureDeformableCorVer(cFrame,cNode,tFrame,tNode);
		if ( dVal < thresH)
		{
			isDst[nodeId] =true;
		}
	}

	PCloudTraj resTraj;
	resTraj.trajLifeSpan.start = resTraj.trajLifeSpan.end = cFrame;

	IndexType flagS,flagE;
	flagS = flagE = cFrame;

	while (cNodeId > 0 && isDst[--cNodeId])
	{
		flagS --;
	}

	cNodeId = cFrame - oriTraj.trajLifeSpan.start;
	while (cNodeId < (len -1) && isDst[++cNodeId])
	{
		flagE ++;
	}

    // assignment nodes id to resTraj
	resTraj.trajLifeSpan.start = flagS;
	resTraj.trajLifeSpan.end   = flagE;

	if (flagE > flagS)
	{
		for (IndexType i = flagS; i <= flagE; i++)
		{
			IndexType nId = oriTraj.trajNode[i - oriTraj.trajLifeSpan.start];
			resTraj.trajNode.push_back(nId);
		}
	}

	return resTraj;
}

ScalarType DeformableRegistration::measureDeformableCorVer(IndexType sFrame,IndexType sId,IndexType tFrame,IndexType tId)
{
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);

	VecX neigDis;     //record ori distance
	VecX resNeigDis;  // record target distances 
	neigDis.setZero(m_neigNum,1);
	resNeigDis.setZero(m_neigNum,1);

	neigSrCoor.setZero();
	neigTgCoor.setZero();

	m_smpSet[sFrame].neighbours(sId,m_neigNum,neigSrGraph);
	m_smpSet[tFrame].neighbours(tId,m_neigNum,neigTgGraph);

	getConstantNeigCoorByIndex(m_smpSet[sFrame],neigSrGraph,neigSrCoor);
	getConstantNeigCoorByIndex(m_smpSet[tFrame],neigTgGraph,neigTgCoor);

	calculateNeigDis(neigSrCoor,neigDis);
	calculateNeigDis(neigTgCoor,resNeigDis);
 
 	return deformableValue(neigDis,resNeigDis);//只用邻域的边长比值
	//return deformableTotal(neigSrCoor,neigTgCoor);//全部变得比值
	//return deformableICP(neigSrCoor,neigTgCoor);//对邻域点进行局部配准,接着调整target的坐标,计算边长

}

//---------------------------
ScalarType DeformableRegistration::deformableTotal(Matrix3X& srCoor, Matrix3X& tgCoor)
{
	ScalarType distor = 0.0;
	IndexType pSize = srCoor.cols();
	IndexType itTime = 0;
	for (IndexType i = 0; i < pSize - 1; i++ )
	{
		for (IndexType j = i + 1; j < pSize; j ++)
		{
			ScalarType srDis = ( srCoor.col(i) - srCoor.col(j) ).norm();
			ScalarType tgDis = ( tgCoor.col(i) - tgCoor.col(j) ).norm();

			ScalarType denominator = srDis;
			ScalarType molecule = abs(srDis - tgDis);
			assert(denominator > 1e-7);
			distor += molecule/denominator;
			itTime ++;
		}
	}

	assert(itTime > 0);

	distor /= itTime;

	return distor;
}
//---------------------------
ScalarType DeformableRegistration::deformableICP(Matrix3X& srCoor, Matrix3X& tgCoor)
{
	Matrix3X srOriCoor = srCoor;
	SICP::Parameters pa(false,2,10,1.2,1e5,20,20,1,1e-5);
	MatrixXXi loaclCor;
	loaclCor.setZero(1,m_neigNum);
	SICP::point_w_point(srCoor,tgCoor,loaclCor,pa);

	alignTargetCoor(tgCoor,loaclCor);

	ScalarType distor = 0.0;
	IndexType pSize = srCoor.cols();
	IndexType itTime = 0;
	for (IndexType i = 0; i < pSize - 1; i++ )
	{
		for (IndexType j = i + 1; j < pSize; j ++)
		{
			ScalarType srDis = ( srOriCoor.col(i) - srOriCoor.col(j) ).norm();
			ScalarType tgDis = ( tgCoor.col(i) - tgCoor.col(j) ).norm();

			ScalarType denominator = srDis;
			ScalarType molecule = abs(srDis - tgDis);
			assert(denominator > 1e-7);
			distor += molecule/denominator;
			itTime ++;
		}
	}

	assert(itTime > 0);

	distor /= itTime;

	return distor;

}
//---------------------------
void DeformableRegistration::calculateVtxDistor(vector<PCloudTraj>& sampleTraj,vector<ScalarType>& disVal,IndexType cFrame)
{
	IndexType trajSize = sampleTraj.size();
	assert(trajSize > 0);

	disVal.resize(trajSize,0);

	for (IndexType i = 0; i< trajSize; i++)
	{

		disVal[i] = maxDistOfTraj(sampleTraj[i],cFrame);
		//disVal[i] = minDistOfTraj(sampleTraj[i],cFrame);
	}
}

ScalarType DeformableRegistration::maxDistOfTraj(PCloudTraj & traj,IndexType Cframe)
{
	IndexType start = traj.trajLifeSpan.start;
	IndexType end   = traj.trajLifeSpan.end;
	IndexType cNode = traj.trajNode[Cframe - start];

	ScalarType val_ = -1.;

	for (IndexType i = start; i <= end; i++)
	{
		if (Cframe == i) continue;
		IndexType tFrame = i;
		IndexType tNode  = traj.trajNode[i - start];
		
		ScalarType dis = measureDeformableCorVer(Cframe,cNode,tFrame,tNode);
		if (dis > val_)
		{
			val_ = dis;
		}
	}

	return val_;
}

ScalarType DeformableRegistration::minDistOfTraj(PCloudTraj & traj,IndexType Cframe)
{
	IndexType start = traj.trajLifeSpan.start;
	IndexType end   = traj.trajLifeSpan.end;
	IndexType cNode = traj.trajNode[Cframe - start];

	ScalarType val_ = 2.;

	for (IndexType i = start; i <= end; i++)
	{
		if (Cframe == i) continue;
		IndexType tFrame = i;
		IndexType tNode  = traj.trajNode[i - start];

		ScalarType dis = measureDeformableCorVer(Cframe,cNode,tFrame,tNode);
		if (dis < val_)
		{
			val_ = dis;
		}
	}

	return val_;
}

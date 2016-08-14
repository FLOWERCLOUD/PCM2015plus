#ifndef _J_LINKAGE_MATLAB_H
#define _J_LINKAGE_MATLAB_H
#include <vector>
#include <engine.h>
#include <time.h>
#ifdef WIN32
#include <direct.h>
#include "DeformaleRegistration.h"
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif

#include<fstream>  
#include <iostream>

using namespace std;

template< class  Point, class Line, class Point2LineDistance >
class J_LinkageAdapter_Matlab{

	typedef		typename Point2LineDistance::DistanceType		P2LDistanceType;
public:
	J_LinkageAdapter_Matlab( std::vector<Point>& points, std::vector<Line>& lines, std::vector<int>& labels,
		Point2LineDistance dist_func, ScalarType theshold ):point_set_(points),line_set_(lines),
		label_set_(labels),p2l_dist_func_(dist_func),theshold_(theshold){  }

	//////////////////////////////////////////////////////////////////////////
	//derive lamda value according  percentage
	void calc_lamda()
	{
		static const IndexType smp_num = 20;

		assert( theshold_<=1.0 && theshold_>0. );

		IndexType num_lines = line_set_.size();
		IndexType num_points = point_set_.size();
		IndexType step = num_points / smp_num;
		IndexType n_d =  num_lines * smp_num;
		P2LDistanceType*	d = new P2LDistanceType[ n_d ];
		IndexType k = 0;
		for ( int i=0;  i<smp_num; i++)
		{
			IndexType bias = rand()%step;
			IndexType smp_idx = i*step + bias;
			for(vector<Line>::iterator l_iter = line_set_.begin() ; l_iter!=line_set_.end(); l_iter++)
			{
				d[k++] = p2l_dist_func_(point_set_[smp_idx], *l_iter);
			}
		}
		sort(d, d+n_d-1);
		p2l_dist_func_.lamda = d[IndexType(theshold_*n_d)-1];
		Logger<<"Lambda:"<<p2l_dist_func_.lamda<<endl;
		delete [] d;
	}
	///

	void compute()
	{
		size_t pn = point_set_.size();
		size_t ln = line_set_.size();
		calc_lamda();

		P2LDistanceType *distMat = new P2LDistanceType[pn*ln];
		clock_t old_time = clock();
		size_t k=0;
		for ( size_t li=0; li<ln; li++  )
		{
			for (size_t pi=0; pi<pn; pi++)
			{
				distMat[ k++ ] = p2l_dist_func_(point_set_[pi], line_set_[li]);
			}
		}
#ifdef J_LINKAGE_DEBUG
		streambuf *backup;
		backup = cout.rdbuf(); 
		ofstream log("G:\\point_data\\hytest\\panda20160314\\pandalabelandcorr\\log.txt");
		streambuf * oldbuf =  cout.rdbuf(log.rdbuf());  
		k=0;
		cout<<"matrix size: "<<ln*pn<<endl;
		cout<<"lamda: "<<p2l_dist_func_.lamda<<endl;
		for ( size_t li=0; li<ln; li++  )
		{
			for (size_t pi=0; pi<pn; pi++)
			{
				cout << distMat[ k++ ]<<"  " ;
			}
		}
		cout <<endl;
		cout.rdbuf(backup);
		log.close();
#endif
		Logger<< "ps init elapse time: "<<(size_t)((clock()-old_time)/CLOCKS_PER_SEC)<<"s"<<std::endl;
		Logger<<"open Matlab Engine.\n";
		old_time = clock();
		Engine*	ep;
		if (! (ep = engOpen(NULL)) )
		{
			Logger<< "Can't not start Matlab engine.\n";
			return;
		}

		// set buffer to display result
		IndexType	result_buffer_size = 1024*1000;
		char*		result_buffer = new char[result_buffer_size];
		engOutputBuffer(ep, result_buffer, result_buffer_size);

		//Get the executable file's path
		char cur_path[FILENAME_MAX];
		if (!get_current_dir(cur_path, sizeof(cur_path)))
		{
			return;
		}
		cur_path[sizeof(cur_path) - 1] = '\0';
		strcat(cur_path,"\\JLinkage");
		char cd_cur_path[FILENAME_MAX + 3] = "cd ";
		strcat(cd_cur_path, cur_path);
		engEvalString(ep, cd_cur_path );

		mxArray *mx_dm = mxCreateDoubleMatrix(pn, ln, mxREAL);
		memcpy((char*)mxGetPr(mx_dm),(char*)distMat, pn*ln*sizeof(P2LDistanceType));
		Logger<<"sizeof(P2LDistanceType): "<<sizeof(P2LDistanceType)<<endl;
		Logger<<"pn: "<<pn<<"ln: "<<ln<<endl;
		Logger<<"matrix size:"<<pn*ln*sizeof(P2LDistanceType)<<endl;
		engPutVariable(ep, "totd", mx_dm);
		mxArray *mx_threshold = mxCreateDoubleMatrix(1,1,mxREAL);
		memcpy((char*)mxGetPr(mx_threshold),(char*)(&p2l_dist_func_.lamda), sizeof(P2LDistanceType));
		engPutVariable(ep, "threshold", mx_threshold);
		engEvalString(ep, "[T,Z,Y,totdbin]=clusterPoints(totd,threshold);");
#ifdef J_LINKAGE_DEBUG
		engEvalString(ep,"save('G:\\point_data\\hytest\\panda20160314\\pandalabelandcorr\\matrx.mat','totd');");
		engEvalString(ep,"save('G:\\point_data\\hytest\\panda20160314\\pandalabelandcorr\\t.mat','T');");
#endif
		Logger<<result_buffer<<std::endl;
		mxArray* mx_labels = NULL;
		mx_labels = engGetVariable( ep, "T" ); 

		if ( mx_labels == NULL )
		{

			mxDestroyArray( mx_threshold );
			mxDestroyArray(mx_dm);
			engClose(ep);
			delete [] distMat;
			delete	[] result_buffer;
			Logger << "Get Data from Matlab error. End Clustering."<<std::endl;
			return;
		}

		numeric::float64 *labels = mxGetPr(mx_labels);	
		label_set_.resize(pn);
		for ( size_t i=0; i<pn; i++ )
		{
			label_set_[i] = (int)labels[i];
		}
#ifdef J_LINKAGE_DEBUG
		backup = cout.rdbuf(); 
		ofstream log2("G:\\point_data\\hytest\\panda20160314\\pandalabelandcorr\\label.txt");
		oldbuf =  cout.rdbuf(log2.rdbuf());  

		for ( size_t i=0; i<pn; i++ )
		{
			cout<<label_set_[i]<<"  ";
		}
		cout <<endl;
		cout.rdbuf(backup);
		log2.close();
#endif
		mxDestroyArray( mx_threshold );
		mxDestroyArray(mx_dm);
		engClose(ep);
		delete [] distMat;
		delete	[] result_buffer;

		Logger<< "ps merge elapse time: "<<(size_t)((clock()-old_time)/CLOCKS_PER_SEC)<<"s"<<std::endl;
	}

protected:
	vector< Point >&							point_set_;
	vector< Line >&							line_set_;
	vector<	int	>&							label_set_;
	Point2LineDistance						p2l_dist_func_;
	ScalarType                               theshold_; 
};

#endif
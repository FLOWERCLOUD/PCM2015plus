#ifndef _BFS_CLASSIFIER_HPP
#define _BFS_CLASSIFIER_HPP
#include <Eigen/Dense>
#include <assert.h>
#include<queue>
#include<memory>
#include <vector>
#include <map>
#include "..\ICP\ICP.h"

//#define USE_RADIUS_NEAREST

template<class Scalar>
class BFSClassifier{
public:
	typedef Eigen::Matrix<Scalar,3,-1,0,3,-1> Matrix3X;
	
	/*		_c: coordinate of data
			_r: radius
	*/

#ifdef USE_RADIUS_NEAREST
	BFSClassifier( Matrix3X & _c,  Scalar _r):coord_(_c),
		label_(nullptr),kind_(0),kd_tree_(_c),radius_neighbor_(_r)
	{

		assert( coord_.cols()!=0 );
		label_ = new int[  coord_.cols() ];
	}
#else
	BFSClassifier( Matrix3X & _c,  Scalar _r, size_t _nn):radius_sq_(_r*_r),coord_(_c),
		label_(nullptr),kind_(0),kd_tree_(_c)
	{

		assert( coord_.cols()!=0 );
		neig_num_ = _nn<coord_.cols()?_nn:coord_.cols();
		label_ = new int[  coord_.cols() ];
	}
#endif

	~BFSClassifier(){

		if ( label_!=nullptr )
		{
			delete [] label_;
		}

	}

	void run()
	{

#ifndef USE_RADIUS_NEAREST
		int *neigs = new int[neig_num_];
		Scalar *neig_dist_sq = new Scalar[neig_num_];
#endif

		size_t n = coord_.cols();
		size_t n_visited = 0;
		bool *visited = new bool[ n ];
		std::fill( visited, visited+n, false); 
		std::queue<size_t> q;
		visited[0] = true;
		n_visited++;
		label_[0] = kind_;
		q.push( 0 );  //random seed

		while (true)
		{
			while ( !q.empty() )
			{
#define raw(i)		{ coord_.col(i)(0), coord_.col(i)(1), coord_.col(i)(2) } 

				size_t s = q.front();
				q.pop();
				Scalar pos[] = raw( s );

#ifdef USE_RADIUS_NEAREST
				std::vector<std::pair<int, Scalar> > indices_dist;
				kd_tree_.radiusSearch(pos, radius_neighbor_ ,indices_dist);
				for ( size_t i=0; i<indices_dist.size(); i++ )
				{
					int neig_idx = indices_dist[i].first;
					if ( !visited[ neig_idx ]  )
					{
						//not visited, and near enough to the current one
						visited[ neig_idx ] = true;
						label_[ neig_idx ] = kind_;
						n_visited++;
						q.push( neig_idx );
					}
				}
#else

				kd_tree_.query( pos, neig_num_, neigs, neig_dist_sq );
				for ( size_t i=0; i<neig_num_; i++ )
				{
					if ( !visited[ neigs[i] ] && neig_dist_sq[i]<radius_sq_ )
					{
						//not visited, and near enough to the current one
						visited[ neigs[i] ] = true;
						label_[ neigs[i] ] = kind_;
						n_visited++;
						q.push( neigs[i] );
					}
				}
#endif

#undef  raw
			}

			if ( n_visited <n )
			{
				size_t start;
				for (start=0; visited[start] ; start++);
				kind_++;
				visited[ start ] = true;
				label_[start] = kind_;
				n_visited++;
				q.push(start);
			}
			else
				break;

		}//outer loop
#ifndef USE_RADIUS_NEAREST
		delete [] neigs;
		delete [] neig_dist_sq;
#endif

	}
	
	int *get_class_label(){ return label_; }
	size_t get_num_of_class() const { return kind_+1; }

private:
	nanoflann::KDTreeAdaptor<Matrix3X, 3> kd_tree_;

	Matrix3X & coord_;
	int *label_;
	size_t kind_;

#ifdef USE_RADIUS_NEAREST
		Scalar radius_neighbor_;
#else
		size_t neig_num_;
		Scalar radius_sq_;
#endif // USE_RADIUS_NEAREST



};



#endif
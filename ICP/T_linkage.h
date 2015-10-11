#ifndef _T_LINKAGE_H
#define _T_LINKAGE_H

#include <unordered_map>
#include <set>
#include <vector>
#include <queue>
#include <algorithm>
#include <memory>

using namespace std;

template<class T>
struct sparse_vec
{
	typedef size_t Index;
	set<Index> indices_;
	unordered_map<Index, T> vals_;


	const T& operator[](const Index index) const 
	{
		if (indices_.find(index)==indices_.end())
		{
			return 0.;
		}
		return vals_[index];
	}

	T& operator[](Index index)
	{
		if (indices_.find(index)==indices_.end())
		{
			indices_.insert(index);
			vals_[index] = 0.;
		}
		return vals_[index];
	}

	T dot(  sparse_vec& rhs )
	{
		set<Index>::iterator lhs_iter,rhs_iter,lhs_end,rhs_end;
		lhs_iter = indices_.begin();
		rhs_iter = rhs.indices_.begin();
		lhs_end = indices_.end();
		rhs_end = rhs.indices_.end();

		T sum = 0.;

		while (lhs_iter!=lhs_end && rhs_iter!=rhs_end)
		{
			if ( *lhs_iter == *rhs_iter )
			{
				sum += vals_[*lhs_iter] * rhs.vals_[*rhs_iter];
				lhs_iter++;
				rhs_iter++;
			}
			else
			{
				*lhs_iter < *rhs_iter? lhs_iter++ : rhs_iter++;
			}

		}

		return sum;

	}

	T sq_norm()
	{
		T s = 0.;
		for ( unordered_map<Index,T>::iterator iter = vals_.begin();
			iter != vals_.end(); iter++)
		{
			s += (*iter).second * (*iter).second;
		}
		return s;
	}

	void min(sparse_vec&  a, sparse_vec& b)
	{
		set<Index>::iterator lhs_iter,rhs_iter,lhs_end,rhs_end;
		lhs_iter = a.indices_.begin();
		rhs_iter = b.indices_.begin();
		lhs_end = a.indices_.end();
		rhs_end = b.indices_.end();


		while (lhs_iter!=lhs_end && rhs_iter!=rhs_end)
		{
			if ( *lhs_iter == *rhs_iter )
			{
				indices_.insert(*lhs_iter);
				T s1 = a.vals_[*lhs_iter];
				T s2 = b.vals_[*rhs_iter];
				vals_[*lhs_iter] = s1<s2?s1:s2;
				++lhs_iter;++rhs_iter;
			}
			else
			{
				*lhs_iter < *rhs_iter? lhs_iter++ : rhs_iter++;
			}

		}

	}

};

template< class  Point, class Line, class Point2LineDistance >
class T_LinkageAdapter{

protected:
	typedef		size_t		IndexType;
	typedef		double		PSDistanceType;
	typedef		typename Point2LineDistance::DistanceType		P2LDistanceType;

	struct PF
	{
		sparse_vec<P2LDistanceType>		phi;
		set<IndexType>		which_points;
		bool				is_valid;
		int					node_idx;
	};

	struct heap_node
	{
		heap_node(shared_ptr<PF>& pp1, shared_ptr<PF>& pp2 ):pf1(pp1),pf2(pp2){}

		weak_ptr<PF>		pf1;
		weak_ptr<PF>		pf2;
		PSDistanceType	dist;
	};

	struct greater_than{
		bool operator()(const heap_node& lhs, const heap_node& rhs )
		{
			return lhs.dist > rhs.dist;
		}

	};

public:
	T_LinkageAdapter( std::vector<Point>& points, std::vector<Line>& lines, std::vector<int>& labels,
		Point2LineDistance dist_func  ):point_set_(points),line_set_(lines),
		label_set_(labels),p2l_dist_func_(dist_func){  }
	~T_LinkageAdapter(){}

	void compute()
	{
		pf_init();

		/** PS Merge Procedure **/

		//Construct min-heap
		std::priority_queue< heap_node, std::vector<heap_node>,greater_than >	min_heap;
		IndexType		pn= pf_set_.size();
		for ( IndexType i=0; i < pn; i++ )
		{
			for ( IndexType j = i+1; j <pn; j++ )
			{
				heap_node	node( pf_set_[i], pf_set_[j] );
				node.dist = pf_distance( *(pf_set_[i]), *(pf_set_[j]) );
				if (node.dist==1.)
				{
					continue;
				}
				min_heap.push( node );
			}
		}

		//Begin merge
		while ( !min_heap.empty() )
		{
			heap_node min_node = min_heap.top();
			if(min_node.dist==1.)break;
			min_heap.pop();
			if (min_node.pf1.expired() || min_node.pf2.expired() )
			{
				continue;
			}

			shared_ptr<PF> pf1 = min_node.pf1.lock();
			shared_ptr<PF> pf2 = min_node.pf2.lock();


			pf_merge( *pf1, *pf2 );


			vector< shared_ptr<PF> >::iterator new_ps_iter = pf_set_.end() - 1;
			for (vector< shared_ptr<PF> >::iterator iter = pf_set_.begin();
				iter != new_ps_iter; )
			{

				if (  (*iter)->is_valid ==false )
				{
					iter = pf_set_.erase( iter );
					new_ps_iter = pf_set_.end() - 1;
				}
				else
				{
					heap_node	node( *iter, *new_ps_iter );
					node.dist = pf_distance( *(*iter), *(*new_ps_iter) );
					if (node.dist==1.)
					{
						++iter;
						continue;
					}
					min_heap.push( node );
					++iter;
				}
			}

		}

		//Store the final label
		IndexType i=0;
		for (vector< shared_ptr<PF> >::iterator pf_iter = pf_set_.begin();
			pf_iter != pf_set_.end();
			++pf_iter,++i)
		{
			for ( set<IndexType>::iterator p_idx_iter = (*pf_iter)->which_points.begin();
				p_idx_iter != (*pf_iter)->which_points.end();
				++p_idx_iter)
			{
				label_set_[ *p_idx_iter ] = i;
			}
		}


	}

protected:

	//Init PS set
	inline void pf_init()
	{
		pf_set_.clear();
		int point_idx =0 ;
		for (vector<Point>::iterator p_iter = point_set_.begin(); p_iter!=point_set_.end(); p_iter++, point_idx++ )
		{


			pf_set_.push_back( shared_ptr<PF>(new PF) );
			PF& pf = *( pf_set_.back() );
			int line_idx = 0;
			P2LDistanceType inv_tau = p2l_dist_func_.lamda;
			for(vector<Line>::iterator l_iter = line_set_.begin() ; l_iter!=line_set_.end(); l_iter++,line_idx++)
			{
				P2LDistanceType p2l_dist = p2l_dist_func_(*p_iter, *l_iter);
				if(  p2l_dist < 5*p2l_dist_func_.lamda)
					pf.phi[ line_idx ] = exp( -p2l_dist*inv_tau );
			}
			pf.is_valid = true;
			pf.which_points.insert(point_idx);

			if ( point_idx==0 )
			{
				Logger<<"pf 0 size:"<<pf_set_[0]->phi.indices_.size()<<std::endl;
			}
		}

		
	}

	//Compute Distance between PF
	PSDistanceType	pf_distance( PF& p,  PF& q )
	{
		if(p.phi.indices_.empty()&&q.phi.indices_.empty())
			return 1.;
		P2LDistanceType pq_dot = p.phi.dot( q.phi );
		return 1. - pq_dot / ( p.phi.sq_norm() + q.phi.sq_norm() - pq_dot );
	}

	//Merge two PF
	void pf_merge( PF& pf1, PF& pf2  )
	{

		pf_set_.push_back( shared_ptr<PF>(new PF) );
		PF& pf3 = *( pf_set_.back() );

		//merge phi
		pf3.phi.min(pf1.phi, pf2.phi);

		//merge point set
		set<IndexType>::iterator first1,last1,first2, last2;
		first1  = pf1.which_points.begin();
		last1 = pf1.which_points.end();
		first2 = pf2.which_points.begin();
		last2 = pf2.which_points.end();

		while (first1!=last1)
		{
			pf3.which_points.insert( *first1 );
			++first1;
		}

		while (first2 != last2 )
		{
			pf3.which_points.insert( *first2 );
			++first2;
		}

		pf3.is_valid = true;
		pf1.is_valid = false;
		pf2.is_valid = false;



	}



protected:
	vector< Point >&							point_set_;
	vector< Line >&							line_set_;
	vector<	int	>&							label_set_;
	vector<shared_ptr<PF>>			pf_set_;
	Point2LineDistance						p2l_dist_func_;

};

#endif
#ifndef _OCTREE_H
#define _OCTREE_H
#include "basic_types.h"
#include <vector>
#include <map>
#include <unordered_set>
using namespace std;



template<class Scalar, class Index>
class Octree{


public:

	struct oct_box
	{
		Scalar near_corner[3];
		Scalar far_corner[3];
	};

	struct octree_node
	{
		unordered_set<Index> tag_in_domain_; //only not empty in leaves
		int level_;
		octree_node* children_[8];
		octree_node( int level ):level_(level)
		{ 
			memset(children_,0,8*sizeof(octree_node*));
		}
	};


	Octree():root_(NULL){  }
	void init_octree( int depth, Scalar near_corner[], Scalar far_corner[]  )
	{
		for ( int i=0; i<3; ++i )
		{
			box_.near_corner[i] = near_corner[i];
			box_.far_corner[i] = far_corner[i];
		}
		depth_ = depth;
		root_ = new octree_node(1);
		return;
	}

	void insert( Scalar pos[], Index tag )
	{
		insert( root_, pos, tag, box_.near_corner, box_.far_corner );
	}


public:
	void release()
	{
		release( root_ );
	}


private:

	void insert( octree_node* cur_node, Scalar pos[], Index tag, Scalar near_corner[], Scalar far_corner[] )
	{
		if ( cur_node==NULL )return;
		if ( cur_node->level_ == depth_ )
		{
			//leaf node
			cur_node->tag_in_domain_.insert( tag );
			leaf_.insert( cur_node );
			return;
		}
		//determine the point belong to which child
		Scalar new_near[3];
		Scalar new_far[3];
		int idx = 0;
		for ( int i=0, mask=1;i<3;++i, mask=mask<<1 )
		{
			Scalar mid = (near_corner[i]+far_corner[i])/2;
			if ( pos[i]>mid )
			{
				idx |= mask;
				new_near[i] = mid;
				new_far[i] = far_corner[i];
			}
			else
			{
				new_near[i] = near_corner[i];
				new_far[i] = mid;
			}
		}
		if ( cur_node->children_[idx]==NULL )
		{
			cur_node->children_[idx] = new octree_node(cur_node->level_+1);
		}
		insert( cur_node->children_[idx], pos, tag, new_near, new_far );

	}

	void release(  octree_node* node )
	{
		if(node==NULL)return;
		for ( int i=0; i<8; ++i )
		{
			if ( node->children_[i]!=NULL )
			{
				release( node->children_[i] );
			}
		}
		delete node;
	}

public:
	vector<vector<Index> >	return_all_domain_tag()
	{
		vector<vector<Index> > tmp_res(leaf_.size());
		int i=0;
		for ( unordered_set<octree_node*>::iterator iter = leaf_.begin(); iter != leaf_.end(); ++iter,++i )
		{
			for ( unordered_set<Index>::iterator tag_iter = (*iter)->tag_in_domain_.begin(); 
				tag_iter!=(*iter)->tag_in_domain_.end();++tag_iter )
			{
				tmp_res[i].push_back(*tag_iter);
			}
		}
		return tmp_res;

	}

private:
	IndexType depth_;
	octree_node*	root_;
	unordered_set<octree_node*> leaf_;
	oct_box box_;
};

#endif
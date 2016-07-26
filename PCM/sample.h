#ifndef _EXAMPLE_H
#define _EXAMPLE_H
#include "selectable_item.h"

#include "pool_allocator.h"
#include "box.h"
#include "..\ICP\ICP.h"
#include "Eigen\Dense"
#include "basic_types.h"
#include <QMutex>

#include <set>
class Vertex;
class TriangleType;
class LinkNode;
namespace ColorMode{
	struct ObjectColorMode;
	struct VertexColorMode;
	struct LabelColorMode;
	struct WrapBoxColorMode; //added by huayun
	struct EdgePointColorMode;  //added by huayun
	struct SphereMode;
}
namespace RenderMode
{
	enum WhichColorMode;
	enum RenderType;
}
class Sample:public SelectableItem
{
public:
	Sample();
	~Sample();
	void clear();

	inline Vertex& operator[]( IndexType i ) const{ return *vertices_[i]; }
	

	Vertex* add_vertex( const PointType& pos,const NormalType& n,
		const ColorType& c);
	TriangleType* add_triangle(const TriangleType& tt);
	void delete_vertex_group( const std::vector<IndexType>& idx_grp );
	void set_vertex_label(const std::vector<IndexType>& idx_grp ,IndexType label);

	void draw(ColorMode::ObjectColorMode&, const Vec3& bias = Vec3(0.,0.,0.));
	void draw(ColorMode::VertexColorMode&,const Vec3& bias = Vec3(0.,0.,0.));
	void draw(ColorMode::LabelColorMode&,const Vec3& bias = Vec3(0.,0.,0.));
	void draw(ColorMode::WrapBoxColorMode&, const Vec3& bias = Vec3(0.,0.,0.)); // add by huayun
	void draw(ColorMode::EdgePointColorMode&, const Vec3& bias = Vec3(0.,0.,0.)); // add by huayun
	void draw(ColorMode::SphereMode&,const Vec3& bias  = Vec3(0.,0.,0.));
	void draw( RenderMode::WhichColorMode& wcm ,RenderMode::RenderType& r ,const Vec3& bias  = Vec3(0.,0.,0.));
	//vector< map<IndexType,Vertex*> >				lb_wrapbox_;
	//vector< set<LinkNode> >				wrap_box_link_;
	//void addWrapBox( std::map<IndexType,Vertex*>  _l){ lb_wrapbox_.push_back(_l);}
	//void addWrapBoxLink( std::set<LinkNode> _l){ wrap_box_link_ .push_back(_l);}
	//void getWrapBox(IndexType _depth ,  std::map<IndexType,Vertex*>& out_)
	//{
	//	if(  (_depth > -1) && ((_depth < lb_wrapbox_.size())&& (lb_wrapbox_.size()>0)) ){
	//		out_ = lb_wrapbox_[_depth];
	//	}
	//}
	//void getWrapBoxLink( IndexType _depth ,  std::set<LinkNode>& out_){
	//	if(  (_depth > -1) && ((_depth < wrap_box_link_.size())&& (wrap_box_link_.size()>0)) ){
	//		out_ = wrap_box_link_[_depth];
	//	}
	//}
	
	void draw_with_name();

	size_t num_vertices() const { return vertices_.size(); }

	typedef std::vector<Vertex*>::iterator	vtx_iterator;

	inline vtx_iterator begin(){ return vertices_.begin(); }
	inline vtx_iterator end(){ return vertices_.end(); }

	//Every time vertex change, the kdtree should rebuild
	void	build_kdtree();

	IndexType closest_vtx( const PointType& query_point ) const;
	bool		neighbours(const IndexType query_point_idx, const IndexType num_closet, IndexType* out_indices);
	/* 
		Get matrix for transforming world-sample space to 
		view-sample space , making sure all samples can be saw
		no matter what original coordinates it is
	*/
	inline Matrix44 matrix_to_scene_coord(  );

	/* Green channel to get all vertex position information */
	inline  Matrix3X&	vertices_matrix()
	{	
		if (kd_tree_should_rebuild_)
		{
			build_kdtree();
		}
		return vtx_matrix_; 
	}
	/*Update vertex position according vertex matrix*/
	void	update();

	inline void lock(){ mutex_.lock(); }
	inline void unlock(){ mutex_.unlock(); }

	bool Sample::neighbours(const IndexType query_point_idx, const IndexType num_closet,
		IndexType* out_indices,ScalarType* out_distances);


	const PointType box_center() const{ return box_.center(); }
	const ScalarType	box_diag() { return box_.diag(); }
	const PointType box_near_corner(){ return box_.low_corner(); }
	const PointType box_far_corner(){ return box_.high_corner(); }
	
	Box getBox(){return box_;}

// public:
// 	QMutex										mutex_;
public:
	IndexType smpId;  //added by huayun
	IndexType n_vertex;
	IndexType n_normal;
	IndexType n_triangle;
	std::vector<Vertex*>	vertices_;
	std::vector<TriangleType*>  triangle_array;
private:




	PoolAllocator			allocator_;
	Box						box_;
	//added by huayun
	
	 
	Matrix3X									vtx_matrix_; // 3 * NumOfVtx
	//Attention: kdtree is just a adapter, it means it use the reference of its data source
	nanoflann::KDTreeAdaptor<Matrix3X, 3>*		kd_tree_;
	bool										kd_tree_should_rebuild_;
	QMutex										mutex_;
public:
	std::vector< std::map<IndexType,Vertex*>>				lb_wrapbox_;

	std::vector< std::vector<LinkNode> >							wrap_box_link_;
	IndexType				clayerDepth_;
};

class LinkNode
{
public:
	IndexType	labelH_;
	IndexType	labelL_;	
	PointType	pointH_;
	PointType	pointL_;
	LinkNode( IndexType	_labelH ,IndexType	_labelL, PointType		_pointH, PointType	_pointL):
		labelH_(_labelH) ,labelL_(_labelL) ,pointH_(_pointH),pointL_(_pointL){}
	//Vertex*		vtxH_;
	//Vertex*		vtxL_;

	//LinkNode( IndexType	_labelH ,IndexType	_labelL, Vertex*		_vtexH, Vertex*		_vtexL):
	//	labelH_(_labelH) ,labelL_(_labelL) ,vtxH_(_vtexH),vtxL_(_vtexL)
	//{

	//}
	//~LinkNode(){
	//	if(vtxH_)delete	vtxH_;
	//	if(vtxL_)delete	vtxL_;

	//}
};

#endif
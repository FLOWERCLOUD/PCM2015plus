#ifndef __BASEIMPORTER_HH__
#define __BASEIMPORTER_HH__
#include "basic_types.h"
typedef int VertexHandle;
typedef int FaceHandle;
typedef int HalfedgeHandle;
typedef int EdgeHandle;
class Sample;
namespace PCM{
	namespace IO{
		class BaseImporter
		{
		public:
			// base class needs virtual destructor
			virtual ~BaseImporter() {}

			// add a vertex with coordinate \c _point
			virtual VertexHandle add_vertex(const PointType& _point) = 0;

			// add a face with indices _indices refering to vertices
			typedef std::vector<VertexHandle> VHandles;
			virtual FaceHandle add_face(const VHandles& _indices) = 0;

			// add texture coordinates per face, _vh references the first texcoord
			//virtual void add_face_texcoords( FaceHandle _fh, VertexHandle _vh, const std::vector<Vec2f>& _face_texcoords) = 0;

			// Set the texture index for a face
			virtual void set_face_texindex( FaceHandle _fh, int _texId ) = 0;

			// set vertex normal
			virtual void set_normal(VertexHandle _vh, const NormalType& _normal) = 0;


			// set vertex color
			virtual void set_color(VertexHandle _vh, const ColorType& _color) = 0;

			//// set edge color
			//virtual void set_color(EdgeHandle _eh, const ColorType& _color) = 0;

			//// set face normal
			//virtual void set_normal(FaceHandle _fh, const NormalType& _normal) = 0;



			//// set face color
			//virtual void set_color(FaceHandle _fh, const ColorType& _color) = 0;

			// Store a property in the mesh mapping from an int to a texture file
			// Use set_face_texindex to set the index for each face
			virtual void add_texture_information( int _id , std::string _name ) = 0;

			// get reference to base kernel
			//virtual BaseKernel* kernel() { return 0; }

			virtual bool is_triangle_mesh()     const { return false; }

			// reserve mem for elements
			virtual void reserve( unsigned int /* nV */,
				unsigned int /* nE */,
				unsigned int /* nF */) {}

			// query number of faces, vertices, normals, texcoords
			virtual size_t n_vertices()   const = 0;
			virtual size_t n_faces()      const = 0;
			virtual size_t n_edges()      const = 0;


			// pre-processing
			virtual void prepare()  {}

			// post-processing
			virtual void finish()  {}

		};

		template <class Mesh>
		class ImporterT: public BaseImporter{

		public:
			typedef PointType       Point;
			typedef NormalType      Point;
			typedef ColorType		Color;
			/*	typedef typename Mesh::Point       Point;
			typedef typename Mesh::Normal      Normal;
			typedef typename Mesh::Color       Color;
			typedef typename Mesh::TexCoord2D  TexCoord2D;*/
			typedef std::vector<VertexHandle>  VHandles;
			ImporterT(Mesh& _mesh) : mesh_(_mesh) {}

			virtual VertexHandle add_vertex(const PointType& _point)
			{
				Vertex* vtx =mesh_.add_vertex( _point);
				return   vtx->
			}


			virtual FaceHandle add_face(const VHandles& _indices)
			{
				//FaceHandle fh;

				//if (_indices.size() > 2)
				//{
				//	VHandles::const_iterator it, it2, end(_indices.end());


				//	// test for valid vertex indices
				//	for (it=_indices.begin(); it!=end; ++it)
				//		if (! mesh_.is_valid_handle(*it))
				//		{
				//			omerr() << "ImporterT: Face contains invalid vertex index\n";
				//			return fh;
				//		}


				//		// don't allow double vertices
				//		for (it=_indices.begin(); it!=end; ++it)
				//			for (it2=it+1; it2!=end; ++it2)
				//				if (*it == *it2)
				//				{
				//					omerr() << "ImporterT: Face has equal vertices\n";
				//					failed_faces_.push_back(_indices);
				//					return fh;
				//				}


				//				// try to add face
				//				fh = mesh_.add_face(_indices);
				//				if (!fh.is_valid())
				//				{
				//					failed_faces_.push_back(_indices);
				//					return fh;
				//				}
				//}

				//return fh;
			}

			// vertex attributes

			virtual void set_normal(VertexHandle _vh, const NormalType& _normal)
			{
				if (mesh_.has_vertex_normals())
					mesh_.set_normal(_vh, _normal);
			}

			virtual void set_color(VertexHandle _vh, const ColorType& _color)
			{
				if (mesh_.has_vertex_colors())
					mesh_.set_color(_vh,_color);
			}
	


			virtual void set_texcoord(VertexHandle _vh, const PointType& _texcoord)
			{
				/*if (mesh_.has_vertex_texcoords2D())
					mesh_.set_texcoord2D(_vh, vector_cast<TexCoord2D>(_texcoord));*/
			}

			//virtual void set_texcoord(HalfedgeHandle _heh, const PointType& _texcoord)
			//{
			//	/*if (mesh_.has_halfedge_texcoords2D())
			//		mesh_.set_texcoord2D(_heh, vector_cast<TexCoord2D>(_texcoord));*/
			//}

			//// edge attributes

			//virtual void set_color(EdgeHandle _eh, const ColorType& _color)
			//{
			//	/*if (mesh_.has_edge_colors())
			//		mesh_.set_color(_eh, color_cast<Color>(_color));*/
			//}


			//// face attributes

			//virtual void set_normal(FaceHandle _fh, const NormalType& _normal)
			//{
			//	if (mesh_.has_face_normals())
			//		mesh_.set_normal(_fh, _normal);
			//}

			//virtual void set_color(FaceHandle _fh, const ColorType& _color)
			//{
			//	if (mesh_.has_face_colors())
			//		mesh_.set_color(_fh, _color);
			//}

			//virtual void set_color(FaceHandle _fh, const ColorType& _color)
			//{
			//	if (mesh_.has_face_colors())
			//		mesh_.set_color(_fh, _color);
			//}

			virtual void add_face_texcoords( FaceHandle _fh, VertexHandle _vh, const std::vector<PointType>& _face_texcoords)
			{
				//// get first halfedge handle
				//HalfedgeHandle cur_heh   = mesh_.halfedge_handle(_fh);
				//HalfedgeHandle end_heh   = mesh_.prev_halfedge_handle(cur_heh);

				//// find start heh
				//while( mesh_.to_vertex_handle(cur_heh) != _vh && cur_heh != end_heh )
				//	cur_heh = mesh_.next_halfedge_handle( cur_heh);

				//for(unsigned int i=0; i<_face_texcoords.size(); ++i)
				//{
				//	set_texcoord( cur_heh, _face_texcoords[i]);
				//	cur_heh = mesh_.next_halfedge_handle( cur_heh);
				//}
			}

			virtual void set_face_texindex( FaceHandle _fh, int _texId ) {
	/*			if ( mesh_.has_face_texture_index() ) {
					mesh_.set_texture_index(_fh , _texId);
				}*/
			}

			virtual void add_texture_information( int _id , std::string _name ) {
	/*			OpenMesh::MPropHandleT< std::map< int, std::string > > property;

				if ( !mesh_.get_property_handle(property,"TextureMapping") ) {
					mesh_.add_property(property,"TextureMapping");
				}

				if ( mesh_.property(property).find( _id ) == mesh_.property(property).end() )
					mesh_.property(property)[_id] = _name;*/
			}

			// low-level access to mesh

			virtual Sample* kernel() { return &mesh_; }

			bool is_triangle_mesh() const
			{ 
				return false; //Mesh::is_triangles(); 
			}

			void reserve(unsigned int nV, unsigned int nE, unsigned int nF)
			{
				mesh_.reserve(nV, nE, nF);
			}

			// query number of faces, vertices, normals, texcoords
			size_t n_vertices()  const { return mesh_.n_vertices(); }
			size_t n_faces()     const { return mesh_.n_faces(); }
			size_t n_edges()     const { return mesh_.n_edges(); }


			void prepare() { /*failed_faces_.clear();*/ }


			void finish()
			{
				//if (!failed_faces_.empty())
				//{
				//	omerr() << failed_faces_.size()
				//		<< " faces failed, adding them as isolated faces\n";

				//	for (unsigned int i=0; i<failed_faces_.size(); ++i)
				//	{
				//		VHandles&  vhandles = failed_faces_[i];

				//		// double vertices
				//		for (unsigned int j=0; j<vhandles.size(); ++j)
				//		{
				//			Point p = mesh_.point(vhandles[j]);
				//			vhandles[j] = mesh_.add_vertex(p);
				//			// DO STORE p, reference may not work since vertex array
				//			// may be relocated after adding a new vertex !

				//			// Mark vertices of failed face as non-manifold
				//			if (mesh_.has_vertex_status()) {
				//				mesh_.status(vhandles[j]).set_fixed_nonmanifold(true);
				//			}
				//		}

				//		// add face
				//		FaceHandle fh = mesh_.add_face(vhandles);

				//		// Mark failed face as non-manifold
				//		if (mesh_.has_face_status())
				//			mesh_.status(fh).set_fixed_nonmanifold(true);

				//		// Mark edges of failed face as non-two-manifold
				//		if (mesh_.has_edge_status()) {
				//			typename Mesh::FaceEdgeIter fe_it = mesh_.fe_iter(fh);
				//			for(; fe_it; ++fe_it) {
				//				mesh_.status(fe_it).set_fixed_nonmanifold(true);
				//			}
				//		}
				//	}

				//	failed_faces_.clear();
				//}
			}

		public:

			Mesh& mesh_;
		};

		typedef int Options ;

	}


}
#endif
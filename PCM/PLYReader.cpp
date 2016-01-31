#include "PLYReader.h"
#include "file_io.h"
#include "basic_types.h"
#include "sample.h"
#include "Importer.h"
//STL
#include <fstream>

namespace PCM{
	namespace IO {
//=== INSTANCIATE =============================================================
		_PLYReader_ __PLYReaderInstance;
		_PLYReader_& PLYReader() {
			return __PLYReaderInstance;
		}

//=== IMPLEMENTATION ==========================================================
		_PLYReader_::_PLYReader_() {
			FileIO::register_module(this);		
		}

		//naive check
		bool _PLYReader_::read(const std::string& _filename, BaseImporter& _bi, Options& _opt) 
		{
			std::fstream in(_filename.c_str(), ( _opt/*options_.is_binary()*/ ? std::ios_base::binary | std::ios_base::in
				: std::ios_base::in));

			if (!in.is_open() || !in.good()) {
				//omerr() << "[PLYReader] : cannot not open file " << _filename << std::endl;
				return false;
			}

			bool result = read(in, _bi, _opt);

			in.close();
			return result;			
		}
	
		bool _PLYReader_::read(std::istream& _in, BaseImporter& _bi, Options& _opt)
		{
			if (!_in.good()) {
			//	omerr() << "[PLYReader] : cannot not use stream" << std::endl;
				return false;
			}

			//// filter relevant options for reading
			//bool swap = _opt.check(Options::Swap);

			//userOptions_ = _opt;

			//// build options to be returned
			//_opt.clear();

			//if (options_.vertex_has_normal() && userOptions_.vertex_has_normal()) {
			//	_opt += Options::VertexNormal;
			//}
			//if (options_.vertex_has_texcoord() && userOptions_.vertex_has_texcoord()) {
			//	_opt += Options::VertexTexCoord;
			//}
			//if (options_.vertex_has_color() && userOptions_.vertex_has_color()) {
			//	_opt += Options::VertexColor;
			//}
			//if (options_.face_has_color() && userOptions_.face_has_color()) {
			//	_opt += Options::FaceColor;
			//}
			//if (options_.is_binary()) {
			//	_opt += Options::Binary;
			//}

			//    //force user-choice for the alpha value when reading binary
			//    if ( options_.is_binary() && userOptions_.color_has_alpha() )
			//      options_ += Options::ColorAlpha;
			bool swap = false;
			return (  0/*options_.is_binary()*/ ? read_binary(_in, _bi, swap) : read_ascii(_in, _bi));
			return true;
		}
		bool _PLYReader_::can_u_read(const std::string& _filename) const
		{
			std::ifstream ifs(_filename.c_str());
			if (ifs.is_open() && can_u_read(ifs)) {
				ifs.close();
				return true;
			}
			return true;
		}
		//naive check
		bool _PLYReader_::can_u_read(std::istream& _is) const
		{
			// read 1st line
			std::string line;
			std::getline(_is, line);
			//Check if this file is really a ply format
			if (line != "PLY" && line != "ply")
				return false;
			return true;

		}

		bool _PLYReader_::read_ascii(std::istream& _in, BaseImporter& _bi) const
		{
			//omlog() << "[PLYReader] : read ascii file\n";

			// Reparse the header
			if (!can_u_read(_in)) {
			//	omerr() << "[PLYReader] : Unable to parse header\n";
				return false;
			}

			unsigned int i, j, k, l, idx;
			unsigned int nV;
			PointType v;//OpenMesh::Vec3f v;
			std::string trash;
			PointType t;//OpenMesh::Vec2f t;
			ColorType c;//OpenMesh::Vec4i c;
			float tmp;
			BaseImporter::VHandles vhandles;
			int vh;//VertexHandle vh;

			_bi.reserve(vertexCount_, 3* vertexCount_ , faceCount_);

		/*	if (vertexDimension_ != 3) {
				omerr() << "[PLYReader] : Only vertex dimension 3 is supported." << std::endl;
				return false;
			}*/

			// read vertices:
			for (i = 0; i < vertexCount_ && !_in.eof(); ++i) {
				v[0] = 0.0;
				v[1] = 0.0;
				v[2] = 0.0;

				t[0] = 0.0;
				t[1] = 0.0;

				c[0] = 0;
				c[1] = 0;
				c[2] = 0;
				c[3] = 255;

				for (uint propertyIndex = 0; propertyIndex < vertexPropertyCount_; ++propertyIndex) {
					switch (vertexPropertyMap_[propertyIndex].first) {
					case XCOORD:
						_in >> v[0];
						break;
					case YCOORD:
						_in >> v[1];
						break;
					case ZCOORD:
						_in >> v[2];
						break;
					case TEXX:
						_in >> t[0];
						break;
					case TEXY:
						_in >> t[1];
						break;
					case COLORRED:
						if (vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT32 ||
							vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT) {
								_in >> tmp;
								c[0] = (ScalarType)(tmp * 255.0f);
						} else
							_in >> c[0];
						break;
					case COLORGREEN:
						if (vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT32 ||
							vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT) {
								_in >> tmp;
								c[1] = (ScalarType)(tmp * 255.0f);
						} else
							_in >> c[1];
						break;
					case COLORBLUE:
						if (vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT32 ||
							vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT) {
								_in >> tmp;
								c[2] = (ScalarType)(tmp * 255.0f);
						} else
							_in >> c[2];
						break;
					case COLORALPHA:
						if (vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT32 ||
							vertexPropertyMap_[propertyIndex].second == ValueTypeFLOAT) {
								_in >> tmp;
								c[3] = (ScalarType)(tmp * 255.0f);
						} else
							_in >> c[3];
						break;
					default:
						_in >> trash;
						break;
					}
				}

				vh = _bi.add_vertex(v);
				//_bi.set_texcoord(vh, t);
				_bi.set_color(vh, ColorType(c));
			}

			// faces
			// #N <v1> <v2> .. <v(n-1)> [color spec]
			for (i = 0; i < faceCount_; ++i) {
				// nV = number of Vertices for current face
				_in >> nV;

				if (nV == 3) {
					vhandles.resize(3);
					_in >> j;
					_in >> k;
					_in >> l;

					vhandles[0] = VertexHandle(j);
					vhandles[1] = VertexHandle(k);
					vhandles[2] = VertexHandle(l);
				} else {
					vhandles.clear();
					for (j = 0; j < nV; ++j) {
						_in >> idx;
						vhandles.push_back(VertexHandle(idx));
					}
				}

//				FaceHandle fh = _bi.add_face(vhandles);

			}

			// File was successfully parsed.
			return true;
		}

		bool _PLYReader_::read_binary(std::istream& _in, BaseImporter& _bi, bool swap) const
		{
			return true;
		}

		float _PLYReader_::readToFloatValue(ValueType _type , std::fstream& _in) const
		{
			return 0.0;
		}

		void _PLYReader_::readValue(ValueType _type , std::istream& _in, float& _value) const
		{
			
		}

		void _PLYReader_::readValue(ValueType _type, std::istream& _in, double& _value) const
		{
			
		}

		void _PLYReader_::readValue(ValueType _type , std::istream& _in, unsigned int& _value) const
		{
			
		}

		void _PLYReader_::readValue(ValueType _type , std::istream& _in, int& _value) const
		{
			
		}

		void _PLYReader_::readInteger(ValueType _type, std::istream& _in, int& _value) const
		{
			
		}

		void _PLYReader_::readInteger(ValueType _type, std::istream& _in, unsigned int& _value) const
		{
			
		}

	}
}



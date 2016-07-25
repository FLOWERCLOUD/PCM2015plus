#ifndef _PLYREADER_H_
#define _PLYREADER_H_

//=== INCLUDES ================================================================


#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>

#include "BASEReader.h"

#ifndef WIN32
#include <string.h>
#endif

//== NAMESPACES ===============================================================


namespace PCM {
namespace IO {


//== FORWARDS =================================================================


class BaseImporter;


//== IMPLEMENTATION ===========================================================


/**
    Implementation of the PLY format reader. This class is singleton'ed by
    SingletonT to OFFReader.

*/

class _PLYReader_ : public BaseReader
{
public:

  _PLYReader_();

  std::string get_description() const { return "PLY polygon file format"; }
  std::string get_extensions()  const { return "ply"; }
  std::string get_magic()       const { return "PLY"; }

  bool read(const std::string& _filename,
  BaseImporter& _bi,
  Options& _opt);

  bool read(std::istream& _is,
  BaseImporter& _bi,
  Options& _opt);

  bool can_u_read(const std::string& _filename) const;

  enum ValueType {
    Unsupported,
    ValueTypeINT8, ValueTypeCHAR,
    ValueTypeUINT8, ValueTypeUCHAR,
    ValueTypeINT16, ValueTypeSHORT,
    ValueTypeUINT16, ValueTypeUSHORT,
    ValueTypeINT32, ValueTypeINT,
    ValueTypeUINT32, ValueTypeUINT,
    ValueTypeFLOAT32, ValueTypeFLOAT,
    ValueTypeFLOAT64, ValueTypeDOUBLE,
  };

private:

  bool can_u_read(std::istream& _is) const;

  bool read_ascii(std::istream& _in, BaseImporter& _bi) const;
  bool read_binary(std::istream& _in, BaseImporter& _bi, bool swap) const;

  float readToFloatValue(ValueType _type , std::fstream& _in) const;

  void readValue(ValueType _type , std::istream& _in, float& _value) const;
  void readValue(ValueType _type, std::istream& _in, double& _value) const;
  void readValue(ValueType _type , std::istream& _in, unsigned int& _value) const;
  void readValue(ValueType _type , std::istream& _in, int& _value) const;

  void readInteger(ValueType _type, std::istream& _in, int& _value) const;
  void readInteger(ValueType _type, std::istream& _in, unsigned int& _value) const;

  /// Read unsupported properties in PLY file 
  void consume_input(std::istream& _in, int _count) const {
	  _in.read(reinterpret_cast<char*>(&buff[0]), _count);
  }

  mutable unsigned char buff[8];

  ///// Available options for reading
  //mutable Options options_;

  ///// Options that the user wants to read
  //mutable Options userOptions_;

  mutable unsigned int vertexCount_;
  mutable unsigned int faceCount_;

  //mutable ValueType vertexType_;
  //mutable uint vertexDimension_;

  //mutable ValueType faceIndexType_;
  //mutable ValueType faceEntryType_;

  enum VertexProperty {
	  XCOORD,YCOORD,ZCOORD,
	  TEXX,TEXY,
	  COLORRED,COLORGREEN,COLORBLUE,COLORALPHA,
	  UNSUPPORTED
  };

  /// Stores sizes of property types
  mutable std::map<ValueType, int> scalar_size_;

  // Number of vertex properties
  mutable unsigned int vertexPropertyCount_;
  mutable std::map< int , std::pair< VertexProperty, ValueType> > vertexPropertyMap_;

};


//== TYPE DEFINITION ==========================================================


/// Declare the single entity of the PLY reader
extern _PLYReader_  __PLYReaderInstance;
_PLYReader_&  PLYReader();


//=============================================================================
} // namespace IO
} // namespace PCM
//=============================================================================

#endif
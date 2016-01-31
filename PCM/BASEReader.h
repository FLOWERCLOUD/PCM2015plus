#ifndef _BASEREADER_H_
#define _BASEREADER_H_

//=== INCLUDES ================================================================
#include "Importer.h"


// STD C++
#include <iostream>
#include <string>

//== NAMESPACES ===============================================================


namespace PCM {
namespace IO {



//=== IMPLEMENTATION ==========================================================


/**
   Base class for reader modules.
   Reader modules access persistent data and pass them to the desired
   data structure by the means of a BaseImporter derivative.
   All reader modules must be derived from this class.
*/
class BaseReader
{
public:

  /// Destructor
  virtual ~BaseReader() {};

  /// Returns a brief description of the file type that can be parsed.
  virtual std::string get_description() const = 0;
  
  /** Returns a string with the accepted file extensions separated by a 
      whitespace and in small caps.
  */
  virtual std::string get_extensions() const = 0;

  /// Return magic bits used to determine file format
  virtual std::string get_magic() const { return std::string(""); }


  /** Reads a mesh given by a filename. Usually this method opens a stream
      and passes it to stream read method. Acceptance checks by filename
      extension can be placed here.

      Options can be passed via _opt. After execution _opt contains the Options
      that were available
  */
  virtual bool read(const std::string& _filename, 
		    BaseImporter& _bi,
                    Options& _opt) = 0;
		
 /** Reads a mesh given by a std::stream. This method usually uses the same stream reading method
    that read uses. Options can be passed via _opt. After execution _opt contains the Options
      that were available
  */
  virtual bool read(std::istream& _is, 
		    BaseImporter& _bi,
                    Options& _opt) = 0;


  /// Returns true if reader can parse _filename (checks extension)
  virtual bool can_u_read(const std::string& _filename) const;


protected:

  // case insensitive search for _ext in _fname.
  bool check_extension(const std::string& _fname, 
		       const std::string& _ext) const;
};


//=============================================================================
} // namespace IO
} // namespace PCM
//=============================================================================
#endif
//=============================================================================


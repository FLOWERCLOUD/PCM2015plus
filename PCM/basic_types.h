#ifndef _BASIC_TYPES_H
#define _BASIC_TYPES_H

#include <Eigen/Sparse>
#include <Eigen/Dense> 
#include <Eigen/LU>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Cholesky>
#include <Eigen/SVD>


#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET


namespace numeric{


	typedef char          int8 ;
	typedef short         int16 ;
	typedef int           int32 ;

	typedef void*         pointer;

#ifdef WIN32
	typedef __int64 int64 ;
#else
	typedef long long int int64 ;	
#endif

	typedef unsigned char  uint8 ;
	typedef unsigned short uint16 ;
	typedef unsigned int   uint32 ;

#ifdef WIN32
	typedef unsigned __int64 uint64 ;
#else
	typedef unsigned long long int uint64 ;
#endif    

	typedef float  float32 ;
	typedef double float64 ;

	typedef char	byte;


}

enum {TW_TYPE_FLOAT32, TW_TYPE_FLOAT64};

typedef numeric::int32 IndexType;
#define  PI  3.141592653

#ifdef USE_FLOAT64
typedef numeric::float64 ScalarType;
#define TW_TYPE_SCALARTYPE TW_TYPE_FLOAT64
#else
typedef numeric::float32 ScalarType;
#define TW_TYPE_SCALARTYPE TW_TYPE_FLOAT32
#endif

typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> MatrixXX;
typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, 2>              MatrixX2;
typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, 3>              MatrixX3;
typedef Eigen::Matrix<ScalarType, 2,              2>              Matrix22;
typedef Eigen::Matrix<ScalarType, 3,              3>              Matrix33;
typedef Eigen::Matrix<ScalarType, 4,			  4>              Matrix44;
typedef Eigen::Matrix<ScalarType, 3,			  4>              Matrix34;
typedef Eigen::Matrix<ScalarType, 7,              7>              Matrix77;
typedef Eigen::Matrix<ScalarType, 6,              6>              Matrix66;
typedef Eigen::Matrix<ScalarType, 10,            10>              Matrix1010;
typedef Eigen::Matrix<ScalarType, 2,              3>              Matrix23;
typedef Eigen::Matrix<ScalarType, 3, Eigen::Dynamic>			  Matrix3X;

typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, 1> VecX;
typedef Eigen::Matrix<ScalarType, 2,              1> Vec2;
typedef Eigen::Matrix<ScalarType, 3,              1> Vec3;
typedef Eigen::Matrix<ScalarType, 4,              1> Vec4;
typedef Eigen::Matrix<ScalarType, 5,              1> Vec5;
typedef Eigen::Matrix<ScalarType, 7,              1> Vec7;
typedef Eigen::Matrix<ScalarType, 6,              1> Vec6;
typedef Eigen::Matrix<ScalarType, 10,             1> Vec10;

typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, Eigen::Dynamic> MatrixXXi;
typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 3>              MatrixX3i;
typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 2>              MatrixX2i;
typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 12>             MatrixX12i;
typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 8>              MatrixX8i;
typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 6>              MatrixX6i;
typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 4>              MatrixX4i;

typedef Eigen::Matrix<IndexType,  Eigen::Dynamic, 26>             MatrixX26i;
//-----------
typedef Eigen::Matrix<IndexType,  3,              6>              Matrix36i;


#ifdef USE_OWN_DEFINE_TYPE
	struct PointType
	{
		PointType(ScalarType x_ = 0.,ScalarType y_ = 0., ScalarType z_=0.):
			x(x_),y(y_),z(z_){}
		PointType(const PointType& rhs){x = rhs.x;y=rhs.y;z=rhs.z;}
		void operator=(const PointType& rhs){x = rhs.x;y=rhs.y;z=rhs.z;}
		ScalarType x;
		ScalarType y;
		ScalarType z;
};
	typedef PointType NormalType;

	struct ColorType
	{
		ColorType(ScalarType r_ = 0., ScalarType g_ = 0., 
			ScalarType b_=0.,ScalarType a_=1.):r(r_),g(g_),b(b_),a(a_){}
		ColorType(const ColorType& rhs){r = rhs.r;g=rhs.g;b=rhs.b;a=rhs.a;}
		void operator=(const ColorType& rhs){r = rhs.r;g=rhs.g;b=rhs.b;a=rhs.a;}
		ScalarType r;
		ScalarType g;
		ScalarType b;
		ScalarType a;
};
#else
	typedef Eigen::Matrix<ScalarType, 3, 1, Eigen::DontAlign> PointType;
	typedef Eigen::Matrix<ScalarType, 3, 1, Eigen::DontAlign> NormalType;
	typedef Eigen::Matrix<ScalarType, 4, 1, Eigen::DontAlign> ColorType;
#endif 






	
#define  NULL_POINT PointType(0.,0.,0.)
#define  NULL_NORMAL NormalType(0.,0.,0.)
#define	 NULL_COLOR	 ColorType(0.,0.,0.,1.)
#include <iostream>
#define Logger std::cout


#endif
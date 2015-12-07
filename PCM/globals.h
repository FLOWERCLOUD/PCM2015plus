#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "basic_types.h"
#include <QMutex>



#define HELLO

namespace Paint_Param
{
	extern	Vec3	g_step_size;
	extern ScalarType g_point_size;
	extern ScalarType g_line_size;
}

namespace Register_Param
{
	extern MatrixXXi g_traj_matrix;
	extern bool g_is_traj_compute;
}
//extern QWaitCondition mWaitcond;
#define		LOCK(x)		x.lock()
#define		UNLOCK(x)	x.unlock()

#endif
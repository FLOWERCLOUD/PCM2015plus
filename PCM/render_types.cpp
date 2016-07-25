#include <windows.h>  //Windows Header ֻ��gl.h ������������
#include <gl/gl.h>
#include "render_types.h"




void SetMaterial(const gl_Material *material)
{
	/* define material properties of all polygons */
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  material->specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
}

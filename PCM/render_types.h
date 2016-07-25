#ifndef  _RENDER_TYPES_
#define  _RENDER_TYPES_
#include "basic_types.h"
//Determine which color render mode
namespace ColorMode{
	struct ObjectColorMode{};
	struct VertexColorMode{};
	struct LabelColorMode{};
	struct WrapBoxColorMode{}; //added by huayun
	struct EdgePointColorMode{};  //added by huayun
	struct SphereMode{};
}
namespace RenderMode
{
	enum RenderType{PointMode,SolidMode,WireMode,SmoothMode,TextureMode,SelectMode};
	//struct PointMode{};
	//struct SolidMode{};
	//struct WireMode{};
	//struct SmoothMode{};
	//struct TextureMode{}
	//struct SelectMode{};
	enum WhichColorMode{ VERTEX_COLOR, OBJECT_COLOR, LABEL_COLOR ,WrapBoxColorMode ,EdgePointColorMode ,SphereMode};
}

#define	 GRAY_COLOR	 ColorType(0.5,0.5,0.5,1.)
#define	WHITE_COLOR	 ColorType(1.0,1.0,1.0,1.)
#define RED_COLOR	 ColorType(1.0,0.0,0.0,1.)
class gl_Material
{
public:
float ambient[4];
float diffuse[4];
float specular[4];
float shininess[4];
};
static const gl_Material material = {
	{0.0f, 0.0f, 1.0f, 0.0f},     /* ambient */
	{0.0f, 1.0f, 0.0f, 0.0f},     /* diffuse */
	{0.0f, 1.0f, 0.0f, 1.0f},     /* specular */
	{2.0f, 0, 0, 0}               /* shininess */
};
void SetMaterial(const gl_Material *material);

#endif
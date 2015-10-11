#include "color_table.h"
#include <time.h>
#include <algorithm>

namespace Color_Utility
{
	ColorType color_from_table(IndexType index)
	{
		if((255 < index) && (index <= 512)) 
			index = 512 - index;
		if((512 < index) && (index <= 768)) 
			index = index - 512;
		if(index > 768) 
			index = 1024-index;

		if(index < 0) 
			index = 0;
		if(index > 255) 
			index = 255;

		ScalarType r = static_cast<ScalarType>( color_table[index][0] );
		ScalarType g = static_cast<ScalarType>( color_table[index][1] );
		ScalarType b = static_cast<ScalarType>( color_table[index][2] );

		return ColorType(r,g,b,1.);
	}

	ColorType random_color_from_table() {

		IndexType random_int = rand() / 256;

		int rand_idx_array[256];
		for (int i=0 ; i <256; i++)
		{
			rand_idx_array[i] = i;
		}

		std::random_shuffle(rand_idx_array, rand_idx_array+256);

		return color_from_table(rand_idx_array[random_int]);
	}


	ColorType	span_color_from_table( IndexType idx )
	{
		//static const IndexType color_step = 67;
		//IndexType	color_idx =  (idx * color_step) % 255;
		//return Color_Utility::color_from_table(color_idx);


		IndexType	color_index =  (idx) % 18;
		ScalarType r = tiny_color_table[color_index][0]/255.0;
		ScalarType g = tiny_color_table[color_index][1]/255.0;
		ScalarType b = tiny_color_table[color_index][2]/255.0;
		return ColorType(r,g,b,1.);

	}

	ColorType  color_map_one(ScalarType index)
	{

		IndexType color_index = (int) (index * 100)<100?(int) (index * 100):99;
		//IndexType color_index = (int) (index * 100);
		ScalarType r = color_table_map[color_index][0]/255.0;
		ScalarType g = color_table_map[color_index][1]/255.0;
		ScalarType b = color_table_map[color_index][2]/255.0;
		return ColorType(r,g,b,1.);
	}

	ColorType		span_color_from_table_with_edge(IndexType label)
	{

		IndexType	color_index =  ( label + 9) % 18;
		ScalarType r = tiny_color_table[color_index][0]/255.0;
		ScalarType g = tiny_color_table[color_index][1]/255.0;
		ScalarType b = tiny_color_table[color_index][2]/255.0;
		return ColorType(r,g,b,1.);


	}

	

	ColorType	span_color_from_hy_table( IndexType idx )
	{
		//static const IndexType color_step = 67;
		//IndexType	color_idx =  (idx * color_step) % 255;
		//return Color_Utility::color_from_table(color_idx);


		IndexType	color_index =  (idx) % 19;
		ScalarType r = huanyun_color_table[color_index][0];
		ScalarType g = huanyun_color_table[color_index][1];
		ScalarType b = huanyun_color_table[color_index][2];
		return ColorType(r,g,b,1.);

	}

	IndexType getColorLabelId(const ColorType& _color)
	{

		IndexType colorNum = sizeof( huanyun_color_table) /sizeof(huanyun_color_table[0]);
		IndexType targetLabel = -1;
		for( IndexType colorId = 0 ; colorId < colorNum ;++colorId ){
			if( (huanyun_color_table[colorId][0] == _color(0 ,0))&&( huanyun_color_table[colorId][1] == _color(1 ,0))&&( huanyun_color_table[colorId][2] == _color(2 ,0))  ){
				targetLabel = colorId;
			}
		}
		return targetLabel;
	}

}
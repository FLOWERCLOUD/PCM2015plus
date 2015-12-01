#ifndef _SAVE_PLY_SETTING_
#define _SAVE_PLY_SETTING_
class SavePlySetting
{
public:
	QString outdir;
	QString basename;
	int counter;
	int resolution;
	//bool transparentBackground;
	//bool snapAllLayers;
	//bool tiledSave; // if true all the tiles are saved as saperated files and not joined
	//bool addToRaster;
	SavePlySetting()
	{
		outdir = ".";
		basename = "snapshot";
		counter = 0 ;
		resolution  =1;
		/*	transparentBackground = true;
		snapAllLayers = false;
		tiledSave = false;
		addToRaster = false;*/
	}

};

#endif 
#ifndef _SAVE_PLY_SETTING_
#define _SAVE_PLY_SETTING_
class SavePlySetting
{
public:
	QString outdir;
	QString basename;

	bool isAjustView;
	bool isOriColor;
	bool islabeledcor;
	int startframe;
	int endframe;

	QString format;

	//bool transparentBackground;
	//bool snapAllLayers;
	//bool tiledSave; // if true all the tiles are saved as saperated files and not joined
	//bool addToRaster;
	SavePlySetting()
	{
		outdir = ".";
		basename = "snapshot";
		isAjustView = false ;
		isOriColor = true;
		islabeledcor = false;
		startframe = 0;
		endframe = 0;
		format = "ply";
		/*	transparentBackground = true;
		snapAllLayers = false;
		tiledSave = false;
		addToRaster = false;*/
	}

};

#endif 
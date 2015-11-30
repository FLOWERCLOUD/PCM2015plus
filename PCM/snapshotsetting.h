#ifndef SNAPSHOTSETTING
#define SNAPSHOTSETTING
class SnapshotSetting
{
public:
	QString outdir;
	QString basename;
	int counter;
	int resolution;
	bool transparentBackground;
	bool snapAllLayers;
	bool tiledSave; // if true all the tiles are saved as saperated files and not joined
	bool addToRaster;
	SnapshotSetting()
	{
		outdir = ".";
		basename = "snapshot";
		counter = 0 ;
		resolution  =1;
		transparentBackground = true;
		snapAllLayers = false;
		tiledSave = false;
		addToRaster = false;
	}

};

#endif 
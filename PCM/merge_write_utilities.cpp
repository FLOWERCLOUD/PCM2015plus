#include "merge_write_utilities.h"
#include "sample_set.h"
#include "vertex.h"


void writePovray(char* _file_in ,char* _file_out)
{
	char* file_in = _file_in;
	char* file_out = _file_out;
	FILE* in_file = fopen( file_in,"r");
	FILE* outfile = fopen( file_out,"w");
	Sample& smp = SampleSet::get_instance()[0];
	ScalarType xmin ,xmax ,ymin ,ymax , zmin ,zmax;
	xmin = ymin = zmin = 10000;
	xmax = ymax = zmax = -10000;
	for( auto vbitr = smp.begin() ; vbitr != smp.end(); ++vbitr ){
		ScalarType xtmp = (*vbitr)->x();
		if( xtmp < xmin ) xmin = xtmp;
		ScalarType ytmp = (*vbitr)->y();
		if( ytmp < ymin) ymin = ytmp;
		ScalarType ztmp = (*vbitr)->z();
		if( ztmp < zmin )zmin = ztmp;

	}
	for( auto vbitr = smp.begin() ; vbitr != smp.end(); ++vbitr ){
		ScalarType xtmp = (*vbitr)->x();
		if( xtmp > xmax ) xmax = xtmp;
		ScalarType ytmp = (*vbitr)->y();
		if( ytmp > ymax) ymax = ytmp;
		ScalarType ztmp = (*vbitr)->z();
		if( ztmp > zmax )zmax = ztmp;

	}
	/* 映射到 [-4 ，4 ]区间*/
	ScalarType xscale = (xmax - xmin)/8;
	ScalarType yscale = (ymax - ymin)/8;
	ScalarType zscale = (zmax - zmin)/8;
	ScalarType maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
	ScalarType xmean =(xmax + xmin)/maxscale*0.5;
	ScalarType ymean =(ymax + ymin)/maxscale*0.5;
	ScalarType zmean = (zmax + zmin)/maxscale*0.5;


	ScalarType pointsize= 0.06;
	IndexType
		label = 0;
	ScalarType x ,y  ,z , n1 ,n2 ,n3 ,r ,g , b;
	while(true){

		int stat = fscanf(in_file,"%f %f %f %d %d %d %d %d %d\n",  &x, &y,&z ,&n1 ,&n2 ,&n3 ,&r ,&g ,&b);	 
		if(stat==EOF)break;
		fprintf(outfile, "sphere{\n <%f,%f,%f>,%f\ntexture{texture%d}}\n",  x/maxscale - xmean ,y/maxscale -ymean ,z/maxscale -zmean ,pointsize ,3*(label%3+2) );
		++label;
	}
	fclose(outfile);	
	fclose(in_file);
}

void writeSamplePovray()
{
	char* file_out = "H:\\computer_graphic_learning\\povay_pointcloud\\partner\\povraypartner_cog.inc";

	char* file_sample = "H:\\computer_graphic_learning\\povay_pointcloud\\nousingbackdeformationcosegoutputsplitlabels(2.txt";

	FILE* outfile = fopen( file_out,"w");
	FILE* in_smpfile = fopen( file_sample ,"r");
	Sample& smp = SampleSet::get_instance()[0];
	ScalarType xmin ,xmax ,ymin ,ymax , zmin ,zmax;
	xmin = ymin = zmin = 10000;
	xmax = ymax = zmax = -10000;
	for( auto vbitr = smp.begin() ; vbitr != smp.end(); ++vbitr ){
		ScalarType xtmp = (*vbitr)->x();
		if( xtmp < xmin ) xmin = xtmp;
		ScalarType ytmp = (*vbitr)->y();
		if( ytmp < ymin) ymin = ytmp;
		ScalarType ztmp = (*vbitr)->z();
		if( ztmp < zmin )zmin = ztmp;

	}
	for( auto vbitr = smp.begin() ; vbitr != smp.end(); ++vbitr ){
		ScalarType xtmp = (*vbitr)->x();
		if( xtmp > xmax ) xmax = xtmp;
		ScalarType ytmp = (*vbitr)->y();
		if( ytmp > ymax) ymax = ytmp;
		ScalarType ztmp = (*vbitr)->z();
		if( ztmp > zmax )zmax = ztmp;

	}
	/* 映射到 [-4 ，4 ]区间*/
	ScalarType xscale = (xmax - xmin)/8;
	ScalarType yscale = (ymax - ymin)/8;
	ScalarType zscale = (zmax - zmin)/8;
	ScalarType maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
	ScalarType xmean =(xmax + xmin)/maxscale*0.5;
	ScalarType ymean =(ymax + ymin)/maxscale*0.5;
	ScalarType zmean = (zmax + zmin)/maxscale*0.5;


	ScalarType pointsize= 0.08;


	IndexType frameId, labelId, vtxId;

	int stat = fscanf( in_smpfile,"%d\n",  &frameId);	  //去除第一行
	while(true){

		int stat = fscanf( in_smpfile,"%d %d %d\n",  &frameId , &labelId ,&vtxId);	 
		if(stat==EOF)break;
		fprintf(outfile, "sphere{\n <%f,%f,%f>,%f\ntexture{texture%d}}\n",  (smp.vertices_matrix().col(vtxId).x())/maxscale - xmean ,
			(smp.vertices_matrix().col(vtxId).y())/maxscale -ymean ,-((smp.vertices_matrix().col(vtxId).z())/maxscale -zmean ) ,pointsize ,labelId );

	}
	fclose(outfile);	
	fclose(in_smpfile);
}

void writeMutiSamplePovray()
{
	char* file_out = "H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\inc\\handShakeDown_Labels0_29.inc";

	char* file_sample = "H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\handShakeDown_Labels05_10_15.txt";

	FILE* outfile = fopen( file_out,"w");
	FILE* in_smpfile = fopen( file_sample ,"r");


	ScalarType pointsize= 0.08;


	IndexType frameId, labelId, vtxId;

	IndexType cframe = -1;
	IndexType endindex = 0;
	ScalarType xmean ;
	ScalarType ymean ;
	ScalarType zmean ;
	ScalarType maxscale;
	Sample* smp;
	while(true){



		int stat = fscanf( in_smpfile,"%d %d %d\n",  &frameId , &labelId ,&vtxId);
		if(stat==EOF){ fprintf(outfile ,"#end\n");break;}



		if( cframe !=frameId){

			smp = &(SampleSet::get_instance()[frameId]);
			ScalarType xmin ,xmax ,ymin ,ymax , zmin ,zmax;
			xmin = ymin = zmin = 10000;
			xmax = ymax = zmax = -10000;
			for( auto vbitr = smp->begin() ; vbitr != smp->end(); ++vbitr ){
				ScalarType xtmp = (*vbitr)->x();
				if( xtmp < xmin ) xmin = xtmp;
				ScalarType ytmp = (*vbitr)->y();
				if( ytmp < ymin) ymin = ytmp;
				ScalarType ztmp = (*vbitr)->z();
				if( ztmp < zmin )zmin = ztmp;

			}
			for( auto vbitr = smp->begin() ; vbitr != smp->end(); ++vbitr ){
				ScalarType xtmp = (*vbitr)->x();
				if( xtmp > xmax ) xmax = xtmp;
				ScalarType ytmp = (*vbitr)->y();
				if( ytmp > ymax) ymax = ytmp;
				ScalarType ztmp = (*vbitr)->z();
				if( ztmp > zmax )zmax = ztmp;

			}
			/* 映射到 [-4 ，4 ]区间*/
			ScalarType xscale = (xmax - xmin)/8;
			ScalarType yscale = (ymax - ymin)/8;
			ScalarType zscale = (zmax - zmin)/8;
			maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
			xmean =(xmax + xmin)/maxscale*0.5;
			ymean =(ymax + ymin)/maxscale*0.5;
			zmean = (zmax + zmin)/maxscale*0.5;


			if( 1 ==endindex )fprintf(outfile ,"#end\n");
			cframe = frameId;
			fprintf(outfile ,"#if(frame_number = %d)\n" , cframe);
			endindex = 1;
		}

		fprintf(outfile, "sphere{\n <%f,%f,%f>,%f\ntexture{texture%d}}\n",  (smp->vertices_matrix().col(vtxId).x())/maxscale - xmean ,
			(smp->vertices_matrix().col(vtxId).y())/maxscale -ymean ,-((smp->vertices_matrix().col(vtxId).z())/maxscale -zmean ) ,pointsize ,labelId );

	}
	fclose(outfile);	
	fclose(in_smpfile);
}

void writeMutiSamplePovray(char* _file_out , char* _file_sample)
{
	char* file_out = _file_out;

	char* file_sample = _file_sample;

	FILE* outfile = fopen( file_out,"w");
	FILE* in_smpfile = fopen( file_sample ,"r");


	ScalarType pointsize= 0.08;


	IndexType frameId, labelId, vtxId;

	IndexType cframe = -1;
	IndexType endindex = 0;
	ScalarType xmean ;
	ScalarType ymean ;
	ScalarType zmean ;
	ScalarType maxscale;
	Sample* smp;
	while(true){



		int stat = fscanf( in_smpfile,"%d %d %d\n",  &frameId , &labelId ,&vtxId);
		if(stat==EOF){ fprintf(outfile ,"#end\n");break;}



		if( cframe !=frameId){

			smp = &(SampleSet::get_instance()[frameId]);
			ScalarType xmin ,xmax ,ymin ,ymax , zmin ,zmax;
			xmin = ymin = zmin = 10000;
			xmax = ymax = zmax = -10000;
			for( auto vbitr = smp->begin() ; vbitr != smp->end(); ++vbitr ){
				ScalarType xtmp = (*vbitr)->x();
				if( xtmp < xmin ) xmin = xtmp;
				ScalarType ytmp = (*vbitr)->y();
				if( ytmp < ymin) ymin = ytmp;
				ScalarType ztmp = (*vbitr)->z();
				if( ztmp < zmin )zmin = ztmp;

			}
			for( auto vbitr = smp->begin() ; vbitr != smp->end(); ++vbitr ){
				ScalarType xtmp = (*vbitr)->x();
				if( xtmp > xmax ) xmax = xtmp;
				ScalarType ytmp = (*vbitr)->y();
				if( ytmp > ymax) ymax = ytmp;
				ScalarType ztmp = (*vbitr)->z();
				if( ztmp > zmax )zmax = ztmp;

			}
			/* 映射到 [-4 ，4 ]区间*/
			ScalarType xscale = (xmax - xmin)/8;
			ScalarType yscale = (ymax - ymin)/8;
			ScalarType zscale = (zmax - zmin)/8;
			maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
			xmean =(xmax + xmin)/maxscale*0.5;
			ymean =(ymax + ymin)/maxscale*0.5;
			zmean = (zmax + zmin)/maxscale*0.5;


			if( 1 ==endindex )fprintf(outfile ,"#end\n");
			cframe = frameId;
			fprintf(outfile ,"#if(frame_number = %d)\n" , cframe);
			endindex = 1;
		}

		fprintf(outfile, "sphere{\n <%f,%f,%f>,%f\ntexture{texture%d}}\n",  (smp->vertices_matrix().col(vtxId).x())/maxscale - xmean ,
			((smp->vertices_matrix().col(vtxId).y())/maxscale -ymean) ,-((smp->vertices_matrix().col(vtxId).z())/maxscale -zmean ) ,pointsize ,labelId );

	}
	fclose(outfile);	
	fclose(in_smpfile);
}

void writeLabelFilenameAfterPropagate(char* _labelfile_out)
{
	FILE* outfile = fopen( _labelfile_out,"w");
	SampleSet& smpset = SampleSet::get_instance();
	IndexType smpsetNum = smpset.size();

	IndexType startFrameId = 0;
	IndexType endFrameId = smpsetNum -1;
	for( IndexType frameId = startFrameId ; frameId < endFrameId +1 ; ++frameId )
	{
		Sample& smp = smpset[frameId];
		auto smpeitr = smp.end();
		IndexType vtxId = 0;
		for( auto smpbitr = smp.begin() ; smpbitr != smpeitr ; ++smpbitr ,++vtxId ){
			Vertex& vtx = **smpbitr;

			fprintf(outfile, "%d %d %d\n",frameId ,vtx.label() , vtxId );
		}

	}

	fclose(outfile);
}

void writeIncFromPly(char* _plyfilename , char* _outIncfilename)
{
	FILE* in_file = fopen(_plyfilename ,"r");
	FILE* outfile = fopen(_outIncfilename ,"w");
	int vcount=0;
	ScalarType pointsize= 0.06;
	std::vector<mvertex> vtxVec;
	fscanf(in_file,"ply\nformat ascii 1.0\nelement vertex %d\n",&vcount);
	fscanf(in_file,"property float x\nproperty float y\nproperty float z\nproperty float nx\nproperty float ny\nproperty float nz\nproperty uchar red\nproperty uchar green\nproperty uchar blue\n");
	fscanf(in_file,"end_header\n");
	while(true){

		ScalarType vx,vy,vz,nx,ny,nz,cx,cy,cz;
		int status = fscanf(in_file, "%f %f %f %f %f %f %f %f %f\n",&vx,&vy,&vz,
			&nx,&ny,&nz,
			&cx,&cy,&cz);
		if(status==EOF)break;

		mvertex vtx;
		vtx.x = vx;
		vtx.y = vy;
		vtx.z = vz;
		vtx.nx = nx;
		vtx.ny = ny;
		vtx.nz = nz;
		vtx.r = cx;
		vtx.g = cy;
		vtx.b = cz;
		vtxVec.push_back( vtx);


	}

	//用于规范化坐标
	ScalarType xmin ,xmax ,ymin ,ymax , zmin ,zmax;
	xmin = ymin = zmin = 10000;
	xmax = ymax = zmax = -10000;
	IndexType vecnum = vtxVec.size();
	for( IndexType ii = 0 ; ii < vecnum ;++ ii ){
		ScalarType xtmp = vtxVec[ii].x;
		if( xtmp < xmin ) xmin = xtmp;
		ScalarType ytmp = vtxVec[ii].y;
		if( ytmp < ymin) ymin = ytmp;
		ScalarType ztmp = vtxVec[ii].z;
		if( ztmp < zmin )zmin = ztmp;

	}
	for( IndexType ii = 0 ; ii < vecnum ;++ ii){
		ScalarType xtmp = vtxVec[ii].x;
		if( xtmp > xmax ) xmax = xtmp;
		ScalarType ytmp = vtxVec[ii].y;
		if( ytmp > ymax) ymax = ytmp;
		ScalarType ztmp = vtxVec[ii].z;
		if( ztmp > zmax )zmax = ztmp;

	}
	/* 映射到 [-4 ，4 ]区间*/
	ScalarType xscale = (xmax - xmin)/8;
	ScalarType yscale = (ymax - ymin)/8;
	ScalarType zscale = (zmax - zmin)/8;
	ScalarType maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
	ScalarType xmean =(xmax + xmin)/maxscale*0.5;
	ScalarType ymean =(ymax + ymin)/maxscale*0.5;
	ScalarType zmean = (zmax + zmin)/maxscale*0.5;
	for( IndexType ii = 0 ; ii < vecnum ;++ ii){

		ColorType ccolor( vtxVec[ii].r ,vtxVec[ii].g ,vtxVec[ii].b ,1.0);
		fprintf(outfile, "sphere{\n <%f,%f,%f>,%f\ntexture{texture%d}}\n", vtxVec[ii].x/maxscale - xmean ,vtxVec[ii].y/maxscale - ymean,-(vtxVec[ii].z/maxscale - zmean) ,pointsize ,Color_Utility::getColorLabelId(ccolor) );

	}

	fclose( in_file );
	fclose( outfile);
}

void writeLabelFromPly(char* _labelfile_out)
{
	FILE* outfile = fopen( _labelfile_out,"w");
	SampleSet& smpset = SampleSet::get_instance();
	IndexType smpsetNum = smpset.size();

	IndexType startFrameId = 0;
	IndexType endFrameId = smpsetNum -1;
	for( IndexType frameId = startFrameId ; frameId < endFrameId +1 ; ++frameId )
	{
		Sample& smp = smpset[frameId];
		auto smpeitr = smp.end();
		IndexType vtxId = 0;
		for( auto smpbitr = smp.begin() ; smpbitr != smpeitr ; ++smpbitr ,++vtxId ){
			Vertex& vtx = **smpbitr;
			ColorType cc( (int)255*vtx.r() , (int)255*vtx.g() ,  (int)255*vtx.b() ,255);
			IndexType labelId = Color_Utility::getColorLabelId(cc);
			fprintf(outfile, "%d %d %d\n",frameId ,labelId , vtxId );
		}

	}

	fclose(outfile);
}

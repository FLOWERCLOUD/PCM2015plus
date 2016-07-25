#pragma once
//************************************
// Method:    depthToCloud
// 根据清华大学一篇文章中的深度图数据获得点云坐标
#include "basic_types.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QString>
#include <QFileDialog>
class Sample;
//************************************
struct mvertex
{
	ScalarType x,y,z,nx,ny,nz;
	IndexType r,g,b;
};


void depthToCloud(char* file_in, char* file_out)
{
	FILE* in_file = fopen( file_in,"r");
	FILE* outfile = fopen( file_out,"w");
	//	ofstream fout(file_out);
	std::string str;

	int buf;
	IndexType x  ,y;
	x= y =0;
	while(true){

		int stat = fscanf(in_file,"%d ",  &buf);	 
		if(stat==EOF)break;

		fprintf(outfile, "%d %d %f\n" , x ,y , buf*0.1 );
		++x;
		if( x == 640) {x=0 ; ++y;}
	}

	fclose(outfile);	
	fclose(in_file);

}

void getDifferentLabelPoints( char* _beforeFileName , char* _afterFileName , char* outFileName)
{
	FILE* beforefile = fopen( _beforeFileName ,"r");
	FILE* afterfile =  fopen( _afterFileName ,"r");
	FILE* outFile   =  fopen( outFileName ,"w");

	char buffer[50];
	int bufBefore[3];   //frameid , label ,vtxid ,
	int bufAfter[3];
	 
	while(true){

		int stat1 = fscanf ( beforefile,"%d %d %d\n",  &bufBefore[0] ,&bufBefore[1] , &bufBefore[2]);	
		int stat2 = fscanf ( afterfile,"%d %d %d\n",  &bufAfter[0] ,&bufAfter[1] , &bufAfter[2]);	 
		if(stat1==EOF || stat2 == EOF)break;
		if( bufBefore[0] == 5){
			if( bufAfter[1] == bufBefore[1]) bufAfter[1]+=0;
			else{

				bufAfter[1] = 2;
			}


		}
		else if(bufBefore[0] ==10){
			if(bufAfter[1] == bufBefore[1]) bufAfter[1]+=3;
			else{

				bufAfter[1] = 5;
			}
		}
		else if( bufBefore[0] ==15 ){

			if( bufAfter[1] == bufBefore[1]) bufAfter[1]+=6;
			else{

				bufAfter[1] = 8;
			}
		}else{
			std::abort();
			continue;
		}
		fprintf(outFile, "%d %d %d\n", bufAfter[0] ,bufAfter[1] ,bufAfter[2] );
	}
	fclose(outFile);
	fclose(beforefile);	
	fclose(afterfile);


}




void appendcorr( const char* file_in ,char* file_out);
void appendlabel( const char* file_in ,char* file_out);

void mergeFile(char* CORR_FILESNAME ,char* CORR_FILEOUT_NAME , char* LABEL_FILESNAME , char* LABEL_FILEOUT_NAME)
{
	QString dir_in( CORR_FILESNAME );
	if (dir_in.isEmpty()){
		std::cout<<"corr_files empty"<<std::endl;
		return ;}

	QDir file_dir(dir_in);
	if ( !file_dir.exists() )
	{
		std::cout<<"corr_files don't exist"<<std::endl;
		return ;
	}
	file_dir.setFilter(QDir::Files);

	QFileInfoList file_list = file_dir.entryInfoList();
	IndexType sample_idx = 0;

	char* file_outputname;

	//FILE* out_file = fopen( file_outputname , "w");
	char* corrout_ = CORR_FILEOUT_NAME;
	FILE* corrTMPfile = fopen( corrout_ ,"w");
	fclose( corrTMPfile);
	for (IndexType file_idx = 0; file_idx < file_list.size(); file_idx++)
	{
		QFileInfo file_info = file_list.at(file_idx);


		std::string file_path = file_info.filePath().toStdString();

		appendcorr(  file_path.c_str() , corrout_);


	}
	std::cout<<"merge corr_files success"<<std::endl;
	QString dir_in2( LABEL_FILESNAME );
	if (dir_in2.isEmpty()){
		std::cout<<"label_files empty"<<std::endl;
		return ;}

	QDir file_dir2(dir_in2);
	if ( !file_dir2.exists() )
	{
		std::cout<<"label_files don't exist"<<std::endl;
		return;
	}
	file_dir2.setFilter(QDir::Files);

	QFileInfoList file_list2 = file_dir2.entryInfoList();

	char* labelout_ = LABEL_FILEOUT_NAME;
	FILE* labelTMPfile = fopen( labelout_ ,"w");
	fclose( labelTMPfile);

	for (IndexType file_idx = 0; file_idx < file_list2.size(); file_idx++)
	{
		QFileInfo file_info = file_list2.at(file_idx);


		std::string file_path = file_info.filePath().toStdString();

		appendlabel(  file_path.c_str() , labelout_);


	}
	std::cout<<"merge label_files success"<<std::endl;

}
void mergeFile( )
{
	char* CORR_FILESNAME = "H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\corrs";
	char* CORR_FILEOUT_NAME= "H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\corroutput0_29center5.txt";
	char* LABEL_FILESNAME = "H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\labels";
	char* LABEL_FILEOUT_NAME = "H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\labeloutput0_29center5.txt";
	mergeFile(CORR_FILESNAME ,CORR_FILEOUT_NAME , LABEL_FILESNAME , LABEL_FILEOUT_NAME);
	//QString dir_in = QFileDialog::getExistingDirectory( &(main_window::getInstance()) ,QString("Import point cloud files"),".");
	


}

void appendcorr( const char* file_in ,char* file_out)
{
	FILE* in_file = fopen( file_in,"r");
	FILE* outfile = fopen( file_out,"a");
//	ofstream fout(file_out);
	std::string str;
	char buffer[50];
	int buf[4];
	while(true){

		int stat = fscanf(in_file,"%d %d %d %d\n",  &buf[0] ,&buf[1] , &buf[2] ,&buf[3]);	 
		if(stat==EOF)break;
		fprintf(outfile, "%d %d %d %d\n", buf[0] ,buf[1] , buf[2] ,buf[3]);
	}
		
	fclose(outfile);	
	fclose(in_file);
		 

}

void appendlabel( const char* file_in ,char* file_out)
{
	FILE* in_file = fopen( file_in,"r");
	FILE* outfile = fopen( file_out,"a");
	//	ofstream fout(file_out);
	std::string str;
	char buffer[50];
	int buf[3];
	//fscanf(in_file,"%d\n",  &buf[0] );	 //略过第一行
	while(true){

		int stat = fscanf(in_file,"%d %d %d\n",  &buf[0] ,&buf[1] , &buf[2]);	 
		if(stat==EOF)break;
		fprintf(outfile, "%d %d %d\n", buf[0] ,buf[1] , buf[2] );
	}

	fclose(outfile);	
	fclose(in_file);


}
/* format
	sphere { <-1,-2,-3>, 0.707107 texture { texture1 } }
*/

void  writePovray( char* _file_in ,char* _file_out);


void writePovray(){
	char* file_in = "C:\\Users\\huayun\\Desktop\\carDepth\\carDepth\\test\\tiaowu\\down_up000.xyz";
	 char* file_out = "C:\\Users\\huayun\\Desktop\\carDepth\\carDepth\\test\\povraytiaowu.inc";
	  writePovray( file_in , file_out);
	

}
void writeSamplePovray();

void writeMutiSamplePovray();

void writeMutiSamplePovray(char* _file_out , char* _file_sample);

void writeLabelFilenameAfterPropagate(char* _labelfile_out);

#include "color_table.h"
void writeIncFromPly(char* _plyfilename , char* _outIncfilename);
//对ply 文件的点颜色进行替换

ScalarType corrColormap[19][3] = {  

	255.f,0.f,0.f,    // 0
	255.f,57.f,0.f,       //1
	227.f,255.f, 0.f,     // 2
	239.f,122.f,47.f,    // 3
	170.f,121.f,180.f,    // 4
	206.f,127.f,97.f,    // 5
	41.f,180.f,192.f,   //34.f,10.f,54.f,    // 6
	25.f,135.f,66.f,	    // 7
	117.f,147.f,148.f,    // 8
	165.f,165.f,65.f,         // 9
	255.f,57.f,0.f,      // 10
	227.f,255.f,0.f,     // 11
	15.f,71.f,127.f,      // 12
	174.f,75.f,93.f,     // 13
	255.f,170.f,0.f,      // 14
	237.f,238.f,143.f,     // 15
	170.f,255.f,0.f,       // 16
	206.f,127.f,97.f,     // 17
	255.f,0.f,0.f     // 18


};

void writePlyFromPly(char* _plyfilename , char* _outplyfilename , ScalarType** colormap)
{

	FILE* in_file = fopen(_plyfilename ,"r");
	std::ofstream coutStream( _outplyfilename , std::ofstream::out);
	int vcount=0;
	std::vector<mvertex> vtxVec;
	fscanf(in_file,"ply\nformat ascii 1.0\nelement vertex %d\n",&vcount);
	fscanf(in_file,"property float x\nproperty float y\nproperty float z\nproperty float nx\nproperty float ny\nproperty float nz\nproperty uchar red\nproperty uchar green\nproperty uchar blue\n");
	fscanf(in_file,"property uchar alpha\n");
	IndexType faceNum;
	fscanf(in_file,"element face %d\n" ,&faceNum);
	fscanf(in_file,"property list uchar int vertex_indices\n");
	fscanf(in_file,"end_header\n");
	IndexType vtxcount = 0;
	while(true){
	 
		
		if( vtxcount == vcount)break;

		ScalarType vx,vy,vz,nx,ny,nz,cx,cy,cz,ulpha;
		int status = fscanf(in_file, "%f %f %f %f %f %f %f %f %f %f\n",&vx,&vy,&vz,
			&nx,&ny,&nz,
			&cx,&cy,&cz ,&ulpha);
		if(status==EOF )break;
		
		++vtxcount;

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
	IndexType vecnum = vtxVec.size();

	coutStream<<"ply"<<std::endl;
	coutStream<<"format ascii 1.0"<<std::endl;
	coutStream<<"element vertex "<<  vtxVec.size()<<std::endl;
	coutStream<<"property   float   x"<<std::endl;
	coutStream<<"property   float   y "<<std::endl;
	coutStream<<"property   float   z "<<std::endl;
	coutStream<<"property   float   nx"<<std::endl;
	coutStream<<"property   float   ny "<<std::endl;
	coutStream<<"property   float   nz "<<std::endl;
	coutStream<<"property   uchar red "<<std::endl;
	coutStream<<"property   uchar   green"<<std::endl;
	coutStream<<"property   uchar  blue"<<std::endl;
	coutStream<<"property uchar alpha\nelement face "<< faceNum<<std::endl;
	coutStream<<"property list uchar int vertex_indices"<<std::endl;
	coutStream<<"end_header"<<std::endl;
	





	for ( IndexType ii = 0 ; ii < vecnum ;++ ii)
	{				
		mvertex& v =vtxVec[ii];
		 ColorType cc( v.r ,v.g , v.b ,1.0);
		IndexType labelId = Color_Utility::getColorLabelId(cc);
	//	std::cout<<corrColormap[labelId][0]<<corrColormap[labelId][1] <<corrColormap[labelId][2] <<std::endl; 
		ColorType corrColor1(  static_cast<ScalarType>(corrColormap[labelId][0]) ,static_cast<ScalarType>(corrColormap[labelId][1]) ,static_cast<ScalarType>(corrColormap[labelId][2]) ,1. );
		

		coutStream<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.nx<<" "<<v.ny<<" "<<v.nz<<" "<<corrColor1(0 ,0)<<" "<<corrColor1(1 ,0)<<" "<< corrColor1(2 ,0)<<" "<<255<<std::endl;
	}

	IndexType faceId , vtxId1 ,vtxId2 , vtxId3;

	for(IndexType ii = 0; ii< faceNum ;++ii){
		fscanf(in_file , "%d %d %d %d\n" , &faceId , &vtxId1 ,&vtxId2 ,&vtxId3);
		coutStream<<faceId<<" "<<vtxId1<<" "<<vtxId2<<" "<<vtxId3<<std::endl;
	}
	coutStream.close();
	fclose( in_file );


}
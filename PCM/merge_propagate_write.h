#pragma once
#include "merge_write_utilities.h"
#include "sample_set.h"
#include "sample.h"
#include "basic_types.h"
#include <iostream>
#include "main_window.h"
#include "GlobalObject.h"

#include "QGLViewer/qglviewer.h"
using namespace qglviewer;
class Proxy;
class Proxy_Propagate;
class Proxy_PropagateAndVisual;
class ProxyAjustViewPly;
class ProxyGrayColorPly;
class ProxyMergeAndPropagate;
class ProxyOricolorPly;
class ProxyOricolorSamplePly;
class ProxyPly;
class ProxyVisualLabel;
class ProxyWriteInc;



class Proxy
{
public:

	virtual void run()=0;
	virtual ColorType getLabelColor(void*){

		return ColorType(0.0 ,0.0 , 0.0 ,1.0);
	}
public:
	void setContext( PropagateThread* _pt)
	{
		pt_ = _pt;
	}
protected:
	PropagateThread* pt_;

};

class ProxyAjustViewPly : public virtual Proxy
{
public:
	ProxyAjustViewPly( char* plyOutputDir ,char* _prefix ):
		output_file_path_(plyOutputDir),prefix_(_prefix){}

protected:
	ColorType getLabelColor(void* pvtx)
	{

		ColorType pClr = Color_Utility::span_color_from_hy_table( 0 );
		return pClr;
	}

	void getAllSampleCurrentViewMatrix()
	{
		PaintCanvas * mcanvas = Global_Window->getCanvas();
	
	//	std::cout<<"getAllSampleCurrentViewMatrix"<<std::endl:
		SampleSet& smt = SampleSet::get_instance();
		IndexType smpNum  = smt.size();
		for( IndexType i = 0 ; i< smpNum ;++i)
		{
			vector< PointType> smpCoodinates;
			vector< PointType> smpNorms;
			vector<ColorType> smpColors;

			Matrix44 adjust_matrix  = smt[i].matrix_to_scene_coord();
			for( auto vtxbitr  = smt[i].begin() ; vtxbitr!= smt[i].end() ;++vtxbitr){
				Vec4 tmp1(  (*vtxbitr)->x(), (*vtxbitr)->y(), (*vtxbitr)->z(),1.);
				Vec4 point_to_show = adjust_matrix * tmp1;  //adjust_matrix * 
				Vec tmpvec(point_to_show.x() , point_to_show.y() ,point_to_show.z());
				Vec coodinate = mcanvas->camera()->cameraCoordinatesOf( tmpvec );

				PointType tmp2( coodinate.x,coodinate.y  ,coodinate.z );
				smpCoodinates.push_back(tmp2);
				smpNorms.push_back(PointType((*vtxbitr)->nx() ,(*vtxbitr)->ny(),(*vtxbitr)->nz()));
				smpColors.push_back( ColorType( (*vtxbitr)->r() ,(*vtxbitr)->g() , (*vtxbitr)->b() ,1.0));
			}
			
			SmpSetcoodinates.push_back( smpCoodinates );
			SmpSetnorms.push_back(smpNorms);
			SmpSetColors.push_back(smpColors);
		}
	//	std::cout<<"end_getAllSampleCurrentViewMatrix"<<std::endl:
	}
	void writePly()
	{
	    //std::cout<<"writePly begin"<<std::endl:
		std::cout<<"ply "<<std::endl;
		SampleSet& smpset =  SampleSet::get_instance();
		IndexType frameNum = smpset.size();

			for(  IndexType i = 0 ;i< frameNum ;++ i){

				char path[100];
				strcpy(path ,output_file_path_);
				char fullPath[250];
				sprintf( fullPath ,"%s%s%.3d%s",path ,prefix_, i ,".ply");     //必须加入.3d ，使得文件排序正常
				std::ofstream outfile( fullPath , std::ofstream::out);

				outfile<<"ply"<<std::endl;
				outfile<<"format ascii 1.0"<<std::endl;
				IndexType vtxnum = smpset[i].num_vertices();
				outfile<<"element vertex "<< vtxnum<<std::endl;
				outfile<<"property   float   x"<<std::endl;
				outfile<<"property   float   y "<<std::endl;
				outfile<<"property   float   z "<<std::endl;
				outfile<<"property   float   nx"<<std::endl;
				outfile<<"property   float   ny "<<std::endl;
				outfile<<"property   float   nz "<<std::endl;
				outfile<<"property   uchar red "<<std::endl;
				outfile<<"property   uchar   green"<<std::endl;
				outfile<<"property   uchar  blue"<<std::endl;
				outfile<<"end_header"<<std::endl;



				auto smpeitr = SmpSetcoodinates[i].end();
				for( auto smpbitr = SmpSetcoodinates[i].begin() ; smpbitr != smpeitr ; ++ smpbitr){

					PointType& vtx = *smpbitr;
					ColorType pClr = getLabelColor( &vtx );

					outfile<<vtx.x()<<" "<<vtx.y()<<" "<< vtx.z()<<" "<<0<<" "<<0<<" "<<1<<" "<<pClr(0 ,0)<<" "<<pClr(1,0)<<" "<<pClr(2,0)<<std::endl;

				}
				outfile.close();

			}

	
	}
	void run()
	{
		getAllSampleCurrentViewMatrix();
		writePly();


	}

protected:
	char* output_file_path_ ; // ply
	char* prefix_;
	vector< vector<PointType> > SmpSetcoodinates;
	vector< vector<PointType> > SmpSetnorms;
	vector< vector<ColorType> > SmpSetColors;


};



class ProxyVisualLabel: public virtual Proxy
{
public:
	ProxyVisualLabel(char* _label_filename ,char* _corr_filename =NULL): 
	  visual_label_filename_(_label_filename) ,visual_corr_filename_(_corr_filename){}

	void realabel(){
		if(NULL == visual_label_filename_)return;
		pt_->dp_.read_label_file_hier(visual_label_filename_);
	}
	void realabel(char* _label_filename){
		if( NULL ==_label_filename)return;
		visual_label_filename_ = _label_filename;
		pt_->dp_.read_label_file_hier(visual_label_filename_);
	}
	void readCorr()
	{
		if(NULL == visual_corr_filename_)return;
		pt_->dp_.read_corres_file_hier(visual_corr_filename_);
	}
	void readCorr(char* _corr_fileName)
	{
		if(NULL == _corr_fileName)return;
		visual_corr_filename_ = _corr_fileName;
		pt_->dp_.read_corres_file_hier(visual_corr_filename_);

	}
	void showSampleLabel()
	{ 		
		pt_->dp_.initialLabelDisplay();
		pt_->visualCosegmentation(visual_label_filename_);
	}
	void showAllvtxLabel()
	{
		pt_->visualAllvtx();
	}
	virtual void run()
	{
		realabel();
		showSampleLabel();
	}
protected:
	char* visual_label_filename_;
	char* visual_corr_filename_;  //有时候需要借助corr 文件 去 去掉空的元素
};
class ProxyWriteInc :public virtual Proxy{
public:
	ProxyWriteInc(  char* incFileName ,char* afProFilenames )
		: incFileName_(incFileName) ,afProFilenames_(afProFilenames) {

	}
protected:
	void writePovray( char* _incFileName ,char* _afProFilenames)
	{
		incFileName_ = _incFileName;
		afProFilenames_ =  _afProFilenames;
		writeMutiSamplePovray(incFileName_ ,afProFilenames_);

	}
	void writePovray()
	{
	
		writeMutiSamplePovray(incFileName_ ,afProFilenames_);

	}
	virtual void run()
	{
		
		 writePovray();

	}
protected:
	char* afProFilenames_;
	char* incFileName_;

};


class ProxyProOri : public ProxyVisualLabel
{
public: 
	 ProxyProOri( char* label_filename ,char* _corr_fileName = NULL):
		ProxyVisualLabel(label_filename ,_corr_fileName){}
protected:
	void propagate2Ori()
	{
		pt_->propagateLabel2Orignal();
	}
	void propagateOriAndShow()
	{
		realabel();
		readCorr();
		propagate2Ori();
		showAllvtxLabel();
	}
public:
	virtual void run()
	{
		propagateOriAndShow();

	}

};

class ProxyProOriAndWriteInc : public ProxyProOri ,public ProxyWriteInc
{
public:
	ProxyProOriAndWriteInc(char* incFileName ,char* label_filename ,char* _corr_fileName = NULL): 
		ProxyProOri(label_filename ,_corr_fileName) ,ProxyWriteInc(incFileName ,label_filename){}

protected:
	void writeProOrilabel()
	{
		char filetmp[250];
		strcat( filetmp ,visual_label_filename_ );
		strcat( filetmp , ".tmp");
		writeLabelFilenameAfterPropagate(filetmp);
		writePovray( incFileName_ ,filetmp);
	}
	void writeOrivtxPovray()
	{
		

	}


public:
	virtual void run()
	{
		propagateOriAndShow();
		 writeProOrilabel();

	}

};

class ProxyPly : public virtual Proxy
{
public: 
	ProxyPly( char* plyOutputDir ,char* _prefix ):
	output_file_path_(plyOutputDir),prefix_(_prefix){}
protected:
	ColorType getLabelColor(void* pvtx)
	{

		ColorType pClr = Color_Utility::span_color_from_hy_table( ((Vertex*)pvtx)->label() );
		return pClr;
	}
	
	void generatePly()
	{
			SampleSet& smpset =  SampleSet::get_instance();
			IndexType frameNum = smpset.size();
			for( IndexType i = 0 ;i<frameNum ;++i){
				char path[100];
				strcpy(path ,output_file_path_);
				char fullPath[250];
				sprintf( fullPath ,"%s%s%.3d%s",path ,prefix_, i ,".ply");     //必须加入.3d ，使得文件排序正常
				std::ofstream outfile( fullPath , std::ofstream::out);

				outfile<<"ply"<<std::endl;
				outfile<<"format ascii 1.0"<<std::endl;
				IndexType vtxnum = smpset[i].num_vertices();
				outfile<<"element vertex "<< vtxnum<<std::endl;
				outfile<<"property   float   x"<<std::endl;
				outfile<<"property   float   y "<<std::endl;
				outfile<<"property   float   z "<<std::endl;
				outfile<<"property   float   nx"<<std::endl;
				outfile<<"property   float   ny "<<std::endl;
				outfile<<"property   float   nz "<<std::endl;
				outfile<<"property   uchar red "<<std::endl;
				outfile<<"property   uchar   green"<<std::endl;
				outfile<<"property   uchar  blue"<<std::endl;
				outfile<<"end_header"<<std::endl;
				for( auto  vtxbitr = smpset[i].begin() ; vtxbitr != smpset[i].end() ;++vtxbitr ){
					Vertex& vtx = **vtxbitr;
					ColorType pClr = getLabelColor( &vtx );

					outfile<<vtx.x()<<" "<<vtx.y()<<" "<< vtx.z()<<" "<<vtx.nx()<<" "<<vtx.ny()<<" "<<vtx.nz()<<" "<<pClr(0 ,0)<<" "<<pClr(1,0)<<" "<<pClr(2,0)<<std::endl;

				}

				outfile.close();

			}
	}

	

	void generateStandaraPLY(vector< vector<PointType> >& SmpSetcoodinates ,vector< vector<PointType> >& SmpSetNorms ,vector< vector<ColorType> >& SmpSetColors)
	{
		SampleSet& smpset =  SampleSet::get_instance();
		IndexType frameNum = smpset.size();



		ScalarType bottomTox ;
		ScalarType bottomToy ;
		ScalarType bottomToz ;
		ScalarType maxscale = -1;
		ScalarType wrapboxminx ,wrapboxminy ,wrapboxminz;
		wrapboxminx = wrapboxminy = wrapboxminz =1000;
		ScalarType wrapboxmaxx ,wrapboxmaxy ,wrapboxmaxz;
		wrapboxmaxx = wrapboxmaxy = wrapboxmaxz = -1000;
		Sample* smp;
		//先得到所有帧的最大包围盒
		
	
		for( IndexType frameId = 0 ;frameId <frameNum ;++frameId)
		{
			auto smpeitr = SmpSetcoodinates[frameId].end();
			//smp = &(SampleSet::get_instance()[frameId]);
			ScalarType xmin ,xmax ,ymin ,ymax , zmin ,zmax;
			xmin = ymin = zmin = 10000;
			xmax = ymax = zmax = -10000;
			for( auto smpbitr = SmpSetcoodinates[frameId].begin() ; smpbitr != smpeitr ; ++ smpbitr)
			{
				ScalarType xtmp = smpbitr->x();
				if( xtmp < xmin ) xmin = xtmp;
				ScalarType ytmp = smpbitr->y();
				if( ytmp < ymin) ymin = ytmp;
				ScalarType ztmp = smpbitr->z();
				if( ztmp < zmin )zmin = ztmp;
			}
			for( auto smpbitr = SmpSetcoodinates[frameId].begin() ; smpbitr != smpeitr ; ++ smpbitr)
			{
				ScalarType xtmp = smpbitr->x();
				if( xtmp > xmax ) xmax = xtmp;
				ScalarType ytmp = smpbitr->y();
				if( ytmp > ymax) ymax = ytmp;
				ScalarType ztmp = smpbitr->z();
				if( ztmp > zmax )zmax = ztmp;
			}
	/*		for( auto vbitr = smp->begin() ; vbitr != smp->end(); ++vbitr ){
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

			}*/
			wrapboxminx = wrapboxminx<xmin? wrapboxminx:xmin;
			wrapboxminy = wrapboxminy<ymin? wrapboxminy:ymin;
			wrapboxminz = wrapboxminz<zmin? wrapboxminz:zmin;
			wrapboxmaxx = wrapboxmaxx> xmax? wrapboxmaxx : xmax;
			wrapboxmaxy = wrapboxmaxy> ymax? wrapboxmaxy : ymax;
			wrapboxmaxz = wrapboxmaxz> zmax? wrapboxmaxz : zmax;
		}
		/* 映射到 [0 ，1 ]区间*/
		ScalarType xscale = (wrapboxmaxx - wrapboxminx);
		ScalarType yscale = (wrapboxmaxy - wrapboxminy);
		ScalarType zscale = (wrapboxmaxz - wrapboxminz);
		/*得到最大的一维*/
		maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
		bottomTox = 0 - wrapboxminx;
		bottomToy = 0 - wrapboxminy;
		bottomToz = 0 - wrapboxminz;

		for( IndexType i = 0 ;i<frameNum ;++i){
			char path[100];
			strcpy(path ,output_file_path_);
			char fullPath[250];
			sprintf( fullPath ,"%s%s%.3d%s",path ,prefix_, i ,".ply");     //必须加入.3d ，使得文件排序正常
			std::ofstream outfile( fullPath , std::ofstream::out);

			outfile<<"ply"<<std::endl;
			outfile<<"format ascii 1.0"<<std::endl;
			IndexType vtxnum = smpset[i].num_vertices();
			outfile<<"element vertex "<< vtxnum<<std::endl;
			outfile<<"property   float   x"<<std::endl;
			outfile<<"property   float   y "<<std::endl;
			outfile<<"property   float   z "<<std::endl;
			outfile<<"property   float   nx"<<std::endl;
			outfile<<"property   float   ny "<<std::endl;
			outfile<<"property   float   nz "<<std::endl;
			outfile<<"property   uchar red "<<std::endl;
			outfile<<"property   uchar   green"<<std::endl;
			outfile<<"property   uchar  blue"<<std::endl;
			outfile<<"end_header"<<std::endl;
			auto smpnormbitr = SmpSetNorms[i].begin();
			auto smpcolorbitr = SmpSetColors[i].begin();
			for( auto smpbitr = SmpSetcoodinates[i].begin() ; smpbitr != SmpSetcoodinates[i].end() ; ++ smpbitr ,++smpnormbitr,++smpcolorbitr)
			{
				PointType& vtx = *smpbitr;
				PointType& norm = *smpnormbitr;
				ColorType& pClr = *smpcolorbitr;//getLabelColor( &vtx );
				outfile<< (vtx.x()+ bottomTox)/maxscale <<" "<<(vtx.y()+bottomToy)/maxscale <<" "<< (vtx.z()+ bottomToz)/maxscale<<" "<<norm.x()<<" "<<norm.y()<<" "<<norm.z()<<" "<<(int)(pClr(0 ,0)*255)<<" "<<(int)(pClr(1,0)*255)<<" "<<(int)(pClr(2,0)*255)<<std::endl;
			}
			/*for( auto  vtxbitr = smpset[i].begin() ; vtxbitr != smpset[i].end() ;++vtxbitr ){
				Vertex& vtx = **vtxbitr;
				ColorType pClr = getLabelColor( &vtx );

				outfile<< (vtx.x()+ bottomTox)/maxscale <<" "<<(vtx.y()+bottomToy)/maxscale <<" "<< (vtx.z()+ bottomToz)/maxscale<<" "<<vtx.nx()<<" "<<vtx.ny()<<" "<<vtx.nz()<<" "<<pClr(0 ,0)<<" "<<pClr(1,0)<<" "<<pClr(2,0)<<std::endl;

			}*/

			outfile.close();
		}
	}
	void generateStandaraPLY()
	{
		SampleSet& smpset =  SampleSet::get_instance();
		IndexType frameNum = smpset.size();

	
		
		ScalarType bottomTox ;
		ScalarType bottomToy ;
		ScalarType bottomToz ;
		ScalarType maxscale = -1;
		ScalarType wrapboxminx ,wrapboxminy ,wrapboxminz;
		wrapboxminx = wrapboxminy = wrapboxminz =1000;
		ScalarType wrapboxmaxx ,wrapboxmaxy ,wrapboxmaxz;
		wrapboxmaxx = wrapboxmaxy = wrapboxmaxz = -1000;
		Sample* smp;
		//先得到所有帧的最大包围盒
		for( IndexType frameId = 0 ;frameId <frameNum ;++frameId)
		{
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
			wrapboxminx = wrapboxminx<xmin? wrapboxminx:xmin;
			wrapboxminy = wrapboxminy<ymin? wrapboxminy:ymin;
			wrapboxminz = wrapboxminz<zmin? wrapboxminz:zmin;
			wrapboxmaxx = wrapboxmaxx> xmax? wrapboxmaxx : xmax;
			wrapboxmaxy = wrapboxmaxy> ymax? wrapboxmaxy : ymax;
			wrapboxmaxz = wrapboxmaxz> zmax? wrapboxmaxz : zmax;
		}
		/* 映射到 [0 ，1 ]区间*/
		ScalarType xscale = (wrapboxmaxx - wrapboxminx);
		ScalarType yscale = (wrapboxmaxy - wrapboxminy);
		ScalarType zscale = (wrapboxmaxz - wrapboxminz);
		/*得到最大的一维*/
		maxscale =( maxscale=xscale>=yscale?xscale:yscale)>= zscale ? maxscale : zscale;  
		bottomTox = 0 - wrapboxminx;
		bottomToy = 0 - wrapboxminy;
		bottomToz = 0 - wrapboxminz;

		for( IndexType i = 0 ;i<frameNum ;++i){
			char path[100];
			strcpy(path ,output_file_path_);
			char fullPath[250];
			sprintf( fullPath ,"%s%s%.3d%s",path ,prefix_, i ,".ply");     //必须加入.3d ，使得文件排序正常
			std::ofstream outfile( fullPath , std::ofstream::out);

			outfile<<"ply"<<std::endl;
			outfile<<"format ascii 1.0"<<std::endl;
			IndexType vtxnum = smpset[i].num_vertices();
			outfile<<"element vertex "<< vtxnum<<std::endl;
			outfile<<"property   float   x"<<std::endl;
			outfile<<"property   float   y "<<std::endl;
			outfile<<"property   float   z "<<std::endl;
			outfile<<"property   float   nx"<<std::endl;
			outfile<<"property   float   ny "<<std::endl;
			outfile<<"property   float   nz "<<std::endl;
			outfile<<"property   uchar red "<<std::endl;
			outfile<<"property   uchar   green"<<std::endl;
			outfile<<"property   uchar  blue"<<std::endl;
			outfile<<"end_header"<<std::endl;
			for( auto  vtxbitr = smpset[i].begin() ; vtxbitr != smpset[i].end() ;++vtxbitr ){
				Vertex& vtx = **vtxbitr;
				ColorType pClr = getLabelColor( &vtx );

				outfile<< (vtx.x()+ bottomTox)/maxscale <<" "<<(vtx.y()+bottomToy)/maxscale <<" "<< (vtx.z()+ bottomToz)/maxscale<<" "<<vtx.nx()<<" "<<vtx.ny()<<" "<<vtx.nz()<<" "<<pClr(0 ,0)<<" "<<pClr(1,0)<<" "<<pClr(2,0)<<std::endl;

			}

			outfile.close();

		}
	}

	void generateSamplePly( char* _label_filename )
	{
		char* file_sample = _label_filename;

		FILE* in_smpfile = fopen( file_sample ,"r");

		IndexType frameId, labelId, vtxId;

		IndexType cframe = -1;
		IndexType endindex = 0;

		Sample* smp;
		std::ofstream* coutStream =NULL;
		std::map<IndexType , vector<mvertex> > verticeMap;
		vector<mvertex> vtxvec;
		while(true){



			int stat = fscanf( in_smpfile,"%d %d %d\n",  &frameId , &labelId ,&vtxId);
			if(stat==EOF){ //fprintf(outfile ,"#end\n");
				if( NULL!=coutStream)coutStream->close();
				if( cframe != -1){
					verticeMap.insert(  make_pair( cframe ,vtxvec ));
					vtxvec.clear();
				}
				break;
			}

			if( cframe !=frameId){


				smp = &(SampleSet::get_instance()[frameId]);
				if( cframe != -1){
					verticeMap.insert(  make_pair( cframe ,vtxvec ));
					vtxvec.clear();
				}
				cframe = frameId;			
			}

			Vertex& cvtx = SampleSet::get_instance()[frameId][vtxId];

			cvtx.set_label( labelId);
			ColorType pClr = getLabelColor(&cvtx );
			mvertex mvtx;
			mvtx.x = cvtx.x();
			mvtx.y = cvtx.y();
			mvtx.z = cvtx.z();
			mvtx.nx = cvtx.nx();
			mvtx.ny = cvtx.ny();
			mvtx.nz = cvtx.nz();
			mvtx.r = pClr(0 , 0);
			mvtx.g = pClr(1 ,0);
			mvtx.b = pClr(2 ,0);
			vtxvec.push_back( mvtx);
			
		}
		auto framebitr = verticeMap.begin();
		auto frameeitr = verticeMap.end();
		for ( ; framebitr != frameeitr ; ++framebitr){
			cframe  = (*framebitr).first;
			char path[100];
			strcpy(path ,output_file_path_);
			char fullPath[250];
			sprintf( fullPath ,"%s%s%.3d%s",path ,prefix_, cframe ,".ply");     //必须加入.3d ，使得文件排序正常

			if( NULL!=coutStream)coutStream->close();
			coutStream  =  new std::ofstream( fullPath , std::ofstream::out);
			std::ofstream& outfile = *coutStream;

			outfile<<"ply"<<std::endl;
			outfile<<"format ascii 1.0"<<std::endl;
			outfile<<"element vertex "<<  framebitr->second.size()<<std::endl;
			outfile<<"property   float   x"<<std::endl;
			outfile<<"property   float   y "<<std::endl;
			outfile<<"property   float   z "<<std::endl;
			outfile<<"property   float   nx"<<std::endl;
			outfile<<"property   float   ny "<<std::endl;
			outfile<<"property   float   nz "<<std::endl;
			outfile<<"property   uchar red "<<std::endl;
			outfile<<"property   uchar   green"<<std::endl;
			outfile<<"property   uchar  blue"<<std::endl;
			outfile<<"end_header"<<std::endl;


			for (int i=0; i<framebitr->second.size(); i++)
			{				
				mvertex& v =(framebitr->second)[i];
				std::ofstream& outfile = *coutStream;
				outfile<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.nx<<" "<<v.ny<<" "<<v.nz<<" "<<v.r<<" "<<v.g<<" "<<v.b<<std::endl;
			}
			coutStream->close();

		} 
		fclose(in_smpfile);
	}
	public:
		virtual void run()
		{
			generatePly();

		}
protected:
	char* output_file_path_ ; // ply
	char* prefix_;
};
/*
 输出原始点云的原始色彩的ply文件
*/
class ProxyOricolorPly : public  ProxyPly
{
public: 
	ProxyOricolorPly( char* plyOutputDir ,char* _prefix ):
	  ProxyPly(plyOutputDir,_prefix){}
public:
	ColorType getLabelColor(void* vtx)
	{
		ColorType m( (int)(((Vertex*)vtx)->r()*255)  ,(int)(((Vertex*)vtx)->g()*255) ,(int)(((Vertex*)vtx)->b()*255) ,1.);
		return m;
		
	}
	void run()
	{
		generatePly();

	}

}; 

class ProxyLabelSamplePly : public ProxyPly , public ProxyVisualLabel
{
public:
	ProxyLabelSamplePly( char* plyOutputDir ,char* _prefix  , char* _label_filename ,char* _corr_filename =NULL) :
	  ProxyPly(plyOutputDir ,_prefix) ,ProxyVisualLabel( _label_filename ,_corr_filename){}
public: 
	
	void run()
	{
		generateSamplePly(visual_label_filename_);
		
	}


};


class ProxyRedColorPly : public ProxyPly
{
public: 
	ProxyRedColorPly( char* plyOutputDir ,char* _prefix ):
	  ProxyPly(plyOutputDir,_prefix){}
public:
	ColorType getLabelColor(void* vtx)
	{
		ColorType m( 171,48,46,1);
		return m;

	}
	void run()
	{
		generatePly();

	}

};

class ProxyGrayColorPly : public ProxyPly
{
public: 
	ProxyGrayColorPly( char* plyOutputDir ,char* _prefix ):
	  ProxyPly(plyOutputDir,_prefix){}
public:
	ColorType getLabelColor(void* vtx)
	{
		ColorType m( 207,207,207,1);
		return m;

	}
	void run()
	{
		generatePly();

	}

};
class ProxyStandaraGrayColorPly : public ProxyPly
{
public: 
	ProxyStandaraGrayColorPly( char* plyOutputDir ,char* _prefix ):
	  ProxyPly(plyOutputDir,_prefix){}
public:
	ColorType getLabelColor(void* vtx)
	{
		ColorType m( 207,207,207,1);
		return m;

	}
	void run()
	{
		generateStandaraPLY();

	}

};

class ProxyStandaraOriColorPly : public ProxyPly
{
public: 
	ProxyStandaraOriColorPly( char* plyOutputDir ,char* _prefix ):
	  ProxyPly(plyOutputDir,_prefix){}
public:
	ColorType getLabelColor(void* vtx)
	{
		ColorType m( (int)(((Vertex*)vtx)->r()*255)  ,(int)(((Vertex*)vtx)->g()*255) ,(int)(((Vertex*)vtx)->b()*255) ,1.);
		return m;
	}
	void run()
	{
		generateStandaraPLY();

	}

};

class ProxyStandaraAjustViewOriColorPly : public ProxyStandaraOriColorPly , public ProxyAjustViewPly
{
public: 
	ProxyStandaraAjustViewOriColorPly( char* plyOutputDir ,char* _prefix ):ProxyStandaraOriColorPly(plyOutputDir,_prefix),ProxyAjustViewPly( plyOutputDir,_prefix ){}
public:
	ColorType getLabelColor(void* vtx){
		return ProxyStandaraOriColorPly::getLabelColor(vtx);
	}
	void run()
	{
		getAllSampleCurrentViewMatrix();
		generateStandaraPLY(SmpSetcoodinates ,SmpSetnorms ,SmpSetColors);

	}

};

class ProxyProOrigAndPly : public ProxyProOri
{
public:
	ProxyProOrigAndPly(char* plyOutputDir ,char* _prefix ,char* label_filename ,char* _corr_fileName = NULL ):ProxyProOri(label_filename ,_corr_fileName) ,
		output_file_path_(plyOutputDir),prefix_(_prefix){}

protected:

	void generatePly()
	{
		SampleSet& smpset =  SampleSet::get_instance();
		IndexType frameNum = smpset.size();
		for( IndexType i = 0 ;i<frameNum ;++i){
			char path[100];
			strcpy(path ,output_file_path_);
			char fullPath[250];
			sprintf( fullPath ,"%s%s%.3d%s",path ,prefix_, i ,".ply");     //必须加入.3d ，使得文件排序正常
			std::ofstream outfile( fullPath , std::ofstream::out);

			outfile<<"ply"<<std::endl;
			outfile<<"format ascii 1.0"<<std::endl;
			IndexType vtxnum = smpset[i].num_vertices();
			outfile<<"element vertex "<< vtxnum<<std::endl;
			outfile<<"property   float   x"<<std::endl;
			outfile<<"property   float   y "<<std::endl;
			outfile<<"property   float   z "<<std::endl;
			outfile<<"property   float   nx"<<std::endl;
			outfile<<"property   float   ny "<<std::endl;
			outfile<<"property   float   nz "<<std::endl;
			outfile<<"property   uchar red "<<std::endl;
			outfile<<"property   uchar   green"<<std::endl;
			outfile<<"property   uchar  blue"<<std::endl;
			outfile<<"end_header"<<std::endl;
			for( auto  vtxbitr = smpset[i].begin() ; vtxbitr != smpset[i].end() ;++vtxbitr ){
				Vertex& vtx = **vtxbitr;
				ColorType pClr = Color_Utility::span_color_from_hy_table(vtx.label() );

				outfile<<vtx.x()<<" "<<vtx.y()<<" "<< vtx.z()<<" "<<vtx.nx()<<" "<<vtx.ny()<<" "<<vtx.nz()<<" "<<pClr(0 ,0)<<" "<<pClr(1,0)<<" "<<pClr(2,0)<<std::endl;
			
			}
	
			outfile.close();

		}
		
	}
public:
	virtual void run()
	{
		propagateOriAndShow();
		generatePly();
		
		
	}
protected:
	char* output_file_path_ ; // ply
	char* prefix_;

};
class Proxy_Propagate:public virtual Proxy{
protected:
	Proxy_Propagate(IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame):
		 startFrame_(_startFrame),endFrame_(_endFrame) ,centerFrame_(_centerFrame)
		 {

		 }
public:
	Proxy_Propagate( char* _label_filename ,char* _corr_filename , IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame): 
	  label_filename_(_label_filename) , corr_filename_(_corr_filename),startFrame_(_startFrame),endFrame_(_endFrame) ,centerFrame_(_centerFrame){

	  }
	  void Propagate( char* _label_filename ,char* _corr_filename , IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame)
	  {
		  label_filename_= _label_filename ;
		  corr_filename_ = _corr_filename;
		  startFrame_ = _startFrame;
		  endFrame_ = _endFrame;
		  centerFrame_ = _centerFrame;
		  Propagate();
	  }
	  void Propagate()
	  {
		  pt_->dp_.read_label_file_hier(label_filename_);
		  pt_->dp_.read_corres_file_hier(corr_filename_);

		  pt_->dp_.init_labeles_graph_hier2();
		  for( int i = centerFrame_ ;i < endFrame_;++i){
			  pt_->split_twoAjacent_graph_next( pt_->dp_,i ,i+1);

		  }
		  for( int i = centerFrame_ ;i > startFrame_;--i){
			  pt_->split_twoAjacent_graph_prev( pt_->dp_,i-1 ,i);

		  }
	  }
	  //should be called before run;

	  virtual void run(){
		  Propagate();
	  }
	  char* getLabelFileName()
	  {
		  return label_filename_;
	  }
protected:
	char* label_filename_;
	char* corr_filename_;
	IndexType startFrame_;
	IndexType endFrame_;
	IndexType centerFrame_;


};
class Proxy_PropagateAndVisual:public  Proxy_Propagate , public ProxyVisualLabel
{
public:
	//Proxy_PropagateAndVisual( char* _label_filename ,char* _corr_filename , IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame ):
	  Proxy_PropagateAndVisual( char* _label_filename1 , char* _corr_filename2 ,IndexType _startFrame1, IndexType _endFrame1 ,IndexType _centerFrame1):
	  Proxy_Propagate(_label_filename1 ,_corr_filename2, _startFrame1 ,_endFrame1,_centerFrame1) , ProxyVisualLabel(_label_filename1 ,_corr_filename2){}
public: 
	void run()
	{
		Propagate();
		showSampleLabel();

	}


};




class  ProxyMergeAndPropagate : public Proxy_Propagate
{
public:
	ProxyMergeAndPropagate( char* CORR_FILESNAME,char* CORR_FILEOUT_NAME,char* LABEL_FILESNAME,char* LABEL_FILEOUT_NAME,
		char* centerFileName,
		IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame):
	Proxy_Propagate(_startFrame ,_endFrame ,_centerFrame) ,CORR_FILESNAME_(CORR_FILESNAME) , CORR_FILEOUT_NAME_(CORR_FILEOUT_NAME) , LABEL_FILESNAME_(LABEL_FILESNAME) , LABEL_FILEOUT_NAME_(LABEL_FILEOUT_NAME),centerFileName_(centerFileName)
	{

	}
protected:
	void replace(char* centerFileName_ , char* targetFileName ,char* newtargetFileName)
	{
		FILE* centerFile = fopen( centerFileName_ ,"r");
		FILE* targetFile = fopen( targetFileName , "r");
		FILE* newtargetFile = fopen( newtargetFileName ,"w");
		//	char targetBuff[50];
		IndexType targetFrameId;
		IndexType targetLabel;
		IndexType targetVtxId;
		//	char centerBuff[50];
		IndexType centerFrameId;
		IndexType centerLabel;
		IndexType centerVtxId;
		while( true){
			IndexType centerScanState = fscanf( centerFile , "%d %d %d\n" , &centerFrameId ,&centerLabel ,&centerVtxId);	     
			IndexType TscanState =  fscanf(targetFile,"%d %d %d\n" ,  &targetFrameId ,&targetLabel ,&targetVtxId);
			while( (targetFrameId != centerFrameId) &&(EOF !=TscanState) ){
				fprintf(newtargetFile, "%d %d %d\n", targetFrameId ,targetLabel ,targetVtxId);
				TscanState =  fscanf(targetFile ,"%d %d %d\n" ,  &targetFrameId ,&targetLabel ,&targetVtxId);
			}
			if( EOF == TscanState ) break;
			if( centerScanState){
				fprintf(newtargetFile, "%d %d %d\n", centerFrameId ,centerLabel ,centerVtxId);
			}else{
				fprintf(newtargetFile, "%d %d %d\n", targetFrameId ,targetLabel ,targetVtxId);
			}
		}
		fclose( centerFile);
		fclose( targetFile);
		fclose(newtargetFile);

	}
	void merge()
	{	
		char fn[100];
		strcpy (fn , LABEL_FILEOUT_NAME_);
		strcat( fn ,".tmp");
		mergeFile(  CORR_FILESNAME_ ,CORR_FILEOUT_NAME_ ,LABEL_FILESNAME_ ,fn);
		replace(  centerFileName_ ,fn ,LABEL_FILEOUT_NAME_ );
	}

public:
	virtual void run()
	{
		merge();
		Propagate( LABEL_FILEOUT_NAME_ ,CORR_FILEOUT_NAME_ , startFrame_ , endFrame_, centerFrame_ );

	}
protected:
	char* CORR_FILESNAME_;
	char* CORR_FILEOUT_NAME_;
	char* LABEL_FILESNAME_ ;
	char* LABEL_FILEOUT_NAME_;
	char* centerFileName_;

};

class ProxyWriteMergePropagate : public ProxyMergeAndPropagate
{
public:
	ProxyWriteMergePropagate(  char* afProFilenames, char* incFileName ,char* CORR_FILESNAME,char* CORR_FILEOUT_NAME,char* LABEL_FILESNAME,char* LABEL_FILEOUT_NAME,
		char* centerFileName,
		IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame)
		:ProxyMergeAndPropagate( CORR_FILESNAME,CORR_FILEOUT_NAME,LABEL_FILESNAME,LABEL_FILEOUT_NAME,centerFileName,
		_startFrame ,  _endFrame ,_centerFrame) ,afProFilenames_(afProFilenames) , incFileName_(incFileName)
	{

	}
	void write()
	{
		pt_->dp_.wirteGraphLablesAtTop(afProFilenames_);
		writeMutiSamplePovray(incFileName_ ,afProFilenames_);

	}
	virtual void run()
	{
		merge();
		Propagate( LABEL_FILEOUT_NAME_ ,CORR_FILEOUT_NAME_ , startFrame_ , endFrame_, centerFrame_ );
		write();

	}
protected:
	char* afProFilenames_;
	char* incFileName_;

};

#ifdef COMPILE_ALL
char fandata[30][250] ={
	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	
	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	
	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	
	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	
	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	
	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	"H:\\povay_pointcloud\\point_data\\fans_ply\\",	
	
};
ProxyVisualLabel fanPrppagateVisual(
	strcat( fandata[0] ,"fanslabeloutputafterPropagate0_24center17.txt")
	
	);
ProxyOricolorPly fanOriPly(
	strcat( fandata[1] ,"ply\\totLabels\\"),
	"fanOri"
	);
ProxyProOrigAndPly fansplitPly(
	strcat( fandata[4] ,"ply\\split\\"),
	"fansplit",
	strcat(	fandata[5] ,"fanslabeloutputafterPropagate0_24center17.txt"),
	strcat( fandata[6] ,"201508030corr_fans0_24.txt")
	);
ProxyLabelSamplePly fansplitsamplePly(
	strcat( fandata[7] ,"ply\\splitsample\\"),
	"fansplitsamplePly",
	strcat(	fandata[8] ,"fanslabeloutputafterPropagate0_24center17.txt"),
	strcat( fandata[9] ,"201508030corr_fans0_24.txt")
	);
Proxy_Propagate fanPropagate(
	strcat(	fandata[11] ,"fanslabeloutputafterPropagate0_24center17.txt"),
	strcat( fandata[12] ,"201508030corr_fans0_24.txt"),0 ,24 ,17
	
	);



char cabinetLabelAndCORR[30][250] = {
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\2015918LabelsCorr\\LabelsCorr\\"	
};
char  cabinetData[30][250] = {
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\",
	"H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\","H:\\povay_pointcloud\\point_data\\cabinet20150904\\",

};

ProxyVisualLabel cabinetPrppagateVisual(
	strcat( cabinetData[0] ,"cabinetlabeloutputafterPropagate0_29center18.txt")
	);


ProxyWriteMergePropagate cabinetDataProxy( strcat( cabinetData[1] ,"cabinetlabeloutputafterPropagate0_29center18.txt") ,
	strcat( cabinetData[2] ,"inc\\cabinetlabeloutputafterPropagate0_29center18.inc"),
	strcat(cabinetLabelAndCORR[0] , "corr\\"), strcat( cabinetLabelAndCORR[1] ,"corr0_29center18.txt"),strcat( cabinetLabelAndCORR[2] ,"label\\"),
	strcat( cabinetLabelAndCORR[3] ,"label_0_29center18.txt"),strcat(cabinetData[3],"cabinet_Labels18.txt"),0 ,29 ,18 );


ProxyOricolorPly cabinetOriPly(
	strcat( cabinetData[4] ,"ply\\totLabels\\"),
	"cabinetOriPly"
	);


ProxyProOrigAndPly cabinetSplitPly(
	strcat( cabinetData[7] ,"ply\\split\\"),
	"cabinetSplitPly",
	strcat(	cabinetData[8] ,"cabinetlabeloutputafterPropagate0_29center18.txt"),
	strcat( cabinetData[9] ,"corr0_29center18.txt")
	);
ProxyLabelSamplePly cabinetSplitSamplePly(
	strcat( cabinetData[10] ,"ply\\splitsample\\"),
	"cabinetSplitSamplePly",
	strcat(	cabinetData[11] ,"cabinetlabeloutputafterPropagate0_29center18.txt"),
	strcat( cabinetData[12] ,"corr0_29center18.txt")
	);
Proxy_Propagate cabinetPropagate(
	strcat(	cabinetData[13] ,"cabinetlabeloutputafterPropagate0_29center18.txt"),
	strcat( cabinetData[14] ,"corr0_29center18.txt"),0 ,29 ,18

	);


char pandaLabelAndCORR[30][250] = {
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\",
	"H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\","H:\\povay_pointcloud\\point_data\\panda20150918\\"

};
ProxyVisualLabel pandaInitVisual(
	strcat( pandaLabelAndCORR[0] ,"0912update7TotLabels(2_10).txt")
);
ProxyVisualLabel pandaMergeVisual(
	strcat( pandaLabelAndCORR[1] ,"0918MergeCosegLabels(2_10)_coseg.txt")
);
ProxyVisualLabel pandaSplitVisual(

	strcat( pandaLabelAndCORR[2] ,"0912SplitLabels(2_10).txt")
);
ProxyVisualLabel pandaCogVisual(
	strcat( pandaLabelAndCORR[3] ,"0912CosegLabels(2_10).txt")
);


ProxyWriteInc pandaInit(
	strcat( pandaLabelAndCORR[4] ,"inc\\0912update7TotLabels(2_10).inc"),
	strcat( pandaLabelAndCORR[5] ,"0912update7TotLabels(2_10).txt")
	);
ProxyWriteInc pandaSplit(
	strcat( pandaLabelAndCORR[6] ,"inc\\0912SplitLabels(2_10).inc"),
	strcat( pandaLabelAndCORR[7] ,"0912SplitLabels(2_10).txt")
	
	);
ProxyWriteInc pandaCog(
	strcat( pandaLabelAndCORR[8] ,"inc\\0912CosegLabels(2_10).inc"),
	strcat( pandaLabelAndCORR[9] ,"0912CosegLabels(2_10).txt")
	);

ProxyWriteInc pandaMerge(
	strcat( pandaLabelAndCORR[10] ,"inc\\0918MergeCosegLabels(2_10)_coseg.inc"),
	strcat( pandaLabelAndCORR[11] ,"0918MergeCosegLabels(2_10)_coseg.txt")
	);

ProxyRedColorPly pandaOriply(
	strcat( pandaLabelAndCORR[12] ,"ply\\orig\\"),
	"pandaOriply"
	);
ProxyGrayColorPly pandaOriGrayply(
	strcat( pandaLabelAndCORR[13] ,"ply\\orig\\"),
	"pandaOriGrayply"
	);
ProxyProOrigAndPly pandaSplitply(
	strcat( pandaLabelAndCORR[15] ,"ply\\split\\"),
	"pandaSplitply",
	strcat(	pandaLabelAndCORR[16] ,"0920SplitLargeLabels(2_10).txt"),
	strcat( pandaLabelAndCORR[17] ,"0904TotCorr_panther(2_10).txt")
	);
ProxyProOrigAndPly pandaFinalCoseply(
	strcat( pandaLabelAndCORR[18] ,"ply\\merge\\"),
	"pandaFinalCosegply",
	strcat(	pandaLabelAndCORR[19] ,"0920FinalCosegLabels(2_10)_copy.txt"),
	strcat( pandaLabelAndCORR[20] ,"0904TotCorr_panther(2_10).txt")
	);
ProxyProOrigAndPly pandaUpdateply(
	strcat( pandaLabelAndCORR[21] ,"ply\\update\\"),
	"pandaUpdateply",
	strcat(	pandaLabelAndCORR[22] ,"0920update7TotLabels(2_10)_edit.txt"),
	strcat( pandaLabelAndCORR[23] ,"0904TotCorr_panther(2_10).txt")
	);
//
//Proxy_PropagateAndVisual PandaPropagate1( 
//	strcat( pandaLabelAndCORR[24] ,"pandalabelandcorr\\totLabelFile(10_39).txt"),strcat( pandaLabelAndCORR[25] ,"pandalabelandcorr\\totcorrFile(10_39).txt") ,
//	NULL ,NULL , 10,39,10
// ):

Proxy_PropagateAndVisual pandaPropagateVisual(
	strcat( pandaLabelAndCORR[24] ,"pandalabelandcorr\\totLabelFile(10_39).txt"),
	strcat( pandaLabelAndCORR[25] ,"pandalabelandcorr\\totcorrFile(10_39).txt") ,
	10,39,10);

//Proxy_Propagate PandaPropagate1(
//	strcat( pandaLabelAndCORR[24] ,"pandalabelandcorr\\totLabelFile(10_39).txt"),
//	strcat( pandaLabelAndCORR[25] ,"pandalabelandcorr\\totcorrFile(10_39).txt") ,
//	 10,39,10);



//
//#ifdef _COMPILE_THIS_
//#endif

char twogirlShakeHand[30][250] = {
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",	"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\",
};

ProxyProOrigAndPly twogirlDanceOriPly(
	strcat(twogirlShakeHand[0] ,"ply\\orig\\"),
	"twogirlDanceOri",
	strcat(twogirlShakeHand[1] ,"totOriSegLabels(30_38).txt")
	);

ProxyProOrigAndPly twogirlDanceSplitPly(
	strcat(twogirlShakeHand[2] ,"ply\\Split\\"),
	"twogirlDanceSplit",
	strcat(twogirlShakeHand[3] ,"totLabelsAfterDiffusion(30_38)_split.txt")
	);

ProxyProOrigAndPly twogirlDanceMergePly(
	strcat(twogirlShakeHand[4] ,"ply\\Merge\\"),
	"twogirlDanceMerge",
	strcat(twogirlShakeHand[5] ,"final_merge_girlsResults(30_38).txt"),
	 strcat( twogirlShakeHand[6], "totCorr(30_38).txt")
	);

ProxyWriteInc twogirlOri(
	strcat( twogirlShakeHand[7] , "inc\\twogirlori(30_38).inc"),
	strcat( twogirlShakeHand[8] , "totOriSegLabels(30_38).txt")
	);
ProxyWriteInc twogirlAfter(
	strcat( twogirlShakeHand[9] , "inc\\AfterDiffusion(30_38).inc"),
	strcat( twogirlShakeHand[10] , "totLabelsAfterDiffusion(30_38)_split.txt")
	);
ProxyWriteInc twogirlMerge(
	strcat( twogirlShakeHand[11] , "inc\\final_merge(30_38).inc"),
	strcat( twogirlShakeHand[12] , "final_merge_girlsResults(30_38).txt")
	);
ProxyAjustViewPly twogirlDanceOriAjustViewPly(
	strcat( twogirlShakeHand[13] ,"ply\\orig\\"),
	"twogirlDanceOriAjustViewPly"	
	);
ProxyAjustViewPly twogirlDanceSplitAjustViewPly(
	strcat( twogirlShakeHand[14] ,"ply\\Split\\"),
	"twogirlDanceSplitAjustViewPly"	
	);
ProxyAjustViewPly twogirlDanceMergeAjustViewPly(
	strcat( twogirlShakeHand[15] ,"ply\\Merge\\"),
	"twogirlDanceMergeAjustViewPly"	
	);

ProxyRedColorPly twogirlDance(
	strcat(twogirlShakeHand[16] ,"ply\\Ori\\"),
	"twogirlDanceOri"
	);

ProxyProOrigAndPly twogirlDanceProOriMergeply(
	strcat( twogirlShakeHand[17] ,"ply\\Merge\\"),
	"twogirlDanceProOriMergeply",
	strcat(	twogirlShakeHand[18] ,"final_merge_girlsResults(30_38).txt"),
	strcat( twogirlShakeHand[19] ,"totCorr(30_38).txt")
	);
ProxyProOriAndWriteInc twogirlDanceAfterPro_MergeInc(
	strcat( twogirlShakeHand[20] , "inc\\twogirlAterPropagate_merge(30_38).inc"),
	strcat(twogirlShakeHand[22] ,"final_merge_girlsResults(30_38).txt"),
	strcat( twogirlShakeHand[23], "totCorr(30_38).txt")
	);

ProxyLabelSamplePly twogirlDanceSamplePly(
	strcat( twogirlShakeHand[24] ,"ply\\Merge\\sample\\"),
	"twogirlDanceSamplePly",
	strcat(	twogirlShakeHand[25] ,"final_merge_girlsResults(30_38).txt"),
	strcat( twogirlShakeHand[26] ,"totCorr(30_38).txt")
	);
ProxyGrayColorPly twogirlGrayColorDance(
	strcat(twogirlShakeHand[27] ,"ply\\OneLabel\\sample_red\\"),
	"twogirlGrayColorDance"
	);
Proxy_PropagateAndVisual twogirl38_50(
	strcat(	twogirlShakeHand[28] ,"resetLabelsandCorr\\twogirlshakehandlabel_0_173 - center38.txt"),
	strcat( twogirlShakeHand[29] ,"resetLabelsandCorr\\twogirlshakehandcorr_0_173.txt"),
	38,
	50,
	38
);

#ifdef _COMPILE_THIS_
#endif


char horseDir[60][250] = {
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\",
	"H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\","H:\\povay_pointcloud\\point_data\\horse\\"
};


ProxyWriteInc horsetotLabelsINC(
	strcat( horseDir[0] , "inc\\totLabels(1_9).inc"),
	strcat( horseDir[1] , "totLabels(1_9).txt")
	);
ProxyWriteInc horsesplitINC(
	strcat( horseDir[2] , "inc\\0919splitResults.inc"),
	strcat( horseDir[3] , "0919splitResults.txt")
	);
ProxyWriteInc horseMergeINC(
	strcat( horseDir[4] , "inc\\0918mergeTinyPartes_testAgainAgain.inc"),
	strcat( horseDir[5] , "0918mergeTinyPartes_testAgainAgain.txt")
	);

ProxyProOrigAndPly horseOriPly(
	strcat( horseDir[6] ,"ply\\totLabels\\"),
	"HorseOri",
	strcat( horseDir[7] ,"totLabels(1_9).txt"),
	strcat( horseDir[8] ,"totCorr(1_9).txt")
	);
ProxyProOrigAndPly horseSplitPly(
	strcat( horseDir[9] ,"ply\\split\\"),
	"HorseSplit",
	strcat( horseDir[10] ,"0919splitResults.txt"),
	strcat( horseDir[11] ,"totCorr(1_9).txt")
	);
ProxyProOrigAndPly horseMergePly(
	strcat( horseDir[12] ,"ply\\merge\\"),
	"HorseMerge",
	strcat( horseDir[13] ,"0918mergeTinyPartes_testAgainAgain.txt"),
	strcat( horseDir[14] ,"totCorr(1_9).txt")
	);
ProxyProOrigAndPly horseOnelabelPly(
	strcat( horseDir[15] ,"ply\\onelabel\\"),
	"horseOnelabelPly",
	strcat( horseDir[16] ,"onelabel(1_9).txt"),
	strcat( horseDir[17] ,"totCorr(1_9).txt")
	);
ProxyProOri horseOri(	
	strcat( horseDir[18] ,"totLabels(1_9).txt"),
	strcat( horseDir[19] ,"totCorr(1_9).txt")
	);
ProxyProOri horseSplit(
	strcat( horseDir[20] ,"0919splitResults.txt"),
	strcat( horseDir[21] ,"totCorr(1_9).txt")
	);	
ProxyProOri horseMerge(
	strcat( horseDir[22] ,"0918mergeTinyPartes_testAgainAgain.txt"),
	strcat( horseDir[23] ,"totCorr(1_9).txt")
	);
ProxyProOrigAndPly horseOrinewPly(
	strcat( horseDir[24] ,"ply\\totLabels\\new\\"),
	"horseOrinewPly",
	strcat( horseDir[25] ,"new2015924\\totLabels(1_9).txt"),
	strcat( horseDir[26] ,"totCorr(1_9).txt")
	);
ProxyProOrigAndPly horseCognewPly(
	strcat( horseDir[27] ,"ply\\cog\\"),
	"horseCognewPly",
	strcat( horseDir[28] ,"new2015924\\finalcoseglabels(1_9).txt"),
	strcat( horseDir[29] ,"totCorr(1_9).txt")
	);

ProxyGrayColorPly horseGrayOriPly(
	strcat( horseDir[30] ,"ply\\onelabel\\gray\\"),
	"horseGrayOriPly"
	);

#ifdef _COMPILE_THIS_
#endif


char hansDir[30][250] = {
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
	"H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\","H:\\povay_pointcloud\\point_data\\finger\\",
};

ProxyProOrigAndPly  hansProOri_oriPly(
	strcat( hansDir[0] ,"ply\\ori\\"),
	"hansProOri_oriPly",
	strcat(	hansDir[1] ,"singleframeseg(1_6).txt"),
	strcat( hansDir[2] ,"0909FingureTot_Corr(1_6).txt")
	
	);
ProxyProOrigAndPly  hansProOri_mergePly(
	strcat( hansDir[3] ,"ply\\merge\\"),
	"hansProOri_mergePly",
	strcat(	hansDir[4] ,"totLabels(1_6)_0922.txt"),
	strcat( hansDir[5] ,"0909FingureTot_Corr(1_6).txt")

	);
ProxyProOrigAndPly  hansProOri_merge20150925Ply(
	strcat( hansDir[6] ,"ply\\merge\\1_6_2015925\\"),
	"hansProOri_merge20150925Ply",
	strcat(	hansDir[7] ,"finalMerge(1_6)_inter.txt"),
	strcat( hansDir[8] ,"0909FingureTot_Corr(1_6).txt")
	);


char manDanceDir[30][250] = {
	"H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\",
	"H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\",
	"H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\",
	"H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\",
	"H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\","H:\\povay_pointcloud\\point_data\\mandance\\"
};

ProxyProOrigAndPly  mandanceProOri_samplePly(
	strcat( manDanceDir[0] ,"ply\\sample\\"),
	"mandanceProOri_samplePly",
	strcat(	manDanceDir[1] ,"0511_smooth_sample20.txt"),
	strcat( manDanceDir[2] ,"0511_corr20_0.70.txt")

	);

ProxyProOrigAndPly  mandanceProOri_ransancPly(
	strcat( manDanceDir[3] ,"ply\\ransanc\\"),
	"mandanceProOri_ransancPly",
	strcat(	manDanceDir[4] ,"0528_smooth_ransac20.txt"),
	strcat( manDanceDir[5] ,"0511_corr20_0.70.txt")

	);

char grilDanceDir[30][250] ={
	"H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\",
	"H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\",
	"H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\",
	"H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\",
	"H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\","H:\\povay_pointcloud\\point_data\\girldance\\"
};

ProxyProOrigAndPly  grildanceProOri_segDiffusion11Ply(
	strcat( grilDanceDir[0] ,"ply\\"),
	"grildanceProOri_segDiffusion11Ply",
	strcat(	grilDanceDir[1] ,"segDiffusion11.txt")
	);
ProxyProOrigAndPly  grildanceProOri_segDiffusion12Ply(
	strcat( grilDanceDir[2] ,"ply\\"),
	"grildanceProOri_segDiffusion12Ply",
	strcat(	grilDanceDir[3] ,"segDiffusion12.txt")
	);
ProxyProOrigAndPly  grildanceProOri_segDiffusion13Ply(
	strcat( grilDanceDir[4] ,"ply\\"),
	"grildanceProOri_segDiffusion13Ply",
	strcat(	grilDanceDir[5] ,"segDiffusion13.txt")
	);
ProxyProOrigAndPly  grildanceProOri_segDiffusion15Ply(
	strcat( grilDanceDir[6] ,"ply\\"),
	"grildanceProOri_segDiffusion15Ply",
	strcat(	grilDanceDir[7] ,"segDiffusion15.txt")
	);

char girlWalkDir[30][250] = {
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\",
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\",
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\",
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\",
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\","H:\\povay_pointcloud\\point_data\\shiqun\\girl_walk\\girl_walk\\"
};

ProxyOricolorPly girlWalk(
	strcat( girlWalkDir[0] ,"ply\\"),
	"girlwalkori-"

	);

ProxyLabelSamplePly girlWalkKmeans(
	strcat( girlWalkDir[1] ,"ply\\"),
	"girlWalkKmeans-",
	strcat( girlWalkDir[2] ,"k-means\\n001.label"),
	strcat( girlWalkDir[3] ,"k-means\\001.corr")

	);

ProxyLabelSamplePly girlWalkSpectralClustering(
	strcat( girlWalkDir[4] ,"ply\\"),
	"girlwalkSpectralClustering-",
	strcat( girlWalkDir[5] ,"spectral_clustering\\n001.label"),
	strcat( girlWalkDir[6] ,"spectral_clustering\\001.corr")
	);

ProxyLabelSamplePly girlWalkJlinkage(
	strcat( girlWalkDir[7] ,"ply\\"),
	"girlwalkJlinkage-",
	strcat( girlWalkDir[8] ,"J-linkage\\n001.label"),
	strcat( girlWalkDir[9] ,"J-linkage\\001.corr")
	);

char girlRaiseHandDir[30][250] = {
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,
	"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,"H:\\povay_pointcloud\\point_data\\shiqun\\girl_shake_hand\\" ,
};

ProxyProOrigAndPly girlAterPropagate0_29_center5(
	strcat( girlRaiseHandDir[0] ,"ply\\"),
	"girlAterPropagate0_29_center5",
	strcat( girlRaiseHandDir[1] ,"Propagation_Info\\Propagation_Info\\labeloutputafterPropagate0_29center5.txt"),
	strcat( girlRaiseHandDir[2] ,"Propagation_Info\\Propagation_Info\\corroutput0_29center5.txt")

	);

ProxyLabelSamplePly girlAterPropagate0_29_center10sample(
	strcat( girlRaiseHandDir[3] ,"ply\\sample\\"),
	"girlAterPropagate0_29_center10sample-",
	strcat( girlRaiseHandDir[4] ,"Propagation_Info\\Propagation_Info\\centeris10\\labeloutputafterPropagate0_29.txt"),
	strcat( girlRaiseHandDir[5] ,"Propagation_Info\\Propagation_Info\\centeris10\\corroutput0_29.txt")

	);
ProxyOricolorPly girlraisehandOricolor(
	strcat( girlRaiseHandDir[6] ,"ply\\oricolor\\"),
	"girlraisehandOricolor-"

	);

ProxyProOrigAndPly girlAterPropagate0_29_center10(
	strcat( girlRaiseHandDir[7] ,"ply\\orisplit\\"),
	"girlAterPropagate0_29_center10-",
	strcat( girlRaiseHandDir[8] ,"Propagation_Info\\Propagation_Info\\centeris10\\labeloutputafterPropagate0_29.txt"),
	strcat( girlRaiseHandDir[9] ,"Propagation_Info\\Propagation_Info\\centeris10\\corroutput0_29.txt")

	);
#endif
char horsenew[30][250] = {
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view00\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view01\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view02\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view03\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view04\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view05\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view06\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view07\\norm\\" ,
	"F:\\VisualcamerAndYaobing\\horsebackinter08\\backinter08\\out\\view08\\norm\\" 
};
ProxyRedColorPly horsrplyview0( horsenew[0],"horse_view_0_0_addnorm_file_" );
ProxyRedColorPly horsrplyview1( horsenew[1],"horse_view_0_1_addnorm_file_" );
ProxyRedColorPly horsrplyview2( horsenew[2],"horse_view_0_2_addnorm_file_" );
ProxyRedColorPly horsrplyview3( horsenew[3],"horse_view_0_3_addnorm_file_" );
ProxyRedColorPly horsrplyview4( horsenew[4],"horse_view_0_4_addnorm_file_" );
ProxyRedColorPly horsrplyview5( horsenew[5],"horse_view_0_5_addnorm_file_" );
ProxyRedColorPly horsrplyview6( horsenew[6],"horse_view_0_6_addnorm_file_" );
ProxyRedColorPly horsrplyview7( horsenew[7],"horse_view_0_7_addnorm_file_" );

#ifdef _COMPILE_THIS_
#endif
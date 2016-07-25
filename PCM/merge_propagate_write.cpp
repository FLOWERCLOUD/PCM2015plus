#include "merge_propagate_write.h"
#include "paint_canvas.h"
#include "sample_set.h"
#include "vertex.h"


void ProxyAjustViewPly::getAllSampleCurrentViewMatrix()
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

void ProxyAjustViewPly::run()
{
	getAllSampleCurrentViewMatrix();
	writePly();
}

void ProxyAjustViewPly::writePly()
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

ColorType ProxyAjustViewPly::getLabelColor(void* pvtx)
{
	ColorType pClr = Color_Utility::span_color_from_hy_table( 0 );
	return pClr;
}

ProxyAjustViewPly::ProxyAjustViewPly(char* plyOutputDir ,char* _prefix) :
output_file_path_(plyOutputDir),prefix_(_prefix)
{

}

void ProxyPly::run()
{
	generatePly();
}

void ProxyPly::generateSamplePly(char* _label_filename)
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

void ProxyPly::generateStandaraPLY()
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

void ProxyPly::generateStandaraPLY(vector< vector<PointType> >& SmpSetcoodinates ,vector< vector<PointType> >& SmpSetNorms ,vector< vector<ColorType> >& SmpSetColors)
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

void ProxyPly::generatePly()
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

ColorType ProxyPly::getLabelColor(void* pvtx)
{
	ColorType pClr = Color_Utility::span_color_from_hy_table( ((Vertex*)pvtx)->label() );
	return pClr;
}

ProxyPly::ProxyPly(char* plyOutputDir ,char* _prefix) :
output_file_path_(plyOutputDir),prefix_(_prefix)
{

}

void ProxyOricolorPly::run()
{
	generatePly();
}

ColorType ProxyOricolorPly::getLabelColor(void* vtx)
{
	ColorType m( (int)(((Vertex*)vtx)->r()*255)  ,(int)(((Vertex*)vtx)->g()*255) ,(int)(((Vertex*)vtx)->b()*255) ,1.);
	return m;
}

ProxyOricolorPly::ProxyOricolorPly(char* plyOutputDir ,char* _prefix) :
ProxyPly(plyOutputDir,_prefix)
{

}

void Proxy::setContext(PropagateThread* _pt)
{
	pt_ = _pt;
}

ColorType Proxy::getLabelColor(void*)
{
	return ColorType(0.0 ,0.0 , 0.0 ,1.0);
}

void ProxyVisualLabel::run()
{
	realabel();
	showSampleLabel();
}

void ProxyVisualLabel::showAllvtxLabel()
{
	pt_->visualAllvtx();
}

void ProxyVisualLabel::showSampleLabel()
{
	pt_->dp_.initialLabelDisplay();
	pt_->visualCosegmentation(visual_label_filename_);
}

void ProxyVisualLabel::readCorr(char* _corr_fileName)
{
	if(NULL == _corr_fileName)return;
	visual_corr_filename_ = _corr_fileName;
	pt_->dp_.read_corres_file_hier(visual_corr_filename_);
}

void ProxyVisualLabel::readCorr()
{
	if(NULL == visual_corr_filename_)return;
	pt_->dp_.read_corres_file_hier(visual_corr_filename_);
}

void ProxyVisualLabel::realabel(char* _label_filename)
{
	if( NULL ==_label_filename)return;
	visual_label_filename_ = _label_filename;
	pt_->dp_.read_label_file_hier(visual_label_filename_);
}

void ProxyVisualLabel::realabel()
{
	if(NULL == visual_label_filename_)return;
	pt_->dp_.read_label_file_hier(visual_label_filename_);
}

ProxyVisualLabel::ProxyVisualLabel(char* _label_filename ,char* _corr_filename /*=NULL*/) : 
visual_label_filename_(_label_filename) ,visual_corr_filename_(_corr_filename)
{

}

void ProxyWriteInc::run()
{
	writePovray();
}

void ProxyWriteInc::writePovray()
{
	writeMutiSamplePovray(incFileName_ ,afProFilenames_);
}

void ProxyWriteInc::writePovray(char* _incFileName ,char* _afProFilenames)
{
	incFileName_ = _incFileName;
	afProFilenames_ =  _afProFilenames;
	writeMutiSamplePovray(incFileName_ ,afProFilenames_);
}

ProxyWriteInc::ProxyWriteInc(char* incFileName ,char* afProFilenames) : incFileName_(incFileName) ,afProFilenames_(afProFilenames)
{

}

void ProxyProOri::run()
{
	propagateOriAndShow();
}

void ProxyProOri::propagateOriAndShow()
{
	realabel();
	readCorr();
	propagate2Ori();
	showAllvtxLabel();
}

void ProxyProOri::propagate2Ori()
{
	pt_->propagateLabel2Orignal();
}

ProxyProOri::ProxyProOri(char* label_filename ,char* _corr_fileName /*= NULL*/) :
ProxyVisualLabel(label_filename ,_corr_fileName)
{

}

void ProxyLabelSamplePly::run()
{
	generateSamplePly(visual_label_filename_);
}

ProxyLabelSamplePly::ProxyLabelSamplePly(char* plyOutputDir ,char* _prefix , char* _label_filename ,char* _corr_filename /*=NULL*/) :
ProxyPly(plyOutputDir ,_prefix) ,ProxyVisualLabel( _label_filename ,_corr_filename)
{

}

void ProxyRedColorPly::run()
{
	generatePly();
}

ColorType ProxyRedColorPly::getLabelColor(void* vtx)
{
	ColorType m( 171,48,46,1);
	return m;
}

ProxyRedColorPly::ProxyRedColorPly(char* plyOutputDir ,char* _prefix) :
ProxyPly(plyOutputDir,_prefix)
{

}

void ProxyGrayColorPly::run()
{
	generatePly();
}

ColorType ProxyGrayColorPly::getLabelColor(void* vtx)
{
	ColorType m( 207,207,207,1);
	return m;
}

ProxyGrayColorPly::ProxyGrayColorPly(char* plyOutputDir ,char* _prefix) :
ProxyPly(plyOutputDir,_prefix)
{

}

void ProxyStandaraGrayColorPly::run()
{
	generateStandaraPLY();
}

ColorType ProxyStandaraGrayColorPly::getLabelColor(void* vtx)
{
	ColorType m( 207,207,207,1);
	return m;
}

ProxyStandaraGrayColorPly::ProxyStandaraGrayColorPly(char* plyOutputDir ,char* _prefix) :
ProxyPly(plyOutputDir,_prefix)
{

}

void ProxyStandaraOriColorPly::run()
{
	generateStandaraPLY();
}

ColorType ProxyStandaraOriColorPly::getLabelColor(void* vtx)
{
	ColorType m( (int)(((Vertex*)vtx)->r()*255)  ,(int)(((Vertex*)vtx)->g()*255) ,(int)(((Vertex*)vtx)->b()*255) ,1.);
	return m;
}

ProxyStandaraOriColorPly::ProxyStandaraOriColorPly(char* plyOutputDir ,char* _prefix) :
ProxyPly(plyOutputDir,_prefix)
{

}

void ProxyStandaraAjustViewOriColorPly::run()
{
	getAllSampleCurrentViewMatrix();
	generateStandaraPLY(SmpSetcoodinates ,SmpSetnorms ,SmpSetColors);
}

ColorType ProxyStandaraAjustViewOriColorPly::getLabelColor(void* vtx)
{
	return ProxyStandaraOriColorPly::getLabelColor(vtx);
}

ProxyStandaraAjustViewOriColorPly::ProxyStandaraAjustViewOriColorPly(char* plyOutputDir ,char* _prefix) :ProxyStandaraOriColorPly(plyOutputDir,_prefix),ProxyAjustViewPly( plyOutputDir,_prefix )
{

}

void ProxyProOrigAndPly::run()
{
	propagateOriAndShow();
	generatePly();
}

void ProxyProOrigAndPly::generatePly()
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

ProxyProOrigAndPly::ProxyProOrigAndPly(char* plyOutputDir ,char* _prefix ,char* label_filename ,char* _corr_fileName /*= NULL */) :ProxyProOri(label_filename ,_corr_fileName) ,
	output_file_path_(plyOutputDir),prefix_(_prefix)
{

}

char* Proxy_Propagate::getLabelFileName()
{
	return label_filename_;
}

void Proxy_Propagate::run()
{
	Propagate();
}

void Proxy_Propagate::Propagate()
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

void Proxy_Propagate::Propagate(char* _label_filename ,char* _corr_filename , IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame)
{
	label_filename_= _label_filename ;
	corr_filename_ = _corr_filename;
	startFrame_ = _startFrame;
	endFrame_ = _endFrame;
	centerFrame_ = _centerFrame;
	Propagate();
}

Proxy_Propagate::Proxy_Propagate(char* _label_filename ,char* _corr_filename , IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame) : 
label_filename_(_label_filename) , corr_filename_(_corr_filename),startFrame_(_startFrame),endFrame_(_endFrame) ,centerFrame_(_centerFrame)
{

}

Proxy_Propagate::Proxy_Propagate(IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame) :
startFrame_(_startFrame),endFrame_(_endFrame) ,centerFrame_(_centerFrame)
{

}

void Proxy_PropagateAndVisual::run()
{
	Propagate();
	showSampleLabel();
}

Proxy_PropagateAndVisual::Proxy_PropagateAndVisual(char* _label_filename1 , char* _corr_filename2 ,IndexType _startFrame1, IndexType _endFrame1 ,IndexType _centerFrame1) :
Proxy_Propagate(_label_filename1 ,_corr_filename2, _startFrame1 ,_endFrame1,_centerFrame1) , ProxyVisualLabel(_label_filename1 ,_corr_filename2)
{

}

void Proxy_PropagateAndStepVisual::run()
{
	showSampleLabel();
	//Propagate();
	pt_->dp_.read_label_file_hier(label_filename_);
	pt_->dp_.read_corres_file_hier(corr_filename_);

	pt_->dp_.init_labeles_graph_hier2();
	pt_->dp_.init_node_link();
	for( int i = centerFrame_ ;i < endFrame_;++i){
		pt_->split_twoAjacent_graph_next( pt_->dp_,i ,i+1);

	}
	for( int i = centerFrame_ ;i > startFrame_;--i){
		pt_->split_twoAjacent_graph_prev( pt_->dp_,i-1 ,i);

	}
}

Proxy_PropagateAndStepVisual::Proxy_PropagateAndStepVisual(char* _label_filename1 , char* _corr_filename2 ,IndexType _startFrame1, IndexType _endFrame1 ,IndexType _centerFrame1) :
Proxy_Propagate(_label_filename1 ,_corr_filename2, _startFrame1 ,_endFrame1,_centerFrame1) , ProxyVisualLabel(_label_filename1 ,_corr_filename2)
{

}

void ProxyMergeAndPropagate::run()
{
	merge();
	Propagate( LABEL_FILEOUT_NAME_ ,CORR_FILEOUT_NAME_ , startFrame_ , endFrame_, centerFrame_ );
}

void ProxyMergeAndPropagate::merge()
{
	char fn[100];
	strcpy (fn , LABEL_FILEOUT_NAME_);
	strcat( fn ,".tmp");
	mergeFile(  CORR_FILESNAME_ ,CORR_FILEOUT_NAME_ ,LABEL_FILESNAME_ ,fn);
	replace(  centerFileName_ ,fn ,LABEL_FILEOUT_NAME_ );
}

void ProxyMergeAndPropagate::replace(char* centerFileName_ , char* targetFileName ,char* newtargetFileName)
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

ProxyMergeAndPropagate::ProxyMergeAndPropagate(char* CORR_FILESNAME,char* CORR_FILEOUT_NAME,char* LABEL_FILESNAME,char* LABEL_FILEOUT_NAME, char* centerFileName, IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame) :
Proxy_Propagate(_startFrame ,_endFrame ,_centerFrame) ,CORR_FILESNAME_(CORR_FILESNAME) , CORR_FILEOUT_NAME_(CORR_FILEOUT_NAME) , LABEL_FILESNAME_(LABEL_FILESNAME) , LABEL_FILEOUT_NAME_(LABEL_FILEOUT_NAME),centerFileName_(centerFileName)
{

}

void ProxyWriteMergePropagate::run()
{
	merge();
	Propagate( LABEL_FILEOUT_NAME_ ,CORR_FILEOUT_NAME_ , startFrame_ , endFrame_, centerFrame_ );
	write();
}

void ProxyWriteMergePropagate::write()
{
	pt_->dp_.wirteGraphLablesAtTop(afProFilenames_);
	writeMutiSamplePovray(incFileName_ ,afProFilenames_);
}

ProxyWriteMergePropagate::ProxyWriteMergePropagate(char* afProFilenames, char* incFileName ,char* CORR_FILESNAME,char* CORR_FILEOUT_NAME,char* LABEL_FILESNAME,char* LABEL_FILEOUT_NAME, char* centerFileName, IndexType _startFrame , IndexType _endFrame ,IndexType _centerFrame) :ProxyMergeAndPropagate( CORR_FILESNAME,CORR_FILEOUT_NAME,LABEL_FILESNAME,LABEL_FILEOUT_NAME,centerFileName,
	_startFrame ,  _endFrame ,_centerFrame) ,afProFilenames_(afProFilenames) , incFileName_(incFileName)
{

}

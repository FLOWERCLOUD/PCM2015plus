#include "file_io.h"
#include "basic_types.h"
#include "sample.h"
#include "triangle.h"
#include "render_types.h"
namespace FileIO
{
	void skip_this_line(FILE* fd)
	{
		int ret =0;
		while( ret = fgetc(fd)!= '\n'&& ret !=EOF );

	}
	FILE* get_mesh_model_scale(const char* path ,
		IndexType* n_vertex,
		IndexType* n_normal,
		IndexType* n_triangle)
	{
		FILE* fd = fopen(path ,"r");
		char pref[3];  //µÚÈý¸ö×Ö·û´æ"\0"
		if(fd)
		{
			*n_vertex = *n_normal = *n_triangle;
			while(fscanf(fd,"%2s",pref)!= EOF)
			{
				if( strcmp(pref, "v")==0) *n_vertex+=1;
				else if(strcmp(pref,"vn")==0)*n_normal +=1;
				else if(strcmp(pref,"f")==0)*n_triangle+=1;
				else{
					skip_this_line(fd);
				}
			}
			fseek(fd,SEEK_SET,0);
		}else
		{
			return NULL;
		}

		return fd;

	}
	

	Sample* load_point_cloud_file( std::string filename, FILE_TYPE type, IndexType sample_idx )
	{
		FILE* in_file = fopen(filename.c_str(), "r");

		if(in_file==NULL)
			return nullptr;

		Sample* new_sample = new Sample;
//		ScalarType vx0,vy0,vz0;
// 		bool first_vertex = true;

		if( type == FileIO::XYZ)
		{
			while ( true )
			{

				ScalarType vx,vy,vz,nx,ny,nz,cx,cy,cz;
				int status = fscanf(in_file, "%f %f %f %f %f %f %f %f %f\n",&vx,&vy,&vz,
					&nx,&ny,&nz,
					&cx,&cy,&cz);
				if(status==EOF)break;
				// 			if ( first_vertex )
				// 			{
				// 				vx0 = vx,vy0 = vy, vz0 = vz;
				// 				PointType v(0.,0.,0.);
				// 				ColorType cv(cx/255.,cy/255.,cz/255.,1.);
				// 				NormalType nv(nx,ny,nz);
				// 				new_sample->add_vertex(v, nv, cv);
				// 				first_vertex = false;
				// 				continue;
				// 			}
				// 			PointType v(vx - vx0,vy - vy0,vz - vz0);
				PointType v(vx,vy,vz);
				ColorType cv(cx/255.,cy/255.,cz/255.,1.);
				NormalType nv(nx,ny,nz);

				new_sample->add_vertex(v, nv, cv);


			}
	
		}else if( type == FileIO::PLY){
				int vcount=0;
				int fcount=0;
				fscanf(in_file,"ply\nformat ascii 1.0\nelement vertex %d\n",&vcount);
				fscanf(in_file,"property float x\nproperty float y\nproperty float z\nproperty float nx\nproperty float ny\nproperty float nz\nproperty uchar red\nproperty uchar green\nproperty uchar blue\n");
				fscanf(in_file,"end_header\n");
			while(true){

				ScalarType vx,vy,vz,nx,ny,nz,cx,cy,cz;
				int status = fscanf(in_file, "%f %f %f %f %f %f %f %f %f\n",&vx,&vy,&vz,
					&nx,&ny,&nz,
					&cx,&cy,&cz);
				if(status==EOF)break;

				PointType v(vx,vy,vz);
				ColorType cv(cx/255.,cy/255.,cz/255.,1.);
				NormalType nv(nx,ny,nz);

				new_sample->add_vertex(v, nv, cv);
			}

		}else if(type = FileIO::OBJ)
		{
			char pref[3];
			new_sample->clear();
			FILE* fd = get_mesh_model_scale( filename.c_str(), &(new_sample->n_vertex),&(new_sample->n_normal),&(new_sample->n_triangle) );
			Logger<<"nvertex "<<new_sample->n_vertex<<"nnormal "<<new_sample->n_normal<<"ntriangle"<<new_sample->n_triangle<<std::endl;
			if(fd)
			{
				ScalarType vx,vy,vz,nx,ny,nz,cx,cy,cz;
				std::vector<PointType> v;
				std::vector<ColorType> cv;
				std::vector<NormalType> nv;
				//std::vector<TriangleType*> ttv;
				while( fscanf(fd ,"%2s",pref) != EOF)
				{
					if(strcmp(pref,"v")==0)
					{
						fscanf(fd, "%f %f %f",&vx,&vy,&vz);
						v.push_back(PointType(vx,vy,vz));
					}else if(strcmp(pref,"vn") ==0)
					{
						fscanf(fd,"%f %f %f",&(nx),&(ny),&(nz));
						nv.push_back( NormalType(nx,ny,nz));
					}else if( strcmp(pref,"f")==0)
					{   
						TriangleType* tt = new TriangleType(*new_sample);
						for(int i_v = 0; i_v<3 ; i_v++)
						{
							fscanf(fd, "%d",&tt->i_vertex[i_v]);
							if((pref[0] = (char)getc(fd)) ==' ')
							{
								while( (pref[0] = (char)getc(fd)) ==' ');
								if( pref[0] == '\n'||pref[0]==EOF )
								{

								}else
								{
									ungetc(pref[0],fd);
								}
							}
							else if(pref[0] =='/')
							{
								if( (pref[0] = (char)getc(fd))!='/')
								{
									//while((char)get(fd) ==" ");
									ungetc(pref[0],fd);
									fscanf(fd ,"%d");
									if( (pref[0] = (char)getc(fd))!='/')
									{
										fscanf(fd,"%d",&tt->i_norm[i_v]);
									}

								}else{
									fscanf(fd,"%d",&tt->i_norm[i_v]);

								}								

							}

						}
						tt->i_vertex[0]--;
						tt->i_norm[0] = tt->i_vertex[0];
						tt->i_vertex[1]--;
						tt->i_norm[1] = tt->i_vertex[1];
						tt->i_vertex[2]--;
						tt->i_norm[2] = tt->i_vertex[2];
						new_sample->add_triangle(*tt);
	//					ttv.push_back(tt);
					}else if(pref[0] =='#')
					{
						skip_this_line(fd);
					}
					else skip_this_line(fd);
				}
				for(int i = 0 ; i< v.size();++i)
				{
					if( i+1 <= nv.size() && i+1 <= cv.size())
					new_sample->add_vertex(v[i], nv[i], cv[i]);
					else if(i+1 <= nv.size())
					{
						new_sample->add_vertex(v[i], nv[i], RED_COLOR);
					}
					else if(i+1 <= cv.size())
						new_sample->add_vertex(v[i], NULL_NORMAL, cv[i]);
					else
						new_sample->add_vertex(v[i], NULL_NORMAL, RED_COLOR);
				}
			//	new_sample->add_triangle(triangle_array= ttv;
				fclose(fd);
			}else
			{		
				return NULL;
			}

		}
		else{
			fclose(in_file);
			return NULL;
		}

		fclose(in_file);
		

		//give the new sample a color
		new_sample->set_visble(false);
		new_sample->set_color( Color_Utility::span_color_from_table( sample_idx ) );
		new_sample->build_kdtree();
		//add by huayun
		Box box = new_sample->getBox();
		ScalarType dia_distance = box.diag();
		return new_sample;

	}
}
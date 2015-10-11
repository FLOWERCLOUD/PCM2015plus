#pragma once


//轨迹聚类
static char* OUTPUT_DIR1 = "H:\\povay_pointcloud\\point_data\\panda20150918\\pandalabelandcorr\\label\\";    //".\\carbinet0904\\";    //".\\horse_multiview08031\\";  //".\\2015-8-17-算法在设计\\horse_multiview08017\\";
static char* OUTPUT_FILENAME1 = "2015919pandalabel"; //"20150803labels_carbinet";
//轨迹聚类
static char* OUTPUT_DIR2 = "H:\\povay_pointcloud\\point_data\\panda20150918\\pandalabelandcorr\\corr\\";          //".\\carbinet0904\\";   //".\\horse_multiview08031\\"; //".\\2015-8-17-算法在设计\\horse_multiview08017\\";
static char* OUTPUT_FILENAME2 = "2015919pandacorr";  //"201508031corr_carbinet"; 

//共分割
static char* IN_LABEL_FILE_GCOP =  "H:\\povay_pointcloud\\point_data\\robotDepth\\totLabels(1_7).txt" ;  //".\\horse_multiview08017\\refine(8_18)\\0829outputLabelsCoSeg.txt";   //0825outputLabels.txt";  //refine(8_18)\\totLabels(1_9).txt";   //0825outputLabels.txt";   //refine(8_18)\\totLabels(1_9).txt";  //201508019labels_horse1_2.txt ";   //0818_horse_ransac01.txt";   //201508019labels_horse10_11.txt";  //201508017labels_horse00_09.txt" ; //"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\RANSAC_mesh4(10_27)\\allRANSACLabels.txt";
static char* IN_CORR_FILE_GCOP = "H:\\povay_pointcloud\\point_data\\robotDepth\\totCorr(1_7).txt";   // ".\\horse_multiview08017\\refine(8_18)\\refine(8_18)\\totCorr(1_9).txt";   //201508019corr_horse1_2.txt";  //0818corr_horseRigid01_0.60.txt";  //201508019corr_horse10_11";   //201508017corr_horse00_09.txt";  //"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\RANSAC_mesh4(10_27)\\allRANSACCorr.txt";
static char* OUT_LABEL_FILE_GCOP = "H:\\povay_pointcloud\\point_data\\robotDepth\\output.txt"; //201508019labels_horse1_2output.txt"; //0818_horse_ransac01output ";   //201508019labels_horse10_11output"; //201508017labels_horse00_09output.txt";  //"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\RANSAC_mesh4(10_27)\\0601outputLabels.txt";
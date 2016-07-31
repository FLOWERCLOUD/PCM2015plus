#include "main_window.h"
#include "paint_canvas.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QStatusBar>
#include <QSettings>
#include <QCloseEvent>
#include <QPlainTextEdit>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QGroupBox>
#include <QColorDialog>
#include <QComboBox>
#include "file_system.h"
#include "sample.h"
#include "sample_set.h"
#include "vertex.h"
#include "file_io.h"
#include "color_table.h"
#include "time.h"

#include "tracer.h"
#include "sample_properity.h"
#include "maching_state.h"
#include "saveSnapshotDialog.h"
#include "savePLYDialog.h"
#include "GLLogStream.h"
#include "render_types.h"
using namespace qglviewer;
using namespace std;
using namespace ANIMATION;
//add by huayun
bool ifGraphBoxVisible = 0;
bool ifEdgeVertexVisible = 0;
IndexType LabelGraphDepth = 0;
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <qtgui/QHBoxLayout>
#include <QtGui/QSizePolicy>
#include <sstream>
#include <QWaitCondition>
// 全局条件变量
QWaitCondition mWaitcond;
GLLogStream logstream;
int REAL_TIME_RENDER = 0;
extern RenderMode::WhichColorMode	which_color_mode_;
extern RenderMode::RenderType which_render_mode;
extern bool isShowNoraml;

main_window::main_window(QWidget *parent)
	: QMainWindow(parent),
	cur_select_sample_idx_(-1),
	last_select_sample_idx_(-1)
{
	//mv_ = this;

	frameTimer = NULL;

	ui.setupUi(this);

	QGLFormat format = QGLFormat::defaultFormat();
	format.setSampleBuffers(true);
	format.setSamples(8);
	main_canvas_ = new PaintCanvas(format, this);

	setCentralWidget(main_canvas_);
	//add Multiple widgets on a QDockWidget
	QDockWidget* layerDock = new QDockWidget(  QString(" LAYERDOCK") , this);
	ui.LayerSpinBox->setMinimumSize(QSize(50, 38));
	ui.LayerSpinBox->setMaximumWidth(300);
	ui.centerframe->setMinimumSize(QSize(50, 38));
	ui.centerframe->setMaximumWidth(300);
	//layerDock->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding ) );
	
	QWidget* multiwidget = new QWidget();
	multiwidget->setMaximumHeight(150);
	QVBoxLayout* layerlayout = new QVBoxLayout();
	QHBoxLayout* LayerDialoglayout = new QHBoxLayout();
	QHBoxLayout* centetframelayout = new QHBoxLayout();
	QHBoxLayout* trj_labelLayout = new QHBoxLayout();
	LayerDialoglayout->addWidget(new QLabel("layer") );
	LayerDialoglayout->addWidget(ui.LayerSpinBox);
	centetframelayout->addWidget(new QLabel("center frame"));
	centetframelayout->addWidget(ui.centerframe);
	trj_labelLayout->addWidget(ui.text_trajectory_label);
	trj_labelLayout->addWidget(ui.button_traj_label);

	layerlayout->addLayout(LayerDialoglayout);
	layerlayout->addLayout(centetframelayout);
	layerlayout->addLayout(trj_labelLayout);
	multiwidget->setLayout(layerlayout);
	layerDock->setWidget(multiwidget);
	this->addDockWidget(static_cast<Qt::DockWidgetArea>(1), layerDock);

	QSplitter *splitter= new QSplitter(this);
	//splitter->setFixedSize(500 ,500);
	//splitter->addWidget(ui.LayerSpinBox);
	splitter->addWidget(main_canvas_);
	this->setCentralWidget(splitter);

	

	setWindowTitle("PCM");

	setContextMenuPolicy(Qt::CustomContextMenu);
//	setWindowState(Qt::WindowMaximized);

	setFocusPolicy(Qt::ClickFocus);

	main_canvas_->camera()->setType(Camera::PERSPECTIVE);

	//Codes for initializing treeWidget
	QStringList headerLabels;
	headerLabels <<"Index"<< "Name" << "VtxNum" ;//<< "#Point";
	ui.treeWidget->setHeaderLabels(headerLabels);
	ui.treeWidget->setColumnWidth( 0 ,50);
	ui.treeWidget->setColumnWidth( 1,50 );
	ui.treeWidget->setColumnWidth( 2,50 );
	connect(ui.treeWidget, SIGNAL(itemClicked( QTreeWidgetItem * , int  ) ),
		this, SLOT(selectedSampleChanged( QTreeWidgetItem * , int  )) );


	createAction();
	createStatusBar();



	//srand(time(NULL));


	//added by huayun
	m_layer = new LayerDialog(this);
	m_layer->setAllowedAreas( Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
	addDockWidget( Qt::RightDockWidgetArea, m_layer);
	m_layer->setVisible(true);

}


void main_window::resetSampleSet()
{
	cur_import_files_attr_.clear();
	cur_select_sample_idx_ = last_select_sample_idx_ = -1;
	for( int i = 0 ;i<SampleSet::get_instance().size();++i)
	{

	}
	SampleSet::get_instance().clear();
}

void main_window::createAction()
{
	createFileMenuAction();
	createPaintSettingAction();
	createAlgorithmAction();
	createToolAction();

}


void main_window::createAlgorithmAction()
{
	connect(ui.actionClustering, SIGNAL(triggered()), this, SLOT(doClustering()));
	connect(ui.actionRegister,SIGNAL(triggered()),this,SLOT(doRegister()));
	//connect(ui.actionSpectral_Cluster,SIGNAL(triggered()),this,SLOT(doSpectralCluster()));
	connect(ui.actionGraphCut,SIGNAL(triggered()),this,SLOT(doGraphCut()));
	connect(ui.actionCalculateNorm,SIGNAL(triggered()),this,SLOT(computeSampleNormal()));
	connect(ui.actionClusterAll,SIGNAL(triggered()),this,SLOT(batchTrajClustering()));
	connect(ui.actionVisDistortion,SIGNAL(triggered()),this,SLOT(visDistor()));

	connect(ui.actionGCopti,SIGNAL(triggered()),this,SLOT(doGCOptimization()));

	connect(ui.actionPlanFit,SIGNAL(triggered()), this ,SLOT(doPlanFit() ) );

	connect( ui.actionPropagate , SIGNAL(triggered()) ,this , SLOT( doPropagate()) );
	
}

void main_window::createPaintSettingAction()
{
	connect(ui.actionSet_Visible, SIGNAL(triggered()), this, SLOT(setSampleVisible()) );
	connect(ui.actionSet_Invisible, SIGNAL(triggered()), this, SLOT(setSampleInvisible()));
	connect(ui.actionObject_Color, SIGNAL(triggered()), this, SLOT(setObjectColorMode()));
	connect(ui.actionVertex_Color, SIGNAL(triggered()), this, SLOT(setVertexColorMode()));
	connect(ui.actionLabel_Color, SIGNAL(triggered()), this, SLOT(setLabelColorMode()));
	connect(ui.actionShow_Tracjectory, SIGNAL(triggered()), this, SLOT(showTracer()));
	connect(ui.actionBallvertex ,SIGNAL(triggered()), this, SLOT(setBallvertexMode()));
	connect(ui.actionShow_normal ,SIGNAL(triggered()), this, SLOT(show_normal()));
	//render mode
	connect(ui.actionPoint_mode, SIGNAL(triggered()), this, SLOT(setPointMode()) );
	connect(ui.actionFlat_mode, SIGNAL(triggered()), this, SLOT(setFlatMode()) );
	connect(ui.actionWire_mode, SIGNAL(triggered()), this, SLOT(setWireMode()) );
	connect(ui.actionFlatWire_mode, SIGNAL(triggered()), this, SLOT(setFlatWireMode()) );
	connect(ui.actionSmooth_mode, SIGNAL(triggered()), this, SLOT(setSmoothMode()) );
	connect(ui.actionTexture_mode, SIGNAL(triggered()), this, SLOT(setTextureMode()) );
	connect(ui.actionSelect_Mode_render, SIGNAL(triggered()), this, SLOT(setSelectMode()) );



	connect(ui.actionDont_Trace,SIGNAL(triggered()), this, SLOT(clearTracer()));
	connect(ui.actionShow_Graph_WrapBox ,SIGNAL(triggered()) ,this ,SLOT(chooseGraph_WrapBox_visiable() ));
	connect(ui.actionShow_EdgeVertexs ,SIGNAL(triggered()) ,this ,SLOT(chooseEdgeVertexs__visiable() ));
	//added by huayun
	//connect( ui.LayerSpinBox ,SIGNAL( triggered() ) ,this ,SLOT(layerSpinBoxChanged( 2)) );
	connect( ui.LayerSpinBox ,SIGNAL( valueChanged(int)) ,this ,SLOT(layerSpinBoxChanged(int)) );
	//connect( ui.actionButtonback, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );
	connect( ui.centerframe ,SIGNAL( valueChanged(int)) ,this ,SLOT(centerframeChanged(int)) );

	connect(ui.button_traj_label, SIGNAL(clicked()), this, SLOT(dealtarjlabel()) );
	connect(ui.actionWakeWorkThread ,  SIGNAL(triggered()), this, SLOT(wakeUpThread() ) );
	//connect( ui.actionButton2stop, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );
	StateManager::getInstance().setWindowrefer(this);
	int runorp = connect( ui.actionButtonRunOrPause, SIGNAL(triggered() ) , this ,SLOT( runOrPause() ) );
	std::cout<<"runorp"<< runorp<<std::endl;
	int stop1 = connect( ui.actionButton2stop, SIGNAL(triggered() ) , this ,SLOT( stop() ) );
	std::cout<<"stop1"<< stop1<<std::endl;
	int staech = connect( &(StateManager::getInstance()) , SIGNAL( stateChanged(ANIMATION::AState,ANIMATION::AState ) ) ,
		this ,SLOT( mediaStateChanged(ANIMATION::AState ,ANIMATION::AState) ) );
	std::cout<<"staech "<< staech<<std::endl;
	connect( ui.actionButtonback, SIGNAL(triggered() ) , this ,SLOT( increaseInterval() ) );
	connect( ui.actionButtonadvance, SIGNAL(triggered() ) , this ,SLOT( decreaseInterval()) );
	//connect( ui.actionButtonadvance, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );

}

void main_window::createToolAction()
{
	connect( ui.actionSelect_Mode, SIGNAL(triggered()), this, SLOT(setSelectToolMode()) );
	connect( ui.actionScene_Mode, SIGNAL(triggered()),this, SLOT(setSceneToolMode()));
}

void main_window::setObjectColorMode()
{
	which_color_mode_ =  RenderMode::OBJECT_COLOR;
	main_canvas_->updateGL();
}

void main_window::setVertexColorMode()
{
	which_color_mode_ =  RenderMode::VERTEX_COLOR;
	main_canvas_->updateGL();
}

void main_window::setLabelColorMode()
{
	which_color_mode_ = RenderMode::LABEL_COLOR;
	main_canvas_->updateGL();
}
void main_window::setBallvertexMode()
{
	which_color_mode_ = RenderMode::SphereMode;
	main_canvas_->updateGL();
}
void main_window::show_normal()
{
	if(isShowNoraml) 
	{
		isShowNoraml = false;
	}else
	{
		isShowNoraml = true;
	}
	main_canvas_->updateGL();
}
void main_window::setSelectToolMode()
{
	if (cur_select_sample_idx_==-1)
	{
		return;
	}

	if (main_canvas_->single_operate_tool_)
	{
		delete main_canvas_->single_operate_tool_;
	}
	main_canvas_->single_operate_tool_ = new SelectTool(main_canvas_);
	main_canvas_->single_operate_tool_->set_tool_type(Tool::SELECT_TOOL);
	main_canvas_->single_operate_tool_->set_cur_smaple_to_operate(cur_select_sample_idx_);

	main_canvas_->updateGL();

}

void main_window::setSceneToolMode()
{
	main_canvas_->single_operate_tool_->set_tool_type(Tool::EMPTY_TOOL);
	main_canvas_->updateGL();
}

void main_window::setPointMode()
{
	which_render_mode = RenderMode::PointMode;
	main_canvas_->updateGL();
};
void main_window::setFlatMode()
{
	which_render_mode = RenderMode::FlatMode;
	main_canvas_->updateGL();
};
void main_window::setWireMode()
{
	which_render_mode = RenderMode::WireMode;
	main_canvas_->updateGL();
};
void main_window::setFlatWireMode()
{
	which_render_mode = RenderMode::FlatWireMode;
	main_canvas_->updateGL();
};
void main_window::setSmoothMode()
{
	which_render_mode = RenderMode::PointMode;
	main_canvas_->updateGL();
};
void main_window::setTextureMode()
{
	which_render_mode = RenderMode::TextureMode;
	main_canvas_->updateGL();
};
void main_window::setSelectMode()
{
	which_render_mode = RenderMode::SelectMode;
	main_canvas_->updateGL();
};
void main_window::createFileMenuAction()
{
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile()));
	}
	separatorAct = ui.menuFiles->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		ui.menuFiles->addAction(recentFileActs[i]);
	ui.menuFiles->addSeparator();

	updateRecentFileActions();
	connect(ui.actionImportFiles, SIGNAL(triggered()),this, SLOT(openFiles()));
	connect(ui.actionImportFiles_Lazy, SIGNAL(triggered()),this, SLOT(openFilesLazy()));
	connect(ui.actionSaveSnapshot ,SIGNAL(triggered()) , this ,SLOT(saveSnapshot()));
	connect(ui.actionSavePly ,SIGNAL(triggered()) , this ,SLOT(savePLY()));
	connect(ui.actionsaveLabelFile ,SIGNAL(triggered()) , this ,SLOT(saveLabelFile()));
	connect(ui.actionGetlabel_from_file ,SIGNAL(triggered()) , this ,SLOT(getLabelFromFile()));
}

bool main_window::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Import point cloud from file"), ".",
		tr(
		"Ascii Point cloud (*.lab *.xyz *.pwn *.pcd)\n"
		"All files (*.*)")
		);

	if (fileName.isEmpty())
		return false;

	return true;
}


bool main_window::setSampleVisible()
{
	SampleSet::get_instance()[cur_select_sample_idx_].set_visble(true);
	m_layer->updateTable(cur_select_sample_idx_);
	main_canvas_->updateGL();
	return true;
}

bool main_window::setSampleInvisible()
{
	SampleSet::get_instance()[cur_select_sample_idx_].set_visble(false);
	m_layer->updateTable(cur_select_sample_idx_);
	main_canvas_->updateGL();
	return true;
}

void main_window::selectedSampleChanged(QTreeWidgetItem * item, int column)
{
	last_select_sample_idx_ = cur_select_sample_idx_;
	cur_select_sample_idx_ = item->text(0).toInt();

	//change the active frame


	if (last_select_sample_idx_!= -1)
	{
		SampleSet::get_instance()[last_select_sample_idx_].set_selected(false);
	}
	if ( cur_select_sample_idx_ != -1)
	{
		SampleSet::get_instance()[cur_select_sample_idx_].set_selected(true);
	}
	main_canvas_->updateGL();

}

void main_window::showTracer()
{
	main_canvas_->setTracerShowOrNot(true);
	main_canvas_->updateGL();
}

void main_window::clearTracer()
{
	main_canvas_->setTracerShowOrNot(false);
	Tracer::get_instance().clear_records();
	main_canvas_->updateGL();
}
//bool ifGraphBoxVisible = 0;
//bool ifEdgeVertixVisible = 0;

//QIcon icon;
//icon.addFile(QString::fromUtf8("Resources/openFile.png"), QSize(), QIcon::Normal, QIcon::Off);
//actionImportFiles->setIcon(icon);
void main_window::layerSpinBoxChanged(int i)
{
	Logger<<"layer 值改变  "<< i <<std::endl;
	if( i != LabelGraphDepth){
		LabelGraphDepth = i;
		SampleSet::get_instance()[cur_select_sample_idx_].clayerDepth_ = i;
		emit LabelGraphDepthChanged(i);
	} 
	main_canvas_->updateGL();

}
void main_window::centerframeChanged(int i)
{
	Logger<<"centerframe 值改变  "<< i <<std::endl;
	main_canvas_->centerframeNum = i;
	main_canvas_->updateGL();
}
void main_window::setSampleSelectedIndex(int i)
{
	last_select_sample_idx_ = cur_select_sample_idx_;
	cur_select_sample_idx_ =  i;


	if (last_select_sample_idx_!= -1)
	{
		SampleSet::get_instance()[last_select_sample_idx_].set_selected(false);
	}
	if ( cur_select_sample_idx_ != -1)
	{
		SampleSet::get_instance()[cur_select_sample_idx_].set_selected(true);
	}
	main_canvas_->updateGL();
}
void main_window::dealtarjlabel()
{
	std::stringstream istring(ui.text_trajectory_label->text().toStdString());
	static int labelidx = 0;
	Logger<<"dealtarjlabel  "<<std::endl;

	std::vector<int> frame;

	frame.push_back(main_canvas_->centerframeNum);
	/*frame.push_back(52);
	frame.push_back(53);
	frame.push_back(54);*/
	//std::vector<int> traj_label_vec;
	
	int label;
	traj_label_vec.clear();
	while(istring>>label){
		traj_label_vec.push_back(label);
	};

	main_canvas_->updateGL();
}
//connect( ui.actionButtonback, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );
//connect( ui.actionButton2stop, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );
//connect( ui.actionButtonRunOrPause, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );
//connect( ui.actionButtonadvance, SIGNAL(triggered() ) , this ,SLOT(layerSpinBoxChanged(int)) );

void main_window::processButton()
{

}

void main_window::processButtonBack()
{

}
void main_window::processButton2Stop()
{

}

void main_window::processButtonadvance()
{

}





void  main_window::chooseGraph_WrapBox_visiable()
{
	std::cout<<"Show_Graph_WrapBox"<<std::endl;
	if( ifGraphBoxVisible){
		std::cout<<"Visible"<<ifGraphBoxVisible<<std::endl;
		ifGraphBoxVisible = 0;
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/nnolinkNode.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionShow_Graph_WrapBox->setIcon(icon);
		ui.actionShow_Graph_WrapBox->setText(QApplication::translate("main_windowClass", "Show Graph WrapBox", 0, QApplication::UnicodeUTF8));
		unShow_Graph_WrapBox();
	}else{
		std::cout<<"Visible"<<ifGraphBoxVisible<<std::endl;
		ifGraphBoxVisible =1;
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/linkNode.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionShow_Graph_WrapBox->setIcon(icon);
		ui.actionShow_Graph_WrapBox->setText(QApplication::translate("main_windowClass", "unShow Graph WrapBox", 0, QApplication::UnicodeUTF8));
		Show_Graph_WrapBox();
	}
}
void  main_window::Show_Graph_WrapBox()
{

	std::cout<<"Show_Graph_WrapBox"<<std::endl;

	which_color_mode_ =  RenderMode::WrapBoxColorMode;
	//main_canvas_->which_color_mode_ = PaintCanvas::SphereMode;
	main_canvas_->setGraph_WrapBoxShowOrNot(true);

	main_canvas_->updateGL();
}
void  main_window::unShow_Graph_WrapBox()
{
	std::cout<<"unShow_Graph_WrapBox"<<std::endl;

	which_color_mode_ =  RenderMode::LABEL_COLOR;
	main_canvas_->setGraph_WrapBoxShowOrNot(false);
	//Tracer::get_instance().clear_records();
	main_canvas_->updateGL();
}
//  actionSet_Invisible->setText(QApplication::translate("main_windowClass", "Set Invisible", 0, QApplication::UnicodeUTF8));
void  main_window::chooseEdgeVertexs__visiable()
{
	std::cout<<"Show_EdgeVertexs"<<std::endl;
	if( ifEdgeVertexVisible){
		std::cout<<"Visible"<<ifEdgeVertexVisible<<std::endl;
		ifEdgeVertexVisible = 0;
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/NoedgeVertexs.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionShow_EdgeVertexs->setIcon(icon);
		ui.actionShow_EdgeVertexs->setText(QApplication::translate("main_windowClass", "Show EdgeVertexs", 0, QApplication::UnicodeUTF8));
		unShow_EdgeVertexs();
	}else{
		std::cout<<"Visible"<<ifEdgeVertexVisible<<std::endl;
		ifEdgeVertexVisible =1;
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/edgeVertexs.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionShow_EdgeVertexs->setIcon(icon);
		ui.actionShow_EdgeVertexs->setText(QApplication::translate("main_windowClass", "unShow EdgeVertexs", 0, QApplication::UnicodeUTF8));
		Show_EdgeVertexs();
	}
}
void  main_window::Show_EdgeVertexs()
{
	//	main_canvas_->which_color_mode_ = PaintCanvas::LABEL_COLOR;
	//main_canvas_->updateGL();
	which_color_mode_ =  RenderMode::EdgePointColorMode;
	main_canvas_->setEdgeVertexsShowOrNot(true);
	main_canvas_->updateGL();
}
void  main_window::unShow_EdgeVertexs()
{	
	which_color_mode_ =  RenderMode::LABEL_COLOR;
	main_canvas_->setEdgeVertexsShowOrNot(false);
	//Tracer::get_instance().clear_records();
	main_canvas_->updateGL();

}

bool main_window::openFiles()
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	QString dir;
	if(files.size() >0){
		dir = QFileDialog::getExistingDirectory(this,tr("Import point cloud files"), files[0]);
	}
	else{
		dir = QFileDialog::getExistingDirectory(this,tr("Import point cloud files") ,QString("./") );
	}

	if (dir.isEmpty())
		return false;
	setCurrentFile(dir);

	resetSampleSet();

	QDir file_dir(dir);
	if ( !file_dir.exists() )
	{
		return false;
	}
	file_dir.setFilter(QDir::Files);

	QFileInfoList file_list = file_dir.entryInfoList();
	IndexType sample_idx = 0;
	for (IndexType file_idx = 0; file_idx < file_list.size(); file_idx++)
	{
		QFileInfo file_info = file_list.at(file_idx);
		FileIO::FILE_TYPE file_type;

		if (file_info.suffix() == "xyz")
		{
			file_type = FileIO::XYZ;
		}
		else if(file_info.suffix() == "ply")
		{
			file_type = FileIO::PLY;
		}
		else if(file_info.suffix() == "obj")
		{
			file_type = FileIO::OBJ;
		}
		else
		{
			continue;
		}

		string file_path = file_info.filePath().toStdString();
		cur_import_files_attr_.push_back( make_pair(FileSystem::base_name(file_path), 
			FileSystem::extension(file_path)) );

		Sample* new_sample = FileIO::load_point_cloud_file(file_path, file_type);
		if (new_sample != nullptr)
		{
			new_sample->isload_ = true;
			new_sample->set_color( Color_Utility::span_color_from_table( file_idx ) );
			SampleSet& smpset = SampleSet::get_instance();
			smpset.push_back(new_sample);
			new_sample->smpId = sample_idx;
			sample_idx++;
		}
		

	}


	createTreeWidgetItems();
	m_layer->updateTable();

	return true;
}

bool main_window::openFilesLazy()
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	QString dir;
	if(files.size() >0){
		dir = QFileDialog::getExistingDirectory(this,tr("Import point cloud files"), files[0]);
	}
	else{
		dir = QFileDialog::getExistingDirectory(this,tr("Import point cloud files") ,QString("./") );
	}

	if (dir.isEmpty())
		return false;
	setCurrentFile(dir);

	resetSampleSet();

	QDir file_dir(dir);
	if ( !file_dir.exists() )
	{
		return false;
	}
	file_dir.setFilter(QDir::Files);

	QFileInfoList file_list = file_dir.entryInfoList();
	IndexType sample_idx = 0;
	for (IndexType file_idx = 0; file_idx < file_list.size(); file_idx++)
	{
		if(file_idx  == 506)
		{
			QFileInfo file_info = file_list.at(file_idx);
		}

		QFileInfo file_info = file_list.at(file_idx);
		FileIO::FILE_TYPE file_type;

		if (file_info.suffix() == "xyz")
		{
			file_type = FileIO::XYZ;
		}
		else if(file_info.suffix() == "ply")
		{
			file_type = FileIO::PLY;
		}
		else if(file_info.suffix() == "obj")
		{
			file_type = FileIO::OBJ;
		}
		else
		{
			continue;
		}

		string file_path = file_info.filePath().toStdString();
		cur_import_files_attr_.push_back( make_pair(FileSystem::base_name(file_path), 
			FileSystem::extension(file_path)) );

		Sample* new_sample = FileIO::lazy_load_point_cloud_file(file_path, file_type);
		if (new_sample != nullptr)
		{
			new_sample->isload_ = false;			
			new_sample->set_color( Color_Utility::span_color_from_table( file_idx ) );
			SampleSet& smpset = SampleSet::get_instance();
			smpset.push_back(new_sample);
			new_sample->smpId = sample_idx;
			sample_idx++;
		}


	}
	SampleSet& smpset = SampleSet::get_instance();

	createTreeWidgetItems();
	m_layer->updateTable();

	return true;
}

void main_window::createStatusBar()
{
	coord_underMouse_label_ = new QLabel(this);
	coord_underMouse_label_->setAlignment(Qt::AlignLeft);

	vtx_idx_underMouse_label_ = new QLabel(this);
	coord_underMouse_label_->setAlignment(Qt::AlignRight);
	
	ui.statusBar->addWidget( coord_underMouse_label_ , 1 );
	ui.statusBar->addWidget( vtx_idx_underMouse_label_, 0 );
}

void main_window::createTreeWidgetItems()
{
	ui.treeWidget->clear();
	
	SampleSet& set = SampleSet::get_instance();
	for ( int sample_idx=0; sample_idx < set.size(); sample_idx++ )
	{
		//QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget); 
		//QTreeWidgetItem* itemA = new QTreeWidgetItem( ui.treeWidget);
		//QTreeWidgetItem* itemB = new QTreeWidgetItem(ui.treeWidget);
		//QIcon icon;
		//icon.addFile(QString::fromUtf8("Resources/invisible.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		//ui.actionShow_Graph_WrapBox->setIcon(icon);

		//ColorType color = set[ sample_idx ].color();
		//itemA->setData(0,Qt::DecorationRole, icon);
		//itemB->setData(0, Qt::DisplayRole, sample_idx);
		//itemB->setData(1,Qt::DecorationRole, QColor(color(0)*255, color(1)*255, color(2)*255) );
		//itemB->setData(2, Qt::DisplayRole, set[sample_idx].num_vertices() );
		///*item->insertChild(sample_idx ,itemA);
		//item->insertChild( sample_idx ,itemB);*/
		//ui.treeWidget->insertTopLevelItem(sample_idx, itemA);
		//ui.treeWidget->insertTopLevelItem(sample_idx, itemB);

		QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget); 
	//	QIcon icon2;
	//	icon2.addFile(QString::fromUtf8("Resources/invisible.png") ,QSize() , QIcon::Normal ,QIcon::Off);
	//	ui.actionShow_Graph_WrapBox->setIcon(icon2);

		ColorType color = set[ sample_idx ].color();
	//	item->setData(0,Qt::DecorationRole, icon2);
		item->setData(0, Qt::DisplayRole, sample_idx);
		item->setData(1,Qt::DecorationRole, QColor(color(0)*255, color(1)*255, color(2)*255) );
		item->setData(2, Qt::DisplayRole, set[sample_idx].num_vertices() );

		ui.treeWidget->insertTopLevelItem(sample_idx, item);
	}
}

void main_window::showCoordinateAndIndexUnderMouse( const QPoint& point )
{
	//Mouse point info come from canvas
	bool found = false;
    qglviewer::Vec v = main_canvas_->camera()->pointUnderPixel(point, found);
	if ( !found )
	{
		v = qglviewer::Vec();
	}
	QString coord_str = QString("XYZ = [%1, %2, %3]").arg(v.x).arg(v.y).arg(v.z);
	coord_underMouse_label_->setText(coord_str);

	IndexType idx;
	IndexType label;
	if ( !found || cur_select_sample_idx_==-1 )
	{
		idx = -1;
		label = -1;
	}
	else
	{
		Sample& cur_selected_sample = SampleSet::get_instance()[cur_select_sample_idx_];
		Vec4 v_pre(v.x - Paint_Param::g_step_size(0)*(cur_select_sample_idx_-main_canvas_->centerframeNum),
			v.y - Paint_Param::g_step_size(1)*(cur_select_sample_idx_-main_canvas_->centerframeNum),
			v.z - Paint_Param::g_step_size(2)*(cur_select_sample_idx_-main_canvas_->centerframeNum) ,1.);
		//Necessary to do this step, convert view-sample space to world-sample space
		v_pre = cur_selected_sample.matrix_to_scene_coord().inverse() * v_pre;
		idx = cur_selected_sample.closest_vtx( PointType(v_pre(0), v_pre(1), v_pre(2)) );
		label = cur_selected_sample[idx].label();
	}
	QString idx_str = QString("VERTEX INDEX = [%1],LABEL = [%2]").arg(idx).arg(label);
	vtx_idx_underMouse_label_->setText( idx_str );

	return;
}

void main_window::doClustering()
{





}
void main_window::doRegister()
{

}
void main_window::doGraphCut()
{

}

void main_window::doGCOptimization()
{

}

void main_window::doPlanFit()
{


}

void main_window::doPropagate()
{

}

// void main_window::doSpectralCluster()
// {
// 	SpectralClusteringThread* specCla = new SpectralClusteringThread();
// 	connect(specCla,SIGNAL(finish_compute()),this,SLOT(finishSpectralCluster()));
// 	connect(specCla,SIGNAL(finished()),specCla,SLOT(deleteLater()));
// 	specCla->start();
// }
void main_window::finishClustering()
{

}
void main_window::finishRegister()
{

}
void main_window::finishGraphCut()
{

}

void main_window::finishGCotimization()
{

}
// void main_window::finishDoPlanFit()
// {
// 
// }

// void main_window::finishSpectralCluster()
// {
// 
// }
main_window::~main_window()
{
	SampleSet::get_instance().clear();
	if( frameTimer != NULL){
		delete frameTimer;
		frameTimer = NULL;
	}


}
void main_window::computeSampleNormal()
{
	auto camera_look_at = main_canvas_->camera()->viewDirection();
	//SampleManipulation::computerMinMax(cur_select_sample_idx_);
	SampleManipulation::compute_normal_all( NormalType(-camera_look_at.x, -camera_look_at.y, -camera_look_at.z));
	//SampleManipulation::compute_normal( cur_select_sample_idx_ ,NormalType(-camera_look_at.x, -camera_look_at.y, -camera_look_at.z));
	//SampleManipulation::compute_normal( cur_select_sample_idx_ ,NormalType(-1.0,0.0,0.0));
}

void main_window::batchTrajClustering()
{

}

void main_window::visDistor()
{

}

void main_window::iterateTrajClustering()
{

}
using namespace ANIMATION;
bool main_window::runOrPause()
{
	if(  StateManager::getInstance().state() ==  RUNSTATE){
//	Logger<<" old state is runstate"<<std::endl;
		StateManager::getInstance().pause();
		 

	}else if(StateManager::getInstance().state() ==  PAUSESTATE){
//		Logger<<" old state is pause state"<<std::endl;
		StateManager::getInstance().run();
	}else if(StateManager::getInstance().state() ==  STOPSTATE){
//		Logger<<" old state is stop state"<<std::endl;
		StateManager::getInstance().run();
	}
	return false;
}
bool main_window::stop()
{
	if(StateManager::getInstance().state() ==  RUNSTATE){
//		Logger<<" old state is runstate"<<std::endl;
		StateManager::getInstance().stop();
	}else if (StateManager::getInstance().state() ==  PAUSESTATE){
//		Logger<<" old state is pause state"<<std::endl;

	}else{


	}
	return false;
}



bool main_window::mediaStateChanged(ANIMATION::AState _newState ,ANIMATION::AState _oldState)
{
	Logger<<"mediaStateChanged"<< std::endl;
	switch( _newState)
	{
	
	case ANIMATION::RUNSTATE:{
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/buttonrun2pause.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionButtonRunOrPause->setIcon(icon);
	 
		
		break;
							 }
	case ANIMATION::PAUSESTATE:{
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/buttonstop2run.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionButtonRunOrPause->setIcon(icon);
 

		break;
							   }
	case ANIMATION::STOPSTATE:{
		QIcon icon;
		icon.addFile(QString::fromUtf8("Resources/buttonstop2run.png") ,QSize() , QIcon::Normal ,QIcon::Off);
		ui.actionButtonRunOrPause->setIcon(icon);
		
		break;
							  }
	case ANIMATION::ERRORSTATE:{
		break;
							   }
	default:
		break;


	}
	return false;
}

void main_window::processButtonRunOrPuase()
{
	bool i;
	//创建定时器
	if( NULL == frameTimer ){
		frameTimer = new QTimer(this);

		i =connect( frameTimer, SIGNAL(timeout()), this, SLOT( excFrameAnimation()) );
		frameTimer->start(50);
	}
	Logger<<"曹"<<i<<std::endl;
	//if(!REAL_TIME_RENDER)
}

void main_window::excFrameAnimation()
{

	main_canvas_->updateGL();
}

bool main_window::increaseInterval()
{
	if(  StateManager::getInstance().state() ==  RUNSTATE){
//		Logger<<" add duration"<<std::endl;
		StateManager::getInstance().increaseInterval();


	}else if(StateManager::getInstance().state() ==  PAUSESTATE){
//		Logger<<" add duration"<<std::endl;
		StateManager::getInstance().increaseInterval();
	}else if(StateManager::getInstance().state() ==  STOPSTATE){
//		Logger<<" old state is stop state"<<std::endl;
	}
	return false;
}

bool main_window::decreaseInterval()
{
	std::cout<<"decreaseInterval()"<<std::endl;
	if(  StateManager::getInstance().state() ==  RUNSTATE){
//		Logger<<" lessenduration"<<std::endl;
		StateManager::getInstance().decreaseInterval();


	}else if(StateManager::getInstance().state() ==  PAUSESTATE){
//		Logger<<" lessenduration"<<std::endl;
		StateManager::getInstance().decreaseInterval();
	}else if(StateManager::getInstance().state() ==  STOPSTATE){
//		Logger<<"lessenduration"<<std::endl;
		
	}
	return false;
}

bool main_window::saveSnapshot()
{ 
	SaveSnapshotDialog dialog(this);

	dialog.setValues(getCanvas()->ss);

	if (dialog.exec()==QDialog::Accepted)
	{
	getCanvas()->ss=dialog.getValues();
	getCanvas()->saveSnapshot();

	// if user ask to add the snapshot to raster layers
	/*
	if(dialog.addToRasters())
	{
	  QString savedfile = QString("%1/%2%3.png")
	.arg(GLA()->ss.outdir).arg(GLA()->ss.basename)
	.arg(GLA()->ss.counter,2,10,QChar('0'));

	  importRaster(savedfile);
	}
	*/
	return true;
	}

	return false;
}
bool main_window::savePLY()
{
	SavePlyDialog dialog(this);

	dialog.setValues(getCanvas()->splys);

	if (dialog.exec()==QDialog::Accepted)
	{
		
		getCanvas()->savePLY( dialog.getValues());

	// if user ask to add the snapshot to raster layers
	/*
	if(dialog.addToRasters())
	{
	  QString savedfile = QString("%1/%2%3.png")
	.arg(GLA()->ss.outdir).arg(GLA()->ss.basename)
	.arg(GLA()->ss.counter,2,10,QChar('0'));

	  importRaster(savedfile);
	}
	*/
	return true;
	}

	return false; 
}
bool main_window::saveLabelFile()
{
	getCanvas()->saveLabelFile();
	return true;
}
bool main_window::getLabelFromFile()
{
	getCanvas()->getLabelFromFile();
	return true;
}
bool main_window::wakeUpThread()
{

	mWaitcond.wakeOne();
	//REAL_TIME_RENDER =1;
	return true;

}
void main_window::logText(QString as,int level)
{
	//std::cout<<"logText"<<as.toStdString()<<std::endl;
	string tmp = as.toStdString();
	char* text = (char*)tmp.c_str();
	int mode = level;
	GLLogStream::Levels lveltype;
	switch(mode){
		case 0 : lveltype  = GLLogStream::SYSTEM;break;
		case 1 : lveltype  = GLLogStream::WARNING;break;
		case 2 : lveltype  = GLLogStream::FILTER;break;
		case 3 : lveltype  = GLLogStream::DEBUG;break;
		default: lveltype  = GLLogStream::DEBUG;
	}
	logstream.Log( lveltype,text);
	m_layer->updateLog(logstream);
	//return true;

}

void main_window::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		loadFile(action->data().toString());

}
void main_window::updateRecentFileActions()
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recentFileActs[j]->setVisible(false);

	separatorAct->setVisible(numRecentFiles > 0);



}
QString main_window::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}
void main_window::loadFile(const QString &dir)
{
	//QString dir = QFileDialog::getExistingDirectory(this,tr("Import point cloud files"),".");
	if (dir.isEmpty())
		return ;
	setCurrentFile(dir);

	resetSampleSet();

	QDir file_dir(dir);
	if ( !file_dir.exists() )
	{
		QMessageBox::warning(this, tr("Recent Files"),
			tr("Cannot read file %1:\n%2.")
			.arg("ss")
			.arg("ss"));
		return;
	}
	file_dir.setFilter(QDir::Files);

	QFileInfoList file_list = file_dir.entryInfoList();
	IndexType sample_idx = 0;
	for (IndexType file_idx = 0; file_idx < file_list.size(); file_idx++)
	{
		QFileInfo file_info = file_list.at(file_idx);
		FileIO::FILE_TYPE file_type;

		if (file_info.suffix() == "xyz")
		{
			file_type = FileIO::XYZ;
		}
		else if(file_info.suffix() == "ply")
		{
			file_type = FileIO::PLY;
		}
		else if(file_info.suffix() == "obj")
		{
			file_type = FileIO::OBJ;
		}
		else
		{
			continue;
		}

		string file_path = file_info.filePath().toStdString();
		cur_import_files_attr_.push_back( make_pair(FileSystem::base_name(file_path), 
			FileSystem::extension(file_path)) );

		Sample* new_sample = FileIO::load_point_cloud_file(file_path, file_type);
		if (new_sample != nullptr)
		{
			new_sample->isload_ = true;
			new_sample->set_color( Color_Utility::span_color_from_table( file_idx ) );
			SampleSet& smpset = SampleSet::get_instance();
			smpset.push_back(new_sample);					
			new_sample->smpId = sample_idx;
			sample_idx++;
		}


	}
	createTreeWidgetItems();
	m_layer->updateTable();

	//QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	//textEdit->setPlainText(in.readAll());
	QApplication::restoreOverrideCursor();
//	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);

}

void main_window::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	std::cout<<curFile.toStdString()<<std::endl;
	//QDir::setCurrent(curFile);
	//setWindowFilePath(curFile);

	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	settings.setValue("recentFileList", files);

	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		main_window *mainWin = qobject_cast<main_window *>(widget);
		if (mainWin)
			mainWin->updateRecentFileActions();
	}

}

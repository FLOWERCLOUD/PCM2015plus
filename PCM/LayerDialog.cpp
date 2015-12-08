#include "LayerDialog.h"

//#include <GL/glew.h>
#include <QToolBar>
#include <QActionGroup>
#include <QDebug>
#include "ui_layerDialog.h"
#include "sample_set.h"
#include "main_window.h"
#include "GLLogStream.h"
using namespace std;



FrameTreeWidgetItem::FrameTreeWidgetItem(Sample* smp , QTreeWidget* tree , QWidget* additional): QTreeWidgetItem( tree)
{
	if( smp->is_visible())
		setIcon(0,QIcon("Resources/visible.png"));
	else
		setIcon(0,QIcon("Resources/invisible.png"));
	setText(1, QString::number(smp->smpId));

	QString meshName = QString("test frame")+ QString(smp->smpId);
	//if (meshModel->meshModified())
	//	meshName += " *";
	setText(2, meshName);
	if (additional != NULL)
		tree->setItemWidget(this,3,additional);
	m = smp;
}

FrameTreeWidgetItem::~FrameTreeWidgetItem()
{

}

LayerDialog::LayerDialog(QWidget* parent /*= 0*/) :QDockWidget(parent)
{
	ui_ = new Ui::layerDialog();
	setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );
	this-> QWidget::setAttribute( Qt::WA_MacAlwaysShowToolWindow);
	setVisible(false);
	ui_->setupUi(this);
	mv_ = qobject_cast<main_window*>(parent);
	this->layout();

	connect(ui_->frameTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(frameItemClicked(QTreeWidgetItem * , int ) ) );

	connect(ui_->frameTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));
	connect(ui_->frameTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));

	// The following connection is used to associate the click with the switch between raster and mesh view.
	

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	ui_->frameTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui_->rasterTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui_->decParsTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui_->frameTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	connect(ui_->rasterTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	connect(ui_->decParsTree, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
}

void LayerDialog::frameItemClicked(QTreeWidgetItem* _item, int col)
{
	FrameTreeWidgetItem* mItem = dynamic_cast<FrameTreeWidgetItem*>(_item);
	if(mItem){
		int clickedId = mItem->m->smpId;
		switch(col){
		case 0:{
					SampleSet& smpset = SampleSet::get_instance(); 
					if(QApplication::keyboardModifiers() == Qt::ControlModifier){

						foreach( Sample* msmp  , smpset.getSmpVector()){

							msmp->set_visble(false);
							
						}

					}
					bool p =smpset[clickedId].is_visible();
					smpset[clickedId].set_visble(!p);
					


			   }break;

		case 1:

		case 2:

		case 3:
			
			/////

			break;
		}
		updateTable();
		// 此处应有更新界面的函数


	}
}





void LayerDialog::updateLog(GLLogStream& log)
{
	QList< pair<int,QString> > &logStringList=log.S;
	ui_->logPlainTextEdit->clear();
	//ui->logPlainTextEdit->setFont(QFont("Courier",10));

	pair<int,QString> logElem;
	QString preWarn    = "<font face=\"courier\" size=4 color=\"red\"> Warning: " ;
	QString preSystem  = "<font face=\"courier\" size=3 color=\"grey\">" ;
	QString preFilter  = "<font face=\"courier\" size=3 color=\"black\">" ;

	QString post   = "</font>";
	QString logText;
	foreach(logElem, logStringList){
		logText += logElem.second;
		if(logElem.first == GLLogStream::SYSTEM)
			logText = preSystem + logText + post;
		if(logElem.first == GLLogStream::WARNING)
			logText = preWarn + logText + post;
		if(logElem.first == GLLogStream::FILTER)
			logText = preFilter + logText + post;
		logText += "<BR>";
	}
	ui_->logPlainTextEdit->appendHtml(logText);
}

void LayerDialog::updateTableVisibility( IndexType itemid)
{
	if(!isVisible()) return;
	/*ui_->frameTreeWidget->fin*/

}

void LayerDialog::updateTable()
{
	 if(!isVisible()) return;
    //if(isVisible() && !mw_)
    //{
    //    setVisible(false);
    //    //The layer dialog cannot be opened unless a new document is opened
    //    return;
    //}
	 /* MeshDocument *md=mw->meshDoc();
	 this->setWindowTitle(md->docLabel());
	 */
    /*for(int ii = 0;ii < tobedel.size();++ii)
        delete tobedel[ii];*/
    ui_->frameTreeWidget->clear();

    ui_->frameTreeWidget->setColumnCount(3);
    ui_->frameTreeWidget->setColumnWidth(0,40);
    ui_->frameTreeWidget->setColumnWidth(1,40);
    //ui->meshTreeWidget->setColumnWidth(2,40);
    ui_->frameTreeWidget->header()->hide();
    FrameTreeWidgetItem *selitem = NULL;

	SampleSet& smpset = SampleSet::get_instance(); 
	foreach( Sample* msmp  , smpset.getSmpVector()){

		//msmp->set_visble(false);
		FrameTreeWidgetItem *item = new FrameTreeWidgetItem(msmp,ui_->frameTreeWidget,NULL);

		if(! msmp->is_visible()){
			item->setBackground(1,QBrush(Qt::white));
			item->setForeground(1,QBrush(Qt::blue));
			item->setBackground(2,QBrush(Qt::white));
			item->setForeground(2,QBrush(Qt::blue));
			item->setBackground(3,QBrush(Qt::white));
			item->setForeground(3,QBrush(Qt::blue));

		}
		// item->setExpanded(expandedMap.value(qMakePair( msmp->smpId,-1)));
		 ui_->frameTreeWidget->addTopLevelItem(item);
		 addDefaultNotes(item , msmp);
	}

	int wid = 0;
	for(int i=0; i< ui_->frameTreeWidget->columnCount(); i++)
	{
		ui_->frameTreeWidget->resizeColumnToContents(i);
		wid += ui_->frameTreeWidget->columnWidth(i);
	}
	ui_->frameTreeWidget->setMinimumWidth(wid);

}



void LayerDialog::showEvent(QShowEvent*)
{
	updateTable();
}

void LayerDialog::showContextMenu(const QPoint& pos)
{

}

void LayerDialog::adaptLayout(QTreeWidgetItem* item)
{
	item->setExpanded(item->isExpanded());
	for(int i=3; i< ui_->frameTreeWidget->columnCount(); i++)
		ui_->frameTreeWidget->resizeColumnToContents(i);

	//Update expandedMap
	FrameTreeWidgetItem *mItem = dynamic_cast<FrameTreeWidgetItem *>(item);
	if(mItem){
		int meshId = mItem->m->smpId;
		bool ok;
		int tagId = mItem->text(2).toInt(&ok);
		if(!ok || tagId < 0 )
			//MeshTreeWidgetItems don't have a tag id, so we use -1
			updateExpandedMap(meshId, -1, item->isExpanded());
	}
	else { //Other TreeWidgetItems
		FrameTreeWidgetItem *parent = dynamic_cast<FrameTreeWidgetItem *>(item->parent());
		if(parent){
			int meshId = parent->m->smpId;
			bool ok;
			int tagId = item->text(2).toInt(&ok);
			if(!ok)
				updateExpandedMap(meshId, tagId, item->isExpanded());
		}
	}
}

void LayerDialog::addDefaultNotes(QTreeWidgetItem* parent , Sample* smp)
{
	QTreeWidgetItem *fileItem = new QTreeWidgetItem();
	fileItem->setText(2, QString("File"));
	//if(!meshModel->fullName().isEmpty())
	fileItem->setText(3,"expand test");
	parent->addChild(fileItem);
	updateColumnNumber(fileItem);

	QTreeWidgetItem *faceItem = new QTreeWidgetItem();
	faceItem->setText(2, QString("Faces"));
	faceItem->setText(3, QString::number(0));
	parent->addChild(faceItem);
	updateColumnNumber(faceItem);

	if(1){
	QTreeWidgetItem *edgeItem = new QTreeWidgetItem();
	edgeItem->setText(2, QString("Edges"));
	edgeItem->setText(3, QString::number(0));
	parent->addChild(edgeItem);
	updateColumnNumber(edgeItem);
	}
	QTreeWidgetItem *vertItem = new QTreeWidgetItem();
	vertItem->setText(2, QString("Vertices"));
	vertItem->setText(3, QString::number(smp->num_vertices()));
	parent->addChild(vertItem);
	updateColumnNumber(vertItem);
}

void LayerDialog::updateColumnNumber(const QTreeWidgetItem* item)
{
	int columnChild= item->columnCount();
	int columnParent = ui_->frameTreeWidget->columnCount();
	if(columnChild - columnParent>0)
		ui_->frameTreeWidget->setColumnCount(columnParent + (columnChild-columnParent));
}

void LayerDialog::updateExpandedMap(IndexType meshId ,IndexType tgId , bool expanded)
{
	 expandedMap.insert(qMakePair(meshId, tgId),expanded);
}
LayerDialog::~LayerDialog()
{
	delete ui_;
}

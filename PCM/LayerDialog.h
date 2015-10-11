#pragma  once
#include "basic_types.h"
#include <QTreeWidgetItem>
#include <QDockWidget>

class main_window;
class Sample;
class QTreeWidget;
class GLLogStream;
class QToolBar;
class QAction;


#include <QDialog>
namespace Ui
{
class layerDialog;

}


class FrameTreeWidgetItem : public QTreeWidgetItem
{
public:
	FrameTreeWidgetItem( Sample* , QTreeWidget* tree , QWidget* additional );
	~FrameTreeWidgetItem();
	Sample* m;

};

class LayerDialog : public QDockWidget
{
	Q_OBJECT
public:
	LayerDialog(QWidget* parent = 0);
	~LayerDialog();
	void updateLog(GLLogStream& log);
	
	public slots:
		void updateTableVisibility(IndexType itemid);
		void updateTable();
		void frameItemClicked( QTreeWidgetItem* , int);
		void showEvent(QShowEvent* );
		void showContextMenu( const QPoint& pos);
		void adaptLayout( QTreeWidgetItem* item);
private:
	Ui::layerDialog* ui_;
	main_window* mv_;

	QMap<QPair <IndexType , IndexType> ,bool> expandedMap;
	void addDefaultNotes( QTreeWidgetItem* parent , Sample* smp);
	void updateColumnNumber( const QTreeWidgetItem* item);

	void updateExpandedMap( IndexType meshId ,IndexType tgId , bool expanded );
	
	QMap<QAction* , QMap<FrameTreeWidgetItem* , QAction*> > maintb_sidetb_map;
signals:

};

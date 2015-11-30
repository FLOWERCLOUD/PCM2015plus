#pragma  once
#include <QDialog>
#include <QFileDialog>
#include "snapshotsetting.h"
namespace Ui
{
	class SSDialog;
}
class SaveSnapshotDialog : public QDialog
{
Q_OBJECT
public:
	SaveSnapshotDialog(QWidget* parent = 0);
	~SaveSnapshotDialog();
	void setValues(const SnapshotSetting& ss);
	SnapshotSetting getValues();
private slots:
	void browseDir();
private:
	Ui::SSDialog* ui;
	SnapshotSetting settings;


};
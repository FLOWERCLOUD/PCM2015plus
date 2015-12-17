#pragma  once
#include <QDialog>
#include <QFileDialog>
#include "saveplysetting.h"
namespace Ui
{
	class SavePlyDialog;
}
class SavePlyDialog : public QDialog
{
	Q_OBJECT
public:
	SavePlyDialog(QWidget* parent = 0);
	~SavePlyDialog();
	void setValues(const SavePlySetting& ss);
	SavePlySetting getValues();
	private slots:
		void browseDir();
private:
	Ui::SavePlyDialog* ui;
	SavePlySetting settings;
};
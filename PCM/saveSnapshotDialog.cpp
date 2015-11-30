#include "saveSnapshotDialog.h"

#include "ui_savesnapshotDialog.h"
#include "saveSnapshotDialog.h"

SaveSnapshotDialog::SaveSnapshotDialog(QWidget * parent):QDialog(parent)
{
	ui = new Ui::SSDialog();
	SaveSnapshotDialog::ui->setupUi(this);
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->browseDir, SIGNAL(clicked()),this, SLOT(browseDir()));
	ui->outDirLineEdit->setReadOnly(true);
	//	setFixedSize(250,130);
}

void SaveSnapshotDialog::setValues(const SnapshotSetting& ss)
{
	settings=ss;
	ui->baseNameLineEdit->setText(settings.basename);
	ui->outDirLineEdit->setText(settings.outdir);
	ui->resolutionSpinBox->setValue(settings.resolution);
	ui->counterSpinBox->setValue(settings.counter);	
}

SnapshotSetting SaveSnapshotDialog::getValues()
{
	settings.basename=ui->baseNameLineEdit->text();
	settings.outdir=ui->outDirLineEdit->text();
	settings.counter=ui->counterSpinBox->value();
	settings.resolution=ui->resolutionSpinBox->value();
	settings.transparentBackground=ui->backgroundCheckBox->isChecked();
	settings.snapAllLayers=ui->alllayersCheckBox->isChecked();
	settings.tiledSave=ui->tiledSaveCheckBox->isChecked();
	/*settings.addToRasters=ui->addToRastersCheckBox->isChecked();*/
	return settings;
}

void SaveSnapshotDialog::browseDir()
{
	QFileDialog fd(0,"Choose output directory");
	fd.setFileMode(QFileDialog::DirectoryOnly);

	QStringList selection;
	if (fd.exec())
	{
		selection = fd.selectedFiles();
		ui->outDirLineEdit->setText(selection.at(0));;	
	}

}

SaveSnapshotDialog::~SaveSnapshotDialog()
{
	delete ui;
}


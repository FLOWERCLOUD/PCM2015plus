#include "savePLYDialog.h"
#include "ui_savePlyDialog.h"



SavePlyDialog::SavePlyDialog(QWidget* parent /*= 0*/):QDialog(parent)
{
	ui = new Ui::SavePlyDialog();
	SavePlyDialog::ui->setupUi(this);
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->browseDir, SIGNAL(clicked()),this, SLOT(browseDir()));
	ui->outDirLineEdit->setReadOnly(true);
}

SavePlyDialog::~SavePlyDialog()
{
	delete ui;
}

void SavePlyDialog::setValues(const SavePlySetting& ss)
{
	settings=ss;
	ui->baseNameLineEdit->setText(settings.basename);
	ui->outDirLineEdit->setText(settings.outdir);
	//ui->resolutionSpinBox->setValue(settings.resolution);
	ui->counterSpinBox->setValue(settings.counter);	
}

SavePlySetting SavePlyDialog::getValues()
{
	settings.basename=ui->baseNameLineEdit->text();
	settings.outdir=ui->outDirLineEdit->text();
	settings.counter=ui->counterSpinBox->value();
	//settings.resolution=ui->resolutionSpinBox->value();
	//settings.transparentBackground=ui->backgroundCheckBox->isChecked();
	//settings.snapAllLayers=ui->alllayersCheckBox->isChecked();
	//settings.tiledSave=ui->tiledSaveCheckBox->isChecked();
	/*settings.addToRasters=ui->addToRastersCheckBox->isChecked();*/
	return settings;
}

void SavePlyDialog::browseDir()
{
	QFileDialog fd(0,"Choose output directory");
	fd.setFileMode(QFileDialog::DirectoryOnly);

	QStringList selection;
	if (fd.exec())
	{
		selection = fd.selectedFiles();
		ui->outDirLineEdit->setText( selection.at(0) );	//D:\point_data\qinghuadata\standard\single
	}
}

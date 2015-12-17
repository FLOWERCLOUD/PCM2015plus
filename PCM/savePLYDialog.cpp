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
	ui->checkbox_isadjustview->setChecked(settings.isAjustView);
	ui->checkBox_isoricolor->setChecked(settings.isOriColor);
	ui->checkBox_is_labeledcolor->setChecked(settings.islabeledcor);
	ui->line_startframe->setText(settings.startframe);
	ui->line_endframe->setText(settings.endframe);
	if(settings.format == "ply"){
		ui->comboBox->setCurrentIndex(0);
	}
	else if(settings.format == "xyz")
	{
		ui->comboBox->setCurrentIndex(1);
	}
	else if(settings.format == "obj"){
		ui->comboBox->setCurrentIndex(2);
	}
}

SavePlySetting SavePlyDialog::getValues()
{
	settings.basename=ui->baseNameLineEdit->text();
	settings.outdir=ui->outDirLineEdit->text();
	settings.format = ui->comboBox->currentText();
	settings.isAjustView = ui->checkbox_isadjustview;
	settings.isOriColor = ui->checkBox_isoricolor;
	settings.islabeledcor = ui->checkBox_is_labeledcolor;
	settings.startframe = ui->line_startframe->text();
	settings.endframe = ui->line_endframe->text();
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

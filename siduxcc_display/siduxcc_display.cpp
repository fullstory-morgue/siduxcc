
#include <qcombobox.h>
#include <kgenericfactory.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <kled.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qlistbox.h>



#include "siduxcc_display.h"

typedef KGenericFactory<siduxcc_display, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_display, ModuleFactory("siduxccdisplay") )

siduxcc_display::siduxcc_display(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() != 0)
	{
		// Disable User-Input-Widgets
		selectResolution->setDisabled(true);
		fontButton->setDisabled(true);
		mlButton->setDisabled(true);
		restartxButton->setDisabled(true);
	}

}

void siduxcc_display::load()
{

	// get Resolution
	this->shell->setCommand("xdpyinfo | sed -n 's/^.*dimensions:\\s*\\(.*\\) pixels.*$/\\1/p'");
	this->shell->start(true);
	QString resolution = this->shell->getBuffer().stripWhiteSpace();


	// set the resolution as current item for selectResolution
	for(int i = 0; i < selectResolution->count(); i++)
	{
		if(selectResolution->text(i) == resolution)
		{
			selectResolution->setCurrentItem(i);
			break;
		}
	}

}


void siduxcc_display::save()
{
	backup();
	this->shell->setCommand("fix-res "+selectResolution->currentText());
	this->shell->start(true);
}


void siduxcc_display::fontSlot()
{
	this->shell->setCommand("konsole -T 'Fix-Fonts (Sidux-Control-Center)' --nomenubar --notabbar -e fix-fonts&");
	this->shell->start(true);
}

void siduxcc_display::backup()
{
	// backup the file xorg.conf
	this->shell->setCommand("date +%y%m%d%H%M");
	this->shell->start(true);
	QString atime = this->shell->getBuffer().stripWhiteSpace();
	this->shell->setCommand("cp /etc/X11/xorg.conf /etc/X11/xorg.conf-"+atime+".bak");
	this->shell->start(true);
}


void siduxcc_display::mlSlot()
{
	backup();
	this->shell->setCommand("perl -pi -e 's/(Mode.ine.*\\n)/#\\1/' /etc/X11/xorg.conf");
	this->shell->start(true);
	messageText->setText("Modelines removed!");
}

void siduxcc_display::restartxSlot()
{
	this->shell->setCommand("nohup /usr/share/siduxcc/restartx.sh");
	this->shell->start(true);
}

void siduxcc_display::defaults(){}


#include "siduxcc_display.moc"

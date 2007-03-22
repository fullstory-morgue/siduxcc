#include <qlineedit.h>
#include <qtextedit.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>

#include "siduxcc_info.h"

typedef KGenericFactory<siduxcc_info, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_info, ModuleFactory("siduxccinfo") )

siduxcc_info::siduxcc_info(QWidget *parent, const char *name, const QStringList &)
:InfoDialog(parent, name)
{
	this->shell = new Process();
	load();
}

void siduxcc_info::load(){
	this->shell->setCommand("cat /etc/sidux-version");
	this->shell->start(true);
	siduxVersion->setText(this->shell->getBuffer().simplifyWhiteSpace());

	this->shell->setCommand("infobash -v3 1 | perl -pe 's///g;'");
	this->shell->start(true);
	infobash->setText(this->shell->getBuffer());
}

void siduxcc_info::save(){}
void siduxcc_info::defaults(){}

#include "siduxcc_info.moc"

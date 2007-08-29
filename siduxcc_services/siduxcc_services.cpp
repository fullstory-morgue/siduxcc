/*
 * siduxcc_services.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * siduxcc_services is based on knxcc_services from Andreas Loible
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <qcombobox.h>
#include <kgenericfactory.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <kled.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qlistbox.h>

#include "siduxcc_services.h"

typedef KGenericFactory<siduxcc_services, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_services, ModuleFactory("siduxccservices") )

siduxcc_services::siduxcc_services(QWidget *parent, const char *name, const QStringList &)
:ServicesDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() != 0)
	{
		// Disable User-Input-Widgets
		actionGroup->setDisabled(true);
		serviceDisable->setDisabled(true);
		serviceEnable->setDisabled(true);
	}
}

void siduxcc_services::load()
{
	// add services
	servicesBox->insertItem("apache2");	//0
	servicesBox->insertItem("cupsys");	//1
	servicesBox->insertItem("mysql");	//2
	servicesBox->insertItem("samba");	//3
	servicesBox->insertItem("ssh");		//4
	servicesBox->setCurrentItem(0);

	// update service ionformations
	updateSlot();
}


void siduxcc_services::save()
{

	// update Service
	QString service = servicesBox->currentText();

	if(actionStart->isChecked())
	{
		this->shell->setCommand("/etc/init.d/"+service+" start");
		this->shell->start(true); sleep(1);
	}
	else if(actionStop->isChecked())
	{
		this->shell->setCommand("/etc/init.d/"+service+" stop");
		this->shell->start(true); sleep(1);
	}
	else if(actionRestart->isChecked())
	{
		this->shell->setCommand("/etc/init.d/"+service+" restart");
		this->shell->start(true); sleep(1);
	}
	else if(actionReload->isChecked())
	{
		this->shell->setCommand("/etc/init.d/"+service+" reload");
		this->shell->start(true); sleep(1);
	}
	else if(actionAdd->isChecked())
	{
		this->shell->setCommand("update-rc.d -f "+service+" defaults");
		this->shell->start(true);
	}
	else if(actionDelete->isChecked())
	{
		this->shell->setCommand("update-rc.d -f "+service+" remove");
		this->shell->start(true);
	}

	// Services
	for ( QValueList<QString>::Iterator it = this->servicesToDisable.begin(); it != this->servicesToDisable.end(); ++it ) {
		QString service = *it;
		this->shell->setCommand("update-rc.d -f "+service+" remove");
		this->shell->start(true);
	}
	this->servicesToDisable = QStringList();
	for ( QValueList<QString>::Iterator it = this->servicesToEnable.begin(); it != this->servicesToEnable.end(); ++it ) {
		QString service = *it;
		this->shell->setCommand("update-rc.d -f "+service+" defaults");
		this->shell->start(true);
	}
	this->servicesToEnable = QStringList();

	updateSlot();

}

void siduxcc_services::defaults(){}


void siduxcc_services::updateSlot()
{

	// status
	QString pid [50];
	pid[0] = "apache2";
	pid[1] = "cupsd";
	pid[2] = "mysqld";
	pid[3] = "smbd";
	pid[4] = "sshd";

	this->shell->setCommand("pidof "+pid[servicesBox->currentItem()]);
	this->shell->start(true);
	if(this->shell->exitStatus())
	{
		statusLabel->setText(i18n("Status: %1").arg(i18n("not running...")));
		statusLed->setColor(QColor(0xff0000));
	}
	else
	{
		statusLabel->setText(i18n("Status: %1").arg(i18n("running...")));
		statusLed->setColor(QColor(0x00ff00));
	} 

	// description
	QString description [50];
	description[0] = i18n("Apache v2 is the next generation of the omnipresent Apache web server.");
	description[1] = i18n("The Common UNIX Printing System (or CUPS(tm)) is a printing system and general replacement for lpd and the like.");
	description[2] = i18n("MySQL is a fast, stable and true multi-user, multi-threaded SQL database server.");
	description[3] = i18n("The Samba software suite is a collection of programs that implements the SMB/CIFS protocol for unix systems, allowing you to serve files and printers to Windows, NT, OS/2 and DOS clients.");
	description[4] = i18n("SSH is the portable version of OpenSSH, a free implementation of the Secure Shell protocol as specified by the IETF secsh working group.");

	descriptionLabel->setText(description[servicesBox->currentItem()]);


	 // Services
	activeServices->clear();
	this->shell->setCommand("perl /usr/share/siduxcc/scripts/rcconf.pl | egrep ^active | cut -d\\  -f2");
	this->shell->start(true);
	QStringList activeServicesList = QStringList::split( "\n", this->shell->getBuffer() );
	for ( QStringList::Iterator it = activeServicesList.begin(); it != activeServicesList.end(); ++it ) {
		QString service = *it;
		activeServices->insertItem(service);
	}

	disabledServices->clear();
	this->shell->setCommand("perl /usr/share/siduxcc/scripts/rcconf.pl | egrep ^disabled | cut -d\\  -f2");
	this->shell->start(true);
	QStringList disabledServicesList = QStringList::split( "\n", this->shell->getBuffer() );
	for ( QStringList::Iterator it = disabledServicesList.begin(); it != disabledServicesList.end(); ++it ) {
		QString service = *it;
		disabledServices->insertItem(service);
	}


}

void siduxcc_services::disableService(){
	QString currentService = activeServices->currentText();
	if(!currentService) return;
	QValueList<QString>::Iterator it = this->servicesToEnable.find(currentService);
	if(*it) this->servicesToEnable.erase(it);
	this->servicesToDisable.append(currentService);
	activeServices->removeItem(activeServices->currentItem());
	activeServices->clearSelection();
	disabledServices->insertItem(currentService);
	disabledServices->sort();
	disabledServices->clearSelection();
	emit changed(true);
}

void siduxcc_services::enableService(){
	QString currentService = disabledServices->currentText();
	if(!currentService) return;
	QValueList<QString>::Iterator it = this->servicesToDisable.find(currentService);
	if(*it) this->servicesToDisable.erase(it);
	this->servicesToEnable.append(currentService);
	disabledServices->removeItem(disabledServices->currentItem());
	disabledServices->clearSelection();
	activeServices->insertItem(currentService);
	activeServices->sort();
	activeServices->clearSelection();
	emit changed(true);
}


#include "siduxcc_services.moc"

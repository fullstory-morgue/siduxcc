/*
 * services.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * services is based on knxcc_services from Andreas Loible
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
#include <kled.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qwidgetstack.h>
#include <qcheckbox.h>


#include "services.h"


#include <qmessagebox.h>

services::services(QWidget *parent, const char *name, const QStringList &)
:ServicesDialog(parent, name)
{
	this->shell = new Process();
	load();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void services::load()
{
	// add services
	servicesBox->insertItem("apache2");	//0
	servicesBox->insertItem("cupsys");	//1
	servicesBox->insertItem("mysql");	//2
	servicesBox->insertItem("samba");	//3
	servicesBox->insertItem("ssh");		//4
	servicesBox->setCurrentItem(0);

	// status
	pid.append("apache2");
	pid.append("cupsd");
	pid.append("mysqld");
	pid.append("smbd");
	pid.append("sshd");

	// description

	description.append( i18n("Apache v2 is the next generation of the omnipresent Apache web server.") );
	description.append( i18n("The Common UNIX Printing System (or CUPS(tm)) is a printing system and general replacement for lpd and the like.") );
	description.append( i18n("MySQL is a fast, stable and true multi-user, multi-threaded SQL database server.") );
	description.append( i18n("The Samba software suite is a collection of programs that implements the SMB/CIFS protocol for unix systems, allowing you to serve files and printers to Windows, NT, OS/2 and DOS clients.") );
	description.append( i18n("SSH is the portable version of OpenSSH, a free implementation of the Secure Shell protocol as specified by the IETF secsh working group.") );

	// update service ionformations
	updateSlot();

	applyPushButton->setEnabled(FALSE);

}


void services::loadWidget(int i)
{
	widgetStack2->raiseWidget(i+1);
	widgetStack3->raiseWidget(1);
}


void services::back()
{
	widgetStack2->raiseWidget(0);
	widgetStack3->raiseWidget(0);
}


//------------------------------------------------------------------------------
//--- save ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void services::save()
{

	// update Service
	QString service = servicesBox->currentText();

	if(runlevelCheckBox->isChecked())
	{
		this->shell->setCommand("update-rc.d -f "+service+" defaults");
		this->shell->start(true);
	}
	else
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

	applyPushButton->setEnabled(FALSE);

}


void services::hasChanged()
{
	applyPushButton->setEnabled(TRUE);
}

//------------------------------------------------------------------------------
//--- run service action -------------------------------------------------------
//------------------------------------------------------------------------------

void services::startService()
{
	runServiceAction("start");
}

void services::stopService()
{
	runServiceAction("stop");
}
void services::restartService()
{
	runServiceAction("restart");
}

void services::reloadService()
{
	runServiceAction("reload");
}

void services::runServiceAction(QString action)
{
	QString service = servicesBox->currentText();
	this->shell->setCommand("/etc/init.d/"+service+" "+action);
	this->shell->start(true); sleep(1);
	updateSlot();
}

//------------------------------------------------------------------------------
//--- update -------------------------------------------------------------------
//------------------------------------------------------------------------------

void services::updateSlot()
{


	QString service = servicesBox->currentText();
	int i = servicesBox->currentItem();

	this->shell->setCommand("pidof "+pid[i]);
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


	descriptionLabel->setText(description[i]);


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


	this->shell->setCommand("ls /etc/rc*/*"+service);
	this->shell->start(true);
	if( this->shell->getBuffer() == "" )
		runlevelCheckBox->setChecked(FALSE);
	else
		runlevelCheckBox->setChecked(TRUE);


}


//------------------------------------------------------------------------------
//--- boot ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void services::disableService()
{
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
	hasChanged();
}


void services::enableService(){
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
	hasChanged();
}


#include "services.moc"

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
	description.append( i18n("CUPS (Common UNIX Printing System) is a printing system. It supports the IPP (Internet Printing Protocol), and has its own filtering driver model for handling various document types.") );
	description.append( i18n("MySQL is a fast, stable and true multi-user, multi-threaded SQL database server.") );
	description.append( i18n("The Samba software suite is a collection of programs that implements the SMB/CIFS protocol for unix systems, allowing you to serve files and printers to Windows, NT, OS/2 and DOS clients.") );
	description.append( i18n("SSH is the portable version of OpenSSH, a free implementation of the Secure Shell protocol as specified by the IETF secsh working group.") );

	// update service ionformations
	updateSlot();

	applyPushButton->setEnabled(FALSE);

	descriptionLabel2->setText("");
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


	descriptionLabel1->setText(description[i]);


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

void services::showDescription(QListBoxItem* input)
{
	QString text;
	if( input->text() == "acpid" )
		 text = i18n("Advanced Configuration and Power Interface (ACPI) to allow intelligent power management on your system.");
	else if( input->text() == "apache2" )
		 text = i18n("Apache v2 is the next generation of the omnipresent Apache web server.");
	else if( input->text() == "cron" )
		text = i18n("Cron is a time-based scheduling service.");
	else if( input->text() == "gpm" )
		text = i18n("gpm means general purpose mouse and is the mouse support for linux in the console.");
	else if( input->text() == "hal" )
		text = i18n("HAL provides an abstract view on hardware. This abstraction layer is simply an interface that makes it possible to add support for new devices and new ways of connecting devices to the computer, without modifying every application that uses the device.");
	else if( input->text() == "ifplugd" )
		text = i18n("Ifplugd is a daemon which will automatically configure your ethernet device when a cable is plugged in and automatically unconfigure it if the cable is pulled. This is useful on laptops with onboard network adapters, since it will only configure the interface when a cable is really connected.");
	else if( input->text() == "IrDA" )
		text = i18n("IrDA (Infrared Data Association) is an industry standard for infrared wireless communication. Most laptops sold today are equipped with an IrDA compliant infrared transceiver, which enables you to communicate with devices such as printers, modems, fax, LAN, and other laptops.");
	else if( input->text() == "klogd" )
		text = i18n("klogd is a system daemon which intercepts and logs Linux kernel messages.");
	else if( input->text() == "rsync" )
		text = i18n("Rsync is a command line utility traditionally used in synchronizing files between two computers, but rsync can also be used as an effective backup tool. In daemon mode, rsync listens to the default TCP port of 873, serving files in the native rsync protocol. rsync can also be used to synchronize local directories, or via a remote shell such as RSH or SSH.");
	else if( input->text() == "ssh" )
		text = i18n("SSH is the portable version of OpenSSH, a free implementation of the Secure Shell protocol as specified by the IETF secsh working group.");
	else if( input->text() == "sysklog" )
		text = i18n("Sysklogd provides two system utilities which provide support for system logging and kernel message trapping. Support of both internet and unix domain sockets enables this utility package to support both local and remote logging.");
	else if( input->text() == "vdr" )
		text = i18n("Linux Video Disk Recorder (vdr)");
	else if( input->text() == "vdradmin" )
		text = i18n("vdradmin-am provides a webinterface for managing the Linux Video Disk Recorder (vdr)");
	else if( input->text() == "bluetooth" )
		text = i18n("Enables Universal interface for wireless communication");
	else if( input->text() == "cupsys" )
		text = i18n("CUPS (Common UNIX Printing System) is a printing system. It supports the IPP (Internet Printing Protocol), and has its own filtering driver model for handling various document types.");
	else if( input->text() == "irqbalance" )
		text = i18n("Irqbalance is a Linux daemon that distributes interrupts over the processors and cores you have in your computer system. The design goal of irqbalance is to do find a balance between power savings and optimal performance.");
	else if( input->text() == "isdnutils" )
		text = i18n("Isndutils is only for use with an internal ISDN adapter.");
	else if( input->text() == "lirc" )
		text = i18n("LIRC is a package that allows you to decode and send infra-red signals of many (but not all) commonly used remote controls.");
	else if( input->text() == "mysql" )
		text = i18n("MySQL is a fast, stable and true multi-user, multi-threaded SQL database server.");
	else if( input->text() == "virtualbox-ose" )
		text = i18n("VirtualBox is an application installed on an existing host operating system; within this application, additional operating systems can be loaded and run, each with its own virtual environment.");


	descriptionLabel2->setText( text );

}


#include "services.moc"

/*
 * siduxcc_network.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * siduxcc_network is based on knxcc_network from Andreas Loible
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

#include <kgenericfactory.h>
#include <kpushbutton.h>
#include <klistview.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <keditlistbox.h>
#include <qgroupbox.h>


#include "siduxcc_network.h"

typedef KGenericFactory<siduxcc_network, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_network, ModuleFactory("siduxccnetwork") )

siduxcc_network::siduxcc_network(QWidget *parent, const char *name, const QStringList &)
:NetworkDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() == 0)
	{
		ncEnableButton->setEnabled(true);
		ncDisableButton->setEnabled(true);
		hostnameGroupBox->setEnabled(true);
		hostnameLineEdit->setEnabled(true);
		dnsServers->setEnabled(true);
		nwlButton->setEnabled(false);
	}
}


void siduxcc_network::load()
{
	getNetworkcards();
	ncInfoSlot();

	getHostname();
	getNameservers();
}


void siduxcc_network::save()
{
	setHostname();
	setNameservers();
}


//------------------------------------------------------------------------------
// netcardconfig


void siduxcc_network::getNetworkcards()
{

	ncList->clear();

	//add images
	QPixmap activeEthernetDeviceImg("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_up.png");
	QPixmap inactiveEthernetDeviceImg("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_down.png");
	QPixmap activeWirelessDeviceImg("/usr/share/icons/hicolor/22x22/apps/siduxcc_wlan_up.png");
	QPixmap inactiveWirelessDeviceImg("/usr/share/icons/hicolor/22x22/apps/siduxcc_wlan_down.png");

	// add networkcards to list
	this->shell->setCommand("nicinfo");
	this->shell->start(true);
	QStringList deviceList = QStringList::split( "\n", this->shell->getBuffer() );
	
	QStringList nicInfo;
	QStringList nicStatus;

	for(int i = 0; i < deviceList.count(); i++) {
		QListViewItem * item = new QListViewItem( ncList, 0 );
		nicInfo = QStringList::split( " ", deviceList[i] );

		// get device info
		nicInfo = QStringList::split( " ", deviceList[i] );
		//for(int j = 6; j < nicInfo.count(); j++) {
		//		nicInfo[5] = nicInfo[5]+" "+nicInfo[j]; }

		// get device status
		this->shell->setCommand("nicstatus "+nicInfo[0]);
		this->shell->start(true);
		nicStatus = QStringList::split( " ", this->shell->getBuffer() );
		for(int j = 0; j < nicStatus.count(); j++) {
				if(nicStatus[j] == "-") {nicStatus[j] = ""; }}

		item->setText(0, nicInfo[0]);   // device name (E.g. eth0)
		item->setText(1, nicStatus[0]); // ip-address
		item->setText(2, nicStatus[1]); // method (dhcp/static)
		item->setText(3, nicStatus[2]); // boot (enable/disable)
		item->setText(4, nicInfo[2]);   // driver (E.g. e1000)
		item->setText(5, nicInfo[3]);   // slot (E.g. pci)
		//item->setText(6, nicInfo[4]);   // mac-address (xx:xx:...)
		//item->setText(7, nicInfo[5]);   // description

		// set image
		if(nicInfo[1] == "ethernet") {
			if(nicStatus[0] != "") {item->setPixmap(0,activeEthernetDeviceImg);}
			else {item->setPixmap(0,inactiveEthernetDeviceImg);} }
		if(nicInfo[1] == "wireless") {
			if(nicStatus[0] != "") {item->setPixmap(0,activeWirelessDeviceImg);}
			else {item->setPixmap(0,inactiveWirelessDeviceImg);} }
	}
}

void siduxcc_network::ncInfoSlot()
{
	QListViewItem *item = ncList->currentItem();
	this->shell->setCommand("nicinfo "+item->text(0));
	this->shell->start(true);
	QStringList nicInfo = QStringList::split( " ", this->shell->getBuffer());
	for(int j = 6; j < nicInfo.count(); j++) {
		nicInfo[5] = nicInfo[5]+" "+nicInfo[j]; }

	macText2->setText(nicInfo[4]);         // mac-address (xx:xx:...)
	descriptionText2->setText(nicInfo[5]); // description
}


// start button commands

void siduxcc_network::ncConfigSlot()
{
	QListViewItem *item = ncList->currentItem();
	QString currentDevice = item->text(0);

	this->shell->setCommand("/usr/sbin/netcardconfig "+currentDevice+"&");
	this->shell->start(true);
}


void siduxcc_network::ncEnableSlot()
{
	QListViewItem *item = ncList->currentItem();
	QString currentDevice = item->text(0);
	this->shell->setCommand("ifup "+currentDevice+"&");
	this->shell->start(true);
	getNetworkcards();
}

void siduxcc_network::ncDisableSlot()
{
	QListViewItem *item = ncList->currentItem();
	QString currentDevice = item->text(0);
	this->shell->setCommand("ifdown "+currentDevice+"&");
	this->shell->start(true);
	getNetworkcards();
}



//------------------------------------------------------------------------------
// nameservers

void siduxcc_network::getHostname()
{
	this->shell->setCommand("siduxcc network getHostname");
	this->shell->start(true);
	hostnameLineEdit->setText( this->shell->getBuffer() );
}

void siduxcc_network::setHostname()
{
	this->shell->setCommand("siduxcc network setHostname "+hostnameLineEdit->text());
	this->shell->start(true);
}

void siduxcc_network::setNameservers()
{
	QString command = "siduxcc network setNameservers ";
	QStringList nameserverList = dnsServers->items();
	for ( QStringList::Iterator it = nameserverList.begin(); it != nameserverList.end(); ++it )
		command+=(*it) + " ";
	this->shell->setCommand(command);
	this->shell->start(true);
}

void siduxcc_network::getNameservers()
{
	this->shell->setCommand("siduxcc network getNameservers");
	this->shell->start(true);
	QStringList nameserverList = QStringList::split( "\n", this->shell->getBuffer() );
	dnsServers->clear();
	for ( QStringList::Iterator it = nameserverList.begin(); it != nameserverList.end(); ++it )
		dnsServers->insertItem(*it);
}

//------------------------------------------------------------------------------
// ndiswrapper

void siduxcc_network::nwSlot()
{
	this->shell->setCommand("su-me configure-ndiswrapper&");
	this->shell->start(true);
}

void siduxcc_network::nwlSlot()
{
	this->shell->setCommand("x-www-browser http://ndiswrapper.sourceforge.net/joomla/index.php?/component/option,com_openwiki/Itemid,33/id,list/&");
	this->shell->start(true);
}


#include "siduxcc_network.moc"

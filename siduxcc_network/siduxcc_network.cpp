/*
 * siduxcc_network.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
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
#include <kpushbutton.h>
#include <klistview.h>
#include <qlabel.h>
#include <klocale.h>

#include "siduxcc_network.h"

typedef KGenericFactory<siduxcc_network, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_network, ModuleFactory("siduxccnetwork") )

siduxcc_network::siduxcc_network(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() != 0)
	{
		// Disable User-Input-Widgets
		ncList->setDisabled(true);
		ncConfigButton->setDisabled(true);
		ncEnableButton->setDisabled(true);
		ncDisableButton->setDisabled(true);
		browserSelect->setDisabled(true);
		m3Button->setDisabled(false);
		d1Button->setDisabled(false);
		d2Button->setDisabled(false);
		d2Button->setDisabled(false);
		nwlButton->setDisabled(false);

	}
}

void siduxcc_network::load()
{
	getBrowsers();
	getNetworkcards();
}



void siduxcc_network::save()
{
	// Set browser 
	this->shell->setCommand("update-alternatives --set x-www-browser /usr/bin/"+browserSelect->currentText());
	this->shell->start(true);
}


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

	for(int i = 0; i < deviceList.count(); i++) {
		QListViewItem * item = new QListViewItem( ncList, 0 );
		QStringList valueList = QStringList::split( " ", deviceList[i] );

		// device (E.g. eth0)
		item->setText(0,valueList[0]);

		// ip address
		this->shell->setCommand("/sbin/ifconfig "+valueList[0]+" | grep \"inet addr\" | gawk -F: '{print $2}' | gawk '{print $1}'");
		this->shell->start(true);
		QString tmp = this->shell->getBuffer().stripWhiteSpace();
		item->setText(1,tmp);

		// active?
		if(valueList[1] == "ethernet") {
			if(tmp != "") {item->setPixmap(0,activeEthernetDeviceImg);}
			else {item->setPixmap(0,inactiveEthernetDeviceImg);} }
		if(valueList[1] == "wireless") {
			if(tmp != "") {item->setPixmap(0,activeWirelessDeviceImg);}
			else {item->setPixmap(0,inactiveWirelessDeviceImg);} }
		
		// method
		this->shell->setCommand("awk '/^\\s*iface/{if($2==\""+valueList[0]+"\"){print $4}}' /etc/network/interfaces");
		this->shell->start(true);
		item->setText(2,this->shell->getBuffer().stripWhiteSpace());

		// boot
		this->shell->setCommand("grep auto /etc/network/interfaces | grep -q "+valueList[0]+" && echo auto");
		this->shell->start(true);
		item->setText(3,this->shell->getBuffer().stripWhiteSpace());

		// driver
		item->setText(4,valueList[2]);

		// slot
		item->setText(5,valueList[3]);

		// description
		if(i==0) {
			for(int i = 6; i < valueList.count(); i++) {
				valueList[5] = valueList[5]+" "+valueList[i];}
 			ncLabel->setText(i18n("<b>Interface:</b> ")+valueList[0]+i18n("<br><b>MAC-address:</b> ")+valueList[4]+i18n("<br><b>Description:</b> ")+valueList[5]); }


	}


}

void siduxcc_network::ncDescSlot()
{

	QListViewItem *item = ncList->currentItem();

	this->shell->setCommand("nicinfo "+item->text(0));
	this->shell->start(true);
	QStringList valueList = QStringList::split( " ", this->shell->getBuffer());

	// description
	for(int i = 6; i < valueList.count(); i++) {
		valueList[5] = valueList[5]+" "+valueList[i];}
	// mac-address valueList[4]
 	ncLabel->setText(i18n("<b>Interface:</b> ")+valueList[0]+i18n("<br><b>MAC-address:</b> ")+valueList[4]+i18n("<br><b>Description:</b> ")+valueList[5]);

}


void siduxcc_network::getBrowsers()
{
	// get the current browser	
	this->shell->setCommand("update-alternatives --display x-www-browser | head -n 2 | tail -n 1 | cut -f4 -d/");
	this->shell->start(true);
	QString current = this->shell->getBuffer().stripWhiteSpace();


	this->shell->setCommand("update-alternatives --list x-www-browser");
	this->shell->start(true);
	QStringList browserList = QStringList::split( "\n", this->shell->getBuffer() );

	for(int i = 0; i < browserList.count(); i++) {
		QStringList browser = QStringList::split( "/", browserList[i] );
		browserSelect->insertItem(browser[2]);
		if(browser[2] == current) { browserSelect->setCurrentItem(i);}}
}


// start button commands

void siduxcc_network::nwSlot() {
	this->shell->setCommand("su-me configure-ndiswrapper&");
	this->shell->start(true); }
void siduxcc_network::nwlSlot() {
	this->shell->setCommand("x-www-browser http://ndiswrapper.sourceforge.net/mediawiki/index.php/List&");
	this->shell->start(true); }

void siduxcc_network::m1Slot() {
	this->shell->setCommand("su-me modemlink&");
	this->shell->start(true); }
void siduxcc_network::m2Slot() {
	this->shell->setCommand("su-me gprsconnect&");
	this->shell->start(true); }
void siduxcc_network::m3Slot() {
	this->shell->setCommand("kppp&");
	this->shell->start(true); }
void siduxcc_network::m4Slot() {
	this->shell->setCommand("su-me my-pppconf&");
	this->shell->start(true); }

void siduxcc_network::d1Slot() {
	this->shell->setCommand("fcdsl-pppoeconf&");
	this->shell->start(true); }
void siduxcc_network::d2Slot() {
	this->shell->setCommand("my-pppoeconf&");
	this->shell->start(true); }
void siduxcc_network::d3Slot() {
	this->shell->setCommand("su-me pppoeconf&");
	this->shell->start(true); }

void siduxcc_network::i1Slot() {
	this->shell->setCommand("su-me x-terminal-emulator -e capi-isdnconf&"); this->shell->start(true); }



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

#include "siduxcc_network.moc"

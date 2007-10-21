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
#include <qtabwidget.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <kmessagebox.h>
#include <kaboutdata.h>



#include "siduxcc_network.h"

typedef KGenericFactory<siduxcc_network, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_network, ModuleFactory("siduxccnetwork") )

siduxcc_network::siduxcc_network(QWidget *parent, const char *name, const QStringList &)
:NetworkDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);

	KAboutData* about = new KAboutData("siduxccnetwork", I18N_NOOP("sidux Control Center"), "");
	about->setProgramLogo( QImage("/usr/share/icons/hicolor/32x32/apps/siduxcc.png") );
	about->setShortDescription( I18N_NOOP("Frontend for managing sidux.") );
	about->setLicense(KAboutData::License_GPL_V2);
   about->setHomepage("http://www.sidux.com");
   about->setBugAddress("xadras@sidux.com");
	about->setCopyrightStatement("(c) 2007 Fabian Würtz");
	about->addAuthor("Fabian Würtz (xadras)", I18N_NOOP("Developer"), "xadras@sidux.com", "http://xadras.wordpress.com/");
	about->addCredit("Stefan Lippers-Hollmann (slh)", I18N_NOOP("Packaging") );
	about->addCredit("Andreas Loibl (Acritox)", I18N_NOOP("Developer of the knxcc") , "andreas@andreas-loibl.de", "http://www.andreas-loibl.de/");
	setAboutData(about);

	load();
	
	if (getuid() == 0)
	{
		ncEnableButton->setEnabled(true);
		ncDisableButton->setEnabled(true);
		hostnameGroupBox->setEnabled(true);
		hostnameLineEdit->setEnabled(true);
		dnsServers->setEnabled(true);
		fwInstallPushButton->setEnabled(true);
	}
}


void siduxcc_network::load()
{
	closeTabs();
	loadKonsole();
	konsoleFrame->installEventFilter( this );
}




void siduxcc_network::save()
{
	setHostname();
	setNameservers();
}



//------------------------------------------------------------------------------
//--- console ------------------------------------------------------------------
//------------------------------------------------------------------------------


void siduxcc_network::loadKonsole()
{
	KLibFactory* factory = KLibLoader::self()->factory( "libsanekonsolepart" );
	if (!factory)
		factory = KLibLoader::self()->factory( "libkonsolepart" );
	konsole = static_cast<KParts::Part*>( factory->create( konsoleFrame, "konsolepart", "QObject", "KParts::ReadOnlyPart" ) );
	terminal()->setAutoDestroy( true );
	terminal()->setAutoStartShell( false );
	konsole->widget()->setGeometry(0, 0, konsoleFrame->width(), konsoleFrame->height());	
}


bool siduxcc_network::eventFilter( QObject *o, QEvent *e )
{
	// This function makes the console emulator expanding
	if ( e->type() == QEvent::Resize )
	{
		QResizeEvent *re = dynamic_cast< QResizeEvent * >( e );
		if ( !re ) return false;
		konsole->widget()->setGeometry( 0, 0, re->size().width(), re->size().height() );
	}
	return false;
};


void siduxcc_network::back()
{
	widgetStack->raiseWidget(0);
	loadKonsole();
	konsoleFrame->installEventFilter( this );
}


//------------------------------------------------------------------------------
//--- overview -----------------------------------------------------------------
//------------------------------------------------------------------------------



void siduxcc_network::closeTabs()
{
	tabWidget->removePage(tab1);
	tabWidget->removePage(tab2);
	tabWidget->removePage(tab3);
	tabWidget->removePage(tab4);
	tabWidget->removePage(tab5);

}


void siduxcc_network::loadTab(QListBoxItem* entry)
{

	if(entry->text() == i18n("Configure Networkcards"))
	{
		getNetworkcards();
		ncInfoSlot();
		tabWidget->addTab(tab1, i18n("Netcardconfig") );
		tabWidget->showPage(tab1);
	}
	else if(entry->text() == i18n("Set Hostname") )
	{
		getHostname();
		tabWidget->addTab(tab2, i18n("Hostname") );
		tabWidget->showPage(tab2);
	}
	else if(entry->text() == i18n("Set Nameservers") )
	{
		getNameservers();
		tabWidget->addTab(tab3, i18n("Nameservers") );
		tabWidget->showPage(tab3);
	}
	else if(entry->text() == i18n("Install Wlan-Cards (Ndiswrapper)") )
	{
		tabWidget->addTab(tab4, i18n("Ndiswrapper") );
		tabWidget->showPage(tab4);
	}
	else if(entry->text() == i18n("Install Wlan-Cards (fwdetect)") )
	{
		fwDetect();
		tabWidget->addTab(tab5, i18n("fwdetect") );
		tabWidget->showPage(tab5);
	}
}


void siduxcc_network::viewOverview(QWidget* entry)
{
	if(entry == tab0)
	{
		closeTabs();
	}
}

//------------------------------------------------------------------------------
//--- netcardconfig ------------------------------------------------------------
//------------------------------------------------------------------------------


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
			if(nicStatus[3] == "running") {item->setPixmap(0,activeEthernetDeviceImg);}
			else {item->setPixmap(0,inactiveEthernetDeviceImg);} }
		if(nicInfo[1] == "wireless") {
			if(nicStatus[3] == "running") {item->setPixmap(0,activeWirelessDeviceImg);}
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
//--- hostname -----------------------------------------------------------------
//------------------------------------------------------------------------------


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


//------------------------------------------------------------------------------
//--- nameservers --------------------------------------------------------------
//------------------------------------------------------------------------------


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
//--- ndiswrapper --------------------------------------------------------------
//------------------------------------------------------------------------------


void siduxcc_network::nwSlot()
{
	this->shell->setCommand("su-me configure-ndiswrapper&");
	this->shell->start(true);
}

void siduxcc_network::nwlSlot()
{
	this->shell->setCommand("su wuertz -c \"x-www-browser http://ndiswrapper.sourceforge.net/joomla/index.php?/component/option,com_openwiki/Itemid,33/id,list/\"");
	this->shell->start(true);
}


//------------------------------------------------------------------------------
//--- fwdetect -----------------------------------------------------------------
//------------------------------------------------------------------------------


void siduxcc_network::fwDetect()
{
	// show firmware
	QPixmap hardwareImg("/usr/share/icons/hicolor/32x32/apps/siduxcc_hardware.png");
	hardwareList->clear();
	if(fwComboBox->currentText() == i18n("detected"))
	{ 
		this->shell->setCommand("siduxcc hardware detect");
	}
	else
	{
		this->shell->setCommand("siduxcc hardware allDevices");
	}
	this->shell->start(true);
	QStringList hardwareName = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for(int i = 0; i < hardwareName.count(); i++)
	{
		hardwareList->insertItem(hardwareImg,hardwareName[i]);
	}
}


void siduxcc_network::fwInstall()
{
	KMessageBox::information(this, i18n("Please don't close the window or press the Ok/Cancel button, before it's written, that the  process is done!") );

	// add non-free sources to /etc/apt/sources.list
	this->shell->setCommand("siduxcc hardware addSources");
	this->shell->start(true);

	// change widget
	widgetStack->raiseWidget(1);

	// run command
	QStrList run; run.append( "siduxcc" ); 
		run.append( "hardware" );
		run.append( "install" );
		run.append( hardwareList->currentText() );
	terminal()->startProgram( "siduxcc", run );

	 connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );
}

#include "siduxcc_network.moc"

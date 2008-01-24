/*
 * network.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * network is based on knxcc_network from Andreas Loible
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
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qregexp.h>
#include <kapp.h>
#include <qfile.h>
#include <string.h>

#include "network.h"
#include "console.h"



network::network(QWidget *parent, const char *name, const QStringList &)
: NetworkDialog(parent,name)
{
	this->shell = new Process();
	load();
	connections();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void network::load()
{
	getNetworkcards();
	getHostname();
	getNameservers();
	fwDetect();
	applyPushButton->setEnabled(FALSE);
}


void network::loadWidget(int i)
{
	widgetStack2->raiseWidget(i+1);
	widgetStack3->raiseWidget(1);
}


void network::back()
{
	if( widgetStack2->visibleWidget() == tab6)
	{
		widgetStack2->raiseWidget(1);
	}
	else
	{
		widgetStack2->raiseWidget(0);
		widgetStack3->raiseWidget(0);
	}
	
}


void network::connections()
{
	//overview
		connect( overviewListBox, SIGNAL( selected(int) ), this, SLOT( loadWidget(int) ));
	//netcardconfig overview
		connect( ncEnableButton, SIGNAL( clicked() ), this, SLOT( enableNetworkcard() ));
		connect( ncDisableButton, SIGNAL( clicked() ), this, SLOT( disableNetworkcard() ));
		connect( ncConfigButton, SIGNAL( clicked() ), this, SLOT( getNetworkcardSettings() ));
		connect( ncList, SIGNAL( clicked(QListViewItem*) ), this, SLOT( showNetworkcardInfo() ));
		connect( ncList, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( getNetworkcardSettings() ));
	//hostname
		connect( hostnameLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
	//nameserver
		connect( dnsServers, SIGNAL( changed() ), this, SLOT( hasChanged() ));
	//ndiswrapper
		connect( nwButton,  SIGNAL( clicked() ), this, SLOT( ndiswrapper() ));
		connect( nwlButton, SIGNAL( clicked() ), this, SLOT( ndiswrapperList() ));
	//fwdetect
		connect( fwInstallPushButton, SIGNAL( clicked() ), this, SLOT( fwInstall() ));
		connect( fwComboBox, SIGNAL( activated(int) ), this, SLOT( fwDetect() ));
	//netcardconfig
		connect( networkcardComboBox,     SIGNAL( activated(int) ),              networkcardWidget, SLOT( raiseWidget(int) ));
		// tab 1
			connect( methodComboBox,       SIGNAL( activated(int) ),              this,              SLOT( hasChanged() ));
			connect( methodComboBox,       SIGNAL( activated(int) ),              this,              SLOT( enableStaticFrame() ));
			connect( ipLineEdit,           SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( netmaskLineEdit,      SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( broadcastLineEdit,    SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( gatewayLineEdit,      SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( classComboBox,        SIGNAL( activated(int) ),              this,              SLOT( hasChanged() ));
			connect( classPushButton,      SIGNAL( clicked() ),                   this,              SLOT( classHelp() ));
			connect( methodPushButton,     SIGNAL( clicked() ),                   this,              SLOT( methodHelp() ));


		// tab 2
			connect( essidLineEdit,        SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( modeComboBox,         SIGNAL( activated(int) ),              this,              SLOT( hasChanged() ));
			connect( autoRadioButton,      SIGNAL( clicked() ),                   this,              SLOT( hasChanged() ));
			connect( channelRadioButton,   SIGNAL( clicked() ),                   this,              SLOT( hasChanged() ));
			connect( channelRadioButton,   SIGNAL( toggled(bool) ),               channelSpinBox,    SLOT( setEnabled(bool) ));
			connect( frequencyRadioButton, SIGNAL( clicked() ),                   this,              SLOT( hasChanged() ));
			connect( frequencyRadioButton, SIGNAL( toggled(bool) ),               frequencyLineEdit, SLOT( setEnabled(bool) ));
			connect( channelSpinBox,       SIGNAL( valueChanged(int) ),           this,              SLOT( hasChanged() ));
			connect( frequencyLineEdit,    SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( nwidLineEdit,         SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( iwconfigLineEdit,     SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( iwspyLineEdit,        SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( iwprivLineEdit,       SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
		// tab 3
			connect( securityLineEdit,     SIGNAL( textChanged(const QString&) ), this,              SLOT( hasChanged() ));
			connect( securityLineEdit,     SIGNAL( textChanged(const QString&) ), this,              SLOT( controlKeyLength() ));
			connect( securityPushButton,   SIGNAL( clicked() ),                   this,              SLOT( securityHelp() ));
			connect( securityComboBox,     SIGNAL( activated(int) ),              this,              SLOT( hasChanged() ));
			connect( securityComboBox,     SIGNAL( activated(int) ),              this,              SLOT( enableSecurityLineEdit() ));
			connect( securityComboBox,     SIGNAL( activated(int) ),              this,              SLOT( controlKeyLength() ));
 

	

	//widget3
		connect( applyPushButton, SIGNAL( clicked() ), this, SLOT( save() ));
		connect( backPushButton, SIGNAL( clicked() ), this, SLOT( back() ));

}

//------------------------------------------------------------------------------
//--- save ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void network::save()
{
	setHostname();
	setNameservers();

	QString networkcard = ncList->currentItem()->text(0);
	QString type = ncList->currentItem()->text(6);
	setNetworkcardSettings(networkcard, type);
	applyPushButton->setEnabled(FALSE);
}

void network::hasChanged()
{
	applyPushButton->setEnabled(TRUE);
}


//------------------------------------------------------------------------------
//--- netcardconfig ------------------------------------------------------------
//------------------------------------------------------------------------------



void network::getNetworkcards()
{
	ncList->clear();

	// add networkcards to list

	this->shell->setCommand("nicinfo");
	this->shell->start(true);
	QStringList devices = QStringList::split( "\n", this->shell->getBuffer() );

	for(uint i = 0; i < devices.count(); i++ )
	{

		QStringList info = QStringList::split( ",", devices[i] );

		// get device status
		this->shell->setCommand("siduxcc network getOverview "+info[0]);
		this->shell->start(true);
		QStringList overview = QStringList::split( ",", this->shell->getBuffer(), TRUE );

		QListViewItem * item = new QListViewItem( ncList, 0 );
		item->setText(0, info[0]);      // device name (E.g. eth0)
		item->setText(1, overview[1]);  // ip:      xxx.xxx.xxx.xxx
		item->setText(2, overview[2]);  // method:  dhcp/static
		item->setText(3, overview[3]);  // class:   auto/hotplug
		item->setText(4, info[2]);      // driver   e.g. e1000
		item->setText(5, info[3]);      // slot:    e.g. pci

		item->setText(6, info[1]);      // type:    wireless/ethernet
		item->setText(7, info[4]);      // mac-address (xx:xx:...)
		item->setText(8, info[5]);      // description

		// set image
		if(info[1] == "ethernet")
			if(overview[0] == "activ")
				item->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_up.png"));
			else
				item->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_down.png"));
		if(info[1] == "wireless")
			if(overview[0] == "activ")
				item->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_wlan_up.png"));
			else
				item->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_wlan_down.png"));

		if(i == 0)
		{
			macTextLabel->setText("<b>"+i18n("Mac Address")+"</b>: "+info[4]);
			descriptionTextLabel->setText("<b>"+i18n("Description")+"</b>:: "+info[5]);
		}

	}

}

void network::showNetworkcardInfo()
{
	macTextLabel->setText("<b>"+i18n("Mac Address")+"</b>: "+ncList->currentItem()->text(7));
	descriptionTextLabel->setText("<b>"+i18n("Description")+"</b>:: "+ncList->currentItem()->text(8));
}


// start button commands

void network::enableNetworkcard()
{
	QString networkcard = ncList->currentItem()->text(0);
	this->shell->setCommand("ifup "+networkcard+"&");
	this->shell->start(true);
	updateNetworkcardStatus();
}

void network::disableNetworkcard()
{
	QString networkcard = ncList->currentItem()->text(0);
	this->shell->setCommand("ifdown "+networkcard+"&");
	this->shell->start(true);
	updateNetworkcardStatus();
}


void network::updateNetworkcardStatus()
{
	QString networkcard = ncList->currentItem()->text(0);
	QString type = ncList->currentItem()->text(6);
	this->shell->setCommand("siduxcc network getOverview "+networkcard);
	this->shell->start(true);
	QStringList overview = QStringList::split( ",", this->shell->getBuffer(), TRUE );

	ncList->currentItem()->setText(1, overview[1]);     // ip:      xxx.xxx.xxx.xxx
	ncList->currentItem()->setText(2, overview[2]);     // method:  dhcp/static
	ncList->currentItem()->setText(3, overview[3]);     // class:    auto/hotplug

	if(type == "ethernet")
		if(overview[0] == "activ")
			ncList->currentItem()->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_up.png"));
		else
			ncList->currentItem()->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_down.png"));
	if(type == "wireless")
		if(overview[0] == "activ")
			ncList->currentItem()->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_wlan_up.png"));
		else
			ncList->currentItem()->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_wlan_down.png"));
}


//------------------------------------------------------------------------------
//--- netcardconfig ------------------------------------------------------------
//------------------------------------------------------------------------------


void network::getNetworkcardSettings()
{

	
	widgetStack2->raiseWidget(6);
	networkcardWidget->raiseWidget(0);

	QString networkcard = ncList->currentItem()->text(0); // eth0
	QString type        = ncList->currentItem()->text(6); // ethernet | wireless
	networkcardTextLabel->setText("Card: "+networkcard);

	// get Networkcard settings
	this->shell->setCommand("Ceni_read_config --iface "+networkcard);
	this->shell->start(true);
	QStringList cardsettings = QStringList::split( "\n", this->shell->getBuffer(), TRUE );


	// mode: "" | allow-hotplug | auto
	if( cardsettings.grep("class,allow-hotplug").count() == 1 )
		classComboBox->setCurrentItem(1);
	else if (  cardsettings.grep("class,auto").count() == 1 )
		classComboBox->setCurrentItem(2);

	//method: "" | static | dhcp
	if( cardsettings.grep("method,dhcp").count() == 1 )
		methodComboBox->setCurrentItem(0);
	else if (  cardsettings.grep("method,static").count() == 1 )
		methodComboBox->setCurrentItem(1);
	enableStaticFrame();

	ipLineEdit->setText(        QStringList::split(",", cardsettings.grep( "address,"   )[0] )[1] );  // xxx.xxx.xxx.xxx
	netmaskLineEdit->setText(   QStringList::split(",", cardsettings.grep( "netmask,"   )[0] )[1] );  // xxx.xxx.xxx.xxx
	broadcastLineEdit->setText( QStringList::split(",", cardsettings.grep( "broadcast," )[0] )[1] );  // xxx.xxx.xxx.xxx
	networkLineEdit->setText(   QStringList::split(",", cardsettings.grep( "network,"   )[0] )[1] );  // xxx.xxx.xxx.xxx
	gatewayLineEdit->setText(   QStringList::split(",", cardsettings.grep( "gateway,"   )[0] )[1] );  // xxx.xxx.xxx.xxx


		

	if(type == "wireless")
	{
		networkcardComboBox->show();
		networkcardComboBox->setCurrentItem(0);

		QString essid = QStringList::split(",", cardsettings.grep( "ssid," )[0] )[1];


		// essid
		essidLineEdit->setText( essid );  // E.g. myWlanNetwork


		//wlan mode:
		if( cardsettings.grep("wireless-mode,managed").count() == 1 )
			methodComboBox->setCurrentItem(1);
		else if (  cardsettings.grep("wireless-mode,ad-hoc").count() == 1 )
			methodComboBox->setCurrentItem(2);
		else if (  cardsettings.grep("wireless-mode,master").count() == 1 )
			methodComboBox->setCurrentItem(3);
		else if (  cardsettings.grep("wireless-mode,repeater").count() == 1 )
			methodComboBox->setCurrentItem(4);
		else if (  cardsettings.grep("wireless-mode,second").count() == 1 )
			methodComboBox->setCurrentItem(5);
		else if (  cardsettings.grep("wireless-mode,auto").count() == 1 )
			methodComboBox->setCurrentItem(6);


		// channel/ frequency
		QString channel   = QStringList::split(",", cardsettings.grep( "wireless-channel," )[0] )[1];
		QString frequency = QStringList::split(",", cardsettings.grep( "wireless-freq,"    )[0] )[1];
		if( channel )
		{
			channelRadioButton->setChecked(TRUE);
			channelSpinBox->setValue(channel.toInt());
		}
		else if( frequency )
		{
			frequencyRadioButton->setChecked(TRUE);
			frequencyLineEdit->setText( frequency );
		}
		else
			autoRadioButton->setChecked(TRUE);

		// advanced options
		nwidLineEdit->setText(     QStringList::split(",", cardsettings.grep( "wireless-nwid," )[0] )[1] );
		iwconfigLineEdit->setText( QStringList::split(",", cardsettings.grep( "iwconfig,"      )[0] )[1] );
		iwspyLineEdit->setText(    QStringList::split(",", cardsettings.grep( "iwspy,"         )[0] )[1] );
		iwprivLineEdit->setText(   QStringList::split(",", cardsettings.grep( "iwpriv,"        )[0] )[1] );
		
		// security
		controlKeyLength();
		QString key = QStringList::split(",", cardsettings.grep( "wireless-key,")[0] )[1];
		QString wpa = QStringList::split(",", cardsettings.grep( "wpa-psk,"     )[0] )[1];
		if( wpa )
		{
			securityComboBox->setCurrentItem(1);
			securityLineEdit->setText(wpa);
			securityLineEdit->setEnabled(TRUE);
		}
		else if( wpa )
		{
			securityComboBox->setCurrentItem(2);
			securityLineEdit->setText(wpa);
			securityLineEdit->setEnabled(TRUE);
		}
		else
		{
			securityComboBox->setCurrentItem(0);
			securityLineEdit->setEnabled(FALSE);
		}
	}
	else
		networkcardComboBox->hide();

	applyPushButton->setEnabled(FALSE);

}


void network::setNetworkcardSettings(QString networkcard, QString type)
{

	// example
	// allow-hotplug eth1
	// iface eth1 inet static
   // 	address 10.0.0.1
   // 	broadcast 10.0.0.255
   // 	netmask 255.255.255.0
   // 	network 10.0.0.0 



	QString settings;

	// set class
	if(classComboBox->currentItem() == 1)
		settings+=QString( "allow-hotplug "+networkcard+"\n" );
	else if(classComboBox->currentItem() == 2)
		settings+=QString( "auto "+networkcard+"\n" );

	if(methodComboBox->currentItem() == 0)
		settings += "iface "+networkcard+" inet dhcp\n";
	else
	{
		settings+= "iface "+networkcard+" inet static\n";
		settings+=QString("\taddress "  +toValidIP( ipLineEdit->text())       +"\n");
		settings+=QString("\tbroadcast "+toValidIP( broadcastLineEdit->text())+"\n");
		settings+=QString("\tnetmask "  +toValidIP( netmaskLineEdit->text())  +"\n");
		settings+=QString("\tnetwork "  +toValidIP( networkLineEdit->text() ) +"\n");
		settings+=QString("\tgateway "  +toValidIP( gatewayLineEdit->text() ) +"\n");
	}



	if( type == "wireless" )
	{
		if( essidLineEdit->text() != "" )
			settings += "\twireless-essid "+essidLineEdit->text()+"\n";
		if( modeComboBox->currentText()  != "" )
			settings += "\twireless-mode "+modeComboBox->currentText()+"\n";

		if( nwidLineEdit->text()  != "" )
			settings += "\twireless-nwid "+nwidLineEdit->text()+"\n";
		if( iwconfigLineEdit->text()  != "" )
			settings += "\tiwconfig "+iwconfigLineEdit->text()+"\n";
		if( iwspyLineEdit->text()  != "" )
			settings += "\tiwspy "+iwspyLineEdit->text()+"\n";
		if( iwprivLineEdit->text()  != "" )
			settings += "\tiwpriv "+iwprivLineEdit->text()+"\n";


		if ( channelRadioButton->isChecked() )
			settings += "\twireless-channel "+channelSpinBox->text()+"\n";
		else if ( frequencyRadioButton->isChecked() )
			settings += "\twireless-freq "+frequencyLineEdit->text()+"\n";


		if ( securityComboBox->currentItem() == 1 and securityLineEdit->text().length() > 4)
			settings += "\twireless-key "+securityLineEdit->text()+"\n";
		else if ( securityComboBox->currentItem() == 2 and securityLineEdit->text().length() > 7 and securityLineEdit->text().length() < 64)
		{
			settings += "\twpa-ssid "+essidLineEdit->text()+"\n";
			settings += "\twpa-psk "+securityLineEdit->text()+"\n";
		}

	}


	char random[16];
	int x = rand();
	snprintf(random, 16, "%d", x);
	QString filename = "/tmp/siduxcc-"+QString(random)+".tmp";


	QFile f1(  filename );
	f1.remove();
	f1.open( IO_Raw | IO_ReadWrite | IO_Append );
	f1.writeBlock( settings, qstrlen(settings) );

	this->shell->setCommand("Ceni_write_config --iface "+networkcard+" --input "+filename);
	this->shell->start(true);

	f1.remove();



}


bool network::isValidIP(QString ip)
{
	for(int i = 0; i < 4; i++)
	{
		QString section = ip.section(".", i, i);
		if( section.isEmpty() || section.toInt() >= 256 || section.toInt() < 0 ) return false;
	}
	return true;
}

QString network::toValidIP(QString ip)
{
	if(!isValidIP(ip)) return "";
	return ip;
}


void network::classHelp()
{
	KMessageBox::information(this, i18n("<b>allow hotplug</b>: The network will be activated  when a cable is plugged in and it will be deactivated if the cable is pulled. This is useful on laptops with onboard network adapters, since it will only configure the interface when a cable is really connected.<br><br><b>auto</b>: The networkcard will be activated, when the computer starts"));
}

void network::methodHelp()
{
	KMessageBox::information(this, i18n("..."));
}


void network::enableStaticFrame()
{
	if( methodComboBox->currentItem() == 1 )
		staticFrame->setEnabled(TRUE);
	else
		staticFrame->setEnabled(FALSE);
}

//------------------------------------------------------------------------------
//--- security -----------------------------------------------------------------
//------------------------------------------------------------------------------


void network::controlKeyLength()
{
	if( securityComboBox->currentItem() == 1 )
		if( securityLineEdit->text().length() < 5  )
			securityPushButton->setPixmap( QPixmap("/usr/share/siduxcc/icons/notok2.png") );
		else
			securityPushButton->setPixmap( QPixmap("/usr/share/siduxcc/icons/ok.png") );
	else if( securityComboBox->currentItem() == 2 )
		if( securityLineEdit->text().length() < 8 or securityLineEdit->text().length() > 63 )
			securityPushButton->setPixmap( QPixmap("/usr/share/siduxcc/icons/notok2.png") );
		else
			securityPushButton->setPixmap( QPixmap("/usr/share/siduxcc/icons/ok.png") );
}

void network::securityHelp()
{
	if( securityComboBox->currentItem() == 1 )
		KMessageBox::information(this, i18n("WEP encryption requires a key with at least 5 characters."));
	else if( securityComboBox->currentItem() == 2 )
		KMessageBox::information(this, i18n("WPA encryption requires a key with 8 to 63 characters."));
	else
		KMessageBox::information(this, i18n("..."));
}

void network::enableSecurityLineEdit()
{
	if( securityComboBox->currentItem() > 0 )
		securityLineEdit->setEnabled(TRUE);
	else
		securityLineEdit->setEnabled(FALSE);
}


//------------------------------------------------------------------------------
//--- hostname -----------------------------------------------------------------
//------------------------------------------------------------------------------


void network::getHostname()
{
	this->shell->setCommand("siduxcc network getHostname");
	this->shell->start(true);
	hostnameLineEdit->setText( this->shell->getBuffer() );
}


void network::setHostname()
{
	this->shell->setCommand("siduxcc network setHostname "+hostnameLineEdit->text());
	this->shell->start(true);
}


//------------------------------------------------------------------------------
//--- nameservers --------------------------------------------------------------
//------------------------------------------------------------------------------


void network::setNameservers()
{
	QString command = "siduxcc network setNameservers ";
	QStringList nameserverList = dnsServers->items();
	for ( QStringList::Iterator it = nameserverList.begin(); it != nameserverList.end(); ++it )
		command+=(*it) + " ";
	this->shell->setCommand(command);
	this->shell->start(true);
}


void network::getNameservers()
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


void network::ndiswrapper()
{
	this->shell->setCommand("su-to-root -X -c /usr/sbin/configure-ndiswrapper&");
	this->shell->start(true);
}

void network::ndiswrapperList()
{
		kapp->invokeBrowser( "http://ndiswrapper.sourceforge.net/joomla/index.php?/component/option,com_openwiki/Itemid,33/id,list/" );
}

//------------------------------------------------------------------------------
//--- fwdetect -----------------------------------------------------------------
//------------------------------------------------------------------------------


void network::fwDetect()
{
	// show firmware
	QPixmap hardwareImg("/usr/share/siduxcc/icons/hardware.png");
	hardwareList->clear();
	if(fwComboBox->currentText() == i18n("detected"))
		this->shell->setCommand("fw-detect -s");
	else
		this->shell->setCommand("fw-detect -a");
	this->shell->start(true);
	QStringList hardwareName = QStringList::split( "\n", this->shell->getBuffer() );
	for ( QStringList::Iterator it = hardwareName.begin(); it != hardwareName.end(); ++it )
		hardwareList->insertItem(hardwareImg, *it);
}


void network::fwInstall()
{
	emit menuLocked(TRUE);

	// add non-free sources to /etc/apt/sources.list
	this->shell->setCommand("siduxcc software addSources");
	this->shell->start(true);

	QStrList run; run.append( "siduxcc" );
		run.append( "network" );
		run.append( "installWlanCard" );
		run.append( hardwareList->currentText() );

	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 7);
	widgetStack2->raiseWidget(7);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
}

void network::terminateConsole()
{
	widgetStack2->raiseWidget(5);
	widgetStack3->raiseWidget(1);
	emit menuLocked(FALSE);
	fwDetect();
}

#include "network.moc"

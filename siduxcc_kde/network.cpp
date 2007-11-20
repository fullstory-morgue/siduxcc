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
		widgetStack2->raiseWidget(1);
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
		connect( networkcardComboBox, SIGNAL( activated(int) ), networkcardWidget, SLOT( raiseWidget(int) ));
		// tab 1
			connect( dhcpRadioButton, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( dhcpRadioButton, SIGNAL( toggled(bool) ), staticFrame, SLOT( setDisabled(bool) ));
			connect( staticRadioButton, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( ipLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( netmaskLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( broadcastLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( gatewayLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( bootCheckBox, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( hotplugCheckBox, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
		// tab 2
			connect( essidLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( modeComboBox, SIGNAL( activated(int) ), this, SLOT( hasChanged() ));
			connect( autoRadioButton, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( channelRadioButton, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( channelRadioButton, SIGNAL( toggled(bool) ), channelSpinBox, SLOT( setEnabled(bool) ));
			connect( frequencyRadioButton, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( frequencyRadioButton, SIGNAL( toggled(bool) ), frequencyLineEdit, SLOT( setEnabled(bool) ));
			connect( channelSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( hasChanged() ));
			connect( frequencyLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( nwidLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( iwconfigLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( iwspyLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( iwprivLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
		// tab 3
			connect( wpaCheckBox, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( keyLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( wpaLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( hasChanged() ));
			connect( wpaCheckBox, SIGNAL( clicked() ), this, SLOT( hasChanged() ));
			connect( wpaCheckBox, SIGNAL( toggled(bool) ), keyLineEdit, SLOT( setDisabled(bool) ));
			connect( wpaCheckBox, SIGNAL( toggled(bool) ), wpaLineEdit, SLOT( setEnabled(bool) ));
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

		QStringList info = QStringList::split( " ", devices[i] );
		for(uint j = 6; j < info.count(); j++)
				info[5] = info[5]+" "+info[j]; 

		// get device status
		this->shell->setCommand("siduxcc network getStatus "+info[0]);
		this->shell->start(true);
		QStringList status = QStringList::split( "\n+", this->shell->getBuffer(), TRUE );
		status[0] = status[0].mid(1); // remove +

		QListViewItem * item = new QListViewItem( ncList, 0 );
		item->setText(0, info[0]);   // device name (E.g. eth0)
		item->setText(1, status[0]);     // ip:      xxx.xxx.xxx.xxx
		item->setText(2, status[1]);     // method:  dhcp/static)
		item->setText(3, status[2]);     // boot:    enable/disable)
		item->setText(4, info[2]);          // driver   e.g. e1000
		item->setText(5, info[3]);          // slot:    e.g. pci

		item->setText(6, info[1]);          // type:    wireless/ethernet
		item->setText(7, info[4]);          // mac-address (xx:xx:...)
		item->setText(8, info[5]);          // description

		// set image
		if(info[1] == "ethernet")
			if(status[3].left(7) == "running")
				item->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_up.png"));
			else
				item->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_down.png"));
		if(info[1] == "wireless")
			if(status[3].left(7) == "running")
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
	this->shell->setCommand("siduxcc network getStatus "+networkcard);
	this->shell->start(true);
	QStringList status = QStringList::split( "\n+", this->shell->getBuffer(), TRUE );
	status[0] = status[0].mid(1); // remove +

	ncList->currentItem()->setText(1, status[0]);     // ip:      xxx.xxx.xxx.xxx
	ncList->currentItem()->setText(2, status[1]);     // method:  dhcp/static)
	ncList->currentItem()->setText(3, status[2]);     // boot:    enable/disable)

	if(type == "ethernet")
		if(status[3].left(7) == "running")
			ncList->currentItem()->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_up.png"));
		else
			ncList->currentItem()->setPixmap(0, QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_lan_down.png"));
	if(type == "wireless")
		if(status[3].left(7) == "running")
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

	QString networkcard = ncList->currentItem()->text(0);
	QString description =  QStringList::split( " ", ncList->currentItem()->text(8) )[0]+")";
	networkcardTextLabel->setText(networkcard+" "+description);

	// get Networkcard settings
	this->shell->setCommand("siduxcc network getNetworkcardConfig "+networkcard);
	this->shell->start(true);
	QStringList settings = QStringList::split( "\n+", this->shell->getBuffer(), TRUE );
	settings[0] = settings[0].mid(1); // remove +
	QStringList::Iterator it = settings.begin();

	networkcardComboBox->setHidden(TRUE);
	networkcardWidget->raiseWidget(0);

	// type: lan | wlan
	QString type = *it++;

	// mode: "" | auto | allow-hotplug | auto allow-hotplug
	bootCheckBox->setChecked(FALSE);
	hotplugCheckBox->setChecked(FALSE);
	if( (*it).contains("auto") )
		bootCheckBox->setChecked(TRUE);
	if( (*it).contains("allow-hotplug"))
		hotplugCheckBox->setChecked(TRUE);
	*it++;


	//method: static | dhcp
	if( (*it).left(6) == "static" )
		staticRadioButton->setChecked(TRUE);
	else if( (*it).left(4) == "dhcp" )
		dhcpRadioButton->setChecked(TRUE);
	*it++;

	ipLineEdit->setText(*it++);         // xxx.xxx.xxx.xxx
	netmaskLineEdit->setText(*it++);    // xxx.xxx.xxx.xxx
	broadcastLineEdit->setText(*it++);  // xxx.xxx.xxx.xxx
	gatewayLineEdit->setText(*it++);    // xxx.xxx.xxx.xxx


	// wlan
	if(type == "wlan")
	{
		networkcardComboBox->setHidden(FALSE);
	
		// essid
		essidLineEdit->setText(*it);

		// securitexte
		securityTextLabel1->setText(i18n("Encryption key for ")+networkcard);
		securityTextLabel2->setText(i18n("WPA Passphrase for ")+*it++);

		//mode
		if( *it == "Managed" )
			modeComboBox->setCurrentItem(1);
		else if( *it == "Ad-Hoc" )
			modeComboBox->setCurrentItem(2);
		else if( *it == "Master" )
			modeComboBox->setCurrentItem(3);
		else if( *it == "Repeater" )
			modeComboBox->setCurrentItem(4);
		else if( *it == "Secondary" )
			modeComboBox->setCurrentItem(5);
		else if( *it == "Auto" )
			modeComboBox->setCurrentItem(6);
		else
			modeComboBox->setCurrentItem(0);
		*it++;

		// channel/ frequency
		QString channel = *it++;
		QString frequency = *it++;
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
		nwidLineEdit->setText(*it++);
		iwconfigLineEdit->setText(*it++);
		iwspyLineEdit->setText(*it++);
		iwprivLineEdit->setText(*it++);
		
		// security
		QString key = *it++;
		QString wpa = *it++;
		if( wpa )
		{
			wpaLineEdit->setEnabled(TRUE);
			wpaCheckBox->setChecked(TRUE);
			wpaLineEdit->setText(wpa);
		}
		else
		{
			keyLineEdit->setEnabled(TRUE);
			wpaCheckBox->setChecked(FALSE);
			wpaLineEdit->setText(key);
		}
	}

	applyPushButton->setEnabled(FALSE);

}

void network::setNetworkcardSettings(QString networkcard, QString type)
{

	// set lan settings
	QString cmdargs = "siduxcc network setNetworkcardConfig "+networkcard;
	if(dhcpRadioButton->isChecked())
	{
		cmdargs += " +dhcp";
		cmdargs += " +";
		cmdargs += " +";
		cmdargs += " +";
	}
	else
	{
		cmdargs+=QString(" +"+toValidIP( ipLineEdit->text() ));
		cmdargs+=QString(" +"+toValidIP( netmaskLineEdit->text() ));
		cmdargs+=QString(" +"+toValidIP( broadcastLineEdit->text() ));
		cmdargs+=QString(" +"+toValidIP( gatewayLineEdit->text() ));
	}
	// set/unset autoboot
	if(bootCheckBox->isChecked())
		cmdargs+=QString(" +autoboot" );
	else
		cmdargs+=QString(" +" );
	// set hotplug
	if(hotplugCheckBox->isChecked())
		cmdargs+=QString(" +allow-hotplug" );
	else
		cmdargs+=QString(" +" );


	if( type == "wireless" )
	{
		cmdargs += " +"+essidLineEdit->text();
		cmdargs += " +"+modeComboBox->currentText();
		if ( channelRadioButton->isChecked() )
			cmdargs += " +"+channelSpinBox->text();
		else
			cmdargs += " +";
		if ( frequencyRadioButton->isChecked() )
			cmdargs += " +"+frequencyLineEdit->text();
		else
			cmdargs += " +";
		cmdargs += " +"+nwidLineEdit->text();
		cmdargs += " +"+iwconfigLineEdit->text();
		cmdargs += " +"+iwspyLineEdit->text();
		cmdargs += " +"+iwprivLineEdit->text();
		if ( wpaCheckBox->isChecked() )
		{
			cmdargs += " +";
			cmdargs += " +"+wpaLineEdit->text();
		}
		else
		{
			cmdargs += " +"+keyLineEdit->text();
			cmdargs += " +";
		}
			
		


	}
	this->shell->setCommand(cmdargs);
	this->shell->start();

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
	this->shell->setCommand("su-me configure-ndiswrapper&");
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
		this->shell->setCommand("siduxcc hardware detect");
	else
		this->shell->setCommand("siduxcc hardware allDevices");
	this->shell->start(true);
	QStringList hardwareName = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for ( QStringList::Iterator it = hardwareName.begin(); it != hardwareName.end(); ++it )
		hardwareList->insertItem(hardwareImg, *it);
}


void network::fwInstall()
{
	emit menuLocked(TRUE);

	// add non-free sources to /etc/apt/sources.list
	this->shell->setCommand("siduxcc hardware addSources");
	this->shell->start(true);

	QStrList run; run.append( "siduxcc" );
		run.append( "hardware" );
		run.append( "install" );
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
}

#include "network.moc"

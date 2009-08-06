/*
 * lan.cpp
 *
 * Copyright (c) 2009 Fabian Wuertz <xadras@sidux.com>
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


#include "lan.h"
#include <QProcess>
#include <kdesu/su.h>
#include <QFile>
#include <QTextStream>


lan::lan (QWidget* parent, QString password, QString device): KDialog (parent)
{
	QWidget *widget = new QWidget( this );
	setupUi( widget );
	setMainWidget( widget );

	connect( ipLineEdit, SIGNAL( lostFocus() ), SLOT( autoFill() ) );
	connect( methodComboBox, SIGNAL( currentIndexChanged( const QString & ) ), SLOT( methodToggled() ) );
	connect( this, SIGNAL( okClicked() ), SLOT( saveSettings() ) );

	settingManager = new SettingManager(password, device);
	loadSettings();
}


//------------------------------------------------------------------------------
//-- load/save settings --------------------------------------------------------
//------------------------------------------------------------------------------

void lan::loadSettings()
{
	settingManager->loadSettings();

    QString classValue  =  settingManager->getValue("class");
    QString methodValue =  settingManager->getValue("method");
	// mode: "" | allow-hotplug | auto
    if( classValue == "allow-hotplug" )
		classComboBox->setCurrentIndex (1);
    else if ( classValue == "auto" )
		classComboBox->setCurrentIndex (2);
	//method: "" | static | dhcp
    if (  methodValue == "static" )
		methodComboBox->setCurrentIndex(1);
	else
		staticGroupBox->setEnabled(FALSE);
        ipLineEdit       ->setText( settingManager->getValue("address") );  // xxx.xxx.xxx.xxx
        netmaskLineEdit  ->setText( settingManager->getValue("netmask") );  // xxx.xxx.xxx.xxx
        broadcastLineEdit->setText( settingManager->getValue("broadcast") );  // xxx.xxx.xxx.xxx
        networkLineEdit  ->setText( settingManager->getValue("network") );  // xxx.xxx.xxx.xxx
        gatewayLineEdit  ->setText( settingManager->getValue("gateway") );  // xxx.xxx.xxx.xxx
}


void lan::saveSettings() 
{
	// set setting
	if(classComboBox->currentIndex() == 1)
		settingManager->setValue("class", "allow-hotplug");
	else if(classComboBox->currentIndex() == 2)
		settingManager->setValue("class", "auto");
	else
		settingManager->setValue("class", "");
	
	if(methodComboBox->currentIndex() == 0) {
		settingManager->setValue("method",    "dhcp");
		settingManager->setValue("address",   "");
		settingManager->setValue("broadcast", "");
		settingManager->setValue("netmask",   "");
		settingManager->setValue("network",   "");
		settingManager->setValue("gateway",   "");
	}
	else {
		settingManager->setValue("method",    "static");
		settingManager->setValue("address",   ipLineEdit->text());
		settingManager->setValue("netmask",   netmaskLineEdit->text());
		settingManager->setValue("network",   networkLineEdit->text());
		settingManager->setValue("gateway",   gatewayLineEdit->text());
		settingManager->setValue("broadcast", broadcastLineEdit->text());
	}

	settingManager->saveSettings();
}


//------------------------------------------------------------------------------
//-- enable/disable static fields ----------------------------------------------
//------------------------------------------------------------------------------

void lan::methodToggled()
{
	if( methodComboBox->currentText() == "dhcp" ) 
		staticGroupBox->setEnabled(FALSE);
	else
		staticGroupBox->setEnabled(TRUE);
}


//------------------------------------------------------------------------------
//-- auto fill static values ---------------------------------------------------
//------------------------------------------------------------------------------

void lan::autoFill() 
{
	QStringList splittedIp = ipLineEdit->text().split(".");

	QString networkAddr = networkLineEdit->text().split(".")[3];
	if( networkAddr.isEmpty() )
		networkAddr = "1";


	networkLineEdit->setText(splittedIp[0]+"."+splittedIp[1]+"."+splittedIp[2]+"."+networkAddr);
	broadcastLineEdit->setText(splittedIp[0]+"."+splittedIp[1]+"."+splittedIp[2]+".255");

	if( netmaskLineEdit->text() == "..." )
		netmaskLineEdit->setText("255.255.255.0");
}


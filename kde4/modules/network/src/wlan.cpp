/*
 * wlan.cpp
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



#include <QProcess>
#include <QFile>
#include <QTextStream>

#include <kmessagebox.h>
#include <kdesu/su.h>

#include "wlan.h"

wlan::wlan (QWidget* parent, QString password, QString device): KDialog (parent)
{
	Password = password;
	Device   = device;
	settingManager = new SettingManager(password, device);

	QWidget *widget = new QWidget( this );
	setupUi( widget );
	setMainWidget( widget );

	connect( this, SIGNAL( okClicked() ), SLOT( saveSettings() ) );
	connect( securityComboBox, SIGNAL( currentIndexChanged(int) ), SLOT( securityToggled(int) ) );
	connect( securityLineEdit, SIGNAL( lostFocus() ), SLOT( checkKey() ) );

	loadSettings();
	showNetworks();
}


//------------------------------------------------------------------------------
//-- load/save settings --------------------------------------------------------
//------------------------------------------------------------------------------

void wlan::loadSettings()
{

	settingManager->loadSettings();
    QString key = settingManager->getValue("wireless-key");
	if( key.isEmpty() ) {
		key = settingManager->getValue("wpa-psk");
		if( !key.isEmpty() )
			securityComboBox->setCurrentIndex(2);
	}
	else
		securityComboBox->setCurrentIndex(1);
	securityLineEdit->setText(key);

}


void wlan::saveSettings()
{
	if ( wlanTreeWidget->selectedItems().count() < 1 )
		return;

	QString wirelessEssid = wlanTreeWidget->selectedItems().first()->text(0);
	settingManager->setValue("wireless-essid", wirelessEssid);


	if ( securityComboBox->currentIndex() == 1 and securityLineEdit->text().length() > 4) {
		settingManager->setValue("wireless-key", securityLineEdit->text());
		settingManager->setValue("wpa-ssid",     "");
		settingManager->setValue("wpa-psk",      "");
	}
	else if ( securityComboBox->currentIndex() == 2 and securityLineEdit->text().length() > 7 and securityLineEdit->text().length() < 64) {
		settingManager->setValue("wireless-key", "");
		settingManager->setValue("wpa-ssid",     wirelessEssid);
		settingManager->setValue("wpa-psk",      securityLineEdit->text());
	}
	else if ( securityComboBox->currentIndex() == 0 ) {
		settingManager->setValue("wireless-key", "");
		settingManager->setValue("wpa-ssid",     "");
		settingManager->setValue("wpa-psk",      "");
	}

	settingManager->saveSettings();
}


//------------------------------------------------------------------------------
//-- scan for wlan networks ----------------------------------------------------
//------------------------------------------------------------------------------

void wlan::showNetworks() {

	KDESu::SuProcess activateCardProcess( QByteArray( "root" ), QByteArray("/bin/ip ").append("link ").append("set ").append( Device.toLocal8Bit()+" " ).append("up" ) );
	activateCardProcess.exec( Password.toLocal8Bit() );

	KDESu::SuProcess readNetworksProcess( QByteArray( "root" ), QByteArray("iwlist ").append(Device.toLocal8Bit()+" ").append("scanning ") );
	readNetworksProcess.exec( Password.toLocal8Bit() );
	QStringList networks =  readProcess("/sbin/iwlist", (QStringList() << Device << "scanning") ).split( "\n");

    QString currentEssid = settingManager->getValue("wireless-essid");


	QStringList essid      = networks.filter("ESSID");
	QStringList channel    = networks.filter("Channel:").replaceInStrings("\n","");
	QStringList quality    = networks.filter("Quality");
	QStringList encryption = networks.filter("Encryption").replaceInStrings("\n","");
	QStringList address    = networks.filter("Address").replaceInStrings("\n","");

	for(int i = 0; i < essid.count(); i++ )
	{
		QTreeWidgetItem * item = new QTreeWidgetItem( wlanTreeWidget, 0   );
		item->setText(0, essid[i]     .split("\"")[1] );
		item->setText(1, channel[i]   .split(":")[1] );
		item->setText(2, quality[i]   .split("=")[1].split(" ")[0] );
		item->setText(3, encryption[i].split(":")[1] );
		item->setText(4, address[i]   .split(": ")[1] );
		if( essid[i].split("\"")[1] == currentEssid) 
			item->setSelected(TRUE);
	}
}


//------------------------------------------------------------------------------
//-- enable/disbale securityLineEdit -------------------------------------------
//------------------------------------------------------------------------------

void wlan::securityToggled(int i)
{
	if( i == 0 ) 
		securityLineEdit->setEnabled(FALSE);
	else
		securityLineEdit->setEnabled(TRUE);
}


//------------------------------------------------------------------------------
//-- check security key lenght -------------------------------------------------
//------------------------------------------------------------------------------

void wlan::checkKey()
{
	if ( securityComboBox->currentIndex() == 1 and securityLineEdit->text().length() < 5) {
		KMessageBox::information(0, "A WEP Key must be longer then 4 characters!");
		return;
	}

	if ( securityComboBox->currentIndex() == 2 and securityLineEdit->text().length() < 8) {
		KMessageBox::information(0, "A WPA-PSK Key must be longer then 7 characters!");
		return;
	}
	// Note: The lineEdit has the max length 63.
}


//------------------------------------------------------------------------------
//-- exec ----------------------------------------------------------------------
//------------------------------------------------------------------------------

QString wlan::readProcess(QString run, QStringList arguments)
{
	QProcess exec;
	exec.start(run, arguments);
	exec.waitForFinished();
	return QString( exec.readAll() );
}

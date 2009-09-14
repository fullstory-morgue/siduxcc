/*
 * legend.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz <xadras@sidux.com>
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
#include <QHeaderView>
#include <QFile>
#include <QWhatsThis>

#include <kgenericfactory.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdesu/su.h>
#include <kaboutdata.h>

#include "netcardconfig.h"
#include "lan.h"
#include "wlan.h"



K_PLUGIN_FACTORY(NwaFactory, registerPlugin<Netcardconfig>();)
K_EXPORT_PLUGIN(NwaFactory("kcm_networkaround"));

Netcardconfig::Netcardconfig( QWidget *parent, const QVariantList &)
        : KCModule(NwaFactory::componentData(), parent)
{
  
  
	setButtons(NoAdditionalButton);

	KAboutData *about = new KAboutData( "kcm_networkaround ", 0, ki18n( "Network Settings" ), "1", ki18n( "Configure Network Settings" ), KAboutData::License_GPL, ki18n( "Copyright (C) 2009 Fabian Wuertz" ), KLocalizedString(), "http://linux.wuertz.org/", "xadras@sidux.com" );
	about->addAuthor( ki18n("Fabian Wuertz"), ki18n( "Developer" ), "xadras@sidux.com" );
	setAboutData( about );  
  
  
	// load gui
	setupUi(this);
	configPushButton->setIcon( KIcon( "preferences-system" ) );
	wlanPushButton->setIcon( KIcon( "network-wireless" ) );
	statusPushButton->setIcon( KIcon( "media-playback-start" ) );
	infoPushButton->setIcon( KIcon( "help-about" ) );
	networkCardsTreeWidget->header()->setResizeMode(1,QHeaderView::ResizeToContents);

	// show a list of all network cards
	getNetworkcards();
}

//------------------------------------------------------------------------------
//-- button actions ------------------------------------------------------------
//------------------------------------------------------------------------------

// Button: Configure Interface
void Netcardconfig::lanConfig() {

	if ( networkCardsTreeWidget->selectedItems().count() < 1 )
		return;

	QString device = networkCardsTreeWidget->selectedItems().first()->text(0);

	// checkRootPrivileges
	if(getuid() != 0)
		if ( Password.isEmpty() || KDESu::SuProcess( "root" ).checkInstall( Password.toLocal8Bit() ) != 0 ) {
			  Forward = "lanConfig";
			  dlg = new KPasswordDialog( this);
			  dlg->setPrompt(i18nc("@info", "<para>Administrator privileges are required.</para><para>In order to procede, please enter the system administrator's password.</para>"));
			  connect( dlg, SIGNAL( gotPassword( const QString& , bool ) )  , this, SLOT( setPassword( const QString &) ) );
			  connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
			  dlg->show();
			  return;
		}



	lan* lanWindow = new lan(this, Password, device);
	if( lanWindow->exec() )
		getNetworkcards();
}


// Button: Choose WLAN Network
void Netcardconfig::wlanConfig() {

	if ( networkCardsTreeWidget->selectedItems().count() < 1 )
		return;

	QString device = networkCardsTreeWidget->selectedItems().first()->text(0);

	// checkRootPrivileges
	if(getuid() != 0)
		if ( Password.isEmpty() || KDESu::SuProcess( "root" ).checkInstall( Password.toLocal8Bit() ) != 0 ) {
			Forward = "wlanConfig";
			dlg = new KPasswordDialog( this);
			dlg->setPrompt(i18nc("@info", "<para>Administrator privileges are required.</para><para>In order to procede, please enter the system administrator's password.</para>"));
			connect( dlg, SIGNAL( gotPassword(const QString& , bool ) )  , this, SLOT( setPassword(const QString &) ) );
			connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
			dlg->show();
			return;
		}

	wlan* wlanWindow = new wlan(this, Password, device);
	if ( wlanWindow->exec() )
		getNetworkcards();
}


// Button: Interface Details
void Netcardconfig::cardInfo() {
	if ( networkCardsTreeWidget->selectedItems().count() < 1 )
		return;

	QString device     = networkCardsTreeWidget->selectedItems().first()->text(0);
	QString driver     = networkCardsTreeWidget->selectedItems().first()->text(3);
	QString slot       = networkCardsTreeWidget->selectedItems().first()->text(4);
	QString macAddress = networkCardsTreeWidget->selectedItems().first()->text(5);

	KMessageBox::information(0, "Device: "+device+"\n"+"Driver: "+driver+"\n"+"Slot: "+slot+"\n"+"MAC: "+macAddress);
}


// Button: Enable/Disable Interface
void Netcardconfig::toggleCardStatus() {
	if ( networkCardsTreeWidget->selectedItems().count() < 1 )
		return;

	QString device     = networkCardsTreeWidget->selectedItems().first()->text(0);
	QString status     = networkCardsTreeWidget->selectedItems().first()->text(6);

	if( status == "running")
                kdesu( QByteArray("ifdown ").append( device.toLocal8Bit() ) );
	else
                kdesu( QByteArray("ifup ").append( device.toLocal8Bit() ) );
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void Netcardconfig::cardSelected() {
	if ( networkCardsTreeWidget->selectedItems().count() < 1 )
		return;

	configPushButton->setEnabled(TRUE);
	infoPushButton->setEnabled(TRUE);
	statusPushButton->setEnabled(TRUE);

	QString status = networkCardsTreeWidget->selectedItems().first()->text(6);
	QString type   = networkCardsTreeWidget->selectedItems().first()->text(7);

	if( status == "running")
		statusPushButton->setText(i18n("Disable Interface"));
	else
		statusPushButton->setText(i18n("Enable Interface"));

	if( type == "wireless")
		wlanPushButton->setEnabled(TRUE);
	else
		wlanPushButton->setEnabled(FALSE);
}


//------------------------------------------------------------------------------
//-- list all networkcards -----------------------------------------------------
//------------------------------------------------------------------------------

void Netcardconfig::getNetworkcards()
{
	networkCardsTreeWidget->clear();
    QStringList deviceInfos =  readProcess("nicinfo-plus", QStringList() ).split( "\n");

	// add networkcards to list
	foreach(QString deviceInfo, deviceInfos) {

		QStringList info = deviceInfo.split(",");

		if( info[0] == "" )
			continue;
		while(info.count() < 9 )
			info.append("");

		QString devicename  = info[0];       // device name:  e.g eth0 
		QString type        = info[1];       // type:         wireless/ethernet
		QString driver      = info[2];       // driver        e.g. e1000
		QString slot        = info[3];       // slot:         e.g. pci
		QString macAddress  = info[4];       // mac-address   xx:xx:...
		QString description = info[5];       // description
		QString ip          = info[6];       // ip:           xxx.xxx.xxx.xxx
		QString method      = info[7];       // method:       dhcp/static
		QString status      = info[8];        // status:       running/not running

		QTreeWidgetItem * item = new QTreeWidgetItem( networkCardsTreeWidget, 0 );
		item->setText(0, devicename);
		if( !ip.isEmpty() ) 
			item->setText(1, ip+" ("+method+")");  
		item->setText(2, description);
		item->setText(3, driver);
		item->setText(4, slot);  
		item->setText(5, macAddress);
		item->setText(6, status);
		item->setText(7, type);
		// set image
		if( type == "ethernet" ) {
			if(status == "running")
                                item->setIcon(0, QPixmap("/usr/share/icons/hicolor/16x16/actions/ncc-lan-up.png") );
			else
                                item->setIcon(0, QPixmap("/usr/share/icons/hicolor/16x16/actions/ncc-lan-down.png") );
		}
		else if( type == "wireless" ) {
			if(status == "running")
                                item->setIcon(0, QPixmap("/usr/share/icons/hicolor/16x16/actions/ncc-wlan-up.png") );
			else
                                item->setIcon(0, QPixmap("/usr/share/icons/hicolor/16x16/actions/ncc-wlan-down.png") );
		}
	}
	configPushButton->setEnabled(FALSE);
	infoPushButton->setEnabled(FALSE);
	statusPushButton->setEnabled(FALSE);
	wlanPushButton->setEnabled(FALSE);
}

//------------------------------------------------------------------------------
//-- exec ----------------------------------------------------------------------
//------------------------------------------------------------------------------

QString Netcardconfig::readProcess(QString run, QStringList arguments)
{
	QProcess exec;
	exec.start(run, arguments);
	exec.waitForFinished();
	return QString( exec.readAll() );
}


//------------------------------------------------------------------------------
//-- kdesu ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void Netcardconfig::kdesu( QByteArray input ) {
	cmd = input;
	// checkRootPrivileges
	if(getuid() != 0)
		if ( Password.isEmpty() || KDESu::SuProcess( "root" ).checkInstall( Password.toLocal8Bit() ) != 0 ) {
			  Forward = "kdesu";
			  dlg = new KPasswordDialog( this);
			  dlg->setPrompt(i18nc("@info", "<para>Administrator privileges are required.</para><para>In order to procede, please enter the system administrator's password.</para>"));
			  connect( dlg, SIGNAL( gotPassword( const QString& , bool ) )  , this, SLOT( setPassword( const QString &) ) );
			  connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
			  dlg->show();
			  return;
		}
	KDESu::SuProcess su( QByteArray( "root" ), cmd);
	su.exec( Password.toLocal8Bit() );
	getNetworkcards();
}


void Netcardconfig::setPassword(const QString& password) {
	Password = password;
	dlg->hide();
	if( Forward == "kdesu" )
		kdesu(cmd);
	else if( Forward == "lanConfig" )
		lanConfig();
	else if( Forward == "wlanConfig" )
		wlanConfig();
}

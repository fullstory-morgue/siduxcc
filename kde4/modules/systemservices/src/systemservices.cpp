/***************************************************************************
 *   Copyright (C) 2009 by Fabian Wuertz                                   *
 *   xadras@sidxu.com                                                      *
 *   Project: systemsettings-servicemanager                                *
 *   File:    servicemanager.cpp                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QLabel>
#include <QStringList>
#include <QTreeWidget>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QXmlStreamReader>
#include <QXmlSimpleReader>
#include <QXmlDefaultHandler>
#include <QDomDocument>

#include <kapplication.h>
#include <kglobalsettings.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdesu/su.h>
#include <kpassworddialog.h>
#include <kdesktopfile.h>
#include <KStandardDirs>

#include "systemservices.h"

//------------------------------------------------------------------------------
//--- init ---------------------------------------------------------------------
//------------------------------------------------------------------------------

K_PLUGIN_FACTORY(SystemServicesFactory, registerPlugin<SystemServices>();)
K_EXPORT_PLUGIN(SystemServicesFactory("kmc_systemservices"));


SystemServices::SystemServices( QWidget *parent, const QVariantList &) 
	: KCModule(SystemServicesFactory::componentData(), parent)

{
	setupUi(this);
	services = new QTreeWidget;

	// treeWidget header
	treeWidget->header()->resizeSection(0, 22);
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setToolTip(0, "Start at boot" );
	item->setIcon(0, KIcon("chronometer") );
	item->setText(1, "Name");
	item->setText(2, "Comment");
	treeWidget->setHeaderItem(item);

	// buttons
	startPushButton->setIcon( KIcon("media-playback-start") );
	stopPushButton->setIcon( KIcon("media-playback-stop") );
	bootPushButton->setIcon( KIcon("list-add") );
	
	loadServices();
}


//------------------------------------------------------------------------------
//-- load ----------------------------------------------------------------------
//------------------------------------------------------------------------------

void SystemServices::loadServices()
{
	QStringList serviceNames = QDir( "/etc/init.d/" ).entryList( QDir::Files );
	
	foreach(QString serviceName, serviceNames) {
		if( serviceName == "README" or serviceName.contains(".sh"))
		  continue;

		QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget, 0);
		item->setText(4, serviceName );
                QString configFile = KGlobal::dirs()->findResource ("data", "siduxcc/services/"+serviceName+".desktop");
		
		if (QFile::exists(configFile) ) {
		  KDesktopFile config(configFile);
		  KConfigGroup configGroup = config.desktopGroup();
		  item->setText(1, config.readName() );
		  item->setText(2, config.readComment() );
		  item->setText(3, configGroup.readEntry( "Description", "" ) );
		  item->setText(5, configGroup.readEntry( "Pid", "") );
		  
		}
		else
		  item->setText(1, serviceName );
	}
	updateServices();
}

void SystemServices::updateServices()
{
	QTreeWidgetItemIterator it(treeWidget);
	while (*it) {
		// status
		QString pid = (*it)->text(5);
		if( !pid.isEmpty() ) {
			QColor statusColor;
			if( !readProcess("pidof", (QStringList() << pid)  ).isEmpty() ) {
				statusColor = Qt::green;
				(*it)->setText(6 ,"running");
			}
			else {
				statusColor = Qt::red;
				(*it)->setText(6, "not_running");
			}
			for(int i = 0; i < 3; i++) 
				(*it)->setBackgroundColor(i, statusColor);
		}
		
		// boot
		(*it)->setIcon(0, QIcon() );
		for(int i = 0; i < 7; i++) {
			QStringList bootFiles = QDir( "/etc/rc"+QString::number(i)+".d/").entryList( QDir::Files );
			if( bootFiles.filter(QRegExp("..."+(*it)->text(4) )).count() > 0 ) {
				(*it)->setIcon(0, KIcon("dialog-ok-apply") );
				(*it)->setText(7, "booted");
				break;
			}
		}
		++it;
	}
}

//------------------------------------------------------------------------------
//-- display/search service ----------------------------------------------------
//------------------------------------------------------------------------------


void SystemServices::displayService()
{
	if ( treeWidget->selectedItems().count() < 1 )
		return;
	QString name = treeWidget->selectedItems().first()->text(1);
	QString description = treeWidget->selectedItems().first()->text(3);
	if( description.isEmpty() )
		description = "No description available.";
	descriptionTextBrowser->setText("<h2>"+name+"</h2>"+description);

	QString status = treeWidget->selectedItems().first()->text(6);
	QString boot   = treeWidget->selectedItems().first()->text(7);

	if( status == "running") {
		startPushButton->setEnabled(FALSE);
		stopPushButton->setEnabled(TRUE);
	}
	else if( status == "not_running" ) {
		startPushButton->setEnabled(TRUE);
		stopPushButton->setEnabled(FALSE);
	}
	else {
		startPushButton->setEnabled(TRUE);
		stopPushButton->setEnabled(TRUE);
	}

	bootPushButton->setEnabled(TRUE);
	if( boot == "booted" ) {
		bootPushButton->setText("Disable start at boot");
		bootPushButton->setIcon( KIcon("list-remove") );
	}
	else {
		bootPushButton->setText("Enable start at boot");
		bootPushButton->setIcon( KIcon("list-add") );
	}
}


void SystemServices::searchServices(QString search)
{
	QTreeWidgetItemIterator it(treeWidget);
	while (*it) {
		if( (*it)->text(1).contains(search, Qt::CaseInsensitive) or (*it)->text(3).contains(search, Qt::CaseInsensitive) )
			(*it)->setHidden(FALSE);
		else
			(*it)->setHidden(TRUE);
		 ++it;
	}
}



//------------------------------------------------------------------------------
//-- start/stop service --------------------------------------------------------
//------------------------------------------------------------------------------

void SystemServices::runService()
{
	
	// checkRootPrivileges
	if(getuid() != 0)
		if ( Password.isEmpty() || KDESu::SuProcess( "root" ).checkInstall( Password.toLocal8Bit() ) != 0 ) {
				KPasswordDialog *dlg = new KPasswordDialog( this);
				dlg->setPrompt(i18nc("@info", "<para>Administrator privileges are required.</para><para>In order to procede, please enter the system administrator's password.</para>"));
				connect( dlg, SIGNAL( gotPassword( const QString& , bool ) )  , this, SLOT( setPassword( const QString &) ) );
				connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
				dlg->show();
				return;
		}
	
	if ( treeWidget->selectedItems().count() < 1 )
		return;
	 QString service = treeWidget->selectedItems().first()->text(4);

	KDESu::SuProcess serviceProcess( QByteArray( "root" ), QByteArray("/etc/init.d/"+service.toLocal8Bit()+" ").append( Action.toLocal8Bit()) );
	serviceProcess.exec( Password.toLocal8Bit() );
	
	updateServices();
	displayService();
}


void SystemServices::startService()
{
	Action = "start";
	runService();
}


void SystemServices::stopService()
{
	 Action = "stop";
	 runService();
}



//------------------------------------------------------------------------------
//-- aurtoboot service ---------------------------------------------------------
//------------------------------------------------------------------------------

void SystemServices::toggleAutoBoot()
{
	if ( treeWidget->selectedItems().count() < 1 )
		return;
	QString service = treeWidget->selectedItems().first()->text(4);
	QString boot    = treeWidget->selectedItems().first()->text(7);
	if( boot == "booted" )
		boot = "remove";
	else
		boot = "defaults";
	
	Action = "boot";
	
	// checkRootPrivileges
	if(getuid() != 0)
		if ( Password.isEmpty() || KDESu::SuProcess( "root" ).checkInstall( Password.toLocal8Bit() ) != 0 ) {
				KPasswordDialog *dlg = new KPasswordDialog( this);
				dlg->setPrompt(i18nc("@info", "<para>Administrator privileges are required.</para><para>In order to procede, please enter the system administrator's password.</para>"));
				connect( dlg, SIGNAL( gotPassword( const QString& , bool ) )  , this, SLOT( setPassword( const QString &) ) );
				connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
				dlg->show();
				return;
		}
	
	
	KDESu::SuProcess serviceProcess( QByteArray( "root" ), QByteArray("update-rc.d ").append("-f ").append(service.toLocal8Bit()+" ").append(boot.toLocal8Bit()) );
	serviceProcess.exec( Password.toLocal8Bit() );
	updateServices();
	displayService();
}


//------------------------------------------------------------------------------
//-- admin mode ----------------------------------------------------------------
//------------------------------------------------------------------------------

void SystemServices::adminMode()
{
	// checkRootPrivileges
	if ( Password.isEmpty() || KDESu::SuProcess( "root" ).checkInstall( Password.toLocal8Bit() ) != 0 ) {
		  KPasswordDialog *dlg = new KPasswordDialog( this);
		  dlg->setPrompt(i18nc("@info", "<para>Administrator privileges are required.</para><para>In order to procede, please enter the system administrator's password.</para>"));
		  connect( dlg, SIGNAL( gotPassword( const QString& , bool ) )  , this, SLOT( setPassword( const QString &) ) );
		  //connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
		  dlg->show();
		  return;
	}
}

void SystemServices::setPassword(QString input)
{
	Password = input;
	if(Action == "boot")
		toggleAutoBoot();
	else
		runService();
}


//------------------------------------------------------------------------------
//-- exec ----------------------------------------------------------------------
//------------------------------------------------------------------------------

QString SystemServices::readProcess(QString run, QStringList arguments)
{
	QProcess exec;
	exec.start(run, arguments);
	exec.waitForFinished();
	return QString( exec.readAll() );
}

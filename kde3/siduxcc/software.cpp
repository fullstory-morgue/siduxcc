/*
 * software.cpp
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


#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qwidgetstack.h>
#include <qtextbrowser.h>
#include <qfile.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kled.h>
#include <klistview.h>
#include <kmessagebox.h>

#include "software.h"
#include "console.h"


software::software(QWidget *parent, const char *name, const QStringList &)
:SoftwareDialog(parent, name)
{
	this->shell = new Process();
	bottomFrame->hide();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void software::loadWidget(int i)
{
	
	if(i == 0)
	{
		if( !QFile::exists( "/usr/bin/apt-show-versions" ) )
		{
			installASV();
			return;
		}
		showPackages();
		warning();
		applyPushButton->hide();
	}
	
	bottomFrame->show();
	i++;
	widgetStack2->raiseWidget(i);
}


void software::back()
{
	widgetStack2->raiseWidget(0);
	bottomFrame->hide();
}


//------------------------------------------------------------------------------
//--- check for apt-get-show versions and warnings -----------------------------
//------------------------------------------------------------------------------



void software::installASV()
{
	// check if apt-show-versions is installed
	if( !QFile::exists( "/usr/bin/apt-show-versions" ) )
	{
		if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Apt-show-versions is missing!")+" "+i18n("Do you want to install it?") ) )
		{
			QStringList run; run << "installASV";
			startConsole(run);
		}
	}
}

void software::warning()
{
	// check for Dist-Upgrade warnings
	this->shell->setCommand("siduxcc software duWarnings");
	this->shell->start(true);
	QString status = this->shell->getBuffer().stripWhiteSpace();

	if( status == "" )
	{
		warningLabel->setText(i18n("There are no warnings"));
		warningLed->setColor(QColor(0x00ff00));
	}
	else if ( status == "disconnected")
 	{
		warningLabel->setText(i18n("Couldn't connect to sidux.com!"));
		warningLed->setColor(QColor(0xffa858));
		updateButton->setEnabled(false);
		downloadButton->setEnabled(false);
	}
	else
	{
		warningLabel->setText(i18n("Don't make a dist-Upgrade!"));
		warningLed->setColor(QColor(0xff0000));
	}
}


//------------------------------------------------------------------------------
//--- show packages ------------------------------------------------------------
//------------------------------------------------------------------------------


void software::showPackages()
{
	uList->clear();
	this->shell->setCommand("siduxcc software getUpgradablePackages");
	this->shell->start(true);
	QStringList upgrade = QStringList::split( "\n", this->shell->getBuffer() );

	QStringList tmp;
	for ( QStringList::Iterator it = upgrade.begin(); it != upgrade.end(); ++it )
	{
		QListViewItem * item = new QListViewItem( uList, 0 );
		tmp = QStringList::split( "/", *it );
		item->setText(0,tmp[0]);
		tmp = QStringList::split( " ", *it ); 
		item->setText(1,tmp[3]);
		item->setText(2,tmp[5]); 
	}
}


//------------------------------------------------------------------------------
//--- commands of the buttons  -------------------------------------------------
//------------------------------------------------------------------------------



void software::upgrade()
{
	QWhatsThis::display ( i18n("To update your system you have to leave the graphic modus. Press for this CTR+ALT+F1, login as root and type init 3. After that run siduxcc (Software->Dist-upgrade)."), QCursor::pos(), this );

}

void software::update()
{
	QStringList run; run << "updatePackageIndex";
	startConsole(run);
}


void software::download()
{
	QStringList run; run << "downloadUpgradablePackages";
	startConsole(run);
}


void software::clean()
{
	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to clear your local repository?") ) )
	{
		QStringList run; run << "cleanPackageCache";
		startConsole(run);
	}
}


//------------------------------------------------------------------------------
//--- console ------------------------------------------------------------------
//------------------------------------------------------------------------------

void software::startConsole(QStringList input)
{
	emit menuLocked(TRUE);

	QStrList run; run.append( "siduxcc" );
	run.append( "software" );
	for ( QStringList::Iterator it = input.begin(); it != input.end(); ++it )
		run.append(*it);

	// change widget
	bottomFrame->hide();
	consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 6);
	widgetStack2->raiseWidget(6);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
}


void software::terminateConsole()
{
	emit menuLocked(FALSE);
	widgetStack2->removeWidget(consoleWidget);
	loadWidget(0);
}


//egrep -B 1 "Status: hold" /var/lib/dpkg/statu

#include "software.moc"

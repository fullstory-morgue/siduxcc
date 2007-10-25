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

#include <kgenericfactory.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <qwidgetstack.h>
#include <qtextbrowser.h>
#include <kled.h>

#include "software.h"
#include "console.h"


software::software(QWidget *parent, const char *name, const QStringList &)
:SoftwareDialog(parent, name)
{
	this->shell = new Process();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void software::load()
{
}


void software::loadWidget(int i)
{
	if(i == 0)
	{
		checkASV();
	}
	else
	{
		widgetStack2->raiseWidget(i+1);
		widgetStack3->raiseWidget(1);
	}
}


void software::back()
{
	widgetStack2->raiseWidget(0);
	widgetStack3->raiseWidget(0);
}


//------------------------------------------------------------------------------
// check for apt-get-show versions and warnings


void software::checkASV()
{

	// check if apt-show-versions is installed
	this->shell->setCommand("siduxcc software checkASV");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() == "missing")
	{
		if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Apt-show-versions is missing!")+" "+i18n("Do you want to install it?") ) )
		{
			QStrList run; run.append( "siduxcc" );
				run.append( "software" );
				run.append( "installASV" );
	
			// change widget
			QWidget *consoleWidget = new console(this, run );
			widgetStack2->addWidget(consoleWidget, 6);
			widgetStack2->raiseWidget(6);
			widgetStack3->raiseWidget(2);
			widgetStack2->removeWidget(consoleWidget);
		
			connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
		}
	}
	else
	{
		showPackages();
		warning();
		widgetStack2->raiseWidget(1);
		widgetStack3->raiseWidget(1);
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
		warningLabel->setText(i18n("No internet connection!"));
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
	this->shell->setCommand("LANG=C apt-show-versions | grep upgradeable");
	this->shell->start(true);
	QStringList upgrade = QStringList::split( "\n", this->shell->getBuffer() );

	QStringList tmp;
	for(int i = 0; i < upgrade.count(); i++) {
		QListViewItem * item = new QListViewItem( uList, 0 );
		tmp = QStringList::split( "/", upgrade[i] );
		item->setText(0,tmp[0]);
		tmp = QStringList::split( " ", upgrade[i] ); 
		item->setText(1,tmp[3]);
		item->setText(2,tmp[5]); }
}


//------------------------------------------------------------------------------
//--- commands of the buttons  -------------------------------------------------
//------------------------------------------------------------------------------



void software::upgrade()
{
	KMessageBox::information(this, i18n("To run a dist-upgrade you have to init the runlevel 3. Press for this CTR+ALT+F1, login as root and type init 3. After that run the command siduxcc and choose the option Software->Dist-Upgrade") );
}


void software::update()
{
	QStrList run; run.append( "siduxcc" );
	run.append( "software" );
	run.append( "update" );

	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 6);
	widgetStack2->raiseWidget(6);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
}


void software::download()
{
	QStrList run; run.append( "siduxcc" );
	run.append( "software" );
	run.append( "download" );

	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 6);
	widgetStack2->raiseWidget(6);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
}


void software::clean()
{
	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to clear your local repository?") ) )
	{
		QStrList run; run.append( "siduxcc" );
			run.append( "software" );
			run.append( "clean" );
	
		// change widget
		QWidget *consoleWidget = new console(this, run );
		widgetStack2->addWidget(consoleWidget, 6);
		widgetStack2->raiseWidget(6);
		widgetStack3->raiseWidget(2);
		widgetStack2->removeWidget(consoleWidget);

		connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
	}
//apt-get clean
}


//------------------------------------------------------------------------------
//--- console ------------------------------------------------------------------
//------------------------------------------------------------------------------


void software::terminateConsole()
{
	showPackages();
	warning();
	widgetStack2->raiseWidget(1);
	widgetStack3->raiseWidget(1);
}


#include "software.moc"
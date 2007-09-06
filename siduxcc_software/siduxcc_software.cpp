/*
 * siduxcc_software.cpp
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

#include "siduxcc_software.h"


typedef KGenericFactory<siduxcc_software, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_software, ModuleFactory("siduxccsoftware") )

siduxcc_software::siduxcc_software(QWidget *parent, const char *name, const QStringList &)
:SoftwareDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	
	if (getuid() == 0)
	{
		// Root-Input-Widgets
		updateButton->setEnabled(true);
		downloadButton->setEnabled(true);
		cleanButton->setEnabled(true);
	}
	load();
}


void siduxcc_software::load()
{
	loadKonsole();
	konsoleFrame->installEventFilter( this );

	checkASV();
	showPackages();
	warning();
}


//------------------------------------------------------------------------------
// load console

bool siduxcc_software::eventFilter( QObject *o, QEvent *e )
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

void siduxcc_software::loadKonsole()
{
	KLibFactory* factory = KLibLoader::self()->factory( "libsanekonsolepart" );
	if (!factory)
		factory = KLibLoader::self()->factory( "libkonsolepart" );
	konsole = static_cast<KParts::Part*>( factory->create( konsoleFrame, "konsolepart", "QObject", "KParts::ReadOnlyPart" ) );
	terminal()->setAutoDestroy( true );
	terminal()->setAutoStartShell( false );
	konsole->widget()->setGeometry(0, 0, konsoleFrame->width(), konsoleFrame->height());	
	konsole->widget()->setFocus();
}

//------------------------------------------------------------------------------
// check for apt-get-show versions and warnings


void siduxcc_software::checkASV()
{

	// check if apt-show-versions is installed
	this->shell->setCommand("siduxcc software checkASV");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() == "missing")
	{

		if (getuid() == 0)
		{
			if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Apt-show-versions is missing!")+" "+i18n("Do you want to install it?") ) )
			{
				// change widget
				widgetStack->raiseWidget(1);

				// run command
				QStrList run; run.append( "siduxcc" ); 
					run.append( "software" );
					run.append( "installASV" );
				terminal()->startProgram( "siduxcc", run );
			
				connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );
			}
		}
		else
		{
			KMessageBox::information(this, i18n("Apt-show-versions is missing!")+" "+i18n("Switch to the Administration to install it!") );
		}	

	}
}

void siduxcc_software::warning()
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
// show packages

void siduxcc_software::showPackages()
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
// commands of the buttons 


void siduxcc_software::upgrade()
{
	KMessageBox::information(this, i18n("To run a dist-upgrade you have to init the runlevel 3. Press for this CTR+ALT+F1, login as root and type init 3. After that run the command siduxcc and choose the option Software->Dist-Upgrade") );
}


void siduxcc_software::update()
{
	// change widget
	widgetStack->raiseWidget(1);

	// run command
	QStrList run; run.append( "siduxcc" ); 
		run.append( "software" );
		run.append( "update" );
	terminal()->startProgram( "siduxcc", run );

	connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );

}


void siduxcc_software::download()
{
	// change widget
	widgetStack->raiseWidget(1);

	// run command
	QStrList run; run.append( "siduxcc" ); 
		run.append( "software" );
		run.append( "download" );
	terminal()->startProgram( "siduxcc", run );

	connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );
}


void siduxcc_software::clean()
{
	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to clear your local repository?") ) )
	{
		this->shell->setCommand("apt-get clean");
		this->shell->start(true);
		KMessageBox::information(this, i18n("The local repository was cleaned.") );
	}
}


void siduxcc_software::back()
{
	widgetStack->raiseWidget(0);
	load();
}


//------------------------------------------------------------------------------
// metapackages

void siduxcc_software::metapackages()
{
	this->shell->setCommand("su-me /usr/sbin/metapackage-installer&");
	this->shell->start(true);
}



#include "siduxcc_software.moc"

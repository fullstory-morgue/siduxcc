/*
 * hermes.cpp
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


#include "hermes.h"

#include <kgenericfactory.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <kapp.h>
#include <klocale.h>


hermes::hermes(QWidget* parent, const char* name )
        : hermesBase(parent,name)
{

	this->shell = new Process();

	getPackages();


	if(i18n("en") == "de")
		this->shell->setCommand("siduxcc software duWarningsDe");
	else
		this->shell->setCommand("siduxcc software duWarningsEn");
	this->shell->start(true);
	QStringList list = QStringList::split( "\n", this->shell->getBuffer() );
	QStringList title = list.grep( "title" );
	link  = list.grep( "link" );
	title = title.gres( "<title>"  , "" );
	title = title.gres( "</title>" , "" );
	title = title.gres( "Warning: ", "" );
	link  = link.gres(  "<link>"   , "" );
	link  = link.gres(  "</link>"  , "" );
	for ( QStringList::Iterator it = title.begin(); it != title.end(); ++it )
		warningListBox->insertItem(QPixmap("/usr/share/siduxcc/icons/warning.png"), *it);

	loadKonsole();
	konsoleFrame->installEventFilter( this );

}

void hermes::init(int i)
{
	tabWidget->setCurrentPage(i);
}

void hermes::getPackages()
{
	packageListView->clear();
	this->shell->setCommand("LANG=C apt-show-versions | grep upgradeable");
	this->shell->start(true);
	QStringList upgrade = QStringList::split( "\n", this->shell->getBuffer() );

	QStringList tmp;
	for ( QStringList::Iterator it = upgrade.begin(); it != upgrade.end(); ++it )
	{
		QListViewItem * item = new QListViewItem( packageListView, 0 );
		tmp = QStringList::split( "/", *it );
		item->setText(0,tmp[0]);
		tmp = QStringList::split( " ", *it ); 
		item->setText(1,tmp[3]);
		item->setText(2,tmp[5]); 
	}

	updatePushButton->setHidden(TRUE);

}

void hermes::tabChanged()
{
	if(tabWidget->currentPageIndex() == 1)
		updatePushButton->setHidden(FALSE);
	else
		updatePushButton->setHidden(TRUE);
}

void hermes::openLink(int i)
{
	kapp->invokeBrowser( link[i] );
}

void hermes::update()
{

	QStrList run; run.append( "siduxcc" ); 
		run.append( "software" );
		run.append( "updateHermes" );
	
	// change widget
	widgetStack->raiseWidget(1);

	terminal()->startProgram( "siduxcc", run );

	connect( konsole, SIGNAL(destroyed()), this, SLOT( terminateConsole() ) );
}

void hermes::terminateConsole()
{
	widgetStack->raiseWidget(0);
	updatePushButton->setHidden(FALSE);
	getPackages();

	loadKonsole();
	konsoleFrame->installEventFilter( this );
}


//------------------------------------------------------------------------------
//--- load console -------------------------------------------------------------
//------------------------------------------------------------------------------

void hermes::loadKonsole()
{
	KLibFactory* factory = KLibLoader::self()->factory( "libsanekonsolepart" );
	if (!factory)
		factory = KLibLoader::self()->factory( "libkonsolepart" );
	konsole = static_cast<KParts::Part*>( factory->create( konsoleFrame, "konsolepart", "QObject", "KParts::ReadOnlyPart" ) );
	terminal()->setAutoDestroy( true );
	terminal()->setAutoStartShell( false );
	konsole->widget()->setGeometry(0, 0, konsoleFrame->width(), konsoleFrame->height());	
}

bool hermes::eventFilter( QObject *o, QEvent *e )
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


#include "hermes.moc"

/*
 * up.cpp
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


#include "up.h"

#include <qlabel.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <kapp.h>
#include <klocale.h>


#include <../libsiduxcc/console.h>

up::up(QWidget* parent, const char* name)
        : upBase(parent,name)
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

}

void up::getPackages()
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

void up::tabChanged()
{
	if(tabWidget->currentPageIndex() == 1)
		updatePushButton->setHidden(FALSE);
	else
		updatePushButton->setHidden(TRUE);
}

void up::openLink(int i)
{
	kapp->invokeBrowser( link[i] );
}

void up::update()
{

	QStrList run; run.append( "siduxcc" ); 
		run.append( "software" );
		run.append( "updateHermes" );
	
	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack->addWidget(consoleWidget, 1);
	widgetStack->raiseWidget(1);
	widgetStack->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));
}

void up::terminateConsole()
{
	widgetStack->raiseWidget(0);
	updatePushButton->setHidden(FALSE);
	getPackages();
}




#include "up.moc"

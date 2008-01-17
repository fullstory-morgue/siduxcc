/*
 * start.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * start is based on knxcc_kmain from Andreas Loible
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



#include <qwidgetstack.h>
#include <kaboutapplication.h>
#include <kaboutkde.h>
#include <qstatusbar.h>
#include <klistview.h>
#include <qheader.h>
#include <kprocess.h>
#include <kapp.h>
#include <kswitchlanguagedialog.h>



#include "display.h"
#include "network.h"
#include "services.h"
#include "software.h"
#include "internet.h"
#include "kernel.h"
#include "info.h"


#include "start.h"

start::start(QWidget *parent, const char *name, const QStringList &)
:StartDialog(parent, name)
{

	statusBar()->hide();

	menuListView->setAlternateBackground( QColor(237, 244, 249) );
	menuListView->header()->hide();

	KListViewItem * item0 = new KListViewItem( menuListView, 0 );
	item0->setText(0, i18n("Display") );
	item0->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/display.png") );

	KListViewItem * item1 = new KListViewItem( menuListView, 0 );
	item1->setText(0, i18n("Internet") );
	item1->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/browser.png") );

	KListViewItem * item2 = new KListViewItem( menuListView, 0 );
	item2->setText(0, i18n("Kernel") );
	item2->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/tux.png") );

	KListViewItem * item3 = new KListViewItem( menuListView, 0 );
	item3->setText(0, i18n("Network") );
	item3->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/network.png") );

	KListViewItem * item4 = new KListViewItem( menuListView, 0 );
	item4->setText(0, i18n("Services") );
	item4->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/kcmsystem.png") );

	KListViewItem * item5 = new KListViewItem( menuListView, 0 );
	item5->setText(0, i18n("Software") );
	item5->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/package.png") );

	KListViewItem * item6 = new KListViewItem( menuListView, 0 );
	item6->setText(0, i18n("System-Info") );
	item6->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/info.png") );

}

void start::menu()
{
	

	if( menuListView->selectedItems().count() < 1)
		return;


	QString open = menuListView->selectedItems().first()->text(0);

	widgetStack->removeWidget(widgetStack->widget(0));

	if( open == i18n("Display") )
	{
		QWidget *displayWidget = new display(this);
		widgetStack->addWidget(displayWidget, 0);
		widgetStack->raiseWidget(0);
		connect( displayWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( open == i18n("Network") )
	{
		QWidget *networkWidget = new network(this);
		widgetStack->addWidget(networkWidget, 0);
		widgetStack->raiseWidget(0);
		connect( networkWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( open == i18n("Services") )
	{
		QWidget *servicesWidget = new services(this);
		widgetStack->addWidget(servicesWidget, 0);
		widgetStack->raiseWidget(0);
	}
	else if( open == i18n("Software") )
	{
		QWidget *softwareWidget = new software(this);
		widgetStack->addWidget(softwareWidget, 0);
		widgetStack->raiseWidget(0);
		connect( softwareWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( open == i18n("Internet") )
	{
		QWidget *internetWidget = new internet(this);
		widgetStack->addWidget(internetWidget, 0);
		widgetStack->raiseWidget(0);
	}
	else if( open == i18n("Kernel") )
	{
		QWidget *kernelWidget = new kernel(this);
		widgetStack->addWidget(kernelWidget, 0);
		widgetStack->raiseWidget(0);
		connect( kernelWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( open == i18n("System Info") )
	{
		QWidget *infoWidget = new info(this);
		widgetStack->addWidget(infoWidget, 0);
		widgetStack->raiseWidget(0);
	}
	else if( open == i18n("About") )
	{
		KAboutApplication* about = new KAboutApplication ( this );
		about->show();
	}
	else if( open == i18n("Exit") )
		close();

}

void start::menuStatus(bool b)
{
	if( b )
		menuListView->setEnabled(FALSE);
	else
		menuListView->setEnabled(TRUE);
}

void start::about()
{
	KAboutApplication* about = new KAboutApplication ( this );
	about->show();
}

void start::aboutKDE()
{
	KAboutKDE* about = new KAboutKDE ( this );
	about->show();
}

void start::manual()
{
	kapp->invokeBrowser( "/usr/share/sidux-manual/index.html" );
}

void start::homepage()
{
	kapp->invokeBrowser( "http://www.sidux.com/" );
}

void start::language()
{
	KSwitchLanguageDialog *lang = new KSwitchLanguageDialog( this );
	lang->show();
}



#include "start.moc"

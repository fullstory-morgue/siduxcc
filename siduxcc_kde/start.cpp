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
}

void start::menu(QListBoxItem* entry)
{
	widgetStack->removeWidget(widgetStack->widget(0));

	if( entry->text() == i18n("Display") )
	{
		QWidget *displayWidget = new display(this);
		widgetStack->addWidget(displayWidget, 0);
		widgetStack->raiseWidget(0);
		connect( displayWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( entry->text() == i18n("Network") )
	{
		QWidget *networkWidget = new network(this);
		widgetStack->addWidget(networkWidget, 0);
		widgetStack->raiseWidget(0);
		connect( networkWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( entry->text() == i18n("Services") )
	{
		QWidget *servicesWidget = new services(this);
		widgetStack->addWidget(servicesWidget, 0);
		widgetStack->raiseWidget(0);
	}
	else if( entry->text() == i18n("Software") )
	{
		QWidget *softwareWidget = new software(this);
		widgetStack->addWidget(softwareWidget, 0);
		widgetStack->raiseWidget(0);
		connect( softwareWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( entry->text() == i18n("Internet") )
	{
		QWidget *internetWidget = new internet(this);
		widgetStack->addWidget(internetWidget, 0);
		widgetStack->raiseWidget(0);
	}
	else if( entry->text() == i18n("Kernel") )
	{
		QWidget *kernelWidget = new kernel(this);
		widgetStack->addWidget(kernelWidget, 0);
		widgetStack->raiseWidget(0);
		connect( kernelWidget, SIGNAL( menuLocked(bool) ), this, SLOT( menuStatus(bool) ));
	}
	else if( entry->text() == i18n("System Info") )
	{
		QWidget *infoWidget = new info(this);
		widgetStack->addWidget(infoWidget, 0);
		widgetStack->raiseWidget(0);
	}
	else if( entry->text() == i18n("About") )
	{
		KAboutApplication* about = new KAboutApplication ( this );
		about->show();
	}
	else if( entry->text() == i18n("Exit") )
		close();

}

void start::menuStatus(bool b)
{
	if( b )
		menuListBox->setEnabled(FALSE);
	else
		menuListBox->setEnabled(TRUE);
}

#include "start.moc"

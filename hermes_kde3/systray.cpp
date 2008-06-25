/*
 * systray.cpp
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

#include "systray.h"
#include "legend.h"

#include <kaboutapplication.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <qclipboard.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <klocale.h>
#include <kapplication.h>



SysTray::SysTray ( QWidget* parent, const char* name )
		: KSystemTray ( parent, name )
{
	this->shell = new Process();
	QToolTip::add ( this,i18n("siduxcc-hermes"));

	menu = contextMenu();
	menu->insertItem ( SmallIcon ( "info" ), i18n("Forum - Upgrade Warnings"), this, SLOT ( forum() ) );
	menu->insertItem ( SmallIcon ( "sidux_book" ), i18n("Manual - Upgrade of an Installed System"), this, SLOT ( manual() ) );
	menu->insertItem ( SmallIcon ( "package" ), i18n("Upgradable packages"), this, SLOT ( upgradablePackages() ) );
	menu->insertItem ( SmallIcon ( "info" ), i18n("sidux news"), this, SLOT ( news() ) );
	menu->insertItem ( SmallIcon ( "siduxcc_warning" ), i18n("Warnings"), this, SLOT ( warnings() ) );
	menu->insertSeparator();
	menu->insertItem ( SmallIcon ( "khelpcenter" ), i18n("Show Legend"), this, SLOT ( showLegend() ) );
	menu->insertItem ( SmallIcon ( "siduxcchermes" ), i18n("&About siduxcc-hermes"), this, SLOT ( showAbout() ) );

	updateIcon();

	dialog = new hermes( );
}


void SysTray::forum()
{
	kapp->invokeBrowser( "http://sidux.com/PNphpBB2-viewforum-f-29.html" );
}
void SysTray::manual()
{
	kapp->invokeBrowser( "http://manual.sidux.com/" + i18n("en") + "/sys-admin-apt-" + i18n("en") + ".htm#apt-upgrade" );
}

void SysTray::showAbout()
{
	KAboutApplication* about = new KAboutApplication ( this );
	about->show();
}

void SysTray::showHermes()
{
   if(dialog->isVisible() )
		dialog->hide();
	else
		dialog->show();
}

void SysTray::warnings()
{
	dialog->init(0);
	dialog->show();
}

void SysTray::news()
{
	dialog->init(1);
	dialog->show();
}


void SysTray::upgradablePackages()
{
	dialog->init(2);
	dialog->show();
}


void SysTray::showLegend()
{
	legend* dialog = new legend( );
	dialog->show();
}


void SysTray::updateIcon()
{
	this->shell->setCommand ( "siduxcc software duWarnings" );
	this->shell->start ( true );
	QString tmp = this->shell->getBuffer().stripWhiteSpace();
	if ( tmp == "disconnected" )
	{
		setPixmap( QPixmap("/usr/share/siduxcc/icons/disconnected") );
	}
	else
	{
		if ( tmp == "" )
		{
			setPixmap( QPixmap("/usr/share/siduxcc/icons/ok-hermes.png") );
		}
		else
		{
			setPixmap( QPixmap("/usr/share/siduxcc/icons/notok-hermes.png") );
		}
	}

	QTimer *timer = new QTimer();
	connect ( timer, SIGNAL ( timeout() ), this, SLOT ( updateIcon() ) );
	timer->start ( 1800000, TRUE ); // 30 min single-shot timer
}


void SysTray::mousePressEvent(QMouseEvent* e)
{
	KSystemTray::mousePressEvent(e);
	if(e->button() == LeftButton)
		showHermes();
}

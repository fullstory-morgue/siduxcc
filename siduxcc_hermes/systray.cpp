/*
 * systray.cpp
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

#include "systray.h"
#include "legend.h"
#include "upgradable.h"

#include <kaboutapplication.h>
#include <kiconloader.h>
#include <kkeydialog.h>

#include <qclipboard.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qtooltip.h>

#include <qtimer.h>
#include <klocale.h>



SysTray::SysTray ( QWidget* parent, const char* name )
		: KSystemTray ( parent, name )
{
	this->shell = new Process();
	QToolTip::add ( this,i18n("siduxcc-hermes"));

	menu = contextMenu();
	menu->insertItem ( SmallIcon ( "siduxcc" ), i18n("Forum - Upgrade Warnings"), this, SLOT ( forum() ) );
	menu->insertItem ( SmallIcon ( "sidux_book" ), i18n("Manual - Upgrade of an Installed System"), this, SLOT ( manual() ) );
	menu->insertItem ( SmallIcon ( "package" ), i18n("Show upgradable packages"), this, SLOT ( upgradablePackages() ) );
	menu->insertItem ( SmallIcon ("exec"), i18n("Update (apt-get update)"), this, SLOT (update()) );
	menu->insertSeparator();
	menu->insertItem ( SmallIcon ( "khelpcenter" ), i18n("Show Legend"), this, SLOT ( showLegend() ) );
	menu->insertItem ( SmallIcon ( "siduxcc" ), i18n("&About siduxcc-hermes"), this, SLOT ( showAbout() ) );

	updateIcon();
}


void SysTray::forum()
{
	this->shell->setCommand ("x-www-browser http://sidux.com/PNphpBB2-viewforum-f-29.html&");
	this->shell->start ( true );
}
void SysTray::manual()
{
	this->shell->setCommand ("x-www-browser http://manual.sidux.com/"+i18n("en")+"/sys-admin-apt-"+i18n("en")+".htm#apt-upgrade&");
	this->shell->start ( true );
}

void SysTray::update()
{
	this->shell->setCommand ( "su-me konsole -T \"Download newest Kernel\" --nomenubar --notabbar -e apt-get update&" );
	this->shell->start ( true );
}

void SysTray::showAbout()
{
	KAboutApplication* about = new KAboutApplication ( this );
	about->show();
}

void SysTray::upgradablePackages()
{
	upgradable* dialog = new upgradable();
	dialog->show();
}

void SysTray::showLegend()
{
	legend* dialog = new legend();
	dialog->show();
}

void SysTray::updateIcon()
{
	this->shell->setCommand ( "siduxcc software duWarnings" );
	this->shell->start ( true );
	QString tmp = this->shell->getBuffer().stripWhiteSpace();
	if ( tmp == "disconnected" )
	{
		setPixmap ( loadIcon ( "siduxcc_disconnected" ) );
	}
	else
	{
		if ( tmp == "" )
		{
			setPixmap ( loadIcon ( "siduxcc_ok" ) );
		}
		else
		{
			setPixmap ( loadIcon ( "siduxcc_warning" ) );
		}
	}

	QTimer *timer = new QTimer();
	connect ( timer, SIGNAL ( timeout() ), this, SLOT ( updateIcon() ) );
	timer->start ( 300000, TRUE ); // 30 seconds single-shot timer
}

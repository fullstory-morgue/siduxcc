/*
 * internet.cpp
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

#include <qcombobox.h>
#include <kgenericfactory.h>
#include <kpushbutton.h>

#include "internet.h"

internet::internet(QWidget *parent, const char *name, const QStringList &)
:InternetDialog(parent, name)
{
	this->shell = new Process();
	load();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void internet::load()
{
	getBrowsers();
	applyPushButton->setEnabled(FALSE);
}


//------------------------------------------------------------------------------
//--- save ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void internet::save()
{
	// Set browser 
	this->shell->setCommand("update-alternatives --set x-www-browser /usr/bin/"+browserSelect->currentText());
	this->shell->start(true);
	applyPushButton->setEnabled(FALSE);
}

void internet::hasChanged()
{
	applyPushButton->setEnabled(TRUE);
}


//------------------------------------------------------------------------------
//--- get Browsers -------------------------------------------------------------
//------------------------------------------------------------------------------

void internet::getBrowsers()
{
	// get the current browser	
	this->shell->setCommand("update-alternatives --display x-www-browser | head -n 2 | tail -n 1 | cut -f4 -d/");
	this->shell->start(true);
	QString current = this->shell->getBuffer().stripWhiteSpace();


	this->shell->setCommand("update-alternatives --list x-www-browser");
	this->shell->start(true);
	QStringList browserList = QStringList::split( "\n", this->shell->getBuffer() );

	for(uint i = 0; i < browserList.count(); i++)
	{
		QStringList browser = QStringList::split( "/", browserList[i] );
		browserSelect->insertItem(browser[2]);
		if(browser[2] == current) { browserSelect->setCurrentItem(i);}
	}
}

#include "internet.moc"

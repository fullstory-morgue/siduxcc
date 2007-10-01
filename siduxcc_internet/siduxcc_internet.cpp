/*
 * siduxcc_internet.cpp
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

#include "siduxcc_internet.h"

typedef KGenericFactory<siduxcc_internet, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_internet, ModuleFactory("siduxccinternet") )

siduxcc_internet::siduxcc_internet(QWidget *parent, const char *name, const QStringList &)
:InternetDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() != 0)
	{
		browserSelect->setDisabled(true);
		m3Button->setDisabled(false);
		d1Button->setDisabled(false);
		d2Button->setDisabled(false);
		d2Button->setDisabled(false);
	}
}


void siduxcc_internet::load()
{
	getBrowsers();
}



void siduxcc_internet::save()
{
	// Set browser 
	this->shell->setCommand("update-alternatives --set x-www-browser /usr/bin/"+browserSelect->currentText());
	this->shell->start(true);
}


void siduxcc_internet::getBrowsers()
{
	// get the current browser	
	this->shell->setCommand("update-alternatives --display x-www-browser | head -n 2 | tail -n 1 | cut -f4 -d/");
	this->shell->start(true);
	QString current = this->shell->getBuffer().stripWhiteSpace();


	this->shell->setCommand("update-alternatives --list x-www-browser");
	this->shell->start(true);
	QStringList browserList = QStringList::split( "\n", this->shell->getBuffer() );

	for(int i = 0; i < browserList.count(); i++)
	{
		QStringList browser = QStringList::split( "/", browserList[i] );
		browserSelect->insertItem(browser[2]);
		if(browser[2] == current) { browserSelect->setCurrentItem(i);}
	}
}


// start button commands


void siduxcc_internet::m1Slot()
{
	this->shell->setCommand("su-me modemlink&");
	this->shell->start(true);
}
void siduxcc_internet::m2Slot()
{
	this->shell->setCommand("su-me gprsconnect&");
	this->shell->start(true);
}
void siduxcc_internet::m3Slot() 
{
	this->shell->setCommand("kppp&");
	this->shell->start(true); 
}
void siduxcc_internet::m4Slot() 
{
	this->shell->setCommand("su-me my-pppconf&");
	this->shell->start(true); 
}

void siduxcc_internet::d1Slot()
{
	this->shell->setCommand("fcdsl-pppoeconf&");
	this->shell->start(true);
}
void siduxcc_internet::d2Slot() 
{
	this->shell->setCommand("my-pppoeconf&");
	this->shell->start(true);
}
void siduxcc_internet::d3Slot() 
{
	this->shell->setCommand("su-me pppoeconf&");
	this->shell->start(true); 
}

void siduxcc_internet::i1Slot() 
{
	this->shell->setCommand("su-me x-terminal-emulator -e capi-isdnconf&"); 
	this->shell->start(true); 
}

#include "siduxcc_internet.moc"

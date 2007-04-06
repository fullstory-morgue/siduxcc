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
#include <qpixmap.h>
#include <upgradable.h>

#include "siduxcc_software.h"


typedef KGenericFactory<siduxcc_software, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_software, ModuleFactory("siduxccsoftware") )

siduxcc_software::siduxcc_software(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() == 0)
	{
		// Root-Input-Widgets
		updateButton->setEnabled(true);
		duwarnerButton->setEnabled(false);

	}
}

void siduxcc_software::load()
{
	// check for upgradable packages
	this->shell->setCommand("apt-show-versions | egrep upgradeable | wc -l");
	this->shell->start(true);
	upText->setText("("+this->shell->getBuffer().stripWhiteSpace()+")");
	if(this->shell->getBuffer().toInt() == 0)
	{
		upPixmap->setPixmap(QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_ok.png"));
	}
	else 
	{
		upPixmap->setPixmap(QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_warning.png"));
	}
	

	// check for Dist-Upgrade warnings
	this->shell->setCommand("siduxcc software duWarnings");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() == "")
	{
		duwarnerPixmap->setPixmap(QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_ok.png"));
	}
	else 
	{
		if(this->shell->getBuffer().stripWhiteSpace() == "disconnected")
			{
				duwarnerPixmap->setPixmap(QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_disconnected.png"));
			}
		else
			{
				duwarnerPixmap->setPixmap(QPixmap("/usr/share/icons/hicolor/22x22/apps/siduxcc_warning.png"));
			}
	}
}

void siduxcc_software::update()
{
	this->shell->setCommand("konsole -T \"apt-get update\" --nomenubar --notabbar -e apt-get update&");
	this->shell->start(true);
	load();
}

void siduxcc_software::metapackages()
{
	this->shell->setCommand("su-me /usr/sbin/metapackage-installer&");
	this->shell->start(true);
}

void siduxcc_software::duwarner()
{
	this->shell->setCommand("siduxcc-hermes&");
	this->shell->start(true);
}

void siduxcc_software::upgradablePackages()
{
	upgradable* dialog = new upgradable();
	dialog->show();
}


#include "siduxcc_software.moc"

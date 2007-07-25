/*
 * siduxcc_info.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * siduxcc_info is based on knxcc_info from Andreas Loible
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

#include <qlineedit.h>
#include <qtextedit.h>
#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>


#include "siduxcc_info.h"

typedef KGenericFactory<siduxcc_info, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_info, ModuleFactory("siduxccinfo") )

siduxcc_info::siduxcc_info(QWidget *parent, const char *name, const QStringList &)
:InfoDialog(parent, name)
{
	this->shell = new Process();
	load();
}

void siduxcc_info::load(){
	this->shell->setCommand("cat /etc/sidux-version");
	this->shell->start(true);
	siduxVersion->setText(this->shell->getBuffer().simplifyWhiteSpace());

	this->shell->setCommand("infobash -v3 1 | perl -pe 's///g;'");
	this->shell->start(true);
	infobash->setText(this->shell->getBuffer());
}


#include "siduxcc_info.moc"

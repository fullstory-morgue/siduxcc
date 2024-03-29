/*
 * info.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * info is based on knxcc_info from Andreas Loible
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

#include "info.h"

info::info(QWidget *parent, const char *name, const QStringList &)
:InfoDialog(parent, name)
{
	this->shell = new Process();
	this->shell->setCommand("siduxcc info getSiduxVersion");
	this->shell->start(true);
	siduxVersion->setText(this->shell->getBuffer().simplifyWhiteSpace());

	this->shell->setCommand("siduxcc info getSystemInfo");
	this->shell->start(true);
	infobash->setText(this->shell->getBuffer());
}


#include "info.moc"

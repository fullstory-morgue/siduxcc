/*
 * upgrabale.cpp
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


#include "upgradable.h"
#include <qlabel.h>
#include <klistview.h>

upgradable::upgradable(QWidget* parent, const char* name)
        : upgradableBase(parent,name)
{

	this->shell = new Process();

	this->shell->setCommand("LANG=C apt-show-versions | grep upgradeable");
	this->shell->start(true);
	QStringList upgrade = QStringList::split( "\n", this->shell->getBuffer() );

	QStringList tmp;
	for(int i = 0; i < upgrade.count(); i++) {
		QListViewItem * item = new QListViewItem( uList, 0 );
		tmp = QStringList::split( "/", upgrade[i] );
		item->setText(0,tmp[0]);
		tmp = QStringList::split( " ", upgrade[i] ); 
		item->setText(1,tmp[3]);
		item->setText(2,tmp[5]); }
}

#include "upgradable.moc"

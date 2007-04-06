/*
 * upgradable.h
 *
 * Copyright (c) 2007 Fabian Würtz
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


#ifndef UPGRADABLE_H
#define UPGRADABLE_H

#include "process.h"
#include "upgradablebase.h"


class upgradable : public upgradableBase
{
	Q_OBJECT

	public:
		upgradable(QWidget* parent = 0, const char* name = 0 );

	private:
		Process* shell;

};

#endif

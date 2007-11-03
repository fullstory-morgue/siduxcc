/*
 * start.h
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

#ifndef START_H_
#define START_H_

#include <startdialog.h>

class start : public StartDialog
{
	Q_OBJECT

	public:
		start(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());

	public slots:
		virtual void menu(QListBoxItem*);
		virtual void menuStatus(bool);
};

#endif

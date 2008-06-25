/*
 * internet.h
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

#ifndef INTERNET_H_
#define INTERNET_H_

#include "internetdialog.h"
#include "../libsiduxcc/process.h"

class internet : public InternetDialog
{
	Q_OBJECT

	public:
		internet(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
	
	private:
		Process* shell;
	
	public slots:
		virtual void save();
		virtual void hasChanged();
		virtual void getBrowsers();
};

#endif

/*
 * siduxcc_internet.h
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

#ifndef SIDUXCC_INTERNET_H_
#define SIDUXCC_INTERNET_H_

#include "internetdialog.h"
#include "../libsiduxcc/process.h"

class siduxcc_internet : public InternetDialog
{
	Q_OBJECT

	public:
		siduxcc_internet(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		void save();
	
	private:
		Process* shell;
	
	public slots:
		virtual void m1Slot();
		virtual void m2Slot();
		virtual void m3Slot();
		virtual void m4Slot();
		virtual void d1Slot();
		virtual void d2Slot();
		virtual void d3Slot();
		virtual void i1Slot();

		virtual void getBrowsers();
};

#endif

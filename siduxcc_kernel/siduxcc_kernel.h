/*
 * siduxcc_kernel.h
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

#ifndef SIDUXCC_NETWORK_H_
#define SIDUXCC_NETWORK_H_

#include "displaydialog.h"
#include "../libsiduxcc/process.h"

class siduxcc_kernel : public DisplayDialog
{
	Q_OBJECT

	public:
		siduxcc_kernel(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
	
	private:
		Process* shell;
		KProcess proc;
		bool experimental;
	
	public slots:
		virtual void update1();
		virtual void update2();
		virtual void download();
		virtual void unzip();
		virtual void install();
		virtual void finish();
		virtual void back1();
		virtual void back2();
		virtual void details();
		virtual void remove();
		virtual void getOutput(KProcess *, char *, int);
};

#endif

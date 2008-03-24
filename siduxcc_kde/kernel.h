/*
 * kernel.h
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

#ifndef KERNEL_H_
#define KERNEL_H_

#include "kerneldialog.h"
#include "../libsiduxcc/process.h"


class kernel : public KernelDialog
{
	Q_OBJECT

	public:
		kernel(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		//QString getCurrentKernel();
		//QString installKernel;
		void getModules();
		void getKernels();
		void getMetapackageStatus();
		void getOldKernels();
		bool isInstalled(QString);
		QString stableKernelFull;
		QStringList modules;
		int ci;

	private:
		Process* shell;
		KProcess proc;
		bool experimental;
	
	public slots:
		virtual void loadWidget(int);
		virtual void back();

		virtual void remove();
		virtual void installKernel();
		virtual void showModules();
		virtual void installModules();
		virtual void toggleKernelMetapackage();
		virtual void hasChanged();

		virtual void startConsole(QStringList input);
		virtual void terminateConsole();

		virtual void moduleDescription(QListBoxItem*);

	signals:
		void menuLocked(bool);

};

#endif

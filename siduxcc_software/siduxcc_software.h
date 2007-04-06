/*
 * siduxcc_software.h
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

#ifndef SIDUXCC_SOFTWARE_H_
#define SIDUXCC_SOFTWARE_H_

#include "displaydialog.h"
#include "process.h"

class siduxcc_software : public DisplayDialog
{
	Q_OBJECT

	public:
		siduxcc_software(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
	
	private:
		Process* shell;
	
	public slots:
		virtual void update();
		virtual void metapackages();
		virtual void duwarner();
		virtual void upgradablePackages();
	
};

#endif

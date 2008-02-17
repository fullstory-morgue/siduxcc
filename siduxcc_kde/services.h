/*
 * services.h
 *
 * Copyright (c) 2007 Fabian Wuertz
 * services is based on knxcc_services from Andreas Loible
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

#ifndef SERVICES_H_
#define SERVICES_H_


#include "servicesdialog.h"
#include "../libsiduxcc/process.h"

class services : public ServicesDialog
{
	Q_OBJECT
	
	public:
		services(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		QStringList pid;
		QStringList description;
	
	private:
		Process* shell;
		QValueList<QString> servicesToEnable;
		QValueList<QString> servicesToDisable;
	
	
	public slots:
		virtual void loadWidget(int);
		virtual void back();

		virtual void save();
		virtual void hasChanged();

		virtual void updateSlot();

		virtual void startService();
		virtual void stopService();
		virtual void restartService();
		virtual void reloadService();
		virtual void runServiceAction(QString);

	
	protected slots:
		void disableService();
		void enableService();

};

#endif

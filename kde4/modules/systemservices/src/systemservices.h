/***************************************************************************
 *   Copyright (C) 2009 by Fabian Wuertz                                   *
 *   xadras@sidxu.com                                                      *
 *   Project: systemsettings-servicemanager                                *
 *   File:    servicemanager.h                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SYSTEMSERVICES_H
#define SYSTEMSERVICES_H


#include <QtGui/QtGui>
#include <QMainWindow>
#include <kcmodule.h>
#include <KUrl>
#include "ui_systemservices.h"

class SystemServices : public KCModule, Ui::SystemServicesGui
{
	Q_OBJECT
	public:
		SystemServices(QWidget *parent, const QVariantList &);

	public slots:
		void adminMode();
		void setPassword(QString);
		void startService();
		void stopService();
		void runService();
		void loadServices();
		void displayService();
		void searchServices(QString);
		void updateServices();
		void toggleAutoBoot();

	private:
		QString readProcess(QString, QStringList);
		QString Password, Action;
		QTreeWidget* services;

};

#endif

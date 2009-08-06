/*
 * netcardconfig.h
 *
 * Copyright (c) 2007 Fabian Wuertz <xadras@sidux.com>
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


#ifndef NETCARDCONFIG_H
#define NETCARDCONFIG_H


#include <kcmodule.h>
#include <kpassworddialog.h>

#include "ui_netcardconfig.h"

class Netcardconfig : public KCModule, Ui::NccGui
{
	Q_OBJECT

	public:
		Netcardconfig(QWidget *parent, const QVariantList &);

	private slots:
		void getNetworkcards();
		void setPassword( const QString &);
		void kdesu(QByteArray);
		void toggleCardStatus();
		void cardSelected();
		void cardInfo();
		void lanConfig();
		void wlanConfig();


	private:
		//bool restartCard(QString device);
		QString readProcess(QString, QStringList);
		QString Password;
		QString Forward;
		QByteArray cmd;
		KPasswordDialog *dlg;

};

#endif

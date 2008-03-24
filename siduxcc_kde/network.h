/*
 * network.h
 *
 * Copyright (c) 2007 Fabian Wuertz
 * network is based on knxcc_network from Andreas Loible
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

#ifndef NETWORK_H_
#define NETWORK_H_

#include "networkdialog.h"
#include "../libsiduxcc/process.h"

class network : public NetworkDialog
{
	Q_OBJECT

	public:
		network(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		void connections();

		void setNetworkcardSettings(QString, QString);
		void updateNetworkcardStatus();

	QWidget *consoleWidget;

	private:
		Process* shell;
		bool isValidIP(QString ip);
		QString toValidIP(QString ip);
	
	public slots:

		virtual void loadWidget(int);
		virtual void back();

		virtual void save();
		virtual void hasChanged();

		virtual void getNetworkcardSettings();

		virtual void getNetworkcards();
		virtual void showNetworkcardInfo();
		virtual void enableNetworkcard();
		virtual void disableNetworkcard();

		virtual void classHelp();
		virtual void methodHelp();
		virtual void enableStaticFrame();

		virtual void securityHelp();
		virtual void controlKeyLength();
		virtual void enableSecurityLineEdit();

		virtual void getWlanNetworks();
		virtual void showWlanNetworks();

		virtual void transmitWlanNetwork();

		virtual void getHostname();
		virtual void setHostname();

		virtual void getNameservers();
		virtual void setNameservers();

		virtual void ndiswrapper();
		virtual void ndiswrapperList();

		virtual void fwInstall();
		virtual void fwDetect();

		virtual void terminateConsole();

	signals:
		void menuLocked(bool);

};

#endif

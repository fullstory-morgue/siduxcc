/*
 * wlan.h
 *
 * Copyright (c) 2009 Fabian Wuertz <xadras@sidux.com>
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


#ifndef WLAN_H
#define WLAN_H

#include "ui_wlan.h"
#include "settings.h"


class wlan : public KDialog, Ui::WlanWidget
{
	Q_OBJECT

	public:
		wlan(QWidget* parent = 0, QString password = "", QString device = "");

	private slots:
		void loadSettings();
		void saveSettings();
		void showNetworks();
		void checkKey();
		void securityToggled(int);

	private:
		QString readProcess(QString, QStringList);
		QString Password;
		QString Device;
		SettingManager *settingManager;


};

#endif

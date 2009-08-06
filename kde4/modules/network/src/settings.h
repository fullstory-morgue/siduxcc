/*
 * settings.h
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>

class SettingManager;

class SettingManager
{
	public:
		SettingManager(QString = 0, QString = 0);
		QString getValue(QString);
		QStringList getSettings();
		void setValue(QString attribute, QString value);
		void loadSettings(); 
		void saveSettings();


	private:
		QStringList readFile(QString);
		QString Password;
		QString Device;
		QStringList Settings;
		QString PreSettings;
		QString PostSettings;

};

#endif

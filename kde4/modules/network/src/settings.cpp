/*
 * edu.cpp
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


#include "settings.h"
#include <kdesu/su.h>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <cstdlib>
#include <QRegExp>


SettingManager::SettingManager(QString password, QString device)
{
	  Password = password;
	  Device   = device;
}




void SettingManager::loadSettings()
{
	  // unlock etc/network/interfaces
	  KDESu::SuProcess unlockInterfaces( QByteArray( "root" ), QByteArray("chmod ").append("o+rw ").append("/etc/network/interfaces") );
	  unlockInterfaces.exec( Password.toLocal8Bit() );

	  // read networksettings from "/etc/network/interfaces"
	  QStringList configurations = readFile("/etc/network/interfaces");

	  // read interface settings
	  QString pos = "pre";
	  foreach (QString c, configurations ) {

		  if( pos != "settings") {
			  if( c.contains("iface "+Device) or c.contains( "auto "+Device) or c.contains( "allow-hotplug "+Device ) )
					  pos = "settings";
		  }
		  else {
			  if( !c.contains(Device))
				  if( c.contains(QRegExp("^iface")) or c.contains(QRegExp("^auto")) or c.contains(QRegExp("^allow-hotplug"))  )
					  pos = "post";
		  }


		  if( pos == "pre" )
			  PreSettings += c;
		  else if( pos == "settings" )
			  Settings += c;
		  else if( pos == "post" )
			  PostSettings += c;
	  }

	  Settings.replaceInStrings("\t", "");
	  Settings.replaceInStrings("\n", "");
	  Settings.replaceInStrings(QRegExp("^\\s*"), "");
	  Settings.replaceInStrings("iface "+Device+" inet", "method");
	  Settings.replaceInStrings("allow-hotplug "+Device, "class allow-hotplug");
		Settings.replaceInStrings("auto "+Device, "class auto");


	  // lock etc/network/interfaces
	  KDESu::SuProcess lockInterfaces( QByteArray( "root" ), QByteArray("chmod ").append("o-rw ").append("/etc/network/interfaces") );
	  lockInterfaces.exec( Password.toLocal8Bit() );


}

QStringList SettingManager::getSettings(){
	return Settings;
}


QString SettingManager::getValue(QString input)
{
	QString output;
        if( Settings.filter(QRegExp("^"+input)).count() > 0 )
                output =  Settings.filter(input)[0].split(" ")[1];
	return output;
}


void SettingManager::saveSettings() {

	// example
	// allow-hotplug eth1
	// iface eth1 inet static
	//   address 10.0.0.1
	//   broadcast 10.0.0.255
	//   netmask 255.255.255.0
	//   network 10.0.0.0 


	// transform settings format
	for( int i = 0; i < Settings.count(); i++) {
		QString attribut ,value;
			  if( Settings[i].split(" ").count() > 0 )
					  attribut = Settings[i].split(" ")[0];
			  if( Settings[i].split(" ").count() > 1 )
					  value    = Settings[i].split(" ")[1];

		if( Settings[i] == "") {
			Settings.removeAt(i);
			i--;
		}
		else if( attribut == "class" )
			Settings[i] = value+" "+Device;
		else if( attribut == "method" )
			Settings[i] = "iface "+Device+" inet "+value;
		else
			Settings[i] = "\t"+attribut+" "+value;
	}


	// unlock /etc/network/interfaces
	KDESu::SuProcess unlockInterfaces( QByteArray( "root" ), QByteArray("chmod ").append("o+rw ").append("/etc/network/interfaces") );
	unlockInterfaces.exec( Password.toLocal8Bit() );

	// write "/etc/network/interfaces"
	QFile file("/etc/network/interfaces");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file);
        out << PreSettings << Settings.join("\n") << "\n\n" << PostSettings;
	file.close();


	// lock etc/network/interfaces
	KDESu::SuProcess lockInterfaces( QByteArray( "root" ), QByteArray("chmod ").append("o-rw ").append("/etc/network/interfaces") );
	lockInterfaces.exec( Password.toLocal8Bit() );

	KDESu::SuProcess ifdown( QByteArray( "root" ), QByteArray("ifdown ").append(Device.toLocal8Bit()));
	ifdown.exec( Password.toLocal8Bit() );

	KDESu::SuProcess ifup( QByteArray( "root" ), QByteArray("ifup ").append(Device.toLocal8Bit()));
	ifup.exec( Password.toLocal8Bit() );


}

void SettingManager::setValue(QString attribute, QString value)
{
	int i = Settings.indexOf(QRegExp("^"+attribute+".*"));
        if( i == -1  ) {
            if( value != "..." and value != "") {
                if( attribute == "class")
                    Settings.prepend(attribute+" "+value);
                else
                     Settings.append(attribute+" "+value);
            }
        }
        else {
            if( value == "..." or value == "" )
                Settings.removeAt(i);
            else
                Settings[i] = attribute+" "+value;
        }

}

//------------------------------------------------------------------------------
//-- read ----------------------------------------------------------------------
//------------------------------------------------------------------------------


QStringList SettingManager::readFile(QString filename)
{
        QStringList output;
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return output;
        while (!file.atEnd())
                output += QString(file.readLine());
       return output;
}

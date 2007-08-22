/*
 * siduxcc_hardware.h
 *
 * Copyright (c) 2007 Fabian Wuertz
 *
 * This program is free hardware; you can redistribute it and/or modify
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

#ifndef SIDUXCC_HARDWARE_H_
#define SIDUXCC_HARDWARE_H_

#include "hardwaredialog.h"
#include "../libsiduxcc/process.h"

#include <kde_terminal_interface.h>
#include <kparts/part.h>


class siduxcc_hardware : public HardwareDialog
{
	Q_OBJECT

	public:
		siduxcc_hardware(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		void showDevices();

		ExtTerminalInterface *terminal()
		{
			return static_cast<ExtTerminalInterface*>(konsole->qt_cast( "ExtTerminalInterface" ) );
		}

		virtual bool eventFilter( QObject *o, QEvent *e );

	
	private:
		Process* shell;
	
	public slots:
		virtual void install();
		virtual void back();

	protected:
		void loadKonsole();
		KParts::Part *konsole;

};

#endif

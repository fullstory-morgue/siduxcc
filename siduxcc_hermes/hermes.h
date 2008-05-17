/*
 * hermes.h
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


#ifndef HERMES_H
#define HERMES_H

#include "hermesbase.h"

#include "../libsiduxcc/process.h"

#include <kde_terminal_interface.h>
#include <kparts/part.h>

class hermes : public hermesBase
{
	Q_OBJECT

	public:
		hermes(QWidget* parent = 0, const char* name = 0);
		void getPackages();
		void getNews();
		void getSmxiAlerts();
		void getKernels();
		QStringList newslink;
		QStringList warninglink;
		virtual void init(int i);
		// console
		ExtTerminalInterface *terminal()
		{
			return static_cast<ExtTerminalInterface*>(konsole->qt_cast( "ExtTerminalInterface" ) );
		}
		virtual bool eventFilter( QObject *o, QEvent *e );

	private:
		Process* shell;

	public slots:
		virtual void tabChanged();
		virtual void update();
		virtual void download();
		virtual void terminateConsole();
		virtual void showWarning(int);
		virtual void showNews(int);
		virtual void siduxcc();

	protected slots:
		void loadKonsole();
		KParts::Part *konsole;

};

#endif

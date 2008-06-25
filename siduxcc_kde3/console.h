/***************************************************************************
 *   Copyright (C) 2007 by Fabian Wuertz   *
 *   xadras@sidux.com   *
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


#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "consoledialog.h"

#include <kde_terminal_interface.h>
#include <kparts/part.h>

class console : public ConsoleDialog
{
    Q_OBJECT

	public:
		console(QWidget *parent = 0L, const QStrList &run = QStrList(), const char *name = 0L, const QStringList &foo = QStringList());
		// console
		ExtTerminalInterface *terminal()
		{
			return static_cast<ExtTerminalInterface*>(konsole->qt_cast( "ExtTerminalInterface" ) );
		}
		virtual bool eventFilter( QObject *o, QEvent *e );

		int progress;
		bool processDone;

	public slots:
		virtual void finish();
		virtual void runProgressBar();
		virtual void shellOutput(const QString&);
		virtual void details();

	protected slots:
		void loadKonsole();
		KParts::Part *konsole;

	signals:
		void finished(bool);


};

#endif


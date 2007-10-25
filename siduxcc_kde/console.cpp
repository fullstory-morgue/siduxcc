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



#include <kgenericfactory.h>
#include <kpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qlistview.h>
#include <qtextedit.h>

#include <qlabel.h>
#include <qpushbutton.h>

#include "console.h"

console::console(QWidget *parent, const QStrList &run, const char *name, const QStringList &)
: Widget(parent,name)
{
	//label->setText( name );
	loadKonsole();
	konsoleFrame->installEventFilter( this );

	// run command
	terminal()->startProgram( "siduxcc", run );
	connect( konsole, SIGNAL(destroyed()), this, SLOT( finish() ) );

}




//------------------------------------------------------------------------------
//--- load console -------------------------------------------------------------
//------------------------------------------------------------------------------

void console::loadKonsole()
{
	KLibFactory* factory = KLibLoader::self()->factory( "libsanekonsolepart" );
	if (!factory)
		factory = KLibLoader::self()->factory( "libkonsolepart" );
	konsole = static_cast<KParts::Part*>( factory->create( konsoleFrame, "konsolepart", "QObject", "KParts::ReadOnlyPart" ) );
	terminal()->setAutoDestroy( true );
	terminal()->setAutoStartShell( false );
	konsole->widget()->setGeometry(0, 0, konsoleFrame->width(), konsoleFrame->height());	
}

bool console::eventFilter( QObject *o, QEvent *e )
{
	// This function makes the console emulator expanding
	if ( e->type() == QEvent::Resize )
	{
		QResizeEvent *re = dynamic_cast< QResizeEvent * >( e );
		if ( !re ) return false;
		konsole->widget()->setGeometry( 0, 0, re->size().width(), re->size().height() );
	}
	return false;
};

void console::finish()
{
	emit finished(true);
}



#include "console.moc"


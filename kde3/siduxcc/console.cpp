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

#include <qlistbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qlistview.h>
#include <qtextedit.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qwidgetstack.h>

#include "console.h"

console::console(QWidget *parent, const QStrList &run, const char *name, const QStringList &)
:ConsoleDialog(parent,name)
{
	//QStrList run2; run.append( "siduxcc" );

	borderFrame->hide();
	borderFrame->setGeometry( borderFrame->x(), borderFrame->y(), width()-8, 100 );
	konsoleFrame->setGeometry( konsoleFrame->x(), konsoleFrame->y(), width()-8, 100 );
	pFrame->setGeometry( pFrame->x(), pFrame->y(), pFrame->width(), progressWidgetStack->height()-8 );
	//konsoleFrame->setGeometry( 0, 0, 500, 400);

	processDone = FALSE;
	progress = 5;
	runProgressBar();
	loadKonsole();
	konsoleFrame->installEventFilter( this );

	// run command
	terminal()->startProgram( "siduxcc", run );
	connect( konsole, SIGNAL( receivedData( const QString& ) ), SLOT( shellOutput( const QString&) ) );
	connect( konsole, SIGNAL(destroyed()), this, SLOT( finish() ) );

}

void console::runProgressBar()
{
	if( processDone )
		progressWidgetStack->raiseWidget(1);
	else
	{
		if( pFrame->x()+pFrame->width()+5+4 > progressWidgetStack->width() and progress > 0)
			progress = -5;
		else if( pFrame->x()-5-4 < 0 and progress < 0)
			progress =  5;
	
		pFrame->setGeometry( pFrame->x()+progress, pFrame->y(), pFrame->width(), progressWidgetStack->height()-10 );
	
		QTimer *timer = new QTimer();
		connect ( timer, SIGNAL ( timeout() ), this, SLOT ( runProgressBar() ) );
		timer->start ( 50, TRUE ); // 2 sek single-shot timer
	}
}


void console::shellOutput(const QString& txt)
{
	if( txt.contains("Process done!") )
	{
	 	processDone = TRUE;
		continuoPushButton->setEnabled(TRUE);
		QApplication::beep();
	}

}


void console::details()
{
	if(borderFrame->isShown() )
	{
		borderFrame->hide();
		detailsPushButton->setText( i18n("Show details") );
	}
	else
	{
		borderFrame->show();
		detailsPushButton->setText( i18n("Hide details") );
	}
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


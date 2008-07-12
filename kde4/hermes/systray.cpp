#include <QtGui>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QTimer>

 #include "systray.h"



 SysTray::SysTray()
 {
 
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon( QIcon() );
	trayIcon->show();
	
	updateIcon();

 }

void SysTray::updateIcon()
{

	QProcess exec;
	//exec.start("siduxcc software duWarnings");
	exec.start("/home/wuertz/hermes.sh");
	exec.waitForFinished();
	QString result = QString( exec.readAll() );

	if ( result == "disconnected\n" )
	{
		trayIcon->setIcon( QIcon("/usr/share/siduxcc/icons/notok2.png") );
	}
	else
	{
		if ( result == "" )
		{
			trayIcon->setIcon( QIcon("/usr/share/siduxcc/icons/ok-hermes.svg") );
		}
		else
		{
			trayIcon->setIcon( QIcon("/usr/share/siduxcc/icons/notok-hermes.png") );
		}
	}

	trayIcon->setToolTip( result );
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateIcon()));
	timer->start(5000);
	
}


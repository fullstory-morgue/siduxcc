/*
 * siduxcc_hardware.cpp
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

#include <kgenericfactory.h>
#include <kpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwidgetstack.h>

#include "siduxcc_hardware.h"


typedef KGenericFactory<siduxcc_hardware, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_hardware, ModuleFactory("siduxcchardware") )

siduxcc_hardware::siduxcc_hardware(QWidget *parent, const char *name, const QStringList &)
:HardwareDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() == 0)
	{
		// Root-Input-Widgets
		installButton->setEnabled(true);
	}

}


void siduxcc_hardware::load()
{
	loadKonsole();
	konsoleFrame->installEventFilter( this );
	showDevices();
}


//------------------------------------------------------------------------------
// showDevices

void siduxcc_hardware::showDevices()
{
	// show firmware
	QPixmap hardwareImg("/usr/share/icons/hicolor/32x32/apps/siduxcc_hardware.png");
	hardwareList->clear();
	if(comboBox->currentText() == i18n("detected"))
	{ 
		this->shell->setCommand("siduxcc hardware detect");
	}
	else
	{
		this->shell->setCommand("siduxcc hardware allDevices");
	}
	this->shell->start(true);
	QStringList hardwareName = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for(int i = 0; i < hardwareName.count(); i++)
	{
		hardwareList->insertItem(hardwareImg,hardwareName[i]);
	}
}


//------------------------------------------------------------------------------
// load console


void siduxcc_hardware::loadKonsole()
{
	KLibFactory* factory = KLibLoader::self()->factory( "libsanekonsolepart" );
	if (!factory)
		factory = KLibLoader::self()->factory( "libkonsolepart" );
	konsole = static_cast<KParts::Part*>( factory->create( konsoleFrame, "konsolepart", "QObject", "KParts::ReadOnlyPart" ) );
	terminal()->setAutoDestroy( true );
	terminal()->setAutoStartShell( false );
	konsole->widget()->setGeometry(0, 0, konsoleFrame->width(), konsoleFrame->height());	
}


bool siduxcc_hardware::eventFilter( QObject *o, QEvent *e )
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
//------------------------------------------------------------------------------
// install drivers


void siduxcc_hardware::install()
{
	// add non-free sources to /etc/apt/sources.list
	this->shell->setCommand("siduxcc hardware addSources");
	this->shell->start(true);

	// change widget
	widgetStack->raiseWidget(1);

	// run command
	QStrList run; run.append( "siduxcc" ); 
		run.append( "hardware" );
		run.append( "install" );
		run.append( hardwareList->currentText() );
	terminal()->startProgram( "siduxcc", run );

	 connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );

	// use a eternal console window
	// this->shell->setCommand("konsole -T \"sidux-Control-Center\" --nomenubar --notabbar -e siduxcc hardware install "+hardwareList->currentText());
	// this->shell->start(true);
}


void siduxcc_hardware::back()
{
	widgetStack->raiseWidget(0);
	load();
}

#include "siduxcc_hardware.moc"

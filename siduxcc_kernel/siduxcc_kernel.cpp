/*
 * siduxcc_kernel.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
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

#include <kgenericfactory.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <kmessagebox.h>
#include <qtextbrowser.h>
#include <qprogressbar.h>
#include <kprocess.h>
#include <qwidgetstack.h>
#include <qcombobox.h>

#include "siduxcc_kernel.h"

typedef KGenericFactory<siduxcc_kernel, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_kernel, ModuleFactory("siduxcckernel") )

siduxcc_kernel::siduxcc_kernel(QWidget *parent, const char *name, const QStringList &)
:KernelDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	
	// root access
	if (getuid() == 0)
	{
		removeButton->setEnabled(true);
	}

	load();
}

void siduxcc_kernel::load()
{
	getCurrentKernel();
	getNewKernels();
	getOldKernels();

	loadKonsole();
	konsoleFrame->installEventFilter( this );
}


//------------------------------------------------------------------------------
// get Kernels

void siduxcc_kernel::getCurrentKernel()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	currentKernel->setText(this->shell->getBuffer().stripWhiteSpace());
}

void siduxcc_kernel::getNewKernels()
{

	//QPixmap kernelImg("/usr/share/icons/hicolor/32x32/apps/siduxcc_hardware.png");
	QPixmap kernelImg("/usr/share/siduxcc/images/spacer.png");
	installList->clear();

	if(comboBox->currentText() == i18n("default"))
	{ 
		this->shell->setCommand("siduxcc kernel newestKernel");
		this->shell->start(true);
	}
	else
	{
		this->shell->setCommand("siduxcc kernel experimentalKernel");
		this->shell->start(true);
	}
	QString selectedKernel= this->shell->getBuffer().stripWhiteSpace();
	if(selectedKernel != "")
	{
		installList->insertItem(kernelImg,selectedKernel);
		if(getuid() == 0) // root access
		{
			installButton->setEnabled(true);
		}
	}
	else
	{
		installButton->setEnabled(false);
	}


}

void siduxcc_kernel::getOldKernels()
{
	removeList->clear();
	QPixmap kernelImg("/usr/share/siduxcc/images/spacer.png");
	this->shell->setCommand("siduxcc kernel getOldKernels");
	this->shell->start(true);
	QStringList oldKernels = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for(int i = 0; i < oldKernels.count(); i++) {
		removeList->insertItem(kernelImg,oldKernels[i]); }
}

//------------------------------------------------------------------------------
// load console

bool siduxcc_kernel::eventFilter( QObject *o, QEvent *e )
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

void siduxcc_kernel::loadKonsole()
{
	KLibFactory* factory = KLibLoader::self()->factory( "libsanekonsolepart" );
	if (!factory)
		factory = KLibLoader::self()->factory( "libkonsolepart" );
	konsole = static_cast<KParts::Part*>( factory->create( konsoleFrame, "konsolepart", "QObject", "KParts::ReadOnlyPart" ) );
	terminal()->setAutoDestroy( true );
	terminal()->setAutoStartShell( false );
	konsole->widget()->setGeometry(0, 0, konsoleFrame->width(), konsoleFrame->height());	
	konsole->widget()->setFocus();
}



//------------------------------------------------------------------------------
// kernel-installation

void siduxcc_kernel::install()
{

	if (installList->currentText() == currentKernel->text() )
	{
		KMessageBox::information(this, i18n("The versions of the new and the current Kernel are the same!") );
	}
	else
	{

		if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
		{
			KMessageBox::information(this, i18n("Please don't close the window or press the Ok/Cancel button, before it's written, that the  process is done!") );	

			// change widget
			widgetStack->raiseWidget(1);
	
			// run command
			QStrList run; run.append( "siduxcc" ); 
				run.append( "kernel" );
				run.append( "installKernel" );
				run.append( installList->currentText() );
				if(comboBox->currentText() == i18n("experimental"))
				{
					run.append( "exp" );
				}
			terminal()->startProgram( "siduxcc", run );
	
			connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );
	
		}

	}
}

void siduxcc_kernel::back()
{
	widgetStack->raiseWidget(0);
	load();
}



//------------------------------------------------------------------------------
// kernel-remover

void siduxcc_kernel::remove()
{
	KMessageBox::information(this, i18n("Please don't close the window or press the Ok/Cancel button, before it's written, that the  process is done!") );	

	// change widget
	widgetStack->raiseWidget(1);

	// run command
	QStrList run; run.append( "siduxcc" ); 
		run.append( "kernel" );
		run.append( "removeKernel" );
		run.append( removeList->currentText() );
	terminal()->startProgram( "siduxcc", run );
			
	connect( konsole, SIGNAL(destroyed()), SLOT( back() ) );

	//this->shell->setCommand("kernel-remover -x "+removeList->currentText());
	//this->shell->start(true);
	//KMessageBox::information(this, removeList->currentText()+" "+i18n("removed")+"" );
	//getOldKernels();
}


//------------------------------------------------------------------------------
// END

#include "siduxcc_kernel.moc"

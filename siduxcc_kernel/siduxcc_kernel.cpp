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
#include <qtabwidget.h>

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
		installPushButton2->setEnabled(true);
		clearPushButton->setEnabled(true);
	}

	load();
}

void siduxcc_kernel::load()
{
	//currentKernel->setText( getCurrentKernel() );

	getKernels();
	getOldKernels();
	getKernelDirs();

	loadKonsole();
	konsoleFrame->installEventFilter( this );
}


//------------------------------------------------------------------------------
// get Kernels

QString siduxcc_kernel::getCurrentKernel()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	return this->shell->getBuffer().stripWhiteSpace();
}

void siduxcc_kernel::getKernels()
{

	//QPixmap kernelImg("/usr/share/icons/hicolor/32x32/apps/siduxcc_hardware.png");
	QPixmap kernelImg("/usr/share/siduxcc/images/spacer.png");

	installListBox->clear();

	// get currentKernel
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	installListBox->insertItem(kernelImg, this->shell->getBuffer().stripWhiteSpace()+" ("+i18n("local kernel")+")");

	// get newestKernel
	this->shell->setCommand("siduxcc kernel newestKernel");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() != "" )
		installListBox->insertItem(kernelImg, this->shell->getBuffer().stripWhiteSpace()+" ("+i18n("newest")+")");

	// get newestKernel
	this->shell->setCommand("siduxcc kernel experimentalKernel");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() != "" )
		installListBox->insertItem(kernelImg, this->shell->getBuffer().stripWhiteSpace()+" ("+i18n("experimental")+")");


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

void siduxcc_kernel::enableInstallButton()
{
	int i = installListBox->currentItem();

	if (getuid() == 0 && i > 0)
		installPushButton1->setEnabled(true);
	else
		installPushButton1->setEnabled(false);
}

void siduxcc_kernel::install()
{

	installKernel =  QStringList::split( " ", installListBox->currentText() )[0];

	if (installKernel == getCurrentKernel() )
	{
		KMessageBox::information(this, i18n("The versions of the new and the current Kernel are the same!") );
		return;
	}


	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
	{
		KMessageBox::information(this, i18n("Please don't close the window or press the Ok/Cancel button, before it's written, that the  process is done!") );	

		// change widget
		widgetStack->raiseWidget(1);

		// run command
		QStrList run; run.append( "siduxcc" ); 
			run.append( "kernel" );
			run.append( "installKernel" );
			run.append( installKernel );
			if( QStringList::split( " ", installListBox->currentText() )[1] == "("+i18n("experimental")+")" )
			{
				run.append( "exp" );
			}
		terminal()->startProgram( "siduxcc", run );

	

		connect( konsole, SIGNAL(destroyed()), SLOT( back1() ) );

	}

}


void siduxcc_kernel::back1()
{
	widgetStack->raiseWidget(0);
	tabWidget->setCurrentPage(2);
	load();
	comboBox->setCurrentText("kernel-"+installKernel);
}


//------------------------------------------------------------------------------
// kernelModules


void siduxcc_kernel::getKernelDirs()
{

	comboBox->clear();
	this->shell->setCommand("siduxcc kernel getKernelDirs");
	this->shell->start(true);
	comboBox->insertStringList( QStringList::split( "/\n", this->shell->getBuffer() ) );
	showModules(comboBox->currentText() );

}

void siduxcc_kernel::showModules(const QString& kernel)
{

	modulesListBox->clear();
	this->shell->setCommand("ls /usr/src/kernel-downloads/"+kernel+"/*.deb  | cut -f6 -d/" );
	this->shell->start(true);
	QStringList mods = QStringList::split( "\n", this->shell->getBuffer() );
	QPixmap okImg = QPixmap( "/usr/share/siduxcc/images/ok.png");
	QPixmap notokImg = QPixmap( "/usr/share/siduxcc/images/notok.png");
	QStringList tmp;
	for(int i = 0; i < mods.count(); i++)
	{
		tmp = QStringList::split( "_", mods[i] );
		if(isInstalled(tmp[0]))
			modulesListBox->insertItem(okImg,mods[i]);
		else	
			modulesListBox->insertItem(notokImg,mods[i]);
	}
}

void siduxcc_kernel::installModules()
{

	loadKonsole();
	konsoleFrame->installEventFilter( this );

	QStrList run;
	run.append( "siduxcc" );
	run.append( "kernel" );
	run.append( "installModules" );

	for(int i = 0; i < modulesListBox->count(); i++)
	{
		if ( modulesListBox->isSelected(i) )
		{
			run.append( comboBox->currentText()+"/"+modulesListBox->text(i) );
		}
	}

	//if (run.count() > 4) return;

	// change widget
	widgetStack->raiseWidget(1);

	// run command
	terminal()->startProgram( "siduxcc", run );

	connect( konsole, SIGNAL(destroyed()), SLOT( back2() ) );
}


void siduxcc_kernel::clear()
{

	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to remove the stored kernel files?") ) )
	{
		this->shell->setCommand("rm -rf /usr/src/kernel-downloads/"+comboBox->currentText() );
		this->shell->start(true);
		this->shell->setCommand("rm -f /usr/src/kernel-downloads/"+comboBox->currentText()+".zip" );
		this->shell->start(true);
		getKernelDirs();
	}

}

void siduxcc_kernel::back2()
{
	widgetStack->raiseWidget(0);
	tabWidget->setCurrentPage(2);
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
			
	connect( konsole, SIGNAL(destroyed()), SLOT( back1() ) );

	//this->shell->setCommand("kernel-remover -x "+removeList->currentText());
	//this->shell->start(true);
	//KMessageBox::information(this, removeList->currentText()+" "+i18n("removed")+"" );
	//getOldKernels();
}

//------------------------------------------------------------------------------
// functions


bool siduxcc_kernel::isInstalled(QString package)
{
	this->shell->setCommand("LANG=C apt-cache policy "+package+" | grep Installed | gawk '{print $2}'");
	this->shell->start(true);

	if(this->shell->getBuffer().stripWhiteSpace() == "(none)"  || this->shell->getBuffer().stripWhiteSpace() == "" )
		return FALSE;
	else
		return TRUE;
}


//------------------------------------------------------------------------------
// END

#include "siduxcc_kernel.moc"

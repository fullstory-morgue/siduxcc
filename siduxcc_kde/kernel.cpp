/*
 * kernel.cpp
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

#include "kernel.h"
#include "console.h"


kernel::kernel(QWidget *parent, const char *name, const QStringList &)
:KernelDialog(parent, name)
{
	this->shell = new Process();
	load();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::load()
{
	getKernels();
	getOldKernels();
	getKernelDirs();
}


void kernel::loadWidget(int i)
{
	widgetStack2->raiseWidget(i+1);
	widgetStack3->raiseWidget(1);
}


void kernel::back()
{
	widgetStack2->raiseWidget(0);
	widgetStack3->raiseWidget(0);
}


//------------------------------------------------------------------------------
//--- get Kernels --------------------------------------------------------------
//------------------------------------------------------------------------------


QString kernel::getCurrentKernel()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	return this->shell->getBuffer().stripWhiteSpace();
}


void kernel::getKernels()
{

	QPixmap kernelImg("/usr/share/siduxcc/icons/spacer.png");

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


void kernel::getOldKernels()
{
	removeList->clear();
	QPixmap kernelImg("/usr/share/siduxcc/icons/spacer.png");
	this->shell->setCommand("siduxcc kernel getOldKernels");
	this->shell->start(true);
	QStringList oldKernels = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for(int i = 0; i < oldKernels.count(); i++) {
		removeList->insertItem(kernelImg,oldKernels[i]); }
}


//------------------------------------------------------------------------------
//--- kernel-installation ------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::enableInstallButton()
{
	int i = installListBox->currentItem();

	if (i > 0)
		installPushButton1->setEnabled(true);
	else
		installPushButton1->setEnabled(false);
}

void kernel::install()
{
	if (installKernel == getCurrentKernel() )
	{
		KMessageBox::information(this, i18n("The versions of the new and the current Kernel are the same!") );
		return;
	}

	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
	{

		installKernel =  QStringList::split( " ", installListBox->currentText() )[0];
	
		QStrList run; run.append( "siduxcc" );
			run.append( "kernel" );
			run.append( "installKernel" );
			run.append( installKernel );

		if( QStringList::split( " ", installListBox->currentText() )[1] == "("+i18n("experimental")+")" )
			run.append( "exp" );
		
	
	
		// change widget
		QWidget *consoleWidget = new console(this, run );
		widgetStack2->addWidget(consoleWidget, 4);
		widgetStack2->raiseWidget(4);
		widgetStack3->raiseWidget(2);
		widgetStack2->removeWidget(consoleWidget);
	
		connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole1() ));

	}

}


//------------------------------------------------------------------------------
//--- kernel-remover -----------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::remove()
{
	QStrList run; run.append( "siduxcc" );
		run.append( "kernel" );
		run.append( "removeKernel" );
		run.append( removeList->currentText() );
	

	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 4);
	widgetStack2->raiseWidget(4);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole2() ));
}


//------------------------------------------------------------------------------
//--- kernel Modules -----------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::getKernelDirs()
{

	comboBox->clear();
	this->shell->setCommand("siduxcc kernel getKernelDirs");
	this->shell->start(true);
	comboBox->insertStringList( QStringList::split( "/\n", this->shell->getBuffer() ) );
	// set kernel-module comboBox to newest kernel
	comboBox->setCurrentItem(comboBox->count()-1);
	showModules(comboBox->currentText() );

}

void kernel::showModules(const QString& kernel)
{

	modulesListBox->clear();
	this->shell->setCommand("ls /usr/src/kernel-downloads/"+kernel+"/*.deb  | cut -f6 -d/" );
	this->shell->start(true);
	QStringList mods = QStringList::split( "\n", this->shell->getBuffer() );
	QPixmap okImg = QPixmap( "/usr/share/siduxcc/icons/ok.png");
	QPixmap notokImg = QPixmap( "/usr/share/siduxcc/icons/notok.png");
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

void kernel::installModules()
{

	QStrList run; run.append( "siduxcc" );
		run.append( "kernel" );
		run.append( "installModules" );

	for(int i = 0; i < modulesListBox->count(); i++)
	{
		if ( modulesListBox->isSelected(i) )
			run.append( comboBox->currentText()+"/"+modulesListBox->text(i) );
	}	

	if (run.count() < 4) return;

	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 4);
	widgetStack2->raiseWidget(4);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole3() ));
}


void kernel::clear()
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



//------------------------------------------------------------------------------
//--- functions ----------------------------------------------------------------
//------------------------------------------------------------------------------


bool kernel::isInstalled(QString package)
{
	this->shell->setCommand("LANG=C apt-cache policy "+package+" | grep Installed | gawk '{print $2}'");
	this->shell->start(true);

	if(this->shell->getBuffer().stripWhiteSpace() == "(none)"  || this->shell->getBuffer().stripWhiteSpace() == "" )
		return FALSE;
	else
		return TRUE;
}


//------------------------------------------------------------------------------
//--- console ------------------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::terminateConsole1()
{
	comboBox->setCurrentText("kernel-"+installKernel);
	widgetStack2->raiseWidget(3);
	widgetStack3->raiseWidget(1);
}


void kernel::terminateConsole2()
{
	widgetStack2->raiseWidget(2);
	widgetStack3->raiseWidget(1);
}

void kernel::terminateConsole3()
{
	widgetStack2->raiseWidget(3);
	widgetStack3->raiseWidget(1);
}


//------------------------------------------------------------------------------
//--- END ----------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "kernel.moc"

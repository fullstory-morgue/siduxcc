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
	load(0);
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::load(int i)
{
	if(i == 0)
		getCurrentKernel();
	else if(i == 2)
		getNewKernels();	
	else if(i == 2)
		getOldKernels();	
	else if(i == 3)
		getKernelDirs();
}


void kernel::loadWidget(int i)
{
	i = i+1;
	load(i);
	widgetStack2->raiseWidget(i);
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

/*
QString kernel::getCurrentKernel()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	return this->shell->getBuffer().stripWhiteSpace();
}
*/


void kernel::getCurrentKernel()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	currentKernel =  this->shell->getBuffer().stripWhiteSpace();

	currentKernelTextLabel1->setText(i18n("Current kernel")+": "+currentKernel);
	currentKernelTextLabel2->setText(i18n("Current kernel")+": "+currentKernel);
}


void kernel::getNewKernels()
{

	QPixmap kernelImg("/usr/share/siduxcc/icons/spacer.png");

	installListBox->clear();

	// get newestKernel
	this->shell->setCommand("siduxcc kernel newestKernel");
	this->shell->start(true);
	QString newestKernel =  this->shell->getBuffer().stripWhiteSpace();

	if(newestKernel != "" and newestKernel != currentKernel)
		installListBox->insertItem(kernelImg, newestKernel);

	// get experimentalKernel
	this->shell->setCommand("siduxcc kernel experimentalKernel");
	this->shell->start(true);
	QString experimentalKernel =  this->shell->getBuffer().stripWhiteSpace();

	if(experimentalKernel != "" and experimentalKernel != currentKernel)
		installListBox->insertItem(kernelImg, experimentalKernel+" ("+i18n("experimental")+")");


	if(newestKernel == currentKernel)
		if(experimentalKernel == "" or experimentalKernel == currentKernel)
		{
			messageTextLabel->setText("<b>"+i18n("There is no newer kernel available!")+"</b>");
			installPushButton1->setEnabled(FALSE);
		}
	if(newestKernel == "" and experimentalKernel == "")
	{
		messageTextLabel->setText("<b>"+i18n("Couldn't connect to sidux.com!")+"</b>");
		installPushButton1->setEnabled(FALSE);
	}
}


void kernel::getOldKernels()
{
	removeList->clear();
	QPixmap kernelImg("/usr/share/siduxcc/icons/spacer.png");
	this->shell->setCommand("siduxcc kernel getOldKernels");
	this->shell->start(true);
	QStringList oldKernels = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );

	if(oldKernels.count() > 0)
	{
		for ( QStringList::Iterator it = oldKernels.begin(); it != oldKernels.end(); ++it )
			removeList->insertItem(kernelImg,*it);
		removeButton->setEnabled(TRUE);
	}

		
}


//------------------------------------------------------------------------------
//--- kernel-installation ------------------------------------------------------
//------------------------------------------------------------------------------

void kernel::install()
{

	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
	{

		installKernel =  QStringList::split( " ", installListBox->currentText() )[0];
	

		QStringList run; run << "installKernel" << installKernel;
		if( QStringList::split( " ", installListBox->currentText() )[1] == "("+i18n("experimental")+")" )
			run.append( "exp" );
		startConsole(run);

	}

}


//------------------------------------------------------------------------------
//--- kernel-remover -----------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::remove()
{
		QStringList run; run << "removeKernel" << removeList->currentText();
		startConsole(run);
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
	for ( QStringList::Iterator it = mods.begin(); it != mods.end(); ++it )
	{
		tmp = QStringList::split( "_", *it);
		if(isInstalled(tmp[0]))
			modulesListBox->insertItem(okImg, *it);
		else	
			modulesListBox->insertItem(notokImg, *it);
	}


}

void kernel::installModules()
{

	QStringList run; run << "installModules";

	for(uint i = 0; i < modulesListBox->count(); i++)
		if ( modulesListBox->isSelected(i) )
			run.append( comboBox->currentText()+"/"+modulesListBox->text(i) );

	if (run.count() < 2) return;

	startConsole(run);
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


void kernel::startConsole(QStringList input)
{
	emit menuLocked(TRUE);

	QStrList run; run.append( "siduxcc" );
	run.append( "kernel" );
	for ( QStringList::Iterator it = input.begin(); it != input.end(); ++it )
		run.append(*it);

	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 6);
	widgetStack2->raiseWidget(6);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	if( input[0] == "removeKernel" )
		connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole2() ));
	else
		connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole1() ));
}


void kernel::terminateConsole1()
{
	loadWidget(2);
	emit menuLocked(FALSE);
}

void kernel::terminateConsole2()
{
	loadWidget(1);
	emit menuLocked(FALSE);
}

//------------------------------------------------------------------------------
//--- END ----------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "kernel.moc"

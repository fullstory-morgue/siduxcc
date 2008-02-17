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
#include <qfiledialog.h>
#include <qlistview.h>
#include <qheader.h>
#include <qmessagebox.h>


#include "kernel.h"
#include "console.h"


kernel::kernel(QWidget *parent, const char *name, const QStringList &)
:KernelDialog(parent, name)
{
	this->shell = new Process();

	getKernelData();
	getOldKernels();
	installModulesPushButton->hide();

}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::load(int i)
{
	
	if(i == 3)
	{
		QString kernelpackage = "linux-image-"+QStringList::split( "modules-", modulesListBox->firstItem()->text() )[1];
		if(isInstalled(kernelpackage))
			installModulesPushButton->show();
		else
		{
			QMessageBox::information( this, "kernel", i18n("You only can install modules for the newest stable kernel. But this kernel isn't installed!") );
			back();
		}
	}


}


void kernel::loadWidget(int i)
{


	if(i == 2 && modulesListBox->firstItem() != 0)
	{
		QString kernelpackage = "linux-image-"+QStringList::split( "modules-", modulesListBox->firstItem()->text() )[1];
		if(isInstalled(kernelpackage))
			installModulesPushButton->show();
		else
		{
			QMessageBox::information( this, "kernel", i18n("You only can install modules for the newest stable kernel. But this kernel isn't installed!")+kernelpackage );
			back();
			return;
		}
	}


	i = i+1;
	widgetStack2->raiseWidget(i);
	widgetStack3->raiseWidget(1);

}


void kernel::back()
{
	widgetStack2->raiseWidget(0);
	widgetStack3->raiseWidget(0);
	installModulesPushButton->hide();
}


//------------------------------------------------------------------------------
//--- get kernel data ----------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::getKernelData()
{

	this->shell->setCommand("siduxcc kernel getKernelData" );
	this->shell->start(true);
	QStringList kerneldata = QStringList::split( "\n", this->shell->getBuffer() );

	for(uint i = 5; i < kerneldata.count(); i++)
		modules.append(kerneldata[i]);
	showModules();


	installKernelListView->clear();
	installKernelListView->header()->hide();
	installKernelListView->setSorting(2);

	// show the current kernel
	QString currentKernel = kerneldata[1];
	QListViewItem * item1 = new QListViewItem( installKernelListView, 0 );
	item1->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/spacer.png" ));
	item1->setText(0, "Current kernel:");
	item1->setText(1, currentKernel );
	item1->setText(2, "a" );


	// show the newest stable kernel
	this->shell->setCommand("siduxcc kernel stableKernel");
	this->shell->start(true);
	QString stableKernel =  kerneldata[3];
	QListViewItem * item2 = new QListViewItem( installKernelListView, 0 );
	item2->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/spacer.png" ));
	item2->setText(0, i18n("Newest stable kernel:") );
	item2->setText(1, stableKernel );
	item2->setText(2, "b" );


	// show the newest experimental kernel
	QListViewItem * item3 = new QListViewItem( installKernelListView, 0 );
	item3->setPixmap(0, QPixmap("/usr/share/siduxcc/icons/spacer.png" ));
	item3->setText(0, i18n("Newest experimental kernel:")+"  ");
	item3->setText(1, "");
	item3->setText(2, "c" );






	//i18n("Couldn't connect to sidux.com!")


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

void kernel::installKernel()
{

	QString type = installKernelListView->currentItem()->text(0);

	if( type == i18n("Current kernel:") )
		QMessageBox::information( this, "kernel", i18n("Please select if you want to install the newest stable or experimental kernel!") );
	else if( type == i18n("Newest stable kernel:") )
		if( installKernelListView->currentItem()->text(1) == installKernelListView->firstChild()->text(1) )
			QMessageBox::information( this, "kernel", i18n("You have already installed the newest stable kernel!") );
		else if( installKernelListView->currentItem()->text(1) == "" )
			QMessageBox::information( this, "kernel", i18n("There is no stable kernel avaiable!") );
		else
		{
			if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
			{
				QStringList run;
				run << "installKernel" << "stable";
				startConsole(run);
			}
		}
	else
		QMessageBox::information( this, "kernel", i18n("At the moment there is no support for experimental kernels!") );


	/*
		if( installKernelListView->currentItem()->text(1) == installKernelListView->firstChild()->text(1) )
			QMessageBox::information( this, "kernel", i18n("You have alrady installed the newest experimental kernel!") );
		else if( installKernelListView->currentItem()->text(1) == "" )
			QMessageBox::information( this, "kernel", i18n("There is no experimental kernel avaiable!") );
		else
			QMessageBox::information( this, "kernel", i18n("Install Ex!") );
	*/

}


//------------------------------------------------------------------------------
//--- kernel-remover -----------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::remove()
{
	QStringList run; run << "removeKernel" << removeList->currentText();
	startConsole(run);
	getOldKernels();
}


//------------------------------------------------------------------------------
//--- kernel Modules -----------------------------------------------------------
//------------------------------------------------------------------------------



void kernel::showModules()
{
	QPixmap okImg = QPixmap( "/usr/share/siduxcc/icons/ok.png");
	QPixmap notokImg = QPixmap( "/usr/share/siduxcc/icons/notok.png");
	modulesListBox->clear();
	for ( QStringList::Iterator it = modules.begin(); it != modules.end(); ++it )
	{
		if(isInstalled(*it))
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
			run.append( modulesListBox->text(i) );

	if (run.count() < 2) return;
	startConsole(run);

}


//------------------------------------------------------------------------------
//--- functions ----------------------------------------------------------------
//------------------------------------------------------------------------------


bool kernel::isInstalled(QString package)
{
	this->shell->setCommand("apt-cache policy "+package+" | head -n 2 |tail -n 1");
	this->shell->start(true);

	if(this->shell->getBuffer().stripWhiteSpace().contains("(none)")  || this->shell->getBuffer().stripWhiteSpace() == "" )
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
	showModules();
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

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
#include <qwhatsthis.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qcheckbox.h>


#include "kernel.h"
#include "console.h"


kernel::kernel(QWidget *parent, const char *name, const QStringList &)
:KernelDialog(parent, name)
{
	this->shell = new Process();


	getKernels();
	getModules();
	getOldKernels();

	applyPushButton->setEnabled(FALSE);
	bottomFrame->hide();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------



void kernel::loadWidget(int i)
{

	if( i == 0 )
	{
		// buttons
		installModulesPushButton->hide();
		showMetapackagesCheckBox->hide();
		applyPushButton->show();
		// load
		getMetapackageStatus();
	}
	if( i == 1 )
	{
		// buttons
		installModulesPushButton->hide();
		showMetapackagesCheckBox->hide();
		applyPushButton->hide();
		// load
		getOldKernels();
	}
	else if( i == 2 )
	{
		QString kernelpackage = "linux-image-"+stableKernelFull;
		if( !isInstalled(kernelpackage) )
		{
			QMessageBox::information( this, "kernel", i18n("You only can install modules for the newest stable kernel. But this kernel isn't installed!") );
			back();
			return;
		}
		// buttons
		installModulesPushButton->show();
		showMetapackagesCheckBox->show();
		applyPushButton->hide();
		// load
		showModules();

	}

	bottomFrame->show();
	i++;
	widgetStack2->raiseWidget(i);

}


void kernel::back() 
{
	widgetStack2->raiseWidget(0);
	bottomFrame->hide();
}



//------------------------------------------------------------------------------
//--- get kernels --------------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::getKernels()
{

	// show the current kernel
	this->shell->setCommand("siduxcc kernel currentKernel");
	this->shell->start(true);
	currentKernel->setText( this->shell->getBuffer().stripWhiteSpace() );


	// show the newest stable kernel
	this->shell->setCommand("siduxcc kernel stableKernelFull");
	this->shell->start(true);
	stableKernelFull = this->shell->getBuffer().stripWhiteSpace();
	stableKernel->setText( QStringList::split( "-", stableKernelFull)[1] );


	//i18n("Couldn't connect to sidux.com!")
}


void kernel::getMetapackageStatus()
{
	this->shell->setCommand("siduxcc kernel metapackageStatus");
	this->shell->start(true);
	if( this->shell->getBuffer().stripWhiteSpace() == "enabled" )
		kernelMetapackageCheckBox->setChecked(TRUE);
	else
		kernelMetapackageCheckBox->setChecked(FALSE);
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


	if( stableKernel->text() == currentKernel->text() )
		QMessageBox::information( this, "kernel", i18n("You have already installed the newest stable kernel!") );
	else if( stableKernel->text() == "" )
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


}


//------------------------------------------------------------------------------
//--- kernel-metapackage -------------------------------------------------------
//------------------------------------------------------------------------------


void kernel::hasChanged()
{
	applyPushButton->setEnabled(TRUE);
}

void kernel::toggleKernelMetapackage()
{
	if( kernelMetapackageCheckBox->isChecked() )
		startConsole("enableMetapackage");
	else
		if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("It's recommended to not disable this function! Do you want to continue anyway?") ) )
		{
			this->shell->setCommand("siduxcc kernel disableMetapackage");
			this->shell->start(true);
			getMetapackageStatus();
		}
		else
			kernelMetapackageCheckBox->setChecked(TRUE);
	
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

void kernel::getModules()
{

	this->shell->setCommand("siduxcc kernel getModules" );
	this->shell->start(true);
	modules = QStringList::split( "\n", this->shell->getBuffer() );
	showModules();
}


void kernel::showModules()
{
	QPixmap okImg = QPixmap( "/usr/share/siduxcc/icons/ok.png");
	QPixmap notokImg = QPixmap( "/usr/share/siduxcc/icons/notok.png");
	modulesListBox->clear();

	QString postfix;
	if( showMetapackagesCheckBox->isChecked() )
	{
		QString kernelType = QStringList::split( "-", stableKernelFull )[3];
		postfix = "-modules-2.6-sidux-"+kernelType;
	}
	else
		postfix = "-modules-"+stableKernelFull;
	

	for ( QStringList::Iterator it = modules.begin(); it != modules.end(); ++it )
		if(isInstalled(*it+postfix))
			modulesListBox->insertItem(okImg, *it);
		else	
			modulesListBox->insertItem(notokImg, *it);

}

void kernel::installModules()
{

	QStringList run; run << "installModules";

	QString postfix;
	if( showMetapackagesCheckBox->isChecked() )
	{
		QString kernelType = QStringList::split( "-", stableKernelFull )[3];
		postfix = "-modules-2.6-sidux-"+kernelType;
	}
	else
		postfix = "-modules-"+stableKernelFull;

	for(uint i = 0; i < modulesListBox->count(); i++)
		if ( modulesListBox->isSelected(i) )
			run.append( modulesListBox->text(i)+postfix );

	if (run.count() < 2) return;
		startConsole(run);

}


void kernel::moduleDescription(QListBoxItem* item)
{

	QString desctiptionText;

	if(item->text() == "acer-acpi")
		desctiptionText = i18n( "acer-acpi is a driver to allow you to control various aspects of your Acer laptop hardware using the proc filesystem.  It was originally developed in order to activate the wireless LAN card under a 64-bit version of Linux, as acerhk (the previous fine solution to the problem) relied on making BIOS calls which are not allowed from a 64-bit OS.");
	else if(item->text() == "acerhk")
		desctiptionText = i18n( "This driver will give access to the special keys on notebooks of the Acer Travelmate series, which are not handled by the keyboard driver.  It also works on notebooks from other manufacturers (some Medion, Fujitsu-Siemens, ...).<br><br>It also has some other related functionality (depending on the model): - controlling LEDs (Mail, Wireless) - enable/disable wireless hardware" );
	else if(item->text() == "atl2")
		desctiptionText = i18n( "atl2 is the Linux Base Driver for the Atheros(R) L2 Fast Ethernet Adapter. The the Atheros(R) L2 Fast Ethernet Adapter is present in a few low cost Asus laptop systems, such as the Asus Eee PC." );
	else if(item->text() == "aufs")
		desctiptionText = i18n( "Aufs is a stackable unification filesystem such as Unionfs, which unifies several directories and provides a merged single directory.");
	else if(item->text() == "btrfs")
		desctiptionText = i18n( "This package provides the Driver for a snapshotting filesystem modules.");
	else if(item->text() == "et131x")
		desctiptionText = i18n( "The et131x package is also required in order to make use of these modules. Kernel source or headers are required to compile these modules.<br><br>This works with Agere Systems ET-131x PCI-E Ethernet Controller.");
	else if(item->text() == "fsam7400")
		desctiptionText = i18n( " The fsam7400 module for the Linux kernel provides support for the software RF kill switches on the Fujitsu Siemens Amilo M 7400 laptop. This package provides the source code for the fsam7400 modules.");
	else if(item->text() == "gspca")
		desctiptionText = i18n( "The gpsca video for linux (v4l) driver, provides support for webcams and digital cameras based on the spca5xx range of chips manufactured by SunPlus, Sonix, Z-star, Vimicro, Conexant, Etoms, Mars-semi, Pixart and Transvision.");
	else if(item->text() == "lzma")
		desctiptionText = i18n( "LZMA is a compression algorithm, based on the famous Lempel Ziv compression method.");
	else if(item->text() == "ndiswrapper")
		desctiptionText = i18n( "Some vendors do not release specifications of the hardware or provide a Linux driver for their wireless network cards. This project implements Windows kernel API and NDIS (Network Driver Interface Specification) API within Linux kernel. A Windows driver for wireless network card is then linked to this implementation so that the driver runs natively, as though it is in Windows, without binary emulation.");
	else if(item->text() == "omnibook")
		desctiptionText = i18n( "This package contains the loadable kernel modules for the HP OmniBooks, Pavilions, Toshiba Satellites and some other laptops manufactured by Compal Electronics, Inc as ODM.");
	else if(item->text() == "qc-usb")
		desctiptionText = i18n( "This package provides the source code for the quickcam kernel module for Logitech's QuickCam Express webcam and similar webcams.  This driver supercedes the qce-ga driver.");
	else if(item->text() == "rfswitch")
		desctiptionText = i18n( "Modules for controlling the software RF kill switch on the Averatec 5100P and Packard Bell EasyNote E5. The code may work on other laptops, but these are the only models on which it has been tested.");
	else if(item->text() == "squashfs")
		desctiptionText = i18n( "Squashfs is a highly compressed read-only filesystem for Linux.  It uses zlib compression to compress both files, inodes and directories.  Inodes in the system are very small and all blocks are packed to minimise data overhead.  Block sizes greater than 4K are supported up to a maximum of 64K.<br><br> Squashfs is intended for general read-only filesystem use, for archival use (i.e.  in cases where a .tar.gz  file may be used), and in constrained block device/memory systems (e.g.  embedded systems) where low overhead is needed.");
	else if(item->text() == "virtualbox-ose-guest")
		desctiptionText = i18n( "VirtualBox is a free PC virtualization solution allowing you to run a wide range of PC operating systems on your system.");
	else if(item->text() == "virtualbox-ose")
		desctiptionText = i18n( "VirtualBox is a free PC virtualization solution allowing you to run a wide range of PC operating systems on your system.");

	modulesTextEdit->setText("<b>"+item->text()+"</b><br><br>"+desctiptionText );

}


//------------------------------------------------------------------------------
//--- functions ----------------------------------------------------------------
//------------------------------------------------------------------------------


bool kernel::isInstalled(QString package)
{
	if( QFile::exists( "/usr/share/doc/"+package+"/copyright" ) )
		return TRUE;
	else
		return FALSE;
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
	bottomFrame->hide();
	consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 4);
	widgetStack2->raiseWidget(4);

	if( input[0] == "removeKernel" )
		ci = 1;
	else if( input[0].contains( "Metapackage" ) )
		ci = 0;
	else
		ci = 2;
	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));

}

void kernel::terminateConsole()
{
	loadWidget(ci);
	emit menuLocked(FALSE);
	applyPushButton->setEnabled(FALSE);
	widgetStack2->removeWidget(consoleWidget);
}



//------------------------------------------------------------------------------
//--- END ----------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "kernel.moc"

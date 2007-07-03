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

#include "siduxcc_kernel.h"

typedef KGenericFactory<siduxcc_kernel, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_kernel, ModuleFactory("siduxcckernel") )

siduxcc_kernel::siduxcc_kernel(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() == 0)
	{
		// Disable User-Input-Widgets
		removeList->setEnabled(true);
		manualButton->setEnabled(false);
		removeButton->setEnabled(true);
	}
}

void siduxcc_kernel::load()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	currentKernel->setText(this->shell->getBuffer().stripWhiteSpace());

	this->shell->setCommand("siduxcc kernel newestKernel");
	this->shell->start(true);
	newestKernel->setText(this->shell->getBuffer().stripWhiteSpace());

	//Get old kernels
	this->shell->setCommand("siduxcc kernel getOldKernels");
	this->shell->start(true);
	QStringList oldKernels = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for(int i = 0; i < oldKernels.count(); i++) {
		removeList->insertItem(oldKernels[i]); }
}

void siduxcc_kernel::manual()
{
	this->shell->setCommand("x-www-browser http://manual.sidux.com/"+i18n("en")+"/sys-admin-kern-upg-"+i18n("en")+".htm#kern-upgrade&");
	this->shell->start(true);
}

void siduxcc_kernel::download()
{
	this->shell->setCommand("konsole -T \"Download newest Kernel\" --nomenubar --notabbar -e siduxcc kernel downloadNewestKernel&");
	this->shell->start(true);
}

void siduxcc_kernel::remove()
{
	removeText->setText(i18n("please wait..."));
	this->shell->setCommand("kernel-remover -x "+removeList->currentText());
	this->shell->start(true);
	removeText->setText(removeList->currentText()+i18n(" removed!"));
}


#include "siduxcc_kernel.moc"

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

#include "siduxcc_kernel.h"

typedef KGenericFactory<siduxcc_kernel, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_kernel, ModuleFactory("siduxcckernel") )

siduxcc_kernel::siduxcc_kernel(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	
	if (getuid() == 0)
	{
		// Disable User-Input-Widgets
		removeList->setEnabled(true);
		removeButton->setEnabled(true);
		updateButton1->setEnabled(true);
		updateButton2->setEnabled(true);
	}

	load();
}

void siduxcc_kernel::load()
{
	this->shell->setCommand("uname -r");
	this->shell->start(true);
	currentKernel->setText(this->shell->getBuffer().stripWhiteSpace());

	this->shell->setCommand("siduxcc kernel newestKernel");
	this->shell->start(true);
	newestKernel->setText(this->shell->getBuffer().stripWhiteSpace());

	this->shell->setCommand("siduxcc kernel experimentalKernel");
	this->shell->start(true);
	experimentalKernel->setText(this->shell->getBuffer().stripWhiteSpace());

	if ( experimentalKernel->text() == "")
	{
		updateButton2->setEnabled(false);
	}

	//Get old kernels
	this->shell->setCommand("siduxcc kernel getOldKernels");
	this->shell->start(true);
	QStringList oldKernels = QStringList::split( "\n", this->shell->getBuffer().stripWhiteSpace() );
	for(int i = 0; i < oldKernels.count(); i++) {
		removeList->insertItem(oldKernels[i]); }
}


//------------------------------------------------------------------------------
// kernel-installation

void siduxcc_kernel::update1()
{
	updateKernel->setText("kernel-"+newestKernel->text());
	if (updateKernel->text() == currentKernel->text() )
	{
		KMessageBox::information(this, i18n("The versions of the new and the current Kernel are the same!") );
	}
	else
	{
		if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
		{
			experimental = false;
			download();
		}
	}
}

void siduxcc_kernel::update2()
{
	updateKernel->setText("kernel-"+experimentalKernel->text());
	if (updateKernel->text() == currentKernel->text() )
	{
		KMessageBox::information(this, i18n("The versions of the new and the current Kernel are the same!") );
	}
	else
	{
		if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to install a new kernel?") ) )
		{
			experimental = false;
			download();
		}
	}
}

void siduxcc_kernel::download()
{
	widgetStack->raiseWidget(1);
	statusBar1->setProgress(10);
	statusBar2->setProgress(10);
	statusText->setText(i18n("Download")+": "+i18n("Running")+" ...");

	KProcess* proc = new KProcess();
	*proc << "siduxcc" << "kernel" << "downloadKernel" << updateKernel->text();
	if (experimental == true) {
		*proc << "exp"; }
	proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

	connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(processExited(KProcess *)),
		this, SLOT( unzip() ));
}


void siduxcc_kernel::unzip()
{
	
	statusBar1->setProgress(60);
	statusBar2->setProgress(60);
	statusText->setText(i18n("Download")+": "+i18n("Done")+".<br>"+i18n("Unzip")+": "+i18n("Running")+" ...");

	KProcess* proc = new KProcess();
	*proc << "siduxcc" << "kernel" << "unzipKernel" << updateKernel->text();
	proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);
	connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(processExited(KProcess *)),
		this, SLOT( install() ));
}

void siduxcc_kernel::install()
{
	statusBar1->setProgress(70);
	statusBar2->setProgress(70);
	statusText->setText(i18n("Download")+": "+i18n("Done")+".<br>"+i18n("Unzip")+": "+i18n("Done")+".<br>"+i18n("Installation")+": "+i18n("Running")+" ...");

	KProcess* proc = new KProcess();
	*proc << "siduxcc" << "kernel" << "installKernel" << updateKernel->text();
	proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);
	connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(processExited(KProcess *)),
		this, SLOT( finish() ));
}

void siduxcc_kernel::finish()
{
	statusBar1->setProgress(100);
	statusBar2->setProgress(100);
	statusText->setText(i18n("Download")+": "+i18n("Done")+".<br>"+i18n("Unzip")+": "+i18n("Done")+".<br>"+i18n("Installation")+": "+i18n("Done")+".");
	KMessageBox::information(this, i18n("Installation finished.") );
	backButton1->setEnabled(true);
	updateButton1->setEnabled(false);
	updateButton2->setEnabled(false);
}

void siduxcc_kernel::getOutput(KProcess *, char *data, int len)
{
	char dst[len+1];
	memmove(dst,data,len);
	dst[len]=0;
	consoleBrowser->setText(consoleBrowser->text()+dst);

	consoleBrowser->scrollToBottom ();

}

// widget-changer
void siduxcc_kernel::details()
{
	widgetStack->raiseWidget(2);
}
void siduxcc_kernel::back1()
{
	widgetStack->raiseWidget(0);
	backButton1->setEnabled(false);
}
void siduxcc_kernel::back2()
{
	widgetStack->raiseWidget(1);
}

//------------------------------------------------------------------------------
// kernel-remover

void siduxcc_kernel::remove()
{
	this->shell->setCommand("kernel-remover -x "+removeList->currentText());
	this->shell->start(true);
	KMessageBox::information(this, removeList->currentText()+" "+i18n("removed")+"" );
}

//------------------------------------------------------------------------------
// END

#include "siduxcc_kernel.moc"

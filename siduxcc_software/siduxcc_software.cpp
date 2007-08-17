/*
 * siduxcc_software.cpp
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
#include <klistview.h>
#include <kmessagebox.h>
#include <qwidgetstack.h>
#include <qtextbrowser.h>
#include <kled.h>

#include "siduxcc_software.h"


typedef KGenericFactory<siduxcc_software, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_software, ModuleFactory("siduxccsoftware") )

siduxcc_software::siduxcc_software(QWidget *parent, const char *name, const QStringList &)
:SoftwareDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() == 0)
	{
		// Root-Input-Widgets
		updateButton->setEnabled(true);
		downloadButton->setEnabled(true);

	}
}

void siduxcc_software::load()
{
	checkASV();
	showPackages();
	warning();
}


//------------------------------------------------------------------------------
// upgradable packages


void siduxcc_software::checkASV()
{

	// check if apt-show-versions is installed
	this->shell->setCommand("siduxcc software checkASV");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() == "missing")
	{

		if (getuid() == 0)
		{
			if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Apt-show-versions is missing!")+" "+i18n("Do you want to install it?") ) )
			{
				widgetStack->raiseWidget(1);
				KProcess* proc = new KProcess();
				*proc << "siduxcc" << "software" << "installASV";
				proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);
			
				connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
					this, SLOT(getOutput(KProcess *, char *, int)));
				connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
					this, SLOT(getOutput(KProcess *, char *, int)));
				connect(proc, SIGNAL(processExited(KProcess *)),
					this, SLOT( enableBack() )  );
			}
		}
		else
		{
			KMessageBox::information(this, i18n("Apt-show-versions is missing!")+" "+i18n("Switch to the Administration to install it!") );
		}	

	}
}

void siduxcc_software::warning()
{
	// check for Dist-Upgrade warnings
	this->shell->setCommand("siduxcc software duWarnings");
	this->shell->start(true);
	if(this->shell->getBuffer().stripWhiteSpace() == "")
	{
		warningLabel->setText(i18n("There are no warnings"));
		warningLed->setColor(QColor(0x00ff00));
	}
	else 
 	{
		if(this->shell->getBuffer().stripWhiteSpace() == "disconnected")
		{
			warningLabel->setText(i18n("No internet connection!"));
			warningLed->setColor(QColor(0xffa858));
		}
		else
		{
			warningLabel->setText(i18n("Don't make a dist-Upgrade!"));
			warningLed->setColor(QColor(0xff0000));
		}
	}
}


void siduxcc_software::showPackages()
{
	this->shell->setCommand("LANG=C apt-show-versions | grep upgradeable");
	this->shell->start(true);
	QStringList upgrade = QStringList::split( "\n", this->shell->getBuffer() );

	QStringList tmp;
	for(int i = 0; i < upgrade.count(); i++) {
		QListViewItem * item = new QListViewItem( uList, 0 );
		tmp = QStringList::split( "/", upgrade[i] );
		item->setText(0,tmp[0]);
		tmp = QStringList::split( " ", upgrade[i] ); 
		item->setText(1,tmp[3]);
		item->setText(2,tmp[5]); }
}


void siduxcc_software::upgrade()
{
	KMessageBox::information(this, i18n("To run a dist-upgrade you have to init the runlevel 3. Press for this CTR+ALT+F1, login as root and type init 3. After that run the command siduxcc and choose the option Software->Dist-Upgrade") );
}


void siduxcc_software::update()
{
	widgetStack->raiseWidget(1);

	KProcess* proc = new KProcess();
	*proc << "apt-get" << "update";
	proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

	connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(processExited(KProcess *)),
		this, SLOT( enableBack() )  );
}


void siduxcc_software::download()
{
	widgetStack->raiseWidget(1);

	KProcess* proc = new KProcess();
	*proc << "apt-get" << "-dy" << "dist-upgrade";
	proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

	connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(proc, SIGNAL(processExited(KProcess *)),
		this, SLOT( enableBack() )  );
}


//------------------------------------------------------------------------------
// metapackages

void siduxcc_software::metapackages()
{
	this->shell->setCommand("su-me /usr/sbin/metapackage-installer&");
	this->shell->start(true);
}


//------------------------------------------------------------------------------
// ouput

void siduxcc_software::getOutput(KProcess *, char *data, int len)
{
	char dst[len+1];
	memmove(dst,data,len);
	dst[len]=0;
	outputBrowser->setText(outputBrowser->text()+dst);
	outputBrowser->scrollToBottom ();
}


void siduxcc_software::enableBack()
{
	backButton->setEnabled(true);
}


void siduxcc_software::back()
{
	widgetStack->raiseWidget(0);
	outputBrowser->setText("");
	showPackages();
}




#include "siduxcc_software.moc"

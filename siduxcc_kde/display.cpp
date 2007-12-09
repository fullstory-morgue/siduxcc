/*
 * display.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * display is based on knxcc_display from Andreas Loible
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


#include <qcombobox.h>
#include <kgenericfactory.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlineedit.h>
#include <qslider.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <qspinbox.h>
#include <qwidgetstack.h>
#include <qcheckbox.h>



#include <display.h>
#include <console.h>


display::display(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	load();
}


//------------------------------------------------------------------------------
//--- load ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void display::load()
{

	getDpi();
	dpiSpin->setValue(dpi);
	dpiSlider->setValue(dpi);

	getHorizSync();
	hsEdit->setText(horizsync);

	getVertRefresh();
	vrEdit->setText(vertrefresh);

	getColorDepth();
	for(int i = 0; i < colorDepthList->count(); i++)
	{
		if(colorDepthList->text(i) == colordepth+" bit")
		{
			colorDepthList->setCurrentItem(i);
			break;
		}
	}

	getResolution();
	for(int i = 0; i < resolutionList->count(); i++)
	{
		if(resolutionList->text(i) == resolution)
		{
			resolutionList->setCurrentItem(i);
			break;
		}
	}

	getDriver();

	getComposite();

	compositeSlot();

	applyPushButton->setEnabled(FALSE);
}


void display::loadWidget(int i)
{
	widgetStack2->raiseWidget(i+1);
	widgetStack3->raiseWidget(1);
}


void display::back()
{
	widgetStack2->raiseWidget(0);
	widgetStack3->raiseWidget(0);
}


//------------------------------------------------------------------------------
//--- save ---------------------------------------------------------------------
//------------------------------------------------------------------------------


void display::save()
{
	if(resolutionList->currentText() != resolution and resolutionEdit->text() != resolution)
		{setResolution();}

	if(hsEdit->text() != horizsync)
		{setHorizSync();}

	if(vrEdit->text() != vertrefresh)
		{setVertRefresh();}

	if(dpiSpin->value() != dpi)
		{setDpi();}

	if(colorDepthList->currentText() != colordepth)
		{setColorDepth();}

	if(driverComboBox->currentText() != driver )
		setDriver();

	if(compositeCheckBox->isChecked() != composite)
		setComposite();

	applyPushButton->setEnabled(FALSE);
}


void display::hasChanged()
{
	applyPushButton->setEnabled(TRUE);
}

//------------------------------------------------------------------------------
//--- Tab X-Server -------------------------------------------------------------
//------------------------------------------------------------------------------


void display::getDpi()
{
	this->shell->setCommand("siduxcc display getDpi");
	this->shell->start(true);
	dpi = this->shell->getBuffer().toInt();
}
void display::setDpi()
{
	this->shell->setCommand("siduxcc display setDpi "+QString::number(dpiSpin->value()));
	this->shell->start(true);
}

void display::getColorDepth()
{
	this->shell->setCommand("siduxcc display getColorDepth");
	this->shell->start(true);
	colordepth = this->shell->getBuffer().stripWhiteSpace();
}
void display::setColorDepth()
{
	this->shell->setCommand("siduxcc display setColorDepth "+colorDepthList->currentText());
	this->shell->start(true);
}

void display::getResolution()
{
	this->shell->setCommand("siduxcc display getResolution");
	this->shell->start(true);
	resolution = this->shell->getBuffer().stripWhiteSpace();
}
void display::setResolution()
{
	if(resolutionList->currentText() == i18n("custom"))
		{this->shell->setCommand("siduxcc display setResolution "+resolutionEdit->text());}
	else
		{this->shell->setCommand("siduxcc display setResolution "+resolutionList->currentText());}

	this->shell->start(true);
}

void display::getHorizSync()
{
	this->shell->setCommand("siduxcc display getHorizSync");
	this->shell->start(true);
	horizsync = this->shell->getBuffer().stripWhiteSpace();
}
void display::setHorizSync()
{
	this->shell->setCommand("siduxcc display setHorizSync "+hsEdit->text());
	this->shell->start(true);
}


void display::getVertRefresh()
{
	this->shell->setCommand("siduxcc display getVertRefresh");
	this->shell->start(true);
	vertrefresh = this->shell->getBuffer().stripWhiteSpace();
}
void display::setVertRefresh()
{
	this->shell->setCommand("siduxcc display setVertRefresh "+vrEdit->text());
	this->shell->start(true);
}

void display::backupX()
{
	proc << "siduxcc display backupX";
	proc.start();
}

void display::removeModelines()
{
	this->shell->setCommand("siduxcc display removeModelines");
	this->shell->start(true);
	KMessageBox::information(this, i18n("Modelines removed"));
}

void display::restartX()
{
	if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Are you sure you want to restart the X-Server? All windows will be closed.") ) )
	this->shell->setCommand("siduxcc display restartX");
	this->shell->start(true);
}

void display::customScreenSize()
{
	if(resolutionList->currentText() == i18n("custom"))
	{
		resolutionEdit->setDisabled(false);
		resolutionEdit->setText(resolution);
	}
	else
	{
		resolutionEdit->setDisabled(true);
		resolutionEdit->setText("");
	}
}

void display::monitorType()
{
	if(monitorList->currentText() == "TFT")
	{
		hsEdit->setDisabled(false);
		vrEdit->setDisabled(true);
	}
	else
	{
		if(monitorList->currentText() == "CRT")
		{
		hsEdit->setDisabled(false);
		vrEdit->setDisabled(false);
		}
		else
		{
		hsEdit->setDisabled(true);
		vrEdit->setDisabled(true);
		}
	}
}

//------------------------------------------------------------------------------
//--- Choose driver ------------------------------------------------------------
//------------------------------------------------------------------------------


void display::getDriver()
{
	this->shell->setCommand("siduxcc display getDriver");
	this->shell->start(true);
	driver = this->shell->getBuffer().stripWhiteSpace();
	bool done = FALSE;
	for(int i = 0; i < driverComboBox->count(); i++)
	{
		if(driverComboBox->text(i) == driver)
		{
			driverComboBox->setCurrentItem(i);
			done = TRUE;
			break;
		}
	}
	if( !done )
	{
		driverComboBox->insertItem( driver, driverComboBox->count() );
		driverComboBox->setCurrentItem( driverComboBox->count()-1 );
	}

	if(driver == "fglrx")
		colorDepthList->setEnabled(FALSE);
}


void display::setDriver()
{
	this->shell->setCommand("siduxcc display setDriver "+driverComboBox->currentText());
	this->shell->start(true);
	getDriver();
}

void display::compositeSlot()
{
	if(driverComboBox->currentText() != "nvidia")
	{
		compositeCheckBox->setEnabled(false);
		compositeCheckBox->setChecked(false);
	}
	else
	{
		if( getuid() == 0 )
			compositeCheckBox->setEnabled(true);
		else
			compositeCheckBox->setEnabled(false);
	}
}


void display::getComposite()
{
	this->shell->setCommand("siduxcc display getComposite");
	this->shell->start(true);
	if(this->shell->exitStatus())
	{
		compositeCheckBox->setChecked(false);
		composite = FALSE;
	}
	else
	{
		compositeCheckBox->setChecked(true);
		composite = TRUE;
	}
}

void display::setComposite()
{

	if(compositeCheckBox->isChecked())
		this->shell->setCommand("siduxcc display setComposite true");
	else
		this->shell->setCommand("siduxcc display setComposite false");

	this->shell->start(true);
	getComposite();
}

//------------------------------------------------------------------------------
//--- Install non-free Nvdia/Ati drivers ---------------------------------------
//------------------------------------------------------------------------------


void display::gfx()
{
	emit menuLocked(TRUE);

	QStrList run; run.append( "siduxcc" ); 
		run.append( "display" );
		run.append( "binaryGfx" );
	
	// change widget
	QWidget *consoleWidget = new console(this, run );
	widgetStack2->addWidget(consoleWidget, 6);
	widgetStack2->raiseWidget(6);
	widgetStack3->raiseWidget(2);
	widgetStack2->removeWidget(consoleWidget);

	connect( consoleWidget, SIGNAL( finished(bool) ), this, SLOT( terminateConsole() ));

}

void display::terminateConsole()
{
	widgetStack2->raiseWidget(4);
	widgetStack3->raiseWidget(1);
	emit menuLocked(FALSE);
}


//------------------------------------------------------------------------------
//--- Fonts --------------------------------------------------------------------
//------------------------------------------------------------------------------

void display::fixFonts()
{

	statusText->setText(i18n("Process is running!"));
	widgetStack3->raiseWidget(2);

	proc << "fix-fonts";
	proc.start(KProcess::NotifyOnExit, KProcess::AllOutput);

	connect(&proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));
	connect(&proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
		this, SLOT(getOutput(KProcess *, char *, int)));

	connect(&proc, SIGNAL(processExited(KProcess *)),
		this, SLOT( fixFonts_END() ));

}


void display::fixFonts_END()
{
	statusText->setText(i18n("Process done!"));
	widgetStack3->raiseWidget(1);
}


void display::getOutput(KProcess *, char *data, int len)
{
	char dst[len+1];
	memmove(dst,data,len);
	dst[len]=0;
	consoleText->setText(consoleText->text()+dst);
}


#include "display.moc"

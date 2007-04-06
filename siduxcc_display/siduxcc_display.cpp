/*
 * siduxcc_display.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * siduxcc_display is based on knxcc_display from Andreas Loible
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
#include <qtextbrowser.h>
#include <kled.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qslider.h>


#include <siduxcc_display.h>

typedef KGenericFactory<siduxcc_display, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_siduxcc_display, ModuleFactory("siduxccdisplay") )

siduxcc_display::siduxcc_display(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
	this->shell = new Process();
	this->setUseRootOnlyMsg(true);
	load();
	
	if (getuid() != 0)
	{
		// Disable User-Input-Widgets
		resolutionList->setDisabled(true);
		fontButton->setDisabled(true);
		mlButton->setDisabled(true);
		restartxButton->setDisabled(true);
		restartxButton2->setDisabled(true);
		dpiSlider->setDisabled(true);
		monitorList->setDisabled(true);
		colorDepthList->setDisabled(true);
	}
}

void siduxcc_display::load()
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
}

void siduxcc_display::save()
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



}


// Tab X-Server
// ------------

void siduxcc_display::getDpi()
{
	this->shell->setCommand("siduxcc display getDpi");
	this->shell->start(true);
	dpi = this->shell->getBuffer().toInt();
}
void siduxcc_display::setDpi()
{
	this->shell->setCommand("siduxcc display setDpi "+QString::number(dpiSpin->value()));
	this->shell->start(true);
}

void siduxcc_display::getColorDepth()
{
	this->shell->setCommand("siduxcc display getColorDepth");
	this->shell->start(true);
	colordepth = this->shell->getBuffer().stripWhiteSpace();
}
void siduxcc_display::setColorDepth()
{
	this->shell->setCommand("siduxcc display setColorDepth "+colorDepthList->currentText());
	this->shell->start(true);
}

void siduxcc_display::getResolution()
{
	this->shell->setCommand("siduxcc display getResolution");
	this->shell->start(true);
	resolution = this->shell->getBuffer().stripWhiteSpace();
}
void siduxcc_display::setResolution()
{
	if(resolutionList->currentText() == i18n("custom"))
		{this->shell->setCommand("siduxcc display setResolution "+resolutionEdit->text());}
	else
		{this->shell->setCommand("siduxcc display setResolution "+resolutionList->currentText());}

	this->shell->start(true);
}

void siduxcc_display::getHorizSync()
{
	this->shell->setCommand("siduxcc display getHorizSync");
	this->shell->start(true);
	horizsync = this->shell->getBuffer().stripWhiteSpace();
}
void siduxcc_display::setHorizSync()
{
	this->shell->setCommand("siduxcc display setHorizSync "+hsEdit->text());
	this->shell->start(true);
}


void siduxcc_display::getVertRefresh()
{
	this->shell->setCommand("siduxcc display getVertRefresh");
	this->shell->start(true);
	vertrefresh = this->shell->getBuffer().stripWhiteSpace();
}
void siduxcc_display::setVertRefresh()
{
	this->shell->setCommand("siduxcc display setVertRefresh "+vrEdit->text());
	this->shell->start(true);
}

void siduxcc_display::backupX()
{
	this->shell->setCommand("siduxcc display backupX");
	this->shell->start(true);
}

void siduxcc_display::removeModelines()
{
	this->shell->setCommand("siduxcc display removeModelines");
	this->shell->start(true);
	messageText->setText("Modelines removed!");
}

void siduxcc_display::restartX()
{
	this->shell->setCommand("siduxcc display restartX");
	this->shell->start(true);
}

void siduxcc_display::customScreenSize()
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

void siduxcc_display::monitorType()
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

// Tab Fonts
// ---------

void siduxcc_display::fixFonts()
{
	this->shell->setCommand("siduxcc display fixFonts&");
	this->shell->start(true);
}


void siduxcc_display::defaults(){}

#include "siduxcc_display.moc"

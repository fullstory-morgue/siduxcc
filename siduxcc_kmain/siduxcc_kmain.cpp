/*
 * siduxcc_kmain.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * siduxcc_kmain is based on knxcc_kmain from Andreas Loible
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

#include <qlineedit.h>
#include <qtextedit.h>
#include <qlistbox.h>
#include <kaboutapplication.h>


#include "siduxcc_kmain.h"

siduxcc_kmain::siduxcc_kmain(QWidget *parent, const char *name, const QStringList &)
:KmainDialog(parent, name)
{
	this->shell = new Process();
}

void siduxcc_kmain::click1()
{
	listBox2->clearSelection();
	listBox3->clearSelection();
}

void siduxcc_kmain::click2()
{
	listBox1->clearSelection();
	listBox3->clearSelection();
}

void siduxcc_kmain::click3()
{
	listBox1->clearSelection();
	listBox2->clearSelection();
}

void siduxcc_kmain::run1()
{
	if( listBox1->currentText() == i18n("Display") )
	{
		run = "su-me \"kcmshell siduxcc_display --lang "+i18n("en")+"\"&";
	}
	else
	{
		if(listBox1->currentText() == i18n("Drivers") )
		{
			run = "su-me \"kcmshell siduxcc_hardware --lang "+i18n("en")+"\"&";
			
		}
		else
		{
			if( listBox1->currentText() == i18n("Kernel") )
			{
				run = "su-me \"kcmshell siduxcc_kernel --lang "+i18n("en")+"\"&";
			}
			else
			{
				run = "kcmshell siduxcc_network";
			}
		}
	}

	this->shell->setCommand(run);
	this->shell->start(true);
}

void siduxcc_kmain::run2()
{
	if( listBox2->currentText() == i18n("Services") )
	{
		run = "su-me \"kcmshell siduxcc_services --lang "+i18n("en")+"\"&";
	}
	else
	{
		if(listBox2->currentText() == i18n("Software") )
		{
			run = "su-me \"kcmshell siduxcc_software --lang "+i18n("en")+"\"&";
		}
		else
		{
			if( listBox2->currentText() == i18n("SysInfo") )	
			{
				run = "siduxcc_info&";
			}
			else
			{
				run = "siduxcc-hermes&";
			}
		}
	}

	this->shell->setCommand(run);
	this->shell->start(true);
}


void siduxcc_kmain::run3()
{
	if( listBox3->currentText() == i18n("Metapackages") )
	{
		run = "install-meta&";
		this->shell->setCommand(run);
		this->shell->start(true);
	}
	else
	{
		if(listBox3->currentText() == i18n("Manual") )
		{
			run = "x-www-browser /usr/share/sidux-manual/index.html&";
			this->shell->setCommand(run);
			this->shell->start(true);
		}
		else
		{
			if( listBox3->currentText() == i18n("About") )
			{
				KAboutApplication* about = new KAboutApplication ( this );
				about->show();
			}
			else
			{
				close();
			}
		}
	}
}




#include "siduxcc_kmain.moc"

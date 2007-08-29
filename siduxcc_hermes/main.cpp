/*
 * main.cpp
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


#include <systray.h>
#include <hermes.h>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>
#include <ksystemtray.h>

static KCmdLineOptions options[] =
{
	KCmdLineLastOption
};

int main(int argc, char *argv[])
{
	// specify data for About dialog
	KAboutData* about = new KAboutData("siduxcchermes", "siduxcc-hermes (sidux dist-upgrade alerter)", "");
	about->setShortDescription(I18N_NOOP("A KDE tray application for sidux"));
	about->setLicense(KAboutData::License_GPL_V2);
	about->setCopyrightStatement("(c) 2007 Fabian Würtz");
	
	
	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(options);

	Hermes app;

	// start program
	kdDebug() << "creating first instance" << endl;
	SysTray* tray = new SysTray();
	app.setMainWidget(tray);
	tray->show();

	return app.exec();
}

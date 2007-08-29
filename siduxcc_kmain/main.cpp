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


#include "siduxcc_kmain.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>


static KCmdLineOptions options[] =
{
	//{ "+[URL]", I18N_NOOP( "Document to open" ), 0 },
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	// specify data for About dialog
	KAboutData* about = new KAboutData("siduxcckmain", I18N_NOOP("sidux Control Center"), "");

	about->setProgramLogo( QImage("/usr/share/icons/hicolor/32x32/apps/siduxcc.png") );
	about->setShortDescription( I18N_NOOP("Frontend for managing sidux.") );
	about->setLicense(KAboutData::License_GPL_V2);
   about->setHomepage("http://www.sidux.com");
   about->setBugAddress("xadras@sidux.com");
	about->setCopyrightStatement("(c) 2007 Fabian Würtz");

	about->addAuthor("Fabian Würtz (xadras)", I18N_NOOP("Developer"), "xadras@sidux.com", "http://xadras.wordpress.com/");

	about->addCredit("Stefan Lippers-Hollmann (slh)", I18N_NOOP("Packaging") );
	about->addCredit("Andreas Loibl (Acritox)", I18N_NOOP("Developer of the knxcc") , "andreas@andreas-loibl.de", "http://www.andreas-loibl.de/");



	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;
	siduxcc_kmain *mainWin = 0;

	if (app.isRestored())
	{
		//RESTORE(siduxcc_main);
	}
	else
	{
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

		/// @todo do something with the command line args here

		mainWin = new siduxcc_kmain();
		app.setMainWidget( mainWin );
		mainWin->show();

		args->clear();
	}

	// mainWin has WDestructiveClose flag by default, so it will delete itself.
	return app.exec();
}


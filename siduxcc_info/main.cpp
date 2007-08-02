/***************************************************************************
 *   Copyright (C) 2007 by Fabian Wuertz   *
 *   xadras@sidux.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "siduxcc_info.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] = I18N_NOOP("A KDE KPart Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
	//{ "+[URL]", I18N_NOOP( "Document to open" ), 0 },
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("siduxcc", I18N_NOOP("SysInfo"), version, description,
	KAboutData::License_GPL, "(C) 2007 Fabian Wuertz", 0, 0, "xadras@sidux.com");
	about.addAuthor( "Fabian Wuertz", 0, "xadras@sidux.com" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;
	siduxcc_info *mainWin = 0;

	if (app.isRestored())
	{
		//RESTORE(siduxcc_info);
	}
	else
	{
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

		/// @todo do something with the command line args here

		mainWin = new siduxcc_info();
		app.setMainWidget( mainWin );
		mainWin->show();

		args->clear();
	}

	// mainWin has WDestructiveClose flag by default, so it will delete itself.
	return app.exec();
}


/***************************************************************************
 *   Copyright (C) 2009 by Fabian Wuertz                                   *
 *   xadras@sidxu.com                                                      *
 *   Project: siduxccc                                                     *
 *   File:    starter.cpp                                                  *
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

#include <QLabel>
#include <QStringList>
#include <QTreeWidget>


#include <kapplication.h>
#include <kglobalsettings.h>

#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <KAboutData>
#include <KAboutApplicationDialog>
#include <KHelpMenu>
#include <klibloader.h>
#include <KService>
#include <KServiceTypeTrader>
#include <KDesktopFile>
#include <kcmoduleinfo.h>
#include <KDialogButtonBox>


#include "starter.h"

//------------------------------------------------------------------------------
//--- init ---------------------------------------------------------------------
//------------------------------------------------------------------------------


Starter::Starter()
{
	setupUi(this);

	this->setWindowIcon(KIcon("siduxcc"));
		
	KService::List offers = KServiceTypeTrader::self()->query("KCModule");


	KService::List::const_iterator iter;
	for(iter = offers.begin(); iter < offers.end(); ++iter)
	{
		QString error;
		KService::Ptr service = *iter;
      
		KDesktopFile file( "/usr/share/kde4/services/"+service->library()+".desktop" );
		KConfigGroup desktopGroup = file.desktopGroup();
		QString category = desktopGroup.readEntry("Categories");


		if (category.contains("siduxcc") )
		{


			modules.append(file.readName());
			modules2.append(service->library());


			QListWidgetItem *newItem0 = new QListWidgetItem;
			newItem0->setText(file.readName());
			newItem0->setIcon(KIcon(file.readIcon()));
			newItem0->setSizeHint(QSize(144,82));
			listWidget->insertItem(0, newItem0);
			
			  
			KPluginFactory *factory = KPluginLoader(service->library()).factory();
			infolst.append(factory->create<KCModule>());
		}
 

	}



	 KAboutData *about = new KAboutData( 
		"sidux Control Center",
		0,
		ki18n("siduxcc"),
		KDE_VERSION_STRING,
		KLocalizedString(),
		KAboutData::License_GPL,
		ki18n("Copyright 2009 Fabian Wuertz")
	);
	about->addAuthor(ki18n("Fabian Wuertz"), KLocalizedString(), "xadras@sidux.com");
	about->setProgramIconName("siduxcc");
	about->setVersion("1.0");
	about->setBugAddress("http://sidux.com");
 	aboutDialog = new KAboutApplicationDialog ( about, this );


	KHelpMenu* mHelpMenu = new KHelpMenu( this );
	connect( aboutKDE, SIGNAL(activated()), mHelpMenu, SLOT(aboutKDE()));
	aboutSiduxcc->setIcon(KIcon("siduxcc"));
	aboutKDE->setIcon(KIcon("kde"));
	quitAction->setIcon(KIcon("application-exit.png"));

// 	applyButton->setEnabled(FALSE);
// 	resetButton->setEnabled(FALSE);
	moduleStackedWidget->setEnabled(TRUE);
 	stackedWidget->setCurrentIndex(0);
}




void Starter::showModule()
{

	if ( listWidget->selectedItems().count() < 1 )
		return;


	QString module = listWidget->selectedItems().first()->text();
 	int i = modules.indexOf(module);
	titleLabel->setText(module);


	activeModule = infolst[i];
	const int buttons = activeModule->buttons();


	KDialogButtonBox *mButtons = new KDialogButtonBox( this, Qt::Horizontal );
	applyButton    = mButtons->addButton( KStandardGuiItem::apply(), QDialogButtonBox::ApplyRole );
	defaultsButton = mButtons->addButton( KStandardGuiItem::defaults(), QDialogButtonBox::ResetRole );
	resetButton    = mButtons->addButton( KStandardGuiItem::reset(), QDialogButtonBox::ResetRole );
	applyButton   ->setShown(buttons & KCModule::Apply );
	resetButton   ->setShown(buttons & KCModule::Apply );
	defaultsButton->setShown(buttons & KCModule::Default );
	

	QWidget *moduleWidget = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(activeModule);
	if(buttons & KCModule::Apply or buttons & KCModule::Default)
		layout->addWidget(mButtons);
	moduleWidget->setLayout(layout);


 
	moduleStackedWidget->removeWidget(moduleStackedWidget->currentWidget());
	moduleStackedWidget->addWidget(moduleWidget);
	

	connect( activeModule,   SIGNAL( changed(bool) ),     this, SLOT( moduleChanged(bool) ));
	connect( applyButton,    SIGNAL( pressed() ), activeModule, SLOT( save() ));
	connect( resetButton,  SIGNAL( pressed() ), activeModule, SLOT( load() ));
	connect( defaultsButton, SIGNAL( pressed() ), activeModule, SLOT( defaults() ));
	stackedWidget->setCurrentIndex(1);


	applyButton  ->setEnabled(FALSE);
	resetButton->setEnabled(FALSE);

}


void Starter::showAboutSiduxcc()
{
	aboutDialog->show();
}

void Starter::moduleChanged(bool b)
{

	applyButton->setEnabled(b);
	resetButton->setEnabled(b);
}


QWidget * Starter::getModule(QString module){

	KPluginFactory *factory = KPluginLoader(module).factory();
	return factory->create<KCModule>();

}

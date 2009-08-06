/***************************************************************************
 *   Copyright (C) 2009 by Fabian Wuertz                                   *
 *   xadras@sidxu.com                                                      *
 *   Project: siduxcc.cpp                                                  *
 *   File:    siduxcc.h                                                    *
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

#ifndef SIDUXCC_H
#define SIDUXCC_H


#include <QtGui/QtGui>
#include <QMainWindow>
#include <kcmodule.h>
#include <KUrl>
#include <KAboutApplicationDialog>
#include <KPushButton>



#include "ui_starter.h"



class Starter : public QMainWindow, Ui::Starter
{
	Q_OBJECT
	public:
		Starter();
		KAboutApplicationDialog* aboutDialog;
		QWidget* getModule(QString);
		KCModule *activeModule;


	public slots:
		void showModule();
		void showAboutSiduxcc();
		void moduleChanged(bool);

	private:
		QStringList modules, modules2;
		QList<KCModule*> infolst;
		KPushButton* applyButton;
		KPushButton* resetButton;
		KPushButton* defaultsButton;


};

#endif

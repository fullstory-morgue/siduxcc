/*
 * siduxcc_display.h
 *
 * Copyright (c) 2007 Fabian Wuertz
 * siduxcc is based on knxcc from Andreas Loible
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

#ifndef SIDUXCC_DISPLAY_H_
#define SIDUXCC_DISPLAY_H_

#include "displaydialog.h"
#include "process.h"

class siduxcc_display : public DisplayDialog
{
	Q_OBJECT

	public:
		siduxcc_display(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		void save();
		void defaults();
		void backupX();

		void getDpi();
		void setDpi();

		void getColorDepth();
		void setColorDepth();

		void getResolution();
		void setResolution();

		void getVertRefresh();
		void setVertRefresh();

		void getHorizSync();
		void setHorizSync();

		int dpi;
		QString resolution;
		QString colordepth;
		QString horizsync;
		QString vertrefresh;

	private:
		Process* shell;

	public slots:
		virtual void monitorType();
		virtual void fixFonts();
		virtual void removeModelines();
		virtual void restartX();
		virtual void customScreenSize();
};

#endif

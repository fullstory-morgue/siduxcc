/*
 * siduxcc_kmain.h
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

#ifndef SIDUXCC_KMAIN_H_
#define SIDUXCC_KMAIN_H_

#include <kmaindialog.h>
#include <../libsiduxcc/process.h>


class siduxcc_kmain : public KmainDialog
{
	Q_OBJECT

	public:
		siduxcc_kmain(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
	private:
		Process* shell;	
		QString run;

	public slots:
		virtual void click1();
		virtual void click2();
		virtual void click3();
		virtual void run1();
		virtual void run2();
		virtual void run3();


};

#endif

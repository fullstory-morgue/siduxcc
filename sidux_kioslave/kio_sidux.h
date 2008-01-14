/*
 * kio_sidux.h
 *
 * Copyright (c) 2007 Fabian Wuertz
 * kio_siduxis based on kio_resources from Luke Sandell <lasandell@gmail.com>
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

#include <kio/slavebase.h>
#include <kstandarddirs.h>
#include <qcstring.h>
#include <kurl.h>

struct ResourceInfo;

class SiduxProtocol : public KIO::SlaveBase
{
	public:
		SiduxProtocol( const QCString &pool, const QCString &app );
		virtual void put( const KURL& url, int permissions, bool overwrite, bool resume ); 
		virtual void get( const KURL & url );
		virtual void stat( const KURL & url );
		virtual void listDir( const KURL & url );
		void load();

	private:
		bool decodeURL( const KURL & url, ResourceInfo & info );
		KStandardDirs *m_dirs;

};

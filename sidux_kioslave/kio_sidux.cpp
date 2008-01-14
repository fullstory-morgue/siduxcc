/*
 * kio_sidux.cpp
 *
 * Copyright (c) 2007 Fabian Wuertz
 * kio_sidux is based on kio_resources from Luke Sandell <lasandell@gmail.com>
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



#include "kio_sidux.h"
#include <kstandarddirs.h>
#include <kurl.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <klocale.h>

using namespace KIO;


// ------------------------------------------------------------------------------------- //
// --- Atom functions ------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------- //


static UDSAtom createAtom( uint uds, long lng );
static UDSAtom createAtom( uint uds, const QString & str );
static UDSEntry createDirEntry( const QString & fileName );
static UDSEntry createEntry( const QString & path );


static UDSEntry createEntry( const QString & path )
{
	UDSEntry entry;
	UDSAtom atom;
	QFileInfo info(path);

	// Add name
	entry.append( createAtom( UDS_NAME, path.section( '/', -1 ) ) );

	// Add mode
	entry.append( createAtom( UDS_FILE_TYPE, info.isDir() ? S_IFDIR : 0 ) );

	// Add size
	entry.append( createAtom( UDS_SIZE, info.size() ) );

	// Add modification time
	entry.append( createAtom( UDS_MODIFICATION_TIME, info.lastModified().toTime_t() ) );
	entry.append( createAtom( UDS_ACCESS_TIME, info.lastRead().toTime_t() ) );
	//entry.append( createAtom( UDS_CREATION_TIME, info.created().toTime_t() ) );

	// Add mimetype
	KMimeType::Ptr pMimetype = KMimeType::findByPath( path, info.isDir() ? S_IFDIR : 0 );
	entry.append( createAtom( UDS_MIME_TYPE, pMimetype->name() ) );

	return entry;
}


static UDSEntry createDirEntry( const QString & filename)
{
	UDSEntry entry;
	entry.append( createAtom( UDS_FILE_TYPE, S_IFDIR ) );
	entry.append( createAtom( UDS_NAME, filename ) );
	return entry;
}


static UDSAtom createAtom( uint uds, long lng )
{
	UDSAtom atom;
	atom.m_uds = uds;
	atom.m_long = lng;

	return atom;
}


static UDSAtom createAtom( uint uds, const QString & str )
{
	UDSAtom atom;
	atom.m_uds = uds;
	atom.m_str = str;

	return atom;
}

static void addAtom(KIO::UDSEntry &entry, unsigned int ID, long l,
                    const QString &s = QString::null)
{
	KIO::UDSAtom atom;
	atom.m_uds = ID;
	atom.m_long = l;
	atom.m_str = s;
	entry.append(atom);
}

// ------------------------------------------------------------------------------------- //
// --- init ---------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------- //

extern "C"
{

	int kdemain( int argc, char **argv )
	{
		KInstance instance("kiosidux");
		(void) KGlobal::locale();
		SiduxProtocol slave( argv[2], argv[3] );
		slave.dispatchLoop();
		return 0;
	}
}

SiduxProtocol::SiduxProtocol( const QCString & pool, const QCString & app )
        : SlaveBase( "sidux", pool, app )
{
}



// To simplify our implementation, the decodeURL() member returns a ResourceInfo struct given a URL
struct ResourceInfo
{
    QString name;        // The basename of the file or directory
    QString resType;     // Resource type
    QString relPath;     // Path relative to the resource directory
    QString path;        // Physical path of resource (if it exists)
    QString localPath;   // Physical path of save location
    bool exists;         // Does it exist?
    bool isDir;          // Is it a directory?
    bool isLocal;        // Is it (exclusively) local?
};


bool SiduxProtocol::decodeURL( const KURL & url, ResourceInfo & s )
{
	QString urlPath = url.path(-1);
	s.exists = true;
	s.isDir = true;

	return true;
}


void SiduxProtocol::stat( const KURL & url )
{

	ResourceInfo s;
	if ( !decodeURL( url, s ) )
		return;

	if ( !s.exists )
		error( ERR_DOES_NOT_EXIST, url.prettyURL() );
	else if ( s.isDir )
		statEntry( createDirEntry( s.name ) );
	else
		//redirection(s.path);
		statEntry( createEntry( s.path ) ); 


	finished();
}


void SiduxProtocol::get( const KURL & url )
{
	ResourceInfo s;
	if ( !decodeURL( url, s ) )
		return;
	
	if ( !s.exists )
		error( ERR_DOES_NOT_EXIST, url.prettyURL() );
	else if ( s.isDir )
		error( ERR_IS_DIRECTORY, url.prettyURL() );
	else
	{
		redirection( KURL( s.path ) );
	}
	
	finished();
}

void SiduxProtocol::put( const KURL& url, int permissions, bool overwrite, bool resume )
{
	ResourceInfo s;
	if ( !decodeURL( url, s ) )
		return;

	if ( s.exists && s.isDir )
		error( ERR_IS_DIRECTORY, url.prettyURL() );
	else if ( s.exists && !overwrite )
		error( ERR_FILE_ALREADY_EXIST, url.prettyURL() );
	// Trying to save to the root
	else if ( !s.resType )
		error( ERR_SLAVE_DEFINED, "You cannot save to the root of network:/" );
	else
	{
		locateLocal(s.resType, s.relPath, true);     // Just to create the dirs
		redirection( KURL( s.localPath ) );
	}

	finished();
}

// ------------------------------------------------------------------------------------- //
// --- listdir ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------------------- //

void SiduxProtocol::listDir( const KURL & url )
{

	QString p = "/usr/share/siduxcc/bin/"; // bin path

	KIO::UDSEntry entry;


	// --- main --- //

	if(url.path() == "/")
	{

		// siduxcc
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Control Center"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"controlcenter");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "siduxcc");
		listEntry(entry, false);
		entry.clear();
	
		// install-meta
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Metapackage Installer"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"install-meta");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-meta");
		listEntry(entry, false);
		entry.clear();

		// help menu
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Help"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFDIR);
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "help");
		listEntry(entry, false);
		entry.clear();

		// network menu
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Network"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFDIR);
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux_net");
		listEntry(entry, false);
		entry.clear();

		// others menu
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Others"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFDIR);
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "package");
		listEntry(entry, false);
		entry.clear();



	}


	// --- help --- //

	if(url.path() == "/Help")
	{
		// homepage
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Homepage"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"homepage");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "html");
		listEntry(entry, false);
		entry.clear();
	
		// manual
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Manual"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"manual");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-book2b");
		listEntry(entry, false);
		entry.clear();
	
	
		// chat
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Chat"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, "/usr/bin/sidux-irc");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-irc");
		listEntry(entry, false);
		entry.clear();
	
		// forum
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Forum"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"forum");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "html");
		listEntry(entry, false);
		entry.clear();

		// wiki
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Wiki"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"wiki");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "html");
		listEntry(entry, false);
		entry.clear();
	}

	// --- network --- //

	if(url.path() == "/Network")
	{
		// ndiswrapper
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Ndiswrapper"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"ndiswrapper");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-ndiswrapper");
		listEntry(entry, false);
		entry.clear();
	
		// kpp
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Modem Dialer"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, "/usr/bin/kppp");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "kppp");
		listEntry(entry, false);
		entry.clear();
	
	
		// Ceni
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Ceni"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"ceni");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "ceni");
		listEntry(entry, false);
		entry.clear();
	}

	// --- others --- //

	if(url.path() == "/Others")
	{
		// hermes
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Hermes (upgrade sentinel)"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, "/usr/bin/siduxcc-hermes");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "siduxcc_hermes");
		listEntry(entry, false);
		entry.clear();

		// cups
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Configure printer(s)"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"cups");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-cups");
		listEntry(entry, false);
		entry.clear();
	
		// fix-fonts
		addAtom(entry, KIO::UDS_NAME, 0, i18n("Register Fonts (fix-fonts)"));
		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, p+"fixfonts");
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");
		addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-fonts");
		listEntry(entry, false);
		entry.clear();


	}

	listEntry( UDSEntry(), true );
	finished();

}

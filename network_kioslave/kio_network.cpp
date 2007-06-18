/*
 * kio_network.cpp
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

#include "kio_network.h"
#include <kinstance.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <qstring.h>
#include <sys/stat.h>
#include <qstringlist.h>
#include <kglobal.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <klocale.h>
#include <dcopref.h>
#include <kmessagebox.h>

using namespace KIO;

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

// Helper functions
static UDSAtom createAtom( uint uds, long lng );
static UDSAtom createAtom( uint uds, const QString & str );
static UDSEntry createDirEntry( const QString & fileName );
static UDSEntry createEntry( const QString & path );


static void addAtom(KIO::UDSEntry &entry, unsigned int ID, long l,
                    const QString &s = QString::null)
{
	KIO::UDSAtom atom;
	atom.m_uds = ID;
	atom.m_long = l;
	atom.m_str = s;
	entry.append(atom);
}


extern "C"
{

    int kdemain( int argc, char **argv )
    {
        KInstance instance("kio_network");
        (void) KGlobal::locale();
        NetworkProtocol slave( argv[2], argv[3] );
        slave.dispatchLoop();
        return 0;
    }
}

NetworkProtocol::NetworkProtocol( const QCString & pool, const QCString & app )
        : SlaveBase( "network", pool, app )
{
	this->shell = new Process();
}

void NetworkProtocol::get
    ( const KURL & url )
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

void NetworkProtocol::put( const KURL& url, int permissions, bool overwrite, bool resume )
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
        error( ERR_SLAVE_DEFINED, i18n("You cannot save to the root of network:/") );
    else
    {
        locateLocal(s.resType, s.relPath, true);     // Just to create the dirs
        redirection( KURL( s.localPath ) );
    }

    finished();
}

void NetworkProtocol::stat( const KURL & url )
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

void NetworkProtocol::listDir( const KURL & url )
{

	KIO::UDSEntry entry;

	// add networkcards to list
	this->shell->setCommand("nicinfo");
	this->shell->start(true);
	QStringList deviceList = QStringList::split( "\n", this->shell->getBuffer() );
	QString ip;
	QStringList valueList;

	for(int i = 0; i < deviceList.count(); i++) {

		// get device properties
		valueList = QStringList::split( " ", deviceList[i] );
		for(int j = 6; j < valueList.count(); j++) {
				valueList[5] = valueList[5]+" "+valueList[j]; }
		// valueList[0] device name (E.g. eth0)
		// valueList[1] type (ethernet/wirless)
		// valueList[2] driver (E.g. e1000)
		// valueList[3] slot (E.g. pci)
		// valueList[4] mac-address (xx:xx:...)
		// valueList[5] description

		// get ip-address
		ip = "";
		this->shell->setCommand("/sbin/ifconfig "+valueList[0]+" | grep \"inet addr\" | gawk -F: '{print $2}' | gawk '{print $1}'");
		this->shell->start(true);
		ip = this->shell->getBuffer().stripWhiteSpace();

		addAtom(entry, KIO::UDS_NAME,  0, valueList[0]);  // device name 
		addAtom(entry, KIO::UDS_EXTRA, 0, ip);            // ip
		addAtom(entry, KIO::UDS_EXTRA, 0, valueList[4] ); // mac-address
		addAtom(entry, KIO::UDS_EXTRA, 0, valueList[2] ); // driver
		addAtom(entry, KIO::UDS_EXTRA, 0, valueList[3] ); // slot
		addAtom(entry, KIO::UDS_EXTRA, 0, valueList[5] ); // description

		addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
		addAtom(entry, KIO::UDS_URL, 0, "/usr/share/siduxcc/networkbin/"+valueList[0]);
		addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
		addAtom(entry, KIO::UDS_ACCESS, 0500);
		addAtom(entry, KIO::UDS_MIME_TYPE, 0, "application/x-desktop");

		// set icon
		if(valueList[1] == "ethernet") {
			if(ip != "") {
				addAtom(entry, KIO::UDS_ICON_NAME, 0, "siduxcc_lan_up"); }
			else {
				addAtom(entry, KIO::UDS_ICON_NAME, 0, "siduxcc_lan_up"); }
		}
		if(valueList[1] == "wireless") {
			if(ip != "") {
				addAtom(entry, KIO::UDS_ICON_NAME, 0, "siduxcc_wlan_up"); }
			else {
				addAtom(entry, KIO::UDS_ICON_NAME, 0, "siduxcc_wlan_up"); }
		}

		// add device
		listEntry(entry, false);
		entry.clear();

	}
	

	// More network Options
	addAtom(entry, KIO::UDS_NAME, 0, i18n("More Options"));
	addAtom(entry, KIO::UDS_FILE_TYPE, S_IFREG);
	addAtom(entry, KIO::UDS_URL, 0, "programs:/sidux/Net/");
	addAtom(entry, KIO::UDS_LOCAL_PATH, 0, url.path());
	addAtom(entry, KIO::UDS_ACCESS, 0500);
	addAtom(entry, KIO::UDS_MIME_TYPE, 0, "inode/directory");
	addAtom(entry, KIO::UDS_ICON_NAME, 0, "sidux-net");
	listEntry(entry, false);
	entry.clear();



	listEntry( UDSEntry(), true );
	finished();



	/* Create a folder
		QStringList types;
		types.append("hallo");
		// Show a folder for each resource type
		for ( it = types.begin(); it != types.end(); it++ ) {
			listEntry( createDirEntry( *it ), false ); }
	*/

}



bool NetworkProtocol::decodeURL( const KURL & url, ResourceInfo & s )
{
    QString urlPath = url.path(-1);
    s.exists = true;
    s.isDir = false;

    // Error if empty or relative path
    if ( KURL::isRelativeURL( url.url()) )
    {
        error(ERR_MALFORMED_URL, url.prettyURL() );
        finished();
        return false;
    }

    s.resType = urlPath.section('/', 1, 1);
    s.name = urlPath.section('/', -1);
    s.relPath = urlPath.section('/', 2);

    // If we are browsing the root
    if ( s.resType.isEmpty() )
    {
        s.name =  "/";
        s.isDir = true;
        return true;
    }

    // If we are browsing a top-level resource folder
    if ( s.relPath.isEmpty() )
    {
        if ( m_dirs->allTypes().contains(s.resType) )
        {
            s.isDir = true;
            s.name = s.resType;
            return true;
        }
    }

    // Otherwise
    s.path = locate( s.resType, s.relPath );
    if ( !s.path )
    {
        s.path = locate( s.resType, s.relPath + '/' );
        if ( !s.path )
            s.exists = false;
        else
            s.isDir = true;
    }
    else
    {
        s.exists = true;
        s.isDir = false;
    }

    // Make sure the path exists
    if ( !locate(s.resType, s.relPath.section('/', 0, -2 ) + '/' ) )
    {
        error(ERR_DOES_NOT_EXIST, url.prettyURL() );
        finished();
        return false;
    }

    s.localPath = locateLocal( s.resType, s.relPath + (s.isDir ? "/" : ""), false );

    int count = s.isDir ?  m_dirs->findDirs(s.resType, s.relPath).size() : m_dirs->findAllResources(s.resType, s.relPath ).size();
    s.isLocal = count <= 1 && s.path == s.localPath;

    return true;
}


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

static UDSEntry createDirEntry( const QString & fileName )
{
    UDSEntry entry;

    entry.append( createAtom( UDS_FILE_TYPE, S_IFDIR ) );
    entry.append( createAtom( UDS_NAME, fileName ) );

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

/*
 * Copyright (c) 2002-2003, 2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

    Change History (most recent first):
    
$Log: DNSServices.c,v $
Revision 1.8  2005/05/19 01:40:04  sculi2000
Apply various patches, make OpenBSD work

Revision 1.7  2004/08/02 18:22:36  sculi2000
fix switch of type/class in query_record

Revision 1.6  2004/08/02 05:47:03  sculi2000
fix bug in resolve

Revision 1.5  2004/07/31 22:26:06  sculi2000
get rid of warnings, fix bugs on Windows

Revision 1.4  2004/07/31 20:09:19  sculi2000
new discovery APIs

Revision 1.3  2004/07/31 16:25:22  sculi2000
clean up DNSQueryRecordRelease

Revision 1.2  2004/07/30 23:25:31  sculi2000
support for query_record

Revision 1.1  2004/07/29 00:19:39  sculi2000
add mDNSResponder library

Revision 1.2  2004/06/30 07:27:36  sculi2000
more fixes for new stuff, mDNSResponder starts up and runs, browse client works on Win32

Revision 1.1  2004/03/01 21:14:47  sculi2000
getting things cleaner with new source organization

Revision 1.1  2004/02/28 19:23:34  sculi2000
Move mDNS code into howl library

Revision 1.1  2004/02/25 18:30:40  sculi2000
Port 0.9.4 to Mac OS X

Revision 1.1  2004/02/25 08:55:36  sculi2000
first crack at 0.9.4, make mDNSResponder a shared library

Revision 1.11  2004/01/11 17:58:03  sculi2000
fix installer, verify compile on VS 6.0, update copyright dates

Revision 1.10  2003, 2004/11/24 02:13:33  sculi2000
update to latest rev of Apple mDNSCore...fixed duplicate resolve info bug, and memory corruption bug in Howl

Revision 1.9  2003, 2004/11/12 17:26:39  sculi2000
update version info 0.8.1

Revision 1.8  2003, 2004/10/23 18:57:01  uid125048
update version resources to 0.8, fix some problems with sw_rendezvous_resolve

Revision 1.7  2003, 2004/10/23 17:12:17  sculi2000
sync with Apple code, new APIs for sw_rendezvous

Revision 1.15  2003, 2004/08/20 06:44:24  bradley
Updated to latest internal version of the Rendezvous for Windows code: Added support for interface
specific registrations; Added support for no-such-service registrations; Added support for host
name registrations; Added support for host proxy and service proxy registrations; Added support for
registration record updates (e.g. TXT record updates); Added support for using either a single C
string TXT record, a raw, pre-formatted TXT record potentially containing multiple character string
entries, or a C-string containing a Mac OS X-style \001-delimited set of TXT record character
strings; Added support in resolve callbacks for providing both a simplified C-string for TXT records
and a ptr/size for the raw TXT record data; Added utility routines for dynamically building TXT
records from a variety of sources (\001-delimited, individual strings, etc.) and converting TXT
records to various formats for use in apps; Added utility routines to validate DNS names, DNS
service types, and TXT records; Moved to portable address representation unions (byte-stream vs host
order integer) for consistency, to avoid swapping between host and network byte order, and for IPv6
support; Removed dependence on modified mDNSCore: define structures and prototypes locally; Added
support for automatically renaming services on name conflicts; Detect and correct TXT records from
old versions of mDNS that treated a TXT record as an arbitrary block of data, but prevent other
malformed TXT records from being accepted; Added many more error codes; Added complete HeaderDoc for
all constants, structures, typedefs, macros, and functions. Various other minor cleanup and fixes.

Revision 1.14  2003, 2004/08/14 02:19:56  cheshire
<rdar://problem/3375491> Split generic ResourceRecord type into two separate types: AuthRecord and CacheRecord

Revision 1.13  2003, 2004/08/12 19:56:29  cheshire
Update to APSL 2.0

Revision 1.12  2003, 2004/07/23 00:00:04  cheshire
Add comments

Revision 1.11  2003, 2004/07/15 01:55:17  cheshire
<rdar://problem/3315777> Need to implement service registration with subtypes

Revision 1.10  2003, 2004/07/02 21:20:10  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.9  2003, 2004/05/26 03:21:30  cheshire
Tidy up address structure naming:
mDNSIPAddr         => mDNSv4Addr (for consistency with mDNSv6Addr)
mDNSAddr.addr.ipv4 => mDNSAddr.ip.v4
mDNSAddr.addr.ipv6 => mDNSAddr.ip.v6

Revision 1.8  2003, 2004/05/06 00:00:51  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.7  2003, 2004/03/27 03:30:57  cheshire
<rdar://problem/3210018> Name conflicts not handled properly, resulting in memory corruption, and eventual crash
Problem was that HostNameCallback() was calling mDNS_DeregisterInterface(), which is not safe in a callback
Fixes:
1. Make mDNS_DeregisterInterface() safe to call from a callback
2. Make HostNameCallback() use mDNS_DeadvertiseInterface() instead
   (it never really needed to deregister the interface at all)

Revision 1.6  2003, 2004/03/22 02:57:45  cheshire
Updated mDNSWindows to use new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

Revision 1.5  2003, 2004/02/20 00:59:04  cheshire
Brought Windows code up to date so it complies with
Josh Graessley's interface changes for IPv6 support.
(Actual support for IPv6 on Windows will come later.)

Revision 1.4  2002/09/21 20:44:56  zarzycki
Added APSL info

Revision 1.3  2002/09/20 08:36:50  bradley
Fixed debug messages to output the correct information when resolving.

Revision 1.2  2002/09/20 05:58:01  bradley
DNS Services for Windows

*/

#include	<stddef.h>
#include	<stdlib.h>
#include	<string.h>
#include "mDNSPlatform.h"

#if( __MACH__ )
	#include	<CoreServices/CoreServices.h>
#endif

#include	"mDNSClientAPI.h"

#include	"DNSServices.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == Preprocessor ==
#endif

//===========================================================================================================================
//	Preprocessor
//===========================================================================================================================

#if( defined( _MSC_VER ) )
	#pragma warning( disable:4068 )		// Disable "unknown pragma" warning for "pragma unused".
	#pragma warning( disable:4127 )		// Disable "conditional expression is constant" warning for debug macros.
#endif

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME		"[DNSServices] "

enum
{
	kDNSInitializeValidFlags 				= kDNSFlagAdvertise, 
	
	// Browser
	
	kDNSBrowserCreateValidFlags 			= 0, 
	kDNSBrowserReleaseValidFlags 			= 0, 
	kDNSBrowserStartDomainSearchValidFlags 	= kDNSBrowserFlagRegistrationDomainsOnly, 
	kDNSBrowserStopDomainSearchValidFlags 	= 0, 
	kDNSBrowserStartServiceSearchValidFlags = kDNSBrowserFlagAutoResolve, 
	kDNSBrowserStopServiceSearchValidFlags 	= 0, 
	
	// Resolver
	
	kDNSResolverCreateValidFlags		 	= kDNSResolverFlagOneShot 			| 
											  kDNSResolverFlagOnlyIfUnique 		| 
											  kDNSResolverFlagAutoReleaseByName, 
	kDNSResolverReleaseValidFlags		 	= 0, 
	
	// Service Registration
	
	kDNSRegistrationCreateValidFlags			 	= kDNSRegistrationFlagPreFormattedTextRecord 	|
													  kDNSRegistrationFlagAutoRenameOnConflict, 
	kDNSNoSuchServiceRegistrationCreateValidFlags 	= 0, 
	kDNSRegistrationReleaseValidFlags	 			= 0, 
	kDNSRegistrationUpdateValidFlags	 			= 0, 
	
	kDNSRegistrationFlagPrivateNoSuchService		= ( 1 << 16 ), 
	
	// Domain Registration
	
	kDNSDomainRegistrationCreateValidFlags	= 0, 
	kDNSDomainRegistrationReleaseValidFlags	= 0, 
	
	// Host Registration
	
	kDNSHostRegistrationCreateValidFlags	= kDNSHostRegistrationFlagOnlyIfNotFound | 
											  kDNSHostRegistrationFlagAutoRenameOnConflict, 
	kDNSHostRegistrationReleaseValidFlags	= 0
};

#define	kDNSCountCacheEntryCountDefault		64

#if 0
#pragma mark == Structures ==
#endif

//===========================================================================================================================
//	Structures
//===========================================================================================================================

// Browser

typedef struct	DNSBrowser	DNSBrowser;
struct	DNSBrowser
{
	DNSBrowser *			next;
	DNSBrowserFlags			flags;
	DNSBrowserCallBack		callback;
	void *					callbackContext;
	mDNSBool				isDomainBrowsing;
	DNSQuestion				domainQuestion;
	DNSQuestion				defaultDomainQuestion;
	DNSBrowserFlags			domainSearchFlags;
	mDNSBool				isServiceBrowsing;
	DNSQuestion				serviceBrowseQuestion;
	DNSBrowserFlags			serviceSearchFlags;
	char					searchDomain[ 256 ];
	char					searchServiceType[ 256 ];
};

// Resolver

typedef struct	DNSResolver		DNSResolver;
struct	DNSResolver
{
	DNSResolver *			next;
	DNSResolverFlags		flags;
	DNSResolverCallBack		callback;
	void *					callbackContext;
	DNSBrowserRef			owner;
	ServiceInfoQuery		query;
	ServiceInfo				info;
	mDNSBool				isResolving;
	char					resolveName[ 256 ];
	char					resolveType[ 256 ];
	char					resolveDomain[ 256 ];
};

// Registration

typedef struct	DNSRegistration		DNSRegistration;
struct	DNSRegistration
{
	DNSRegistration *			next;
	DNSRegistrationFlags		flags;
	DNSRegistrationCallBack		callback;
	void *						callbackContext;
	char						interfaceName[ 256 ];
	ServiceRecordSet			set;
	
	// WARNING: Do not add fields after the ServiceRecordSet. This is where oversized TXT record space is allocated.
};

// Domain Registration

typedef struct	DNSDomainRegistration	DNSDomainRegistration;
struct	DNSDomainRegistration
{
	DNSDomainRegistration *			next;
	DNSDomainRegistrationFlags		flags;
	AuthRecord						rr;
};

// Domain Registration

typedef struct	DNSHostRegistration	DNSHostRegistration;
struct	DNSHostRegistration
{
	DNSHostRegistration *			next;
	domainlabel						name;
	domainlabel						domain;
	long							refCount;
	DNSHostRegistrationCallBack		callback;
	void *							callbackContext;
	DNSHostRegistrationFlags		flags;
	char							interfaceName[ 256 ];
	AuthRecord						RR_A;
	AuthRecord						RR_PTR;
};


typedef struct DNSQueryRecord DNSQueryRecord;
struct DNSQueryRecord
{
	DNSQueryRecord			*	next;
	DNSQueryRecordFlags		flags;
	long							refCount;
	DNSQueryRecordCallBack	callback;
	void						*	callbackContext;
	DNSQuestion				*	question;
	domainlabel					fullname;
	mDNSu16						rrclass;
	mDNSu16						rrtype;
};
	

#if 0
#pragma mark == Macros ==
#endif

//===========================================================================================================================
//	Macros
//===========================================================================================================================

// Emulate Mac OS debugging macros for non-Mac platforms.

#if( !TARGET_OS_MAC )
	#define check(assertion)
	#define check_string( assertion, cstring )
	#define check_noerr(err)
	#define check_noerr_string( error, cstring )
	#define debug_string( cstring )
	#define require( assertion, label )									do { if( !(assertion) ) goto label; } while(0/*CONSTCOND*/)
	#define require_string( assertion, label, string )					require(assertion, label)
	#define require_noerr( error, label )								do { if( (error) != 0 ) goto label; } while(0/*CONSTCOND*/)
	#define require_noerr_action( error, label, action )				do { if( (error) != 0 ) { {action;}; goto label; } } while(0/*CONSTCOND*/)
	#define require_action( assertion, label, action )					do { if( !(assertion) ) { {action;}; goto label; } } while(0/*CONSTCOND*/)
	#define require_action_string( assertion, label, action, cstring )	do { if( !(assertion) ) { {action;}; goto label; } } while(0/*CONSTCOND*/)
#endif

#define AssignDomainName(DST, SRC) mDNSPlatformMemCopy((SRC).c, (DST).c, DomainNameLength(&(SRC)))

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

// General

mDNSlocal void	DNSServicesLock( void );
mDNSlocal void	DNSServicesUnlock( void );
mDNSlocal void	DNSServicesMDNSCallBack( mDNS *const inMDNS, mStatus inStatus );
mDNSlocal void	DNSServicesUpdateInterfaceSpecificObjects( mDNS *const inMDNS );

// Browser

mDNSlocal void
	DNSBrowserPrivateCallBack( 
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const 	inAnswer, 
		mDNSBool						inAddRecord );

mDNSlocal void
	DNSBrowserPrivateResolverCallBack( 
		void *						inContext, 
		DNSResolverRef 				inRef, 
		DNSStatus 					inStatusCode, 
		const DNSResolverEvent *	inEvent );

mDNSlocal DNSBrowserRef	DNSBrowserFindObject( DNSBrowserRef inRef );
mDNSlocal DNSBrowserRef	DNSBrowserRemoveObject( DNSBrowserRef inRef );

// Resolver

mDNSlocal void				DNSResolverPrivateCallBack( mDNS * const inMDNS, ServiceInfoQuery *inQuery );
mDNSlocal DNSResolverRef	DNSResolverFindObject( DNSResolverRef inRef );
mDNSlocal DNSResolverRef	DNSResolverRemoveObject( DNSResolverRef inRef );
mDNSlocal void				DNSResolverRemoveDependentByBrowser( DNSBrowserRef inBrowserRef );
mDNSlocal void				DNSResolverRemoveDependentByName( const domainname *inName );
mDNSlocal DNSResolverRef	DNSResolverFindObjectByName( const domainname *inName );

// Registration

mDNSlocal void
	DNSRegistrationPrivateCallBack( 
		mDNS * const 				inMDNS, 
		ServiceRecordSet * const 	inSet, 
		mStatus 					inResult );

mDNSlocal void
	DNSNoSuchServiceRegistrationPrivateCallBack( 
		mDNS * const 		inMDNS, 
		AuthRecord * const 	inRR, 
		mStatus 			inResult );

mDNSlocal void	DNSRegistrationUpdateCallBack( mDNS * const inMDNS, AuthRecord * const inRR, RData *inOldData );

mDNSlocal DNSRegistrationRef *	DNSRegistrationFindObject( DNSRegistrationRef inRef );
mDNSlocal DNSRegistrationRef	DNSRegistrationRemoveObject( DNSRegistrationRef inRef );

// Domain Registration

mDNSlocal DNSDomainRegistrationRef	DNSDomainRegistrationRemoveObject( DNSDomainRegistrationRef inRef );

// Host Registration

mDNSlocal DNSHostRegistrationRef *	DNSHostRegistrationFindObject( DNSHostRegistrationRef inRef );
mDNSlocal DNSHostRegistrationRef	DNSHostRegistrationFindObjectByName( const domainname *inName );
mDNSlocal void	DNSHostRegistrationPrivateCallBack( mDNS * const inMDNS, AuthRecord *const inRR, mStatus inResult );


mDNSlocal void
DNSQueryRecordPrivateCallBack(
						mDNS						*	const inMDNS,
						DNSQuestion				*			question,
						const ResourceRecord	*	const answer,
						mDNSBool								add);

mDNSlocal DNSQueryRecordRef
DNSQueryRecordRemoveObject(
						DNSQueryRecordRef inRef );

// Utilities

mDNSlocal DNSStatus	DNSMemAlloc( size_t inSize, void *outMem );
mDNSlocal void		DNSMemFree( void *inMem );
mDNSlocal void		MDNSAddrToDNSAddress( const mDNSAddr *inAddr, DNSNetworkAddress *outAddr );

// Platform Accessors


#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

mDNSexport mDNS							gMDNS;
mDNS *										gMDNSPtr 					= mDNSNULL;
mDNSlocal CacheRecord *					gMDNSCache 					= mDNSNULL;
mDNSlocal DNSBrowserRef					gDNSBrowserList				= mDNSNULL;
mDNSlocal DNSResolverRef				gDNSResolverList			= mDNSNULL;
mDNSlocal DNSRegistrationRef			gDNSRegistrationList		= mDNSNULL;
mDNSlocal DNSDomainRegistrationRef		gDNSDomainRegistrationList	= mDNSNULL;
mDNSlocal DNSHostRegistrationRef		gDNSHostRegistrationList	= mDNSNULL;
mDNSlocal DNSQueryRecordRef			gDNSQueryRecordList		=	mDNSNULL;

extern mDNS_PlatformSupport		*	mDNSPlatformSupport();
	
#if 0
#pragma mark -
#pragma mark == General ==
#endif

//===========================================================================================================================
//	DNSServicesInitialize
//===========================================================================================================================

DNSStatus	DNSServicesInitialize(
							sw_salt	salt,
							DNSFlags inFlags, DNSCount inCacheEntryCount, sw_string * filters, sw_uint32 num_filters)
{
	mDNS_PlatformSupport	*	p;
	DNSStatus				err;
	mDNSBool		advertise;
	
	require_action( ( inFlags & ~kDNSInitializeValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	
	// Allocate the record cache.
	
	if( inCacheEntryCount == 0 )
	{
		inCacheEntryCount = kDNSCountCacheEntryCountDefault;
	}
	gMDNSCache = (CacheRecord *) malloc( inCacheEntryCount * sizeof( *gMDNSCache ) );
	require_action( gMDNSCache, exit, err = kDNSNoMemoryErr );

	/*
	 * grab the platform support struct
	 */
	p							=	mDNSPlatformSupport();
	p->m_filters			=	filters;
	p->m_num_filters		=	num_filters;
	p->m_salt				=	salt;

	// Initialize mDNS.
	
	if( inFlags & kDNSFlagAdvertise )
	{
		advertise = mDNS_Init_AdvertiseLocalAddresses;
	}
	else
	{
		advertise = mDNS_Init_DontAdvertiseLocalAddresses;
	}
	err = mDNS_Init( &gMDNS, p, gMDNSCache, inCacheEntryCount, advertise, DNSServicesMDNSCallBack, mDNSNULL );
	require_noerr( err, exit );
	err = gMDNS.mDNSPlatformStatus;
	require_noerr( err, exit );
	
	gMDNSPtr = &gMDNS;
	
exit:
	if( err )
	{
		DNSServicesFinalize();
	}
	return( err );
}

//===========================================================================================================================
//	DNSServicesFinalize
//===========================================================================================================================

void	DNSServicesFinalize( void )
{
	if( gMDNSPtr )
	{
		mDNSPlatformLock( &gMDNS );
		
		// Clean up any dangling service registrations.
		
		while( gDNSRegistrationList )
		{
			DNSRegistrationRef		serviceRef;
			
			serviceRef = gDNSRegistrationList;
			DNSRegistrationRelease( serviceRef, 0UL );
			check_string( serviceRef != gDNSRegistrationList, "dangling service registration cannot be cleaned up" );
		}

		// Clean up any dangling domain registrations.
		
		while( gDNSDomainRegistrationList )
		{
			DNSDomainRegistrationRef		domainRef;
			
			domainRef = gDNSDomainRegistrationList;
			DNSDomainRegistrationRelease( domainRef, 0 );
			check_string( domainRef != gDNSDomainRegistrationList, "dangling domain registration cannot be cleaned up" );
		}

		// Clean up any dangling host registrations.
		
		while( gDNSHostRegistrationList )
		{
			DNSHostRegistrationRef		hostRef;
			long						refCount;
			
			hostRef = gDNSHostRegistrationList;
			refCount = hostRef->refCount;
			DNSHostRegistrationRelease( hostRef, 0 );
			check_string( ( refCount > 1 ) || ( hostRef != gDNSHostRegistrationList ), 
						  "dangling host registration cannot be cleaned up" );
		}
		
		// Clean up any dangling browsers.
		
		while( gDNSBrowserList )
		{
			DNSBrowserRef			browserRef;
			
			browserRef = gDNSBrowserList;
			DNSBrowserRelease( browserRef, 0 );
			check_string( browserRef != gDNSBrowserList, "dangling browser cannot be cleaned up" );
		}
		
		// Clean up any dangling resolvers.
		
		while( gDNSResolverList )
		{
			DNSResolverRef			resolverRef;
			
			resolverRef = gDNSResolverList;
			DNSResolverRelease( resolverRef, 0 );
			check_string( resolverRef != gDNSResolverList, "dangling resolver cannot be cleaned up" );
		}
		
		// Null out our MDNS ptr before releasing the lock so no other threads can sneak in and start operations.
		
		gMDNSPtr = mDNSNULL;
		mDNSPlatformUnlock( &gMDNS );
		
		// Tear down mDNS.
		
		mDNS_Close( &gMDNS );
	}
	if( gMDNSCache )
	{
		free( gMDNSCache );
		gMDNSCache = mDNSNULL;
	}
}

//===========================================================================================================================
//	DNSServicesLock
//===========================================================================================================================

mDNSlocal void	DNSServicesLock( void )
{
	if( gMDNSPtr )
	{
		mDNSPlatformLock( gMDNSPtr );
	}
}

//===========================================================================================================================
//	DNSServicesUnlock
//===========================================================================================================================

mDNSlocal void	DNSServicesUnlock( void )
{
	if( gMDNSPtr )
	{
		mDNSPlatformUnlock( gMDNSPtr );
	}
}

//===========================================================================================================================
//	DNSServicesMDNSCallBack
//===========================================================================================================================

mDNSlocal void	DNSServicesMDNSCallBack( mDNS *const inMDNS, mStatus inStatus )
{
	DNS_UNUSED( inMDNS );
	DNS_UNUSED( inStatus );
	check( inMDNS );
	
	debugf( DEBUG_NAME "MDNS callback (status=%ld)", inStatus );
	
	if( inStatus == mStatus_ConfigChanged )
	{
		DNSServicesUpdateInterfaceSpecificObjects( inMDNS );
	}
}

//===========================================================================================================================
//	DNSServicesUpdateInterfaceSpecificObjects
//===========================================================================================================================

mDNSlocal void	DNSServicesUpdateInterfaceSpecificObjects( mDNS *const inMDNS )
{
	DNSRegistration *		serviceRegistration;
	
	DNSServicesLock();
	
	// Update interface-specific service registrations.
	
	for( serviceRegistration = gDNSRegistrationList; serviceRegistration; serviceRegistration = serviceRegistration->next )
	{
		if( serviceRegistration->interfaceName[ 0 ] != '\0' )
		{
			mStatus				err;
			mDNSInterfaceID		interfaceID;
			
			err = mDNSPlatformInterfaceNameToID( inMDNS, serviceRegistration->interfaceName, &interfaceID );
			check_noerr( err );
			if( err == mStatus_NoError )
			{
				// Update all the resource records with the new interface ID.
				
				serviceRegistration->set.RR_ADV.resrec.InterfaceID = interfaceID;
				serviceRegistration->set.RR_PTR.resrec.InterfaceID = interfaceID;
				serviceRegistration->set.RR_SRV.resrec.InterfaceID = interfaceID;
				serviceRegistration->set.RR_TXT.resrec.InterfaceID = interfaceID;
			}
		}
	}
	
	DNSServicesUnlock();
}

#if 0
#pragma mark -
#pragma mark == Browser ==
#endif

//===========================================================================================================================
//	DNSBrowserCreate
//===========================================================================================================================

DNSStatus
	DNSBrowserCreate( 
		DNSBrowserFlags 	inFlags, 
		DNSBrowserCallBack	inCallBack, 
		void *				inCallBackContext, 
		DNSBrowserRef *		outRef )
{
	DNSStatus			err;
	DNSBrowser *		objectPtr;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSBrowserCreateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( inCallBack, exit, err = kDNSBadParamErr );
	
	// Allocate the object and set it up.
	
	err = DNSMemAlloc( sizeof( *objectPtr ), &objectPtr );
	require_noerr( err, exit );
	memset( objectPtr, 0, sizeof( *objectPtr ) );
	
	objectPtr->flags 			= inFlags;
	objectPtr->callback 		= inCallBack;
	objectPtr->callbackContext 	= inCallBackContext;
	
	// Add the object to the list.
	
	objectPtr->next = gDNSBrowserList;
	gDNSBrowserList = objectPtr;
	
	if( outRef )
	{
		*outRef = objectPtr;
	}
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSBrowserRelease
//===========================================================================================================================

DNSStatus	DNSBrowserRelease( DNSBrowserRef inRef, DNSBrowserFlags inFlags )
{
	DNSStatus			err;
	DNSBrowserEvent		event;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSBrowserReleaseValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	
	// Stop service and domain browsing and remove any resolvers dependent on this browser.
	
	DNSBrowserStopDomainSearch( inRef, 0 );
	DNSBrowserStopServiceSearch( inRef, 0 );	
	DNSResolverRemoveDependentByBrowser( inRef );
	
	// Remove the object from the list.
	
	inRef = DNSBrowserRemoveObject( inRef );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	
	// Call the callback with a release event.
	
	check( inRef->callback );
	memset( &event, 0, sizeof( event ) );
	event.type = kDNSBrowserEventTypeRelease;
	inRef->callback( inRef->callbackContext, inRef, kDNSNoErr, &event );
	
	// Release the memory used by the object.
	
	DNSMemFree( inRef );
	err = kDNSNoErr;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSBrowserStartDomainSearch
//===========================================================================================================================

DNSStatus	DNSBrowserStartDomainSearch( DNSBrowserRef inRef, DNSBrowserFlags inFlags, mDNSu32 inInterfaceIndex )
{
	DNSStatus			err;
	mDNS_DomainType		type;
	mDNS_DomainType		defaultType;
	DNSBrowserEvent		event;
	mDNSBool			isDomainBrowsing;
	
	isDomainBrowsing = mDNSfalse;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef && DNSBrowserFindObject( inRef ), exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSBrowserStartDomainSearchValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( !inRef->isDomainBrowsing, exit, err = kDNSBadStateErr );
		
	// Determine whether to browse for normal domains or registration domains.
	
	if( inFlags & kDNSBrowserFlagRegistrationDomainsOnly )
	{
		type 		= mDNS_DomainTypeRegistration;
		defaultType	= mDNS_DomainTypeRegistrationDefault;
	}
	else
	{
		type 		= mDNS_DomainTypeBrowse;
		defaultType	= mDNS_DomainTypeBrowseDefault;
	}
	
	// Start the browse operations.
	
	err = mDNS_GetDomains( gMDNSPtr, &inRef->domainQuestion, type, (mDNSInterfaceID) inInterfaceIndex, DNSBrowserPrivateCallBack, inRef );
	require_noerr( err, exit );
	isDomainBrowsing = mDNStrue;
	
	err = mDNS_GetDomains( gMDNSPtr, &inRef->defaultDomainQuestion, defaultType, mDNSInterface_Any, DNSBrowserPrivateCallBack, inRef );
	require_noerr( err, exit );
	
	inRef->domainSearchFlags 	= inFlags;
	inRef->isDomainBrowsing 	= mDNStrue;
	
	// Call back immediately with "local." since that is always available for all types of browsing.
	
	memset( &event, 0, sizeof( event ) );
	event.type							= kDNSBrowserEventTypeAddDefaultDomain;
	event.data.addDefaultDomain.domain 	= kDNSLocalDomain;
	event.data.addDefaultDomain.flags 	= 0;
	inRef->callback( inRef->callbackContext, inRef, kDNSNoErr, &event );
	
exit:
	if( err && isDomainBrowsing )
	{
		mDNS_StopGetDomains( gMDNSPtr, &inRef->domainQuestion );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSBrowserStopDomainSearch
//===========================================================================================================================

DNSStatus	DNSBrowserStopDomainSearch( DNSBrowserRef inRef, DNSBrowserFlags inFlags )
{
	DNSStatus		err;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef && DNSBrowserFindObject( inRef ), exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSBrowserStopDomainSearchValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	if( !inRef->isDomainBrowsing )
	{
		err = kDNSBadStateErr;
		goto exit;
	}
	
	// Stop the browse operations.
	
	mDNS_StopGetDomains( gMDNSPtr, &inRef->defaultDomainQuestion );
	mDNS_StopGetDomains( gMDNSPtr, &inRef->domainQuestion );
	inRef->isDomainBrowsing = mDNSfalse;
	err = kDNSNoErr;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSBrowserStartServiceSearch
//===========================================================================================================================

DNSStatus
	DNSBrowserStartServiceSearch( 
		DNSBrowserRef 		inRef, 
		DNSBrowserFlags 	inFlags, 
		mDNSu32				inInterfaceIndex,
		const char * 		inType, 
		const char *		inDomain )
{
	DNSStatus		err;
	domainname		type;
	domainname		domain;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef && DNSBrowserFindObject( inRef ), exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSBrowserStartServiceSearchValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( !inRef->isServiceBrowsing, exit, err = kDNSBadStateErr );
	require_action( inType, exit, err = kDNSBadParamErr );
	
	// Default to the local domain when null is passed in.
	
	if( !inDomain || ( inDomain[ 0 ] == '\0' ) || ( inDomain[ 0 ] == '.' ) )
	{
		inDomain = kDNSLocalDomain;
	}
	
	// Save off the search criteria (in case it needs to be automatically restarted later).
	
	inRef->serviceSearchFlags = inFlags;
	
	strncpy( inRef->searchServiceType, inType, sizeof( inRef->searchServiceType ) - 1 );
	inRef->searchServiceType[ sizeof( inRef->searchServiceType ) - 1 ] = '\0';
	
	strncpy( inRef->searchDomain, inDomain, sizeof( inRef->searchDomain ) - 1 );
	inRef->searchDomain[ sizeof( inRef->searchDomain ) - 1 ] = '\0';
	
	// Start the browse operation with mDNS using our private callback.
	
	MakeDomainNameFromDNSNameString( &type, inType );
	MakeDomainNameFromDNSNameString( &domain, inDomain );
	
	err = mDNS_StartBrowse( gMDNSPtr, &inRef->serviceBrowseQuestion, &type, &domain, (mDNSInterfaceID) inInterfaceIndex, 
							DNSBrowserPrivateCallBack, inRef );
	require_noerr( err, exit );
	
	inRef->isServiceBrowsing = mDNStrue;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSBrowserStopServiceSearch
//===========================================================================================================================

DNSStatus	DNSBrowserStopServiceSearch( DNSBrowserRef inRef, DNSBrowserFlags inFlags )
{
	DNSStatus		err;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef && DNSBrowserFindObject( inRef ), exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSBrowserStopServiceSearchValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	if( !inRef->isServiceBrowsing )
	{
		err = kDNSBadStateErr;
		goto exit;
	}
	
	// Stop the browse operation with mDNS. Remove any resolvers dependent on browser since we are no longer searching.
	
	mDNS_StopBrowse( gMDNSPtr, &inRef->serviceBrowseQuestion );
	DNSResolverRemoveDependentByBrowser( inRef );
	inRef->isServiceBrowsing = mDNSfalse;
	err = kDNSNoErr;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSBrowserPrivateCallBack
//===========================================================================================================================

mDNSlocal void
	DNSBrowserPrivateCallBack( 
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const 	inAnswer, 
		mDNSBool						inAddRecord )
{
	DNSBrowserRef		objectPtr;
	domainlabel			name;
	domainname			type;
	domainname			domain;
	char				nameString[ 256 ];
	char				typeString[ 256 ];
	char				domainString[ 256 ];
	DNSBrowserEvent		event;
	mStatus				err;
	
	check( inMDNS );
	check( inQuestion );
	check( inAnswer );
	
	DNSServicesLock();
	
	// Exclude non-PTR answers.
	
	require( inAnswer->rrtype == kDNSType_PTR, exit );
	
	// Exit if object is no longer valid. Should never happen.
	
	objectPtr = DNSBrowserFindObject( (DNSBrowserRef) inQuestion->QuestionContext );
	require( objectPtr, exit );
	
	// Determine what type of callback it is based on the question.
	
	memset( &event, 0, sizeof( event ) );
	if( inQuestion == &objectPtr->serviceBrowseQuestion )
	{
		DNSBrowserEventServiceData *		serviceDataPtr;
		DNSBrowserFlags						browserFlags;
		
		// Extract name, type, and domain from the resource record.
	
		DeconstructServiceName( &inAnswer->rdata->u.name, &name, &type, &domain );
		ConvertDomainLabelToCString_unescaped( &name, nameString );
		ConvertDomainNameToCString( &type, typeString );
		ConvertDomainNameToCString( &domain, domainString );
		
		// Fill in the event data. A TTL of zero means the service is no longer available. If the service instance is going
		// away (ttl == 0), remove any resolvers dependent on the name since it is no longer valid.
		
		if( !inAddRecord )
		{
			DNSResolverRemoveDependentByName( &inAnswer->rdata->u.name );
			
			event.type		= kDNSBrowserEventTypeRemoveService;
			serviceDataPtr 	= &event.data.removeService;
		}
		else
		{
			event.type 		= kDNSBrowserEventTypeAddService;
			serviceDataPtr 	= &event.data.addService;
		}
		serviceDataPtr->interfaceName = "";
		if( inAnswer->InterfaceID != mDNSInterface_Any )
		{
			mDNSPlatformInterfaceInfo		info;
			
			err = mDNSPlatformInterfaceIDToInfo( inMDNS, inAnswer->InterfaceID, &info );
			if( err == mStatus_NoError )
			{
				serviceDataPtr->interfaceName = info.name;
				MDNSAddrToDNSAddress( &info.ip, &serviceDataPtr->interfaceIP );
			}
			else
			{
				serviceDataPtr->interfaceName = "";
			}
		}
		serviceDataPtr->interfaceID	= inAnswer->InterfaceID;
		serviceDataPtr->name		= nameString;
		serviceDataPtr->type 		= typeString;
		serviceDataPtr->domain 		= domainString;
		serviceDataPtr->flags 		= 0;
		
		// Call the callback.
		
		browserFlags = objectPtr->serviceSearchFlags;
		objectPtr->callback( objectPtr->callbackContext, objectPtr, kDNSNoErr, &event );
		
		// Automatically resolve newly discovered names if the auto-resolve option is enabled.
		
		if( ( browserFlags & kDNSBrowserFlagAutoResolve ) && inAddRecord )
		{
			DNSStatus				err;
			DNSResolverFlags		flags;
			
			flags = kDNSResolverFlagOnlyIfUnique | kDNSResolverFlagAutoReleaseByName;
			err = DNSResolverCreate( flags, 0, nameString, typeString, domainString, DNSBrowserPrivateResolverCallBack, 
									 mDNSNULL, objectPtr, mDNSNULL );
			check_noerr( err );
		}
	}
	else
	{
		DNSBrowserEventDomainData *		domainDataPtr;
		
		// Determine the event type. A TTL of zero means the domain is no longer available.
		
		domainDataPtr = mDNSNULL;
		if( inQuestion == &objectPtr->domainQuestion )
		{
			if( !inAddRecord )
			{
				event.type = kDNSBrowserEventTypeRemoveDomain;
				domainDataPtr = &event.data.removeDomain;
			}
			else
			{
				event.type = kDNSBrowserEventTypeAddDomain;
				domainDataPtr = &event.data.addDomain;
			}
		}
		else if( inQuestion == &objectPtr->defaultDomainQuestion )
		{
			if( !inAddRecord )
			{
				event.type = kDNSBrowserEventTypeRemoveDomain;
				domainDataPtr = &event.data.removeDomain;
			}
			else
			{
				event.type = kDNSBrowserEventTypeAddDefaultDomain;
				domainDataPtr = &event.data.addDefaultDomain;
			}
		}
		require_string( domainDataPtr, exit, "domain response for unknown question" );
		
		// Extract domain name from the resource record and fill in the event data.
		
		ConvertDomainNameToCString( &inAnswer->rdata->u.name, domainString );
		
		domainDataPtr->interfaceName = "";
		if( inAnswer->InterfaceID != mDNSInterface_Any )
		{
			mDNSPlatformInterfaceInfo		info;
			
			err = mDNSPlatformInterfaceIDToInfo( inMDNS, inAnswer->InterfaceID, &info );
			if( err == mStatus_NoError )
			{
				domainDataPtr->interfaceName = info.name;
				MDNSAddrToDNSAddress( &info.ip, &domainDataPtr->interfaceIP );
			}
			else
			{
				domainDataPtr->interfaceName = "";
			}
		}
		domainDataPtr->interfaceID	= inAnswer->InterfaceID;
		domainDataPtr->domain 		= domainString;
		domainDataPtr->flags		= 0;
		
		// Call the callback.
		
		objectPtr->callback( objectPtr->callbackContext, objectPtr, kDNSNoErr, &event );
	}

exit:
	DNSServicesUnlock();
}

//===========================================================================================================================
//	DNSBrowserPrivateResolverCallBack
//===========================================================================================================================

mDNSlocal void
	DNSBrowserPrivateResolverCallBack( 
		void *						inContext, 
		DNSResolverRef 				inRef, 
		DNSStatus 					inStatusCode, 
		const DNSResolverEvent *	inEvent )
{
	DNSBrowserRef		objectPtr;
	DNSBrowserEvent		event;
	
	DNS_UNUSED( inContext );
	DNS_UNUSED( inStatusCode );
	
	DNSServicesLock();
	
	// Exit if object is no longer valid. Should never happen.
	
	objectPtr = inRef->owner;
	require( objectPtr, exit );
	
	switch( inEvent->type )
	{
		case kDNSResolverEventTypeResolved:
			verbosedebugf( DEBUG_NAME "private resolver callback: resolved (ref=0x%08X)", inRef );
			verbosedebugf( DEBUG_NAME "    name:   \"%s\"", 	inEvent->data.resolved.name );
			verbosedebugf( DEBUG_NAME "    type:   \"%s\"", 	inEvent->data.resolved.type );
			verbosedebugf( DEBUG_NAME "    domain: \"%s\"", 	inEvent->data.resolved.domain );
			verbosedebugf( DEBUG_NAME "    if:     %.4a", 		&inEvent->data.resolved.interfaceIP.u.ipv4.addr.v32 );
			verbosedebugf( DEBUG_NAME "    ip:     %.4a:%u", 	&inEvent->data.resolved.address.u.ipv4.addr.v32, 
															 	( inEvent->data.resolved.address.u.ipv4.port.v8[ 0 ] << 8 ) |
															 	  inEvent->data.resolved.address.u.ipv4.port.v8[ 1 ] );
			verbosedebugf( DEBUG_NAME "    text:   \"%s\"", 	inEvent->data.resolved.textRecord );
			
			// Re-package the resolver event as a browser event and call the callback.
			
			memset( &event, 0, sizeof( event ) );
			event.type = kDNSBrowserEventTypeResolved;
			event.data.resolved = &inEvent->data.resolved;
			
			objectPtr->callback( objectPtr->callbackContext, objectPtr, kDNSNoErr, &event );
			break;
		
		case kDNSResolverEventTypeRelease:
			verbosedebugf( DEBUG_NAME "private resolver callback: release (ref=0x%08X)", inRef );
			break;
		
		default:
			verbosedebugf( DEBUG_NAME "private resolver callback: unknown event (ref=0x%08X, event=%ld)", inRef, inEvent->type );
			break;
	}

exit:
	DNSServicesUnlock();
}

//===========================================================================================================================
//	DNSBrowserFindObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSBrowserRef	DNSBrowserFindObject( DNSBrowserRef inRef )
{
	DNSBrowser *		p;
	
	check( inRef );
		
	// Find the object in the list.
	
	for( p = gDNSBrowserList; p; p = p->next )
	{
		if( p == inRef )
		{
			break;
		}
	}
	return( p );
}

//===========================================================================================================================
//	DNSBrowserRemoveObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSBrowserRef	DNSBrowserRemoveObject( DNSBrowserRef inRef )
{
	DNSBrowser **		p;
	DNSBrowser *		found;
	
	for( p = &gDNSBrowserList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	found = *p;
	if( found )
	{
		*p = found->next;
	}
	return( found );
}

#if 0
#pragma mark -
#pragma mark == Resolver ==
#endif

//===========================================================================================================================
//	DNSResolverCreate
//===========================================================================================================================

DNSStatus
	DNSResolverCreate( 
		DNSResolverFlags		inFlags, 
		mDNSu32					inInterfaceIndex,
		const char *			inName, 
		const char *			inType, 
		const char *			inDomain, 
		DNSResolverCallBack		inCallBack, 
		void *					inCallBackContext, 
		DNSBrowserRef			inOwner, 
		DNSResolverRef *		outRef )
{	
	DNSStatus			err;
	int					isAutoRelease;
	DNSResolver *		objectPtr;
	domainlabel			name;
	domainname			type;
	domainname			domain;
	domainname			fullName;
	
	objectPtr = mDNSNULL;
	
	// Check parameters.
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSResolverCreateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( inName, exit, err = kDNSBadParamErr );
	require_action( inType, exit, err = kDNSBadParamErr );
	require_action( inDomain, exit, err = kDNSBadParamErr );
	require_action( inCallBack, exit, err = kDNSBadParamErr );
	isAutoRelease = inOwner || ( inFlags & ( kDNSResolverFlagOneShot | kDNSResolverFlagAutoReleaseByName ) );
	require_action( outRef || isAutoRelease, exit, err = kDNSBadParamErr );
	require_action( !inOwner || DNSBrowserFindObject( inOwner ), exit, err = kDNSBadReferenceErr );
	
	// Convert and package up the name, type, and domain into a single fully-qualified domain name to resolve.
	
	MakeDomainLabelFromLiteralString( &name, inName );
	MakeDomainNameFromDNSNameString( &type, inType );
	MakeDomainNameFromDNSNameString( &domain, inDomain );
	ConstructServiceName( &fullName, &name, &type, &domain );
	
	// If the caller only wants to add unique resolvers, check if a resolver for this name is already present.
	
	if( inFlags & kDNSResolverFlagOnlyIfUnique )
	{
		if( DNSResolverFindObjectByName( &fullName ) )
		{
			if( outRef )
			{
				*outRef = mDNSNULL;
			}
			err = kDNSNoErr;
			goto exit;
		}
	}
	
	// Allocate the object and set it up.
	
	err = DNSMemAlloc( sizeof( *objectPtr ), &objectPtr );
	require_noerr( err, exit );
	memset( objectPtr, 0, sizeof( *objectPtr ) );
	
	objectPtr->flags 				= inFlags;
	objectPtr->callback 			= inCallBack;
	objectPtr->callbackContext 		= inCallBackContext;
	objectPtr->owner				= inOwner;
	AssignDomainName( objectPtr->info.name, fullName );
	objectPtr->info.InterfaceID 	= (mDNSInterfaceID) inInterfaceIndex;
	
	// Save off the resolve info so the callback can get it.
	
	strncpy( objectPtr->resolveName, inName, sizeof( objectPtr->resolveName ) - 1 );
	objectPtr->resolveName[ sizeof( objectPtr->resolveName ) - 1 ] = '\0';
	
	strncpy( objectPtr->resolveType, inType, sizeof( objectPtr->resolveType ) - 1 );
	objectPtr->resolveType[ sizeof( objectPtr->resolveType ) - 1 ] = '\0';
	
	strncpy( objectPtr->resolveDomain, inDomain, sizeof( objectPtr->resolveDomain ) - 1 );
	objectPtr->resolveDomain[ sizeof( objectPtr->resolveDomain ) - 1 ] = '\0';
	
	// Add the object to the list.
	
	objectPtr->next = gDNSResolverList;
	gDNSResolverList = objectPtr;
	
	// Start the resolving process.
	
	objectPtr->isResolving = mDNStrue;
	err = mDNS_StartResolveService( gMDNSPtr, &objectPtr->query, &objectPtr->info, DNSResolverPrivateCallBack, objectPtr );
	require_noerr( err, exit );
	
	if( outRef )
	{
		*outRef = objectPtr;
	}
	
exit:
	if( err && objectPtr )
	{
		DNSResolverRemoveObject( objectPtr );
		DNSMemFree( objectPtr );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSResolverRelease
//===========================================================================================================================

DNSStatus	DNSResolverRelease( DNSResolverRef inRef, DNSResolverFlags inFlags )
{
	DNSStatus				err;
	DNSResolverEvent		event;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSResolverReleaseValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	
	// Remove the object from the list.
	
	inRef = DNSResolverRemoveObject( inRef );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	
	// Stop the resolving process.
	
	if( inRef->isResolving )
	{
		inRef->isResolving = mDNSfalse;
		mDNS_StopResolveService( gMDNSPtr, &inRef->query );
	}
	
	// Call the callback with a release event.
	
	check( inRef->callback );
	memset( &event, 0, sizeof( event ) );
	event.type = kDNSResolverEventTypeRelease;
	inRef->callback( inRef->callbackContext, inRef, kDNSNoErr, &event );
	
	// Release the memory used by the object.
	
	DNSMemFree( inRef );
	err = kDNSNoErr;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSResolverFindObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSResolverRef	DNSResolverFindObject( DNSResolverRef inRef )
{
	DNSResolver *		p;
	
	check( inRef );
		
	// Find the object in the list.
	
	for( p = gDNSResolverList; p; p = p->next )
	{
		if( p == inRef )
		{
			break;
		}
	}
	return( p );
}

//===========================================================================================================================
//	DNSResolverFindObjectByName
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSResolverRef	DNSResolverFindObjectByName( const domainname *inName )
{
	DNSResolver *		p;
	
	check( inName );
	
	for( p = gDNSResolverList; p; p = p->next )
	{
		if( SameDomainName( &p->info.name, inName ) )
		{
			break;
		}
	}
	return( p );
}

//===========================================================================================================================
//	DNSResolverPrivateCallBack
//===========================================================================================================================

mDNSlocal void	DNSResolverPrivateCallBack( mDNS * const inMDNS, ServiceInfoQuery *inQuery )
{
	DNSResolverRef			objectPtr;
	DNSResolverEvent		event;
	char *					txtString;
	mStatus					err;
	mDNSBool				release;
	
	txtString = NULL;

	DNSServicesLock();
	
	// Exit if object is no longer valid. Should never happen.
	
	objectPtr = DNSResolverFindObject( (DNSResolverRef) inQuery->ServiceInfoQueryContext );
	require( objectPtr, exit );
	
	// Convert the raw TXT record into a null-terminated string with \001-delimited records for Mac OS X-style clients.
	
	err = DNSTextRecordEscape( inQuery->info->TXTinfo, inQuery->info->TXTlen, &txtString );
	check_noerr( err );
	
	// Package up the results and call the callback.
	
	memset( &event, 0, sizeof( event ) );
	event.type 											= kDNSResolverEventTypeResolved;
	event.data.resolved.name							= objectPtr->resolveName;
	event.data.resolved.type							= objectPtr->resolveType;
	event.data.resolved.domain							= objectPtr->resolveDomain;
	event.data.resolved.interfaceName = "";
	if( inQuery->info->InterfaceID != mDNSInterface_Any )
	{
		mDNSPlatformInterfaceInfo		info;
			
		err = mDNSPlatformInterfaceIDToInfo( inMDNS, inQuery->info->InterfaceID, &info );
		if( err == mStatus_NoError )
		{
			event.data.resolved.interfaceName = info.name;
			MDNSAddrToDNSAddress( &info.ip, &event.data.resolved.interfaceIP );
		}
		else
		{
			event.data.resolved.interfaceName = "";
		}
	}
	event.data.resolved.interfaceID						= inQuery->info->InterfaceID;
	event.data.resolved.address.addressType				= inQuery->info->ip.type;
	event.data.resolved.address.u.ipv4.addr.v32 		= inQuery->info->ip.ip.v4.NotAnInteger;
	event.data.resolved.address.u.ipv4.port.v16			= inQuery->info->port.NotAnInteger;
	event.data.resolved.textRecord						= txtString ? txtString : "";
	event.data.resolved.flags 							= 0;
	event.data.resolved.textRecordRaw					= (const void *) inQuery->info->TXTinfo;
	event.data.resolved.textRecordRawSize				= (DNSCount) inQuery->info->TXTlen;
	release												= (mDNSBool)( ( objectPtr->flags & kDNSResolverFlagOneShot ) != 0 );
	objectPtr->callback( objectPtr->callbackContext, objectPtr, kDNSNoErr, &event );
	
	// Auto-release the object if needed.
	
	if( release )
	{
		DNSResolverRelease( objectPtr, 0 );
	}

exit:
	DNSServicesUnlock();
	if( txtString )
	{
		free( txtString );
	}
}

//===========================================================================================================================
//	DNSResolverRemoveObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSResolverRef	DNSResolverRemoveObject( DNSResolverRef inRef )
{
	DNSResolver **		p;
	DNSResolver *		found;
	
	for( p = &gDNSResolverList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	found = *p;
	if( found )
	{
		*p = found->next;
	}
	return( found );
}

//===========================================================================================================================
//	DNSResolverRemoveDependentByBrowser
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal void	DNSResolverRemoveDependentByBrowser( DNSBrowserRef inBrowserRef )
{
	DNSResolver *		p;
		
	check( inBrowserRef );
			
	// Removes all the resolver objects dependent on the specified browser. Restart the search from the beginning of the 
	// list after each removal to handle the list changing in possible callbacks that may be invoked.
	
	do
	{
		for( p = gDNSResolverList; p; p = p->next )
		{
			if( p->owner == inBrowserRef )
			{
				DNSResolverRelease( p, 0 );
				break;
			}
		}
		
	}	while( p );
}

//===========================================================================================================================
//	DNSResolverRemoveDependentByName
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal void	DNSResolverRemoveDependentByName( const domainname *inName )
{
	DNSResolver *		p;
		
	check( inName );
		
	// Removes all the resolver objects dependent on the specified name that want to be auto-released by name. Restart 
	// the search from the beginning of the list after each removal to handle the list changing in possible callbacks 
	// that may be invoked.
	
	do
	{
		for( p = gDNSResolverList; p; p = p->next )
		{
			if( ( p->flags & kDNSResolverFlagAutoReleaseByName ) && SameDomainName( &p->info.name, inName ) )
			{
				DNSResolverRelease( p, 0 );
				break;
			}
		}
		
	}	while( p );
}

#if 0
#pragma mark -
#pragma mark == Registration ==
#endif

//===========================================================================================================================
//	DNSRegistrationCreate
//===========================================================================================================================

DNSStatus
	DNSRegistrationCreate( 
		DNSRegistrationFlags		inFlags, 
		const char *				inName, 
		const char *				inType, 
		const char *				inDomain, 
		DNSPort						inPort, 
		const void *				inTextRecord, 
		DNSCount					inTextRecordSize, 
		const char *				inHost, 
		mDNSInterfaceID			inInterfaceID,
		DNSRegistrationCallBack		inCallBack, 
		void *						inCallBackContext, 
		DNSRegistrationRef *		outRef )
{	
	DNSStatus				err;
	size_t					size;
	DNSRegistration *		objectPtr;
	domainlabel				name;
	domainname				type;
	domainname				domain;
	mDNSIPPort				port;
	mDNSu8					textRecord[ 256 ];
	const mDNSu8 *			textRecordPtr;
	domainname *			host;
	domainname				tempHost;
	
	objectPtr = mDNSNULL;
	
	// Check parameters.
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSRegistrationCreateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( inType, exit, err = kDNSBadParamErr );
	require_action( inTextRecord || ( inTextRecordSize == 0 ), exit, err = kDNSBadParamErr );
	require_action( ( inFlags & kDNSRegistrationFlagPreFormattedTextRecord ) || 
					( inTextRecordSize < sizeof( textRecord ) ), exit, err = kDNSBadParamErr );
	
	// Default to the local domain when null is passed in.
	
	if( !inDomain )
	{
		inDomain = kDNSLocalDomain;
	}
	
	// Set up the text record. If the pre-formatted flag is used, the input text is assumed to be a valid text record 
	// and is used directly. Otherwise, the input text is assumed to be raw text and is converted to a text record.
	
	textRecordPtr = (const mDNSu8 *) inTextRecord;
	if( !( inFlags & kDNSRegistrationFlagPreFormattedTextRecord ) )
	{
		// Convert the raw input text to a length-prefixed text record.
		
		if( inTextRecordSize > 0 )
		{
			textRecord[ 0 ] = (mDNSu8) inTextRecordSize;
			memcpy( &textRecord[ 1 ], inTextRecord, inTextRecordSize );
			textRecordPtr = textRecord;
			inTextRecordSize += 1;
		}
	}
	
	// Allocate the object and set it up. If the TXT record is larger than the standard RDataBody, allocate more space.
	
	size = sizeof( *objectPtr );
	if( inTextRecordSize > sizeof( RDataBody ) )
	{
		size += ( inTextRecordSize - sizeof( RDataBody ) );
	}
	
	err = DNSMemAlloc( size, &objectPtr );
	require_noerr( err, exit );
	memset( objectPtr, 0, size );
	
	objectPtr->flags 			= inFlags;
	objectPtr->callback 		= inCallBack;
	objectPtr->callbackContext 	= inCallBackContext;
	
	// Set up the interface for interface-specific operations.
	
	// Add the object to the list.
	
	objectPtr->next = gDNSRegistrationList;
	gDNSRegistrationList = objectPtr;
	
	// Convert the name, type, domain, and port to a format suitable for mDNS. If the name is NULL or an empty string, 
	// use the UTF-8 name of the system as the service name to make it easy for clients to use the standard name.
	// If we're using the system name (i.e. name is NULL), automatically rename on conflicts to keep things in sync.
	
	if( !inName || ( *inName == '\0' ) )
	{
		name = gMDNSPtr->nicelabel;
		inFlags |= kDNSRegistrationFlagAutoRenameOnConflict;
	}
	else
	{
		MakeDomainLabelFromLiteralString( &name, inName );
	}
	MakeDomainNameFromDNSNameString( &type, inType );
	MakeDomainNameFromDNSNameString( &domain, inDomain );
	port.b[ 0 ] = ( mDNSu8 )( inPort >> 8 );
	port.b[ 1 ] = ( mDNSu8 )( inPort >> 0 );
	
	// Set up the host name (if not using the default).
	
	host = mDNSNULL;
	if( inHost )
	{
		host = &tempHost;
		MakeDomainNameFromDNSNameString( host, inHost );
		AppendDomainName( host, &domain );
	}
		
	// Register the service with mDNS.
	
	err = mDNS_RegisterService( gMDNSPtr, &objectPtr->set, &name, &type, &domain, host, port, textRecordPtr, 
								(mDNSu16) inTextRecordSize, NULL, 0, inInterfaceID, 
								DNSRegistrationPrivateCallBack, objectPtr );
	require_noerr( err, exit );
	
	if( outRef )
	{
		*outRef = objectPtr;
	}
	
exit:
	if( err && objectPtr )
	{
		DNSRegistrationRemoveObject( objectPtr );
		DNSMemFree( objectPtr );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSNoSuchServiceRegistrationCreate
//===========================================================================================================================

DNSStatus
	DNSNoSuchServiceRegistrationCreate( 
		DNSRegistrationFlags		inFlags, 
		const char *				inName, 
		const char *				inType, 
		const char *				inDomain, 
		const char *				inInterfaceName, 
		DNSRegistrationCallBack		inCallBack, 
		void *						inCallBackContext, 
		DNSRegistrationRef *		outRef )
{	
	DNSStatus				err;
	size_t					size;
	DNSRegistration *		objectPtr;
	mDNSInterfaceID			interfaceID;
	domainlabel				name;
	domainname				type;
	domainname				domain;
	
	objectPtr = mDNSNULL;
	
	// Check parameters.
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSNoSuchServiceRegistrationCreateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	inFlags |= kDNSRegistrationFlagPrivateNoSuchService;
	require_action( inType, exit, err = kDNSBadParamErr );
	require_action( !inInterfaceName || 
					( strlen( inInterfaceName ) < sizeof( objectPtr->interfaceName ) ), exit, err = kDNSBadParamErr );
	
	// Default to the local domain when null is passed in.
	
	if( !inDomain )
	{
		inDomain = kDNSLocalDomain;
	}
	
	// Allocate the object and set it up. If the TXT record is larger than the standard RDataBody, allocate more space.
	
	size = sizeof( *objectPtr );
	
	err = DNSMemAlloc( size, &objectPtr );
	require_noerr( err, exit );
	memset( objectPtr, 0, size );
	
	objectPtr->flags 			= inFlags;
	objectPtr->callback 		= inCallBack;
	objectPtr->callbackContext 	= inCallBackContext;
	
	// Set up the interface for interface-specific operations.
	
	if( inInterfaceName && ( *inInterfaceName != '\0' ) )
	{
		strcpy( objectPtr->interfaceName, inInterfaceName );
		
		err = mDNSPlatformInterfaceNameToID( gMDNSPtr, inInterfaceName, &interfaceID );
		require_noerr( err, exit );
	}
	else
	{
		interfaceID = mDNSInterface_Any;
	}
	
	// Add the object to the list.
	
	objectPtr->next = gDNSRegistrationList;
	gDNSRegistrationList = objectPtr;
	
	// Convert the name, type, domain, and port to a format suitable for mDNS. If the name is NULL or an empty string, 
	// use the UTF-8 name of the system as the service name to make it easy for clients to use the standard name.
	
	if( !inName || ( *inName == '\0' ) )
	{
		name = gMDNSPtr->nicelabel;
	}
	else
	{
		MakeDomainLabelFromLiteralString( &name, inName );
	}
	MakeDomainNameFromDNSNameString( &type, inType );
	MakeDomainNameFromDNSNameString( &domain, inDomain );
	
	// Register the service with mDNS.
	
	err = mDNS_RegisterNoSuchService( gMDNSPtr, &objectPtr->set.RR_SRV, &name, &type, &domain, mDNSNULL, 
									  interfaceID, DNSNoSuchServiceRegistrationPrivateCallBack, objectPtr );
	require_noerr( err, exit );
	
	if( outRef )
	{
		*outRef = objectPtr;
	}
	
exit:
	if( err && objectPtr )
	{
		DNSRegistrationRemoveObject( objectPtr );
		DNSMemFree( objectPtr );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSRegistrationRelease
//===========================================================================================================================

DNSStatus	DNSRegistrationRelease( DNSRegistrationRef inRef, DNSRegistrationFlags inFlags )
{
	DNSStatus					err;
	DNSRegistrationEvent		event;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSRegistrationReleaseValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	
	// Notify the client of the registration release. Remove the object first so they cannot try to use it in the callback.
	
	inRef = DNSRegistrationRemoveObject( inRef );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	
	if( inRef->callback )
	{
		memset( &event, 0, sizeof( event ) );
		event.type = kDNSRegistrationEventTypeRelease;
		inRef->callback( inRef->callbackContext, inRef, kDNSNoErr, &event );
	}
	
	// Deregister from mDNS after everything else since it will call us back to free the memory.
	
	if( !( inRef->flags & kDNSRegistrationFlagPrivateNoSuchService ) )
	{
		err = mDNS_DeregisterService( gMDNSPtr, &inRef->set );
		require_noerr( err, exit );
	}
	else
	{
		err = mDNS_DeregisterNoSuchService( gMDNSPtr, &inRef->set.RR_SRV );
		require_noerr( err, exit );
	}
	
	// Note: Don't free here. Wait for mDNS to call us back with a mem free result.
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSRegistrationUpdate
//===========================================================================================================================

DNSStatus
	DNSRegistrationUpdate( 
		DNSRegistrationRef 			inRef, 
		DNSRecordFlags				inFlags, 
		DNSRegistrationRecordRef 	inRecord, 
		const void *				inData, 
		DNSCount					inSize, 
		DNSUInt32					inNewTTL )
{
	DNSStatus			err;
	AuthRecord *		rr;
	size_t				maxRDLength;
	RData *				newRData;
	
	newRData = mDNSNULL;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( DNSRegistrationFindObject( inRef ), exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSRegistrationUpdateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( inData || ( inSize == 0 ), exit, err = kDNSBadParamErr );
	
	// If a non-NULL record is specified, update it. Otherwise, use the standard TXT record.
	
	if( inRecord )
	{
		// $$$ TO DO: Add support for updating extra records (support adding and removing them too).
		
		rr = mDNSNULL;
		err = kDNSUnsupportedErr;
		require_noerr( err, exit );
	}
	else
	{
		rr = &inRef->set.RR_TXT;
	}
	
	// Allocate storage for the new data and set it up.
	
	maxRDLength = sizeof( RDataBody );
	if( inSize > maxRDLength )
	{
		maxRDLength = inSize;
	}
	err = DNSMemAlloc( ( sizeof( *newRData ) - sizeof( RDataBody ) ) + maxRDLength, &newRData );
	require_noerr( err, exit );
	
	newRData->MaxRDLength = (mDNSu16) maxRDLength;
	memcpy( &newRData->u, inData, inSize );
	
	// Update the record with mDNS.
	
	err = mDNS_Update( gMDNSPtr, rr, inNewTTL, (mDNSu16) inSize, newRData, DNSRegistrationUpdateCallBack );
	require_noerr( err, exit );
	
	newRData = mDNSNULL;
	
exit:
	if( newRData )
	{
		DNSMemFree( newRData );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSRegistrationPrivateCallBack
//===========================================================================================================================

mDNSlocal void	DNSRegistrationPrivateCallBack( mDNS * const inMDNS, ServiceRecordSet * const inSet, mStatus inResult )
{	
	DNSRegistrationRef			object;
	DNSRegistrationEvent		event;
	
	DNS_UNUSED( inMDNS );
	
	DNSServicesLock();
	
	// Exit if object is no longer valid. Should never happen.
	
	object = (DNSRegistrationRef) inSet->ServiceContext;
	require( object, exit );
	
	// Dispatch based on the status code.
	
	switch( inResult )
	{
		case mStatus_NoError:
			debugf( DEBUG_NAME "registration callback: \"%##s\" name successfully registered", inSet->RR_SRV.resrec.name.c );
			
			// Notify the client of a successful registration.
			
			if( object->callback )
			{
				memset( &event, 0, sizeof( event ) );
				event.type = kDNSRegistrationEventTypeRegistered;
				object->callback( object->callbackContext, object, kDNSNoErr, &event );
			}
			break;
		
		case mStatus_NameConflict:
		{
			DNSStatus		err;
			mDNSBool		remove;
			
			debugf( DEBUG_NAME "registration callback: \"%##s\" name conflict", inSet->RR_SRV.resrec.name.c );
			
			// Name conflict. If the auto-rename option is enabled, uniquely rename the service and re-register it. Otherwise, 
			// remove the object so they cannot try to use it in the callback and notify the client of the name conflict.
			
			remove = mDNStrue;
			if( object->flags & kDNSRegistrationFlagAutoRenameOnConflict )
			{
				err = mDNS_RenameAndReregisterService( inMDNS, inSet, mDNSNULL );
				check_noerr( err );
				if( err == mStatus_NoError )
				{
					debugf( DEBUG_NAME "registration callback: auto-renamed to \"%##s\"", inSet->RR_SRV.resrec.name.c );
					remove = mDNSfalse;
				}
			}
			if( remove )
			{
				object = DNSRegistrationRemoveObject( object );
				require( object, exit );
				
				// Notify the client of the name collision.
				
				if( object->callback )
				{
					memset( &event, 0, sizeof( event ) );
					event.type = kDNSRegistrationEventTypeNameCollision;
					object->callback( object->callbackContext, object, kDNSNoErr, &event );
				}
				
				// Notify the client that the registration is being released.
				
				if( object->callback )
				{
					memset( &event, 0, sizeof( event ) );
					event.type = kDNSRegistrationEventTypeRelease;
					object->callback( object->callbackContext, object, kDNSNoErr, &event );
				}
				
				// When a name conflict occurs, mDNS will not send a separate mem free result so free the memory here.
				
				DNSMemFree( object );
			}
			break;
		}
		
		case mStatus_MemFree:
			debugf( DEBUG_NAME "registration callback: \"%##s\" memory free", inSet->RR_SRV.resrec.name.c );
			
			if( object->set.RR_TXT.resrec.rdata != &object->set.RR_TXT.rdatastorage )
			{
				// Standard TXT record was updated with new data so free that data separately.
				
				DNSMemFree( object->set.RR_TXT.resrec.rdata );
			}
			DNSMemFree( object );
			break;
		
		default:
			debugf( DEBUG_NAME "registration callback: \"%##s\" unknown result %d", inSet->RR_SRV.resrec.name.c, inResult );
			break;
	}

exit:
	DNSServicesUnlock();
}

//===========================================================================================================================
//	DNSNoSuchServiceRegistrationPrivateCallBack
//===========================================================================================================================

mDNSlocal void	DNSNoSuchServiceRegistrationPrivateCallBack( mDNS * const inMDNS, AuthRecord * const inRR, mStatus inResult )
{	
	DNSRegistrationRef			object;
	DNSRegistrationEvent		event;
	
	DNS_UNUSED( inMDNS );
	
	DNSServicesLock();
	
	// Exit if object is no longer valid. Should never happen.
	
	object = (DNSRegistrationRef) inRR->RecordContext;
	require( object, exit );
	
	// Dispatch based on the status code.
	
	switch( inResult )
	{
		case mStatus_NoError:
			debugf( DEBUG_NAME "registration callback: \"%##s\" name successfully registered", inRR->resrec.name.c );
			
			// Notify the client of a successful registration.
			
			if( object->callback )
			{
				memset( &event, 0, sizeof( event ) );
				event.type = kDNSRegistrationEventTypeRegistered;
				object->callback( object->callbackContext, object, kDNSNoErr, &event );
			}
			break;
		
		case mStatus_NameConflict:
		{
			debugf( DEBUG_NAME "registration callback: \"%##s\" name conflict", inRR->resrec.name.c );
			
			// Name conflict. Name conflicts for no-such-service registrations often do not make sense since the main goal 
			// is to assert that no other service exists with a name. Because of this, name conflicts should be handled by
			// the code registering the no-such-service since it is likely that if another service is already using the 
			// name that the service registering the no-such-service should rename its other services as well. The name
			// collision client callback invoked here can do any of this client-specific behavior. It may be worth adding
			// support for the auto-rename feature in the future though, if that becomes necessary.
			
			object = DNSRegistrationRemoveObject( object );
			require( object, exit );
			
			// Notify the client of the name collision.
			
			if( object->callback )
			{
				memset( &event, 0, sizeof( event ) );
				event.type = kDNSRegistrationEventTypeNameCollision;
				object->callback( object->callbackContext, object, kDNSNoErr, &event );
			}
			
			// Notify the client that the registration is being released.
			
			if( object->callback )
			{
				memset( &event, 0, sizeof( event ) );
				event.type = kDNSRegistrationEventTypeRelease;
				object->callback( object->callbackContext, object, kDNSNoErr, &event );
			}
			
			// When a name conflict occurs, mDNS will not send a separate mem free result so free the memory here.
			
			DNSMemFree( object );
			break;
		}
		
		case mStatus_MemFree:
			debugf( DEBUG_NAME "registration callback: \"%##s\" memory free", inRR->resrec.name.c );
			
			DNSMemFree( object );
			break;
		
		default:
			debugf( DEBUG_NAME "registration callback: \"%##s\" unknown result %d", inRR->resrec.name.c, inResult );
			break;
	}

exit:
	DNSServicesUnlock();
}

//===========================================================================================================================
//	DNSRegistrationUpdateCallBack
//===========================================================================================================================

mDNSlocal void	DNSRegistrationUpdateCallBack( mDNS * const inMDNS, AuthRecord * const inRR, RData *inOldData )
{
	DNS_UNUSED( inMDNS );
	
	check( inRR );
	check( inOldData );
	
	if( inOldData != &inRR->rdatastorage )
	{
		DNSMemFree( inOldData );
	}
}

//===========================================================================================================================
//	DNSRegistrationFindObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSRegistrationRef *	DNSRegistrationFindObject( DNSRegistrationRef inRef )
{
	DNSRegistration **		p;
	
	for( p = &gDNSRegistrationList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	return( p );
}

//===========================================================================================================================
//	DNSRegistrationRemoveObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSRegistrationRef	DNSRegistrationRemoveObject( DNSRegistrationRef inRef )
{
	DNSRegistration **		p;
	DNSRegistration *		found;
	
	for( p = &gDNSRegistrationList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	found = *p;
	if( found )
	{
		*p = found->next;
	}
	return( found );
}

#if 0
#pragma mark -
#pragma mark == Domain Registration ==
#endif

//===========================================================================================================================
//	DNSDomainRegistrationCreate
//===========================================================================================================================

DNSStatus
	DNSDomainRegistrationCreate( 
		DNSDomainRegistrationFlags		inFlags, 
		const char *					inName, 
		DNSDomainRegistrationType		inType, 
		DNSDomainRegistrationRef *		outRef )
{
	DNSStatus					err;
	DNSDomainRegistration *		objectPtr;
		
	objectPtr = mDNSNULL;
	
	// Check parameters.
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSDomainRegistrationCreateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( inName, exit, err = kDNSBadParamErr );
	require_action( inType < kDNSDomainRegistrationTypeMax, exit, err = kDNSBadParamErr );
	
	// Allocate the object and set it up.
	
	err = DNSMemAlloc( sizeof( *objectPtr ), &objectPtr );
	require_noerr( err, exit );
	memset( objectPtr, 0, sizeof( *objectPtr ) );
	
	objectPtr->flags = inFlags;
	
	// Add the object to the list.
	
	objectPtr->next = gDNSDomainRegistrationList;
	gDNSDomainRegistrationList = objectPtr;
	
	// Register the domain with mDNS.
	
	err = mDNS_AdvertiseDomains( gMDNSPtr, &objectPtr->rr, (mDNS_DomainType) inType, mDNSInterface_Any, (char *) inName );
	require_noerr( err, exit );
	
	if( outRef )
	{
		*outRef = objectPtr;
	}
	
exit:
	if( err && objectPtr )
	{
		DNSDomainRegistrationRemoveObject( objectPtr );
		DNSMemFree( objectPtr );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSDomainRegistrationRelease
//===========================================================================================================================

DNSStatus	DNSDomainRegistrationRelease( DNSDomainRegistrationRef inRef, DNSDomainRegistrationFlags inFlags )
{
	DNSStatus		err;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSDomainRegistrationReleaseValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	
	// Remove the object and deregister the domain with mDNS.
	
	inRef = DNSDomainRegistrationRemoveObject( inRef );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	
	mDNS_StopAdvertiseDomains( gMDNSPtr, &inRef->rr );
	
	// Release the memory used by the object.
	
	DNSMemFree( inRef );
	err = kDNSNoErr;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSDomainRegistrationRemoveObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSDomainRegistrationRef	DNSDomainRegistrationRemoveObject( DNSDomainRegistrationRef inRef )
{
	DNSDomainRegistration **		p;
	DNSDomainRegistration *			found;
	
	for( p = &gDNSDomainRegistrationList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	found = *p;
	if( found )
	{
		*p = found->next;
	}
	return( found );
}

#if 0
#pragma mark -
#pragma mark == Domain Registration ==
#endif

//===========================================================================================================================
//	DNSHostRegistrationCreate
//===========================================================================================================================

DNSStatus
	DNSHostRegistrationCreate( 
		DNSHostRegistrationFlags	inFlags, 
		const char *				inName, 
		const char *				inDomain, 
		const DNSNetworkAddress *	inAddr, 
		mDNSInterfaceID				inInterfaceID, 
		DNSHostRegistrationCallBack	inCallBack, 
		void *						inCallBackContext, 
		DNSHostRegistrationRef *	outRef )
{
	DNSStatus					err;
	domainname					name;
	DNSHostRegistration *		object;
	mDNSv4Addr					ip;
	char						buffer[ 64 ];
	
	object = mDNSNULL;
	
	// Check parameters.
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( ( inFlags & ~kDNSHostRegistrationCreateValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	require_action( inName, exit, err = kDNSBadParamErr );
	require_action( inAddr && ( inAddr->addressType == kDNSNetworkAddressTypeIPv4 ), exit, err = kDNSUnsupportedErr );
	
	// Default to the local domain when null is passed in.
	
	if( !inDomain )
	{
		inDomain = kDNSLocalDomain;
	}
	
	// If the caller only wants to add if not found, check if a host with this name was already registered.
	
	MakeDomainNameFromDNSNameString( &name, inName );
	AppendDNSNameString( &name, inDomain );
	
	if( inFlags & kDNSHostRegistrationFlagOnlyIfNotFound )
	{
		object = DNSHostRegistrationFindObjectByName( &name );
		if( object )
		{
			++object->refCount;
			if( outRef )
			{
				*outRef = object;
			}
			object = mDNSNULL;
			err = kDNSNoErr;
			goto exit;
		}
	}
	
	// Allocate the object and set it up.
	
	err = DNSMemAlloc( sizeof( *object ), &object );
	require_noerr( err, exit );
	memset( object, 0, sizeof( *object ) );
	
	MakeDomainLabelFromLiteralString( &object->name, inName );
	MakeDomainLabelFromLiteralString( &object->domain, inDomain );
	object->refCount		= 1;
	object->flags 			= inFlags;
	object->callback		= inCallBack;
	object->callbackContext	= inCallBackContext;
	
	// Convert the IP address to a format suitable for mDNS.
	
	ip.NotAnInteger = inAddr->u.ipv4.addr.v32;

	// Set up the resource records and name.
	
	mDNS_SetupResourceRecord( &object->RR_A,   mDNSNULL, inInterfaceID, kDNSType_A,   60, kDNSRecordTypeUnique, 
							  DNSHostRegistrationPrivateCallBack, object );
	mDNS_SetupResourceRecord( &object->RR_PTR, mDNSNULL, inInterfaceID, kDNSType_PTR, 60, kDNSRecordTypeKnownUnique, 
							  DNSHostRegistrationPrivateCallBack, object );
	
	AssignDomainName( object->RR_A.resrec.name, name );
	
	mDNS_snprintf( buffer, sizeof( buffer ), "%d.%d.%d.%d.in-addr.arpa.", ip.b[ 3 ], ip.b[ 2 ], ip.b[ 1 ], ip.b[ 0 ] );
	MakeDomainNameFromDNSNameString( &object->RR_PTR.resrec.name, buffer );
	
	object->RR_A.resrec.rdata->u.ip = ip;
	AssignDomainName( object->RR_PTR.resrec.rdata->u.name, object->RR_A.resrec.name );
	
	// Add the object to the list.
	
	object->next = gDNSHostRegistrationList;
	gDNSHostRegistrationList = object;
	
	// Register with mDNS.

	err = mDNS_Register( gMDNSPtr, &object->RR_A );
	require_noerr( err, exit );
	
	err = mDNS_Register( gMDNSPtr, &object->RR_PTR );
	if( err != mStatus_NoError )
	{
		mDNS_Deregister( gMDNSPtr, &object->RR_A );
	}
	require_noerr( err, exit );
	
	if( outRef )
	{
		*outRef = object;
	}
	
exit:
	if( err && object )
	{
		DNSHostRegistration **		p;
		
		p = DNSHostRegistrationFindObject( object );
		*p = object->next;
		DNSMemFree( object );
	}
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSHostRegistrationRelease
//===========================================================================================================================

DNSStatus	DNSHostRegistrationRelease( DNSHostRegistrationRef inRef, DNSHostRegistrationFlags inFlags )
{
	DNSStatus						err;
	DNSHostRegistrationRef *		p;
	
	DNSServicesLock();
	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	require_action( ( inFlags & ~kDNSHostRegistrationReleaseValidFlags ) == 0, exit, err = kDNSBadFlagsErr );
	
	// Decrement the reference count and if it drops to 0, remove the object and deregister with mDNS.
	
	p = DNSHostRegistrationFindObject( inRef );
	inRef = *p;
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	
	check( inRef->refCount > 0 );
	if( --inRef->refCount == 0 )
	{
		*p = inRef->next;
		
		mDNS_Deregister( gMDNSPtr, &inRef->RR_A );
		mDNS_Deregister( gMDNSPtr, &inRef->RR_PTR );
	
		// Release the memory used by the object.
		
		DNSMemFree( inRef );
	}
	err = kDNSNoErr;
	
exit:
	DNSServicesUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSHostRegistrationFindObject
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSHostRegistrationRef *	DNSHostRegistrationFindObject( DNSHostRegistrationRef inRef )
{
	DNSHostRegistration **		p;
	
	for( p = &gDNSHostRegistrationList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	return( p );
}

//===========================================================================================================================
//	DNSHostRegistrationFindObjectByName
//
//	Warning: Assumes the DNS lock is held.
//===========================================================================================================================

mDNSlocal DNSHostRegistrationRef	DNSHostRegistrationFindObjectByName( const domainname *inName )
{
	DNSHostRegistration *		p;
	
	check( inName );
	
	for( p = gDNSHostRegistrationList; p; p = p->next )
	{
		if( SameDomainName( &p->RR_A.resrec.name, inName ) )
		{
			break;
		}
	}
	return( p );
}

//===========================================================================================================================
//	DNSHostRegistrationPrivateCallBack
//===========================================================================================================================

mDNSlocal void	DNSHostRegistrationPrivateCallBack( mDNS * const inMDNS, AuthRecord *const inRR, mStatus inResult )
{	
	DNSHostRegistrationRef		object;
	
	DNS_UNUSED( inMDNS );
	
	DNSServicesLock();
	
	// Exit if object is no longer valid. Should never happen.
	
	object = (DNSHostRegistrationRef) inRR->RecordContext;
	require( object, exit );
	
	// Dispatch based on the status code.
	
	if( inResult == mStatus_NoError )
	{
		debugf( DEBUG_NAME "host registration callback: \"%##s\" name successfully registered", inRR->resrec.name.c );
		if( object->callback )
		{
			object->callback( object->callbackContext, object, kDNSNoErr, mDNSNULL );
		}
	}
	else if( inResult == mStatus_NameConflict )
	{
		debugf( DEBUG_NAME "host registration callback: \"%##s\" name conflict", inRR->resrec.name.c );
		
		if( object->flags & kDNSHostRegistrationFlagAutoRenameOnConflict )
		{
			DNSStatus		err;
			domainname		name;
			
			// De-register any resource records still registered.
			
			if( object->RR_A.resrec.RecordType )
			{
				mDNS_Deregister( gMDNSPtr, &object->RR_A );
			}
			if( object->RR_PTR.resrec.RecordType )
			{
				mDNS_Deregister( gMDNSPtr, &object->RR_PTR );
			}
			
			// Rename the host and re-register to try again.
			
			IncrementLabelSuffix( &object->name, mDNSfalse );
			name.c[ 0 ] = 0;
			AppendDomainLabel( &name, &object->name );
			AppendDomainLabel( &name, &object->domain );
			AssignDomainName( object->RR_PTR.resrec.name, name );
			
			err = mDNS_Register( gMDNSPtr, &object->RR_A );
			check_noerr( err );
			
			err = mDNS_Register( gMDNSPtr, &object->RR_PTR );
			check_noerr( err );
		}
		else
		{
			if( object->callback )
			{
				object->callback( object->callbackContext, object, kDNSNameConflictErr, mDNSNULL );
			}
		}
	}
	else
	{
		debugf( DEBUG_NAME "host registration callback: \"%##s\" unknown result", inRR->resrec.name.c, inResult );
	}
	
exit:
	DNSServicesUnlock();
}


DNSStatus
DNSQueryRecordCreate(
	DNSQueryRecordFlags		inFlags,
	mDNSu32						inInterfaceIndex,
	const char 				*	inFullName,
	mDNSu16						inRRType,
	mDNSu16          			inRRClass,
	DNSQueryRecordCallBack	inCallBack,
	void						*	inCallBackContext,
	DNSQueryRecordRef		*	outRef)
{
	DNSStatus			err;
	DNSQueryRecord *	object;
	
	object = mDNSNULL;
	
	// Check parameters.
	
	DNSServicesLock();

	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inFullName, exit, err = kDNSBadParamErr );
	
	// Allocate the object and set it up.
	err = DNSMemAlloc( sizeof( *object ), &object );
	require_noerr( err, exit );
	memset( object, 0, sizeof( *object ) );

	// Allocate the question
	err = DNSMemAlloc( sizeof(*object->question), &object->question );
	require_noerr( err, exit);
	memset( object->question, 0, sizeof( *object->question ) );

	object->refCount			= 1;
	object->flags 				= inFlags;
	object->callback			= inCallBack;
	object->callbackContext	= inCallBackContext;

	if (!MakeDomainNameFromDNSNameString( &object->question->qname, inFullName ))
	{
		err = kDNSBadParamErr;
		goto exit;
	}

	object->question->QuestionContext	=	object;
	object->question->QuestionCallback	=  DNSQueryRecordPrivateCallBack;
	object->question->qtype					=	inRRType;
	object->question->qclass				=	inRRClass;
	object->question->InterfaceID 		=	inInterfaceIndex;
	// q->Target 				=	zeroAddr;

/*
	if (inFlags & kDNSQueryRecordFlagsLongLivedQuery)
	{
		q->LongLived = mDNStrue;
	}
*/

	// Register with mDNS.

	err = mDNS_StartQuery( gMDNSPtr, object->question );
	require_noerr( err, exit );
	
	object->next = gDNSQueryRecordList;
	gDNSQueryRecordList = object;

	if( outRef )
	{
		*outRef = object;
	}
	
exit:

	if( err && object )
	{
		if (object->question)
		{
			DNSMemFree(object->question);
		}

		DNSMemFree( object );
	}

	DNSServicesUnlock();

	return( err );
}


DNSStatus
DNSQueryRecordRelease(
			DNSQueryRecordRef inRef)

{
	DNSStatus err;
	
	DNSServicesLock();

	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inRef, exit, err = kDNSBadReferenceErr );
	
	// Stop service and domain browsing and remove any resolvers dependent on this browser.

	mDNS_StopQuery(gMDNSPtr, inRef->question);
	
	// Remove the object from the list.
	
	inRef = DNSQueryRecordRemoveObject( inRef );
	require_action( inRef, exit, err = kDNSBadReferenceErr );

	// Release the memory used by the question

	DNSMemFree( inRef->question );
	
	// Release the memory used by the object.
	
	DNSMemFree( inRef );
	err = kDNSNoErr;
	
exit:

	DNSServicesUnlock();
	return( err );
}


mDNSlocal void
DNSQueryRecordPrivateCallBack(
						mDNS						*	const inMDNS,
						DNSQuestion				*			question,
						const ResourceRecord	*	const answer,
						mDNSBool								add)
{
	DNSQueryRecord	*	object;
	char					name[MAX_ESCAPED_DOMAIN_NAME];
	DNSStatus			err;

	require_action( gMDNSPtr, exit, err = kDNSNotInitializedErr );
	require_action( inMDNS, exit, err = kDNSBadReferenceErr );

	object = (DNSQueryRecord*) question->QuestionContext;

	ConvertDomainNameToCString(&answer->name, name);

	(*object->callback)(object->callbackContext, object, 0, add ? 1 : 0, (mDNSu32) answer->InterfaceID, name, answer->rrtype, answer->rrclass, answer->rdlength, (void*) &answer->rdata->u, add ? answer->rroriginalttl : 0 );

exit:

	return;
}


mDNSlocal DNSQueryRecordRef
DNSQueryRecordRemoveObject( DNSQueryRecordRef inRef )
{
	DNSQueryRecord **		p;
	DNSQueryRecord *		found;
	
	for( p = &gDNSQueryRecordList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}

	found = *p;
	if( found )
	{
		*p = found->next;
	}
	return( found );
}


#if 0
#pragma mark -
#pragma mark == Utilities ==
#endif

//===========================================================================================================================
//	DNSMemAlloc
//===========================================================================================================================

mDNSlocal DNSStatus	DNSMemAlloc( size_t inSize, void *outMem )
{
	void *		mem;
	
	check( inSize > 0 );
	check( outMem );
	
	mem = malloc( inSize );
	*( (void **) outMem ) = mem;
	if( mem )
	{
		return( kDNSNoErr );
	}
	return( kDNSNoMemoryErr );
}

//===========================================================================================================================
//	DNSMemFree
//===========================================================================================================================

mDNSlocal void	DNSMemFree( void *inMem )
{
	check( inMem );
	
	free( inMem );
}

//===========================================================================================================================
//	DNSDynamicTextRecordBuildEscaped
//===========================================================================================================================

DNSStatus	DNSDynamicTextRecordBuildEscaped( const char *inFormat, void *outTextRecord, size_t *outSize )
{
	DNSStatus		err;
	size_t			size;
	void *			textRecord;
	
	textRecord = NULL;

	// Calculate the size of the built text record, allocate a buffer for it, then build it in that buffer.
	
	err = DNSTextRecordValidate( inFormat, 0x7FFFFFFF, NULL, &size );
	require_noerr( err, exit );
	
	textRecord = malloc( size );
	require_action( textRecord, exit, err = kDNSNoMemoryErr );
	
	err = DNSTextRecordValidate( inFormat, size, textRecord, &size );
	require_noerr( err, exit );
	
	// Success!
	
	if( outTextRecord )
	{
		*( (void **) outTextRecord ) = textRecord;
		textRecord = NULL;
	}
	if( outSize )
	{
		*outSize = size;
	}
	
exit:
	if( textRecord )
	{
		free( textRecord );
	}
	return( err );
}

//===========================================================================================================================
//	DNSDynamicTextRecordAppendCString
//===========================================================================================================================

DNSStatus	DNSDynamicTextRecordAppendCString( void *ioTxt, size_t *ioTxtSize, const char *inName, const char *inValue )
{
	DNSStatus		err;
	size_t			valueSize;
	
	require_action( inName, exit, err = kDNSBadParamErr );
	require_action( inValue, exit, err = kDNSBadParamErr );
	
	if( inValue != kDNSTextRecordStringNoValue )
	{
		valueSize = strlen( inValue );
	}
	else
	{
		valueSize = kDNSTextRecordNoSize;
	}
	err = DNSDynamicTextRecordAppendData( ioTxt, ioTxtSize, inName, inValue, valueSize );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSDynamicTextRecordAppendData
//===========================================================================================================================

DNSStatus
	DNSDynamicTextRecordAppendData( 
		void *			ioTxt, 
		size_t * 		ioTxtSize, 
		const char *	inName, 
		const void *	inValue, 
		size_t			inValueSize )
{
	DNSStatus		err;
	size_t			oldSize;
	size_t			newSize;
	int				hasName;
	int				hasValue;
	void **			bufferPtr;
	void *			newBuffer;
	
	require_action( ioTxt, exit, err = kDNSBadParamErr );
	require_action( ioTxtSize, exit, err = kDNSBadParamErr );
	require_action( inName, exit, err = kDNSBadParamErr );
	
	// Check for special flags to indicate no name or no value is used (e.g. "color" instead of "color=").
	
	hasName	 = ( inName != kDNSTextRecordStringNoValue ) && ( *inName != '\0' );
	hasValue = ( inValue != kDNSTextRecordNoValue ) && ( inValueSize != kDNSTextRecordNoSize );
	require_action( hasName || hasValue, exit, err = kDNSUnsupportedErr );
	
	// Calculate the size needed for the new data (old size + length byte + name size + '=' + value size).
	
	oldSize = *ioTxtSize;
	newSize = oldSize + 1;				// add length byte size
	if( hasName )
	{
		newSize += strlen( inName );	// add name size
		if( hasValue )
		{
			newSize += 1;				// add '=' size
		}
	}
	if( hasValue )
	{
		newSize += inValueSize;			// add value size
	}
	
	// Reallocate the buffer to make room for the new data.
	
	bufferPtr = (void **) ioTxt;
	newBuffer = realloc( *bufferPtr, newSize );
	require_action( newBuffer, exit, err = kDNSNoMemoryErr );
	*bufferPtr = newBuffer;
	
	err = DNSTextRecordAppendData( newBuffer, oldSize, newSize, inName, inValue, inValueSize, &newSize );
	require_noerr( err, exit );
	
	// Success!
	
	*ioTxtSize = newSize;
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSDynamicTextRecordRelease
//===========================================================================================================================

void	DNSDynamicTextRecordRelease( void *inTxt )
{
	if( inTxt )
	{
		free( inTxt );
	}
}

//===========================================================================================================================
//	DNSTextRecordAppendCString
//===========================================================================================================================

DNSStatus
	DNSTextRecordAppendCString( 
		void *			inTxt, 
		size_t 			inTxtSize, 
		size_t 			inTxtMaxSize, 
		const char *	inName, 
		const char *	inValue, 
		size_t *		outTxtSize )
{
	DNSStatus		err;
	size_t			valueSize;
	
	require_action( inName, exit, err = kDNSBadParamErr );
	require_action( inValue, exit, err = kDNSBadParamErr );
	
	if( inValue != kDNSTextRecordStringNoValue )
	{
		valueSize = strlen( inValue );
	}
	else
	{
		valueSize = kDNSTextRecordNoSize;
	}
	err = DNSTextRecordAppendData( inTxt, inTxtSize, inTxtMaxSize, inName, inValue, valueSize, outTxtSize );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSTextRecordAppendData
//===========================================================================================================================

DNSStatus
	DNSTextRecordAppendData( 
		void *			inTxt, 
		size_t 			inTxtSize, 
		size_t 			inTxtMaxSize, 
		const char *	inName, 
		const void *	inValue, 
		size_t			inValueSize, 
		size_t *		outTxtSize )
{
	DNSStatus			err;
	mDNSu8 *			p;
	int					hasName;
	int					hasValue;
	size_t 				size;
	size_t				newSize;
	const mDNSu8 *		q;
	
	require_action( inTxt, exit, err = kDNSBadParamErr );
	require_action( inName, exit, err = kDNSBadParamErr );
	
	// Check for special flags to indicate no name or no value is used (e.g. "color" instead of "color=").
	
	hasName	 = ( inName != kDNSTextRecordStringNoValue ) && ( *inName != '\0' );
	hasValue = ( inValue != kDNSTextRecordNoValue ) && ( inValueSize != kDNSTextRecordNoSize );
	require_action( hasName || hasValue, exit, err = kDNSUnsupportedErr );
	
	// Calculate the size and make sure there is enough total room and enough room in an individual segment.

	size = 0;
	if( hasName )
	{
		size += strlen( inName );		// add name size
		if( hasValue )
		{
			size += 1;					// add '=' size
		}
	}
	if( hasValue )
	{
		size += inValueSize;			// add value size
	}
	newSize = inTxtSize + 1 + size;		// old size + length byte + new data
	
	require_action( size < 256, exit, err = kDNSNoMemoryErr );
	require_action( newSize <= inTxtMaxSize, exit, err = kDNSNoMemoryErr );
	
	// Write the length-prefix byte containing the size of this segment.
	
	p = ( (mDNSu8 *) inTxt ) + inTxtSize;
	*p++ = (mDNSu8) size;
	
	// Copy the name.
	
	if( hasName )
	{
		q = (const mDNSu8 *) inName;
		while( *q != '\0' )
		{
			*p++ = *q++;
		}
		if( hasValue )
		{
			*p++ = '=';
		}
	}
	if( hasValue )
	{
		// Copy the value.
		
		q = (const mDNSu8 *) inValue;
		while( inValueSize-- > 0 )
		{
			*p++ = *q++;
		}
	}
	
	// Success!
	
	if( outTxtSize )
	{
		*outTxtSize = newSize;
	}
	err = kDNSNoErr;
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSTextRecordEscape
//===========================================================================================================================

DNSStatus	DNSTextRecordEscape( const void *inTextRecord, size_t inTextSize, char **outEscapedString )
{
	DNSStatus				err;
	const DNSUInt8 *		src;
	const DNSUInt8 *		end;
	DNSUInt8 *				dstStorage;
	DNSUInt8 *				dst;
	int						size;
	
	check( inTextRecord || ( inTextSize == 0 ) );
	
	// Mac OS X uses a single null-terminated string to hold all the text record data with a \001 byte to delimit 
	// individual records within the entire block. The following code converts a packed array of length-prefixed 
	// records into a single \001-delimited, null-terminated string. Allocate size + 1 for the null terminator.
	
	dstStorage = (DNSUInt8 *) malloc( inTextSize + 1 );
	require_action( dstStorage, exit, err = kDNSNoMemoryErr );
	dst = dstStorage;
	
	if( inTextSize > 0 )
	{
		src	= (const DNSUInt8 *) inTextRecord;
		end = src + inTextSize;
		while( src < end )
		{
			size = *src++;
			if( ( src + size ) > end )
			{
				// Malformed TXT record. Most likely an old-style TXT record.
				
				src = NULL;
				break;
			}
			while( size-- > 0 )
			{
				*dst++ = *src++;
			}
			*dst++ = '\001';	// \001 record separator. May be overwritten later if this is the last record.
		}
		check( ( dst - dstStorage ) <= inTextSize );
		if( src != end )
		{
			// Malformed TXT record. Assume an old-style TXT record and use the TXT record as a whole.
			
			memcpy( dstStorage, inTextRecord, inTextSize );
			dstStorage[ inTextSize ] = '\0';
		}
		else
		{
			dstStorage[ inTextSize - 1 ] = '\0';
		}
	}
	else
	{
		// No text record data so just return an empty string.
		
		*dst = '\0';
	}
	
	// Success!
	
	if( outEscapedString )
	{
		*outEscapedString = (char *) dstStorage;
		dstStorage = NULL;
	}
	err = kDNSNoErr;

exit:
	if( dstStorage )
	{
		free( dstStorage );
	}
	return( err );
}

//===========================================================================================================================
//	DNSNameValidate
//===========================================================================================================================

DNSStatus	DNSNameValidate( const char *inName )
{
	DNSStatus		err;
	mDNSu8 *		p;
	domainname		name;
		
	p = MakeDomainNameFromDNSNameString( &name, inName );
	if( p )
	{
		err = kDNSNoErr;
	}
	else
	{
		err = kDNSBadParamErr;
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceTypeValidate
//===========================================================================================================================

DNSStatus	DNSServiceTypeValidate( const char *inServiceType )
{
	DNSStatus		err;
	mDNSu8 *		p;
	domainname		type;
	domainname		domain;
	domainname		fqdn;
	
	// Construct a fake fully-qualified domain name with a known good domain and the service type to be verified since 
	// there is currently no canned way to test just a service type by itself.
	
	p = MakeDomainNameFromDNSNameString( &type, inServiceType );
	if( !p )
	{
		err = kDNSBadParamErr;
		goto exit;
	}
	
	p = MakeDomainNameFromDNSNameString( &domain, "local." );
	if( !p )
	{
		err = kDNSBadParamErr;
		goto exit;
	}
	
	p = ConstructServiceName( &fqdn, mDNSNULL, &type, &domain );
	if( !p )
	{
		err = kDNSBadParamErr;
		goto exit;
	}
	
	err = kDNSNoErr;
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSTextRecordValidate
//===========================================================================================================================

DNSStatus	DNSTextRecordValidate( const char *inText, size_t inMaxSize, void *outRecord, size_t *outActualSize )
{
	DNSStatus			err;
	const mDNSu8 *		p;
	size_t				totalSize;
	mDNSu8				sectionSize;
	mDNSu8 *			dst;
	mDNSu8 *			section;
	
	require_action( inText, exit, err = kDNSBadParamErr );
	
	// A DNS TXT record consists of a packed block of length-prefixed strings of up to 255 characters each. To allow 
	// this to be described with a null-terminated C-string, a special escape sequence of \001 is used to separate 
	// individual character strings within the C-string.
	
	totalSize 	= 0;
	sectionSize = 0;
	dst			= (mDNSu8 *) outRecord;
	section		= dst;
	
	p = (const mDNSu8 *) inText;	
	while( *p != '\0' )
	{
		++totalSize;
		if( totalSize >= inMaxSize )
		{
			err = kDNSBadParamErr;
			goto exit;
		}
				
		if( *p == '\001' )
		{
			// Separator Escape sequence, start a new string section.
			
			if( sectionSize <= 0 )
			{
				err = kDNSBadParamErr;
				goto exit;
			}
			sectionSize = 0;
			if( section )
			{
				section = &dst[ totalSize ];
				section[ 0 ] = 0;
			}
		}
		else
		{
			if( sectionSize >= 255 )
			{
				err = kDNSBadParamErr;
				goto exit;
			}
			++sectionSize;
			if( section )
			{
				section[ 0 ] = sectionSize;
				section[ sectionSize ] = *p;
			}
		}
		++p;
	}
	++totalSize;
	
	// Success!
	
	if( outActualSize )
	{
		*outActualSize = totalSize;
	}
	err = kDNSNoErr;
	
exit:
	return( err );
}

//===========================================================================================================================
//	MDNSAddrToDNSAddress
//===========================================================================================================================

mDNSlocal void	MDNSAddrToDNSAddress( const mDNSAddr *inAddr, DNSNetworkAddress *outAddr )
{
	switch( inAddr->type )
	{
		case mDNSAddrType_IPv4:
			outAddr->addressType		= kDNSNetworkAddressTypeIPv4;
			outAddr->u.ipv4.addr.v32 	= inAddr->ip.v4.NotAnInteger;
			break;
		
		case mDNSAddrType_IPv6:
			outAddr->addressType			= kDNSNetworkAddressTypeIPv6;
			outAddr->u.ipv6.addr.v32[ 0 ] 	= inAddr->ip.v6.l[ 0 ];
			outAddr->u.ipv6.addr.v32[ 1 ] 	= inAddr->ip.v6.l[ 1 ];
			outAddr->u.ipv6.addr.v32[ 2 ] 	= inAddr->ip.v6.l[ 2 ];
			outAddr->u.ipv6.addr.v32[ 3 ] 	= inAddr->ip.v6.l[ 3 ];
			break;
		
		default:
			outAddr->addressType = kDNSNetworkAddressTypeInvalid;
			break;
	}
}

#ifdef	__cplusplus
	}
#endif

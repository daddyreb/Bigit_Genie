/*
 * Copyright 2003, 2004 Porchdog Software. All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without modification,
 *	are permitted provided that the following conditions are met:
 *
 *		1. Redistributions of source code must retain the above copyright notice,
 *		   this list of conditions and the following disclaimer.   
 *		2. Redistributions in binary form must reproduce the above copyright notice,
 *		   this list of conditions and the following disclaimer in the documentation
 *		   and/or other materials provided with the distribution.
 *
 *	THIS SOFTWARE IS PROVIDED BY PORCHDOG SOFTWARE ``AS IS'' AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE HOWL PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *	OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The views and conclusions contained in the software and documentation are those
 *	of the authors and should not be interpreted as representing official policies,
 *	either expressed or implied, of Porchdog Software.
 */

/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*
    Change History (most recent first):
    
        $Log: win32_main.h,v $
        Revision 1.2  2004/07/29 06:18:36  sculi2000
        major reorg, use .def files, port two libraries to Win32

        Revision 1.1  2004/02/25 08:55:39  sculi2000
        first crack at 0.9.4, make mDNSResponder a shared library

        Revision 1.8  2004/02/11 05:51:24  sculi2000
        Single library, got to work on Windows

        Revision 1.7  2004/01/11 17:58:03  sculi2000
        fix installer, verify compile on VS 6.0, update copyright dates

        Revision 1.6  2003, 2004/12/17 20:31:46  sculi2000
        oops...didn't quite get the find/replace right last time!  This time for sure.

        Revision 1.5  2003, 2004/12/17 20:13:16  sculi2000
        finished reference change to Porchdog Software.

        Revision 1.4  2003, 2004/12/02 21:20:56  sculi2000
        fixed some list management bugs in salt, corby...compiles release/debug on VS 6.0, compiles release/debug on VS .NET...'cept for explorer bar

        Revision 1.3  2003, 2004/10/23 17:12:18  sculi2000
        sync with Apple code, new APIs for sw_rendezvous

        Revision 1.2  2003, 2004/08/22 17:08:30  sculi2000
        major re-org, fixed event dispatching on Windows, rendezvous cleanup

        Revision 1.1.1.1  2003, 2004/06/14 06:53:57  sculi2000
        Initial import.

 * 
 *    Rev 1.0   Apr 27 2003, 2004 21:32:52   Scott Herscher
 * Initial revision.
        Revision 1.2  2003, 2004/03/31 19:04:44  scott
        fixed memory leaks, added dynamic interface notifications, lots and lots of clean up

        Revision 1.1  2003, 2004/03/29 22:49:07  scott
        lots of cleanup, consolidated in one directory, fixed some memory leaks

*/

#ifndef	_MDNS_WIN32_H
#define	_MDNS_WIN32_H


#ifdef	__cplusplus
extern "C"
{
#endif


#ifdef	__cplusplus
}
#endif


#endif


#include <Windows.h>
#include "WlanApiInvoker.h"
#include "DiagnoseDll.h"
#include <stdio.h>
#include <Rpc.h>
#include <string>
#include <TCHAR.h>
#include "WiFiUtilities.h"

#define NICGUID_LEN	  36
#define PARAMETER_LEN 10
#define MAX_NETWORKKEY_LEN 64
#define MAX_PREFERRED_NETWORKS 32
#define MAX_TRUSTED_ROOT_CA_LEN 256
#define MAX_CERT_NUM 10
#define HASH_LEN 20

#define EAP_TYPE_GTC 6

#define WLSAMPLE_REASON_STRING_LEN 256   // the max lenght of the reason string in characters

#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"Rpcrt4.lib")

#if 0 
//
// Retrieve the thumbprint of the certificate
//
DWORD GetCertificate(IN WCHAR* pwcTrustRootCA, 
					 IN PBYTE  pbCertHash,
					 IN DWORD* pcbCertHash)
{
	HCERTSTORE		hCertStore;
	WCHAR			*pwcSubjectName;
	DWORD			cwcSubjectName; 
	PBYTE			pbSHA1;
	DWORD			cbSHA1;
	PCCERT_CONTEXT	pCertContext = NULL;
	DWORD			dwRet;

	dwRet = NO_ERROR;

	if( ( hCertStore = CertOpenStore( CERT_STORE_PROV_SYSTEM, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		( HCRYPTPROV ) NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE,
		L"ROOT" ) ) )
	{
		BOOL	bFoundCert = FALSE;

		while( ( pCertContext = CertEnumCertificatesInStore( hCertStore,
			pCertContext ) ) &&
			bFoundCert == FALSE )
		{
			if( ( cwcSubjectName  = CertGetNameString( pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0, 
				NULL,
				NULL,
				0 ) ) > 0 )
			{
				if( ( pwcSubjectName = ( WCHAR* ) malloc( cwcSubjectName * sizeof( WCHAR ) ) ) )
				{
					if( CertGetNameStringW( pCertContext,
						CERT_NAME_SIMPLE_DISPLAY_TYPE,
						0, 
						NULL,
						pwcSubjectName,
						cwcSubjectName ) > 0 )
					{
						//printf("Found the certificate [%ws] in the system store\n", pwcSubjectName);

						if ( wcscmp(pwcTrustRootCA, pwcSubjectName) == 0)
						{
							bFoundCert = TRUE;

							// Retrieve information on the property by first getting the property size.
							if(CertGetCertificateContextProperty( pCertContext, 
								CERT_SHA1_HASH_PROP_ID , // work fine on XP 
								NULL, 
								&cbSHA1))
							{
								// Use the size to allocate the memory for the property
								if ( pbSHA1 = (BYTE *) malloc(cbSHA1 * sizeof(BYTE) ) )
								{
									// Retrieve HASH of the certificate
									if(CertGetCertificateContextProperty( pCertContext, 
										CERT_SHA1_HASH_PROP_ID ,  
										pbSHA1, 
										&cbSHA1))
									{
										memcpy(pbCertHash, pbSHA1, cbSHA1);
										*pcbCertHash = cbSHA1;
									}
									else
									{
										printf("->GetCertificate :: Error retrieving certificate HASH.\n");
										dwRet = ERROR_CANTOPEN;
									}

									free(pbSHA1);

								}
								else
								{
									printf("->GetCertificate :: Error allocating memory.\n");
									dwRet = ERROR_NOT_ENOUGH_MEMORY;
								}
							}
							else
							{
								printf("->GetCertificate :: Error getting certificate property.\n");
								dwRet = ERROR_CANTOPEN;
							}

						} 
					}
					else
					{
						printf("->GetCertificate :: Error getting certificate name string.\n");
						dwRet = ERROR_CANTOPEN;
					}

					free( pwcSubjectName );
					cwcSubjectName = 0;
				}
				else
				{
					printf("->GetCertificate :: Error allocating memory.\n");
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
				}
			}
			else
				dwRet = ERROR_CANTOPEN;
		}

		if( !bFoundCert )
		{
			printf("->GetCertificate :: Error looking for the certificate in the system store.\n");
			dwRet = ERROR_CANTOPEN;
		}

		if( dwRet != NO_ERROR )
		{
			if( pCertContext )
				CertFreeCertificateContext( pCertContext );

		}

		CertCloseStore( hCertStore, CERT_CLOSE_STORE_CHECK_FLAG );
	}
	else
	{
		printf("->GetCertificate :: Error opening system store.\n");
		dwRet = ERROR_CANTOPEN;
	}


	return dwRet;

}

// Helper function that converts a BYTE string to HEX WCHAR string
DWORD ByteToHexW_s( BYTE *xBytes, int xLength, WCHAR** xOutBuffer )
{
	WCHAR *xPtr;
	BYTE *xbPtr;
	int i, xByte;

	if( ( *xOutBuffer = ( WCHAR* ) malloc( sizeof( WCHAR ) * ( xLength + xLength + 1  ) ) ) )
	{
		xPtr = *xOutBuffer;
		xbPtr = xBytes;

		for (i = 0; i < xLength; i++)
		{
			xByte = ( *xbPtr & 0xf0 ) >> 4;
			*xPtr++ = ( xByte <= 9) ? xByte + '0' : ( xByte - 10 ) + 'A';
			xByte = (*xbPtr & 0x0f );
			*xPtr++ = (xByte <= 9 ) ? xByte + '0' : ( xByte - 10 ) + 'A';
			xbPtr++;
		}

		*xPtr++ = 0;
	}
	else
		return ERROR_NOT_ENOUGH_MEMORY;

	return NO_ERROR;
}


DWORD SetProfile(int argc, const TCHAR *argv[])
{ 
	DWORD                            dwRet = NO_ERROR; 
	DWORD                              i;
	HANDLE                           hClientHandle = NULL;
	WCHAR                            strReason[WLSAMPLE_REASON_STRING_LEN] = L"";
	DWORD                            dwReason;
	WCHAR					         *pwcXMLTemplate=NULL;
	DWORD					         ccXMLTemplate; 	
	WCHAR                            *pwcConfigData;
	PBYTE	                         pbConfig, pbConfigPtr;
	DWORD                            cbConfig;
	DWORD							 dwServiceVersion;
	CWlanApiInvoker					 invoker;

	// input parameters
	GUID  NICGuid;
	TCHAR  pcGUID[NICGUID_LEN+1]=TEXT("");
	TCHAR  pcSSID[ DOT11_SSID_MAX_LENGTH + 1 ]=TEXT("");
	WCHAR pwcSSID[ DOT11_SSID_MAX_LENGTH+ 1 ]= L"";
	WCHAR pwcConnectionType[PARAMETER_LEN] = L"ESS";        // IBSS, ESS
	WCHAR pwcConnectionMode[PARAMETER_LEN] = L"auto";       // auto, manual
	WCHAR pwcAuthenticationMode[PARAMETER_LEN] = L"";       // open, shared, WPA, WPAPSK, WPA2, WPA2PSK
	WCHAR pwcEncryptionType[PARAMETER_LEN] = L"";           // none, WEP, TKIP, AES
	WCHAR pwcNetworkKey[MAX_NETWORKKEY_LEN] = L"";          // NULL, WEP keys are of 5 or 13 characters (ASCII) or of 10 or 26 characters (hexadecimal)
	// WPAPSK and WPA2PSK keys are 8-63 characters (ASCII) or 64 characters (hexadecimal)
	DWORD ulKeyIndex = 1;                                   // NULL, 1, 2, 3, 4
	WCHAR pwcEAPMethod[PARAMETER_LEN] = L"";                // EAP-TLS, PEAP, SecureW2
	DWORD dwEAPMethod = 25;                                 // 13, 25 
	WCHAR pwcInnerAuthenticationMethod[PARAMETER_LEN] = L"";  // GTC, MSCHAPv2
	BOOL  bUseWindowsLogonInfo = FALSE;
	BOOL  bEnableFastReconnect = FALSE;
	WCHAR pwcUseOneX[PARAMETER_LEN] = L"false";                   // true, false
	BOOL  bValidateServerCert = FALSE;
	WCHAR pwcTrustRootCA[MAX_CERT_NUM][MAX_TRUSTED_ROOT_CA_LEN];  // store names of certificates 
	BYTE  pbCertHash[MAX_CERT_NUM][HASH_LEN];                     // store HASH of certificates
	DWORD cbCertHash;
	DWORD dwCertNum = 0;   

	//
	// EAP patterns
	//

	// EAP-TLS

	BYTE	pbPattern_TLS[] = 	{ 
		0x02,0x00,0x00,0x00, 0x2A,0x00,0x00,0x00, 
		0x17,0x00,0x00,0x00                        // pbPattern[8] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate"
	};


	BYTE    pbPattern_TLS_Cert_Num[] = { 
		0x00,0x00,0x00,0x00, 0x00,0x00           // pbPattern[2]: the number of certificates
	};

	// PEAP-GTC


	BYTE	pbPattern_PEAP_GTC[] = 	{ 
		0x01,0x00,0x00,0x00, 0x36,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, // pbPattern[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
		0x01,0x00,0x00,0x00, 0x15,0x00,0x00,0x00,
		0x17,0x00,0x00,0x00, 0x00,0x00,0x00,0x00  // pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate"
	};

	BYTE	pbPattern_GTC[] = 	{  
		0x00,0x00,0x01,0x00, 0x00,0x00,0x0F,0x00,
		0x00,0x00,0x06,0x00, 0x00,0x00,0x00,0x00, // 0x06 -> GTC
		0x00,0x00,0x00,0x00, 0x00,0x00
	};

	// PEAP-MSCHAPv2


	BYTE	pbPattern_PEAP_MSCHAPv2[] = { 
 		0x01,0x00,0x00,0x00, 0x3E,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, // pbPattern[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
		0x01,0x00,0x00,0x00, 0x15,0x00,0x00,0x00,
		0x17,0x00,0x00,0x00, 0x00,0x00,0x00,0x00  // pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate"
	};


	BYTE	pbPattern_MSCHAPv2[] = 	{
		0x00,0x00,0x01,0x00, 0x00,0x00,0x17,0x00,
		0x00,0x00,0x1A,0x00, 0x00,0x00,0x01,0x00, // pbPattern[10] : 0x1A -> MSCHAPv2
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, // pbPattern[18] : 0x02 -> UseWindowsLogonInfo; 0x00 -> !UseWindowsLogonInfo
		0x00,0x00,0x00,0x00, 0x00,0x00
	}; 


	// Certificate pattern

	BYTE	pbPattern_Cert[] = 	{
		0x14,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00
	}; 

	BYTE	pbPattern_Cert_Prefix[] = 	{
		0x14,0x00,0x00,0x00
	}; 



	//----------------------------------------------------------------------------------------------------

	// Initialize
	ZeroMemory(pwcTrustRootCA, sizeof(pwcTrustRootCA));

	// Scan the input parameters
	for (i=0; i<(DWORD)argc; i++)
	{
		const TCHAR *pStr;
		if (_tcsncmp(argv[i], TEXT("/NICGUID:"), _tcslen(TEXT("/NICGUID:"))) == 0 )
		{
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcslen(pStr)!=36)
			{
				printf("->SetProfile :: [NICGUID] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}
			else
			{ 
				_tcsncpy_s(pcGUID,NICGUID_LEN+1, pStr, _tcslen(pStr));
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/SSID:"), _tcslen(TEXT("/SSID:"))) == 0 )
		{
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( ( _tcslen(pStr) < 1 ) || ( _tcslen(pStr) > DOT11_SSID_MAX_LENGTH ) )
			{
				printf("->SetProfile :: [SSID] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
			else
			{ 
				_tcsncpy_s(pcSSID,DOT11_SSID_MAX_LENGTH + 1, pStr, _tcslen(pStr));
			}
		}	
		else if ( _tcsncmp(argv[i], TEXT("/Authentication:"), _tcslen(TEXT("/Authentication:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( ( _tcscmp(pStr, TEXT("open"))==0 ) ||
				( _tcscmp(pStr, TEXT("shared"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPA"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPAPSK"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPA2"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPA2PSK"))==0 ) )
			{
				#if defined _UNICODE||defined UNICODE
					wcscpy(pwcAuthenticationMode,pStr);
				#else
					sscanf(pStr, "%ws", pwcAuthenticationMode);
				#endif
				
				if ( _tcscmp(pStr, TEXT("WPA"))==0 || _tcscmp(pStr, TEXT("WPA2"))==0 )
					wcscpy_s(pwcUseOneX,PARAMETER_LEN, L"true");
			}
			else
			{
				printf("->SetProfile :: [AuthenticationMode] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/Encryption:"), _tcslen(TEXT("/Encryption:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("WEP"))==0) || 
				(_tcscmp(pStr, TEXT("none"))==0) ||
				(_tcscmp(pStr, TEXT("AES"))== 0) || 
				(_tcscmp(pStr, TEXT("TKIP"))==0) )  
			{
				#if defined _UNICODE||defined UNICODE
					wcscpy(pwcEncryptionType,pStr);
				#else
					sscanf(pStr, "%ws", pwcEncryptionType);
				#endif
			}
			else
			{
				printf("->SetProfile :: [Encryptiontype] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/NetworkKey:"), _tcslen(TEXT("/NetworkKey:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( _tcslen(pStr) <= 64 )

			{
				#if defined _UNICODE||defined UNICODE
					wcscpy(pwcNetworkKey,pStr);
				#else
					sscanf(pStr, "%ws", pwcNetworkKey);
				#endif	 
			} 
			else
			{
				printf("->SetProfile :: [NetworkKey] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/KeyIndex:"), _tcslen(TEXT("/KeyIndex:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("1"))== 0) ||
				(_tcscmp(pStr, TEXT("2"))== 0)  ||
				(_tcscmp(pStr, TEXT("3"))==0)   ||
				(_tcscmp(pStr, TEXT("4"))==0) )
			{
				_stscanf( pStr, TEXT("%d"), &ulKeyIndex);
			} 
			else
			{
				printf("->SetProfile :: [KeyIndex] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/EAPType:"), _tcslen(TEXT("/EAPType:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("PEAP"))==0) || (_tcscmp(pStr, TEXT("EAP-TLS"))==0) )
			{
				#if defined _UNICODE||defined UNICODE
					wcscpy(pwcEAPMethod,pStr);
				#else
					sscanf( pStr, "%ws", pwcEAPMethod);
				#endif	

				if ( _tcscmp(pStr, TEXT("PEAP"))==0 )
					dwEAPMethod = 25;
				else if ( _tcscmp(pStr, TEXT("EAP-TLS"))==0 )
					dwEAPMethod = 13;
			} 
			else
			{
				printf("->SetProfile :: [EAPType] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		} 
		else if ( _tcsncmp(argv[i], TEXT("/InnerAuthentication:"), _tcslen(TEXT("/InnerAuthentication:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("GTC"))== 0) || (_tcscmp(pStr, TEXT("MSCHAPv2"))== 0) )
			{
				_stscanf( pStr,TEXT("%ws"), pwcInnerAuthenticationMethod); 
			} 
			else
			{
				printf("->SetProfile :: [InnerAuthenticationMethod] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/UseWindowsLogonInfo:"), _tcslen(TEXT("/UseWindowsLogonInfo:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bUseWindowsLogonInfo = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bUseWindowsLogonInfo = FALSE;
			}
			else
			{
				printf("->SetProfile :: [UseWindowsLogonInfo] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/EnableFastReconnect:"), _tcslen(TEXT("/EnableFastReconnect:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bEnableFastReconnect = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bEnableFastReconnect = FALSE;
			}
			else
			{
				printf("->SetProfile :: [EnableFastReconnect] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/ValidateServerCert:"), _tcslen(TEXT("/ValidateServerCert:"))) == 0 )
		{
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bValidateServerCert = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bValidateServerCert = FALSE;
			}
			else
			{
				printf("->SetProfile :: [ValidateServerCert] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/TrustRootCA:"), _tcslen(TEXT("/TrustRootCA:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( ( _tcslen(pStr) < 1 ) || ( _tcslen(pStr) > MAX_TRUSTED_ROOT_CA_LEN ) )
			{
				printf("->SetProfile :: [TrustRootCA] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
			else
			{
				if ( dwCertNum < MAX_CERT_NUM )
				{
					size_t i;

					_stscanf(pStr, TEXT("%ws"), pwcTrustRootCA[dwCertNum]);

					// Change the format of the certificate from "xxxx_xxxx_xxxx" to "xxxx xxxx xxxx" 
					for ( i = 0; i < wcslen(pwcTrustRootCA[dwCertNum]); i++ )
					{
						if (pwcTrustRootCA[dwCertNum][i] == L'_')
							pwcTrustRootCA[dwCertNum][i] = L' ';
					} 

					dwCertNum++;
				}
				else
				{
					printf("->SetProfile :: [TrustRootCA] : the max number of certificates is %d.\n", MAX_CERT_NUM);
					dwRet = ERROR_INVALID_DATA;

					goto LDone;
				}
			}

		}
		else
		{
			printf("->SetProfile :: invalid input\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}//for

	//--------------------------------------------------------------------------------------------------

	// Check the validity of input parameters

	//NICGUID
	if ( _tcscmp(pcGUID, TEXT(""))==0 )
	{
		printf("->SetProfile :: [NICGUID] must be set\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone;
	}
	else if (UuidFromString(
	#if defined _UNDERFLOW||defined UNICODE
		(RPC_WSTR)pcGUID
	#else
		(RPC_CSTR)pcGUID
	#endif
		,&NICGuid)!=RPC_S_OK)
	{
		printf("->SetProfile :: [NICGUID] is incorrect\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone;
	}
	// SSID
	if ( _tcscmp(pcSSID, TEXT(""))==0 )
	{
		printf("->SetProfile :: [SSID] must be set\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone;
	}

	// Authentication mode
	{ 
		if ( (wcscmp(pwcAuthenticationMode, L"WPA")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2")==0) )
		{
			// A networkKey must not be present
			if ( wcscmp(pwcNetworkKey, L"") != 0)
			{
				printf("->SetProfile :: [NetworkKey] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			} 
		}

		if ( (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0) )
		{ 
			// A networkKey must be present
			if ( wcscmp(pwcNetworkKey, L"") == 0)
			{
				printf("->SetProfile :: [NetworkKey] must be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}
		}
	}

	// Encryption type
	if ( ( (wcscmp(pwcEncryptionType, L"WEP")==0) || (wcscmp(pwcEncryptionType, L"none")==0) ) && 
		( (wcscmp(pwcAuthenticationMode, L"open")==0) || (wcscmp(pwcAuthenticationMode, L"shared")==0) )
		||
		( (wcscmp(pwcEncryptionType, L"AES")== 0) || (wcscmp(pwcEncryptionType, L"TKIP")==0) ) &&
		( (wcscmp(pwcAuthenticationMode, L"WPA")==0) || (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0) ) )
	{ 

		if ( (wcscmp(pwcAuthenticationMode, L"open")==0) && (wcscmp(pwcEncryptionType, L"none")==0) )
		{
			if ( wcscmp(pwcNetworkKey, L"") != 0)
			{
				printf("->SetProfile :: [NetworkKey] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}
		}
	}
	else
	{
		printf("->SetProfile :: [Encryptiontype] : invalid input\n");
		dwRet = ERROR_INVALID_DATA;

		goto LDone;
	}

	// Network key
	// WEP keys are of 5 or 13 characters (ASCII) or of 10 or 26 characters (hexadecimal)
	// WPAPSK and WPA2PSK keys are 8-63 characters (ASCII) or 64 characters (hexadecimal)
	if ( ( (wcscmp(pwcEncryptionType, L"WEP")==0) && ( ( wcslen(pwcNetworkKey)!=5 ) && ( wcslen(pwcNetworkKey)!=13 ) && ( wcslen(pwcNetworkKey)!=10 ) && ( wcslen(pwcNetworkKey)!=26 ) ) )
		||
		( ( (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0)) && ( ( ( wcslen(pwcNetworkKey) < 8 ) || ( wcslen(pwcNetworkKey) > 64 ) ) ) ) )

	{
		printf("->SetProfile :: [NetworkKey] : invalid input\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone; 
	}


	// EAP Method
	if (  (wcscmp(pwcEAPMethod, L"PEAP")==0) || (wcscmp(pwcEAPMethod, L"EAP-TLS")==0) )
	{ 
		if ( wcscmp(pwcEAPMethod, L"PEAP")==0 )
		{
			if ( wcscmp(pwcInnerAuthenticationMethod, L"")==0 )
			{
				printf("->SetProfile :: the element [InnerAuthenticationMethod] should be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}
		}

		if ( wcscmp(pwcEAPMethod, L"EAP-TLS")==0 )
		{ 
			if ( wcscmp(pwcInnerAuthenticationMethod, L"")!=0 )
			{
				printf("->SetProfile :: the element [InnerAuthenticationMethod] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}

			if ( bUseWindowsLogonInfo != FALSE )
			{
				printf("->SetProfile :: the element [UseWindowsLogonInfo] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}

			if ( bEnableFastReconnect != FALSE )
			{
				printf("->SetProfile :: the element [EnableFastReconnect] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}
		}
	}  


	// Inner authentication method
	if ( (wcscmp(pwcInnerAuthenticationMethod, L"GTC")== 0) || (wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2")== 0) )
	{ 
		if ( wcscmp(pwcEAPMethod, L"PEAP")!=0 )
		{
			printf("->SetProfile :: the element [EAPType] should be set to PEAP\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}  

	// Use Windows logon info
	if ( bUseWindowsLogonInfo == TRUE )
	{
		if ( wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2") !=0) 
		{
			printf("->SetProfile :: the element [InnerAuthenticationMethod] should be set to MSCHAPv2\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}

	// Enable Fast Reconnect
	if ( bEnableFastReconnect == TRUE )
	{ 
		if ( wcscmp(pwcEAPMethod, L"PEAP")!=0 )
		{
			printf("->SetProfile :: the element [EAPType] should be set to PEAP\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}

		if ( (wcscmp(pwcInnerAuthenticationMethod, L"GTC")!= 0) && (wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2")!= 0) )
		{
			printf("->SetProfile :: the element [InnerAuthenticationMethod] should be set to GTC or MSCHAPv2\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}

	// retrieve HASH of the certificates
	if ( (bValidateServerCert == TRUE) && (dwCertNum != 0) )
	{
		// initialize
		memset( pbCertHash, 0, sizeof(pbCertHash) );
		cbCertHash = HASH_LEN;

		for(i = 0; i < dwCertNum; i++)
		{ 
			if ( NO_ERROR != GetCertificate(pwcTrustRootCA[i], pbCertHash[i], &cbCertHash))
			{
				printf("->SetProfile :: Error looking for the certificate [%ws] in the sytem store.\n", pwcTrustRootCA[i]);
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
	}
	else if ( (bValidateServerCert == FALSE) && (dwCertNum > 0) )
	{
		printf("->SetProfile :: [ValidateServerCert] is not set.\n");
		dwRet = ERROR_INVALID_DATA;

		goto LDone;
	}

	//
	// print the input parameters
	//
	printf("->SetProfile :: input parameter: SSID : %s\n", pcSSID); 
	printf("->SetProfile :: input parameter: AuthenticationMode : %ws\n", pwcAuthenticationMode); 
	printf("->SetProfile :: input parameter: EncryptionType : %ws\n", pwcEncryptionType); 
	printf("->SetProfile :: input parameter: NetworkKey : %ws\n", pwcNetworkKey); 
	printf("->SetProfile :: input parameter: KeyIndex : %d\n", ulKeyIndex);  
	printf("->SetProfile :: input parameter: EAPType : %ws\n", pwcEAPMethod);  
	printf("->SetProfile :: input parameter: InnerAuthenticaionMode : %ws\n", pwcInnerAuthenticationMethod); 
	printf("->SetProfile :: input parameter: UseWindowsLogonInfo : %d\n", bUseWindowsLogonInfo); 
	printf("->SetProfile :: input parameter: EnableFastReconnect : %d\n", bEnableFastReconnect);
	printf("->SetProfile :: input parameter: ValidateServerCert : %d\n", bValidateServerCert);

	for ( i = 0; i < dwCertNum; i++)
		printf("->SetProfile :: input parameter: TrustRootCA[%d] : %ws\n", i, pwcTrustRootCA[i]);



	//---------------------------------------------------------------------------------------------------------------


	// open handle	
	if(invoker.WlanOpenHandle(WLAN_API_VERSION,NULL,&dwServiceVersion,&hClientHandle))
	{
		#if defined _UNDERFLOW||defined UNICODE
			wcscpy(pwcSSID,pcSSID);
		#else
			if ( (MultiByteToWideChar(CP_ACP, 0, pcSSID, -1, pwcSSID, sizeof(pwcSSID))) == 0)
			{
				dwRet = GetLastError();
				goto LDone;
			}
		#endif

		// 
		// Create XML Template
		//
		if ( (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0) )
		{					 
			// Cases: WPAPSK/WPA2PSK --> AES/TKIP --> key
			// <useOneX> must be set to "false"
			// <keyType> must be set to "passPhrase"

			// calculate the size of the XML WirelessProfile Schema
			ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial></keyMaterial></sharedKey></security></MSM></WLANProfile>" )
				+ wcslen(pwcSSID)
				+ wcslen(pwcSSID)
				+ wcslen(pwcConnectionType)
				+ wcslen(pwcConnectionMode)
				+ wcslen(pwcAuthenticationMode)
				+ wcslen(pwcEncryptionType)
				+ wcslen(pwcUseOneX)
				+ wcslen(pwcNetworkKey) ) * sizeof( WCHAR );

			// allocate memory for the XML template
			if( ( pwcXMLTemplate = (WCHAR*)malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
			{			
				memset( pwcXMLTemplate, 0, ccXMLTemplate );

				swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial>%s</keyMaterial></sharedKey></security></MSM></WLANProfile>",
					pwcSSID,
					pwcSSID, 
					pwcConnectionType,
					pwcConnectionMode,
					pwcAuthenticationMode, 
					pwcEncryptionType,
					pwcUseOneX,
					pwcNetworkKey
					);
			}
			else
			{
				dwRet = ERROR_NOT_ENOUGH_MEMORY;
				goto LDone;
			}

		}
		else if ( (wcscmp(pwcAuthenticationMode, L"open")==0) || (wcscmp(pwcAuthenticationMode, L"shared")==0) ) 
		{
			if ( wcscmp(pwcEncryptionType, L"none")==0 ) 
			{
				// Cases: open/shared --> none

				// calculate the size of the XML WirelessProfile Schema
				ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption></security></MSM></WLANProfile>" )
					+ wcslen(pwcSSID)
					+ wcslen(pwcSSID)
					+ wcslen(pwcConnectionType)
					+ wcslen(pwcConnectionMode)
					+ wcslen(pwcAuthenticationMode)
					+ wcslen(pwcEncryptionType)
					+ wcslen(pwcUseOneX)  ) * sizeof( WCHAR );

				// allocate memory for the XML template
				if( ( pwcXMLTemplate = (WCHAR*)malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
				{					
					memset( pwcXMLTemplate, 0, ccXMLTemplate );

					swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption></security></MSM></WLANProfile>",
						pwcSSID,
						pwcSSID, 
						pwcConnectionType,
						pwcConnectionMode,
						pwcAuthenticationMode, 
						pwcEncryptionType,
						pwcUseOneX
						);
				}
				else
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}
			}
			else if ( wcscmp(pwcEncryptionType, L"WEP")==0 )
			{
				// Cases: open/shared --> WEP

				// calculate the size of the XML WirelessProfile Schema
				ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption><sharedKey><keyType>networkKey</keyType><protected>false</protected><keyMaterial></keyMaterial></sharedKey><keyIndex></keyIndex></security></MSM></WLANProfile>" )
					+ wcslen(pwcSSID)
					+ wcslen(pwcSSID)
					+ wcslen(pwcConnectionType)
					+ wcslen(pwcConnectionMode)
					+ wcslen(pwcAuthenticationMode)
					+ wcslen(pwcEncryptionType)
					+ wcslen(pwcNetworkKey)  
					+ sizeof(DWORD)) * sizeof( WCHAR );

				// allocate memory for the XML template
				if( ( pwcXMLTemplate = ( WCHAR* ) malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
				{					
					memset( pwcXMLTemplate, 0, ccXMLTemplate );

					swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption><sharedKey><keyType>networkKey</keyType><protected>false</protected><keyMaterial>%s</keyMaterial></sharedKey><keyIndex>%d</keyIndex></security></MSM></WLANProfile>",
						pwcSSID,
						pwcSSID, 
						pwcConnectionType,
						pwcConnectionMode,
						pwcAuthenticationMode, 
						pwcEncryptionType,
						pwcUseOneX,
						pwcNetworkKey,
						(ulKeyIndex-1));	
				}
				else
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}
			}
		}
		else if ( wcscmp(pwcUseOneX, L"true") == 0 )
		{
			// Cases: WPA/WPA2 --> AES/TKIP --> PEAP/TLS --> EAP options
			// <useOneX> must be set to "true"

			// construct the config blob
			if ( (wcscmp(pwcEAPMethod, L"PEAP")==0) && (wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2")==0) )
			{
				// PEAP-MSCHAPv2
				// calculate the size of the memory
				// size = Patterns + Certificates
				cbConfig = sizeof( pbPattern_PEAP_MSCHAPv2 )+ sizeof( pbPattern_MSCHAPv2 );

				if ( dwCertNum != 0)
					cbConfig += dwCertNum * ( sizeof(pbPattern_Cert_Prefix) + HASH_LEN ); 

				// allocate and initialize memory
				pbConfig = (PBYTE) malloc (cbConfig);
				if ( pbConfig == NULL )
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}

				memset( pbConfig, 0, cbConfig );
				pbConfigPtr = pbConfig;

				// set the [EnableFastReconnect] option
				// pbPattern_PEAP_MSCHAPv2[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
				// Default: 0x00
				if ( bEnableFastReconnect == TRUE )
				{
					pbPattern_PEAP_MSCHAPv2[12] = 0x01;
				}

				// set the [ValidateServerCert] option
				// pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate".
				// Default: 0x17    
				if ( bValidateServerCert == TRUE)
					pbPattern_PEAP_MSCHAPv2[24] = 0x15;

				// set the certificate-related elements
				if ( dwCertNum != 0 )
				{
					// pbPattern_PEAP_MSCHAPv2[28] represents the number of certificates
					// pbPattern_PEAP_MSCHAPv2[4] and pbPattern_PEAP_MSCHAPv2[20] increase (0x18) for each cert
					pbPattern_PEAP_MSCHAPv2[4] = (BYTE)(pbPattern_PEAP_MSCHAPv2[4] + dwCertNum * (0x18));
					pbPattern_PEAP_MSCHAPv2[20] = (BYTE)(pbPattern_PEAP_MSCHAPv2[20] + dwCertNum * (0x18));
					pbPattern_PEAP_MSCHAPv2[28] = (BYTE)(pbPattern_PEAP_MSCHAPv2[28] + dwCertNum);
				}

				// set the [UseWindowsLogonInfo] option
				// pbPattern_MSCHAPv2[18] : 0x02 -> UseWindowsLogonInfo; 0x00 -> !UseWindowsLogonInfo
				// Default: 0x00
				if ( bUseWindowsLogonInfo == TRUE )
				{  
					pbPattern_MSCHAPv2[18] = 0x02;
				}

				// construct the content of the config blob
				memcpy( pbConfigPtr, pbPattern_PEAP_MSCHAPv2, sizeof( pbPattern_PEAP_MSCHAPv2 ) );
				pbConfigPtr += sizeof( pbPattern_PEAP_MSCHAPv2 );

				if ( dwCertNum != 0)
				{ 
					for ( i = 0 ; i < dwCertNum ; i++)
					{
						memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
						pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
						memcpy( pbConfigPtr, pbCertHash[i], HASH_LEN );
						pbConfigPtr += HASH_LEN;
					}

					memcpy( pbConfigPtr, pbPattern_MSCHAPv2, sizeof( pbPattern_MSCHAPv2 ) );
				}
				else
					memcpy( pbConfigPtr, pbPattern_MSCHAPv2, sizeof( pbPattern_MSCHAPv2 ) );

			} 
			else if ( (wcscmp(pwcEAPMethod, L"PEAP")==0) && (wcscmp(pwcInnerAuthenticationMethod, L"GTC")==0) )
			{
				// PEAP-GTC
				// calculate the size of the memory
				// size = Patterns + Certificates

				cbConfig = sizeof( pbPattern_PEAP_GTC )+ sizeof( pbPattern_GTC );

				if ( dwCertNum != 0)
					cbConfig += dwCertNum * ( sizeof(pbPattern_Cert_Prefix) + HASH_LEN );

				// allocate and initialize memory
				pbConfig = (PBYTE) malloc (cbConfig);
				if ( pbConfig == NULL )
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}

				memset( pbConfig, 0, cbConfig );
				pbConfigPtr = pbConfig;

				// set the [EnableFastReconnect] option
				// pbPattern_PEAP_GTC[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
				// Default: 0x00
				if ( bEnableFastReconnect == TRUE )
				{
					pbPattern_PEAP_GTC[12] = 0x01;
				}

				// set the [ValidateServerCert] option
				// pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate".
				// Default: 0x17    
				if ( bValidateServerCert == TRUE)
					pbPattern_PEAP_GTC[24] = 0x15;

				// set the certificate-related elements
				if ( dwCertNum != 0 )
				{
					// pbPattern_PEAP_GTC[28] represents the number of certificates
					// pbPattern_PEAP_GTC[4] and pbPattern_PEAP_GTC[20] increase (0x18) for each cert
					pbPattern_PEAP_GTC[4] = (BYTE)(pbPattern_PEAP_GTC[4] + dwCertNum * (0x18));
					pbPattern_PEAP_GTC[20] = (BYTE)(pbPattern_PEAP_GTC[20] + dwCertNum * (0x18));
					pbPattern_PEAP_GTC[28] =(BYTE)(pbPattern_PEAP_GTC[28] + dwCertNum) ;
				}

				// construct the content of the config blob
				memcpy( pbConfigPtr, pbPattern_PEAP_GTC, sizeof( pbPattern_PEAP_GTC ) );
				pbConfigPtr += sizeof( pbPattern_PEAP_GTC );

				if ( dwCertNum != 0)
				{ 
					for ( i = 0 ; i < dwCertNum ; i++)
					{
						memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
						pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
						memcpy( pbConfigPtr, pbCertHash[i], HASH_LEN );
						pbConfigPtr += HASH_LEN;
					}

					memcpy( pbConfigPtr, pbPattern_GTC, sizeof( pbPattern_GTC ) );
				}
				else
					memcpy( pbConfigPtr, pbPattern_GTC, sizeof( pbPattern_GTC ) );
			}
			else if ( wcscmp(pwcEAPMethod, L"EAP-TLS")==0 )
			{
				// EAP-TLS 

				// size = Patterns + Certificates
				if (dwCertNum <= 1)
				{
					cbConfig = sizeof( pbPattern_TLS ) + sizeof( pbPattern_Cert_Prefix ) + HASH_LEN 
						+ sizeof( pbPattern_TLS_Cert_Num );
				}
				else
				{
					cbConfig = sizeof( pbPattern_TLS ) + dwCertNum * ( sizeof( pbPattern_Cert_Prefix ) + HASH_LEN ) 
						+ sizeof( pbPattern_TLS_Cert_Num );
				}

				// allocate and initialize memory
				if ( NULL == ( pbConfig = (PBYTE) malloc (cbConfig) ) )
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}

				memset( pbConfig, 0, cbConfig );
				pbConfigPtr = pbConfig;


				// set the [ValidateServerCert] option
				// pbPattern[8] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate".
				// Default: 0x17    
				if ( bValidateServerCert == TRUE)
					pbPattern_TLS[8] = 0x15;

				// set the certificate-related elements
				if ( dwCertNum >=2 )
				{ 
					// pbPattern_TLS[4] increase (0x18) for each cert if there is more than one cert
					pbPattern_TLS[4] = (BYTE)(pbPattern_TLS[4] + (dwCertNum-1) * (0x18)); 
				}


				// set the number of certificates
				pbPattern_TLS_Cert_Num[2] = (BYTE)dwCertNum;

				// construct the blob content
				memcpy( pbConfigPtr, pbPattern_TLS, sizeof( pbPattern_TLS ) );
				pbConfigPtr += sizeof( pbPattern_TLS );

				if ( dwCertNum == 0)
				{
					pbConfigPtr += ( sizeof(pbPattern_Cert_Prefix) + HASH_LEN );
					memcpy( pbConfigPtr, pbPattern_TLS_Cert_Num, sizeof(pbPattern_TLS_Cert_Num) );						 
				}
				else
				{
					memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
					pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
					memcpy( pbConfigPtr, pbCertHash[0], HASH_LEN );
					pbConfigPtr += HASH_LEN;
					memcpy( pbConfigPtr, pbPattern_TLS_Cert_Num, sizeof(pbPattern_TLS_Cert_Num) );
					pbConfigPtr += sizeof( pbPattern_TLS_Cert_Num );

					if ( dwCertNum >= 2 )
					{
						for ( i = 1 ; i < dwCertNum ; i++)
						{
							memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
							pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
							memcpy( pbConfigPtr, pbCertHash[i], HASH_LEN );
							pbConfigPtr += HASH_LEN;
						}
					} 
				}
			}

			ByteToHexW_s( pbConfig, cbConfig, &pwcConfigData );


			// calculate the size of the XML WirelessProfile Schema
			ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption><OneX xmlns=\"http://www.microsoft.com/networking/OneX/v1\"><EAPConfig><EapHostConfig xmlns=\"http://www.microsoft.com/provisioning/EapHostConfig\"><EapMethod><Type xmlns=\"http://www.microsoft.com/provisioning/EapCommon\"></Type><VendorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorId><VendorType xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorType><AuthorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</AuthorId></EapMethod><ConfigBlob></ConfigBlob></EapHostConfig></EAPConfig></OneX></security></MSM></WLANProfile>" )
				+ wcslen(pwcSSID)
				+ wcslen(pwcSSID)
				+ wcslen(pwcConnectionType)
				+ wcslen(pwcConnectionMode)
				+ wcslen(pwcAuthenticationMode)
				+ wcslen(pwcEncryptionType)
				+ wcslen(pwcUseOneX)
				+ sizeof(DWORD)	
				+ wcslen(pwcConfigData) ) * sizeof( WCHAR );


			// allocate memory for the XML template
			if( ( pwcXMLTemplate = (WCHAR*)malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
			{				
				memset( pwcXMLTemplate, 0, ccXMLTemplate );

				swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption><OneX xmlns=\"http://www.microsoft.com/networking/OneX/v1\"><EAPConfig><EapHostConfig xmlns=\"http://www.microsoft.com/provisioning/EapHostConfig\"><EapMethod><Type xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">%d</Type><VendorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorId><VendorType xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorType><AuthorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</AuthorId></EapMethod><ConfigBlob>%s</ConfigBlob></EapHostConfig></EAPConfig></OneX></security></MSM></WLANProfile>",
					pwcSSID,
					pwcSSID, 
					pwcConnectionType,
					pwcConnectionMode,
					pwcAuthenticationMode, 
					pwcEncryptionType,
					pwcUseOneX,
					dwEAPMethod,
					pwcConfigData
					);

			}
			else
			{
				dwRet = ERROR_NOT_ENOUGH_MEMORY;
				goto LDone;
			}

		}
		else
		{ 
			dwRet = ERROR_NOT_SUPPORTED;
			goto LDone;
		}

		//
		// Add to interface which GUID is in NICGuid
		//

		// set profile remove old first
		if (invoker.WlanDeleteProfile(hClientHandle,&NICGuid,pwcSSID,NULL))
		{
			printf("->SetProfile :: Delete old profile succeeded\n");
		}
		if (invoker.WlanSetProfile(
			hClientHandle, 
			&NICGuid,      // GUID of the interface 
			0,                                               // flags to set on the profile. an all-user profile.
			pwcXMLTemplate,                                  // the XML representation of the profile
			NULL,                                            // use the default ACL
			TRUE,		                                     // overwrite a profile if it already exists
			NULL,                                            // reserved
			&dwReason
			))
		{
			printf("->SetProfile :: WlanSetProfile succeeded\n");
		}
		else
		{
			printf("->SetProfile :: WlanSetProfile ERROR!");
			dwRet=ERROR_NOT_SUPPORTED;//
		}
	}
	else
	{
		dwRet=ERROR_NOT_SUPPORTED;//
	}

	//------------------------------------------------------------- 

LDone: 

	// clean up
	if (pwcXMLTemplate != NULL)
		free(pwcXMLTemplate);

	if (hClientHandle != NULL)
	{
		invoker.WlanCloseHandle(hClientHandle, NULL);
	} 

	return dwRet;
}
#endif 
//功能：	在指定的无线网卡上为指定的无线网络增加profile
//参数：	lpNicGuid为要增加profile的无线网卡
//			lpszSsid为profile对应的无线网络的SSID
//			lpszAuth指定认证方式
//			lpszCipher指定加密方式
//			lpszPwd指定密钥
//返回值:	profile增加成功返回TRUE，否则返回FALSE
//说明：	该接口只对启用的无线网卡有效

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd)
{
#if defined _UNICODE || defined UNICODE
	#define stringt std::wstring
#else
	#define stringt std::string
#endif
	int argc=5;
	stringt strGuid=TEXT("/NICGUID:");
	strGuid+=lpNicGuid;

	stringt strSsid=TEXT("/SSID:");
	strSsid+=lpszSsid;

	stringt strAuth=TEXT("/Authentication:");
	strAuth+=lpszAuth;

	stringt strCipher=TEXT("/Encryption:");
	strCipher+=lpszCipher;

	stringt strPwd=TEXT("/NetworkKey:");
	strPwd+=lpszPwd;

	const TCHAR* argv[5]={NULL};
	argv[0]=strGuid.c_str();
	argv[1]=strSsid.c_str();
	argv[2]=strAuth.c_str();
	argv[3]=strCipher.c_str();
	argv[4]=strPwd.c_str();

//#if defined _UNICODE||defined UNICODE
//	argv[0]=strGuid.c_str();
//	argv[1]=strSsid.c_str();
//	argv[2]=strAuth.c_str();
//	argv[3]=strCipher.c_str();
//	argv[4]=strPwd.c_str();
//#else
//	char chGuid[512]	={0};
//	char chSsid[512]	={0};
//	char chAuth[512]	={0};
//	char chCipher[512]	={0};
//	char chPwd[512]		={0};
//	WideCharToMultiByte(CP_ACP,0,strGuid.c_str(),-1,chGuid,512,NULL,FALSE);
//	WideCharToMultiByte(CP_ACP,0,strSsid.c_str(),-1,chSsid,512,NULL,FALSE);
//	WideCharToMultiByte(CP_ACP,0,strAuth.c_str(),-1,chAuth,512,NULL,FALSE);
//	WideCharToMultiByte(CP_ACP,0,strCipher.c_str(),-1,chCipher,512,NULL,FALSE);
//	WideCharToMultiByte(CP_ACP,0,strPwd.c_str(),-1,chPwd,512,NULL,FALSE);
//	argv[0]=(const TCHAR*)chGuid;
//	argv[1]=(const TCHAR*)chSsid;
//	argv[2]=(const TCHAR*)chAuth;
//	argv[3]=(const TCHAR*)chCipher;
//	argv[4]=(const TCHAR*)chPwd;
//#endif
	return NO_ERROR==SetProfile(5,argv);
}


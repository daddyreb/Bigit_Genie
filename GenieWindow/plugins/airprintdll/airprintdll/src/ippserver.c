#include "ippserver.h"
register_printer_info_t registerprinterinfo;
int returnflag;
rwlock_t returnlock;
rwlock_t printingthreadlock;
int initcount=0;
int adobeType = 0;///kai.yan add
int port=5001;
typedef void(* pGetFileName)(const char * ,const char *,int );
pGetFileName getFileName = NULL;
extern mutex_t sp_mutex;
extern mutex_t ipp_mutex;


_declspec(dllexport) int init(pGetFileName pgetfilename)
{
	getFileName = pgetfilename;
	if(initcount == 0)
	{
		returnflag = 0;
		RWInit(&returnlock);
		RWInit(&printingthreadlock);
		if (!threadCreate((thread_func_t)ippserver, NULL))
		{
			perror("Unable to create client thread");
		}
		initcount++;
	}
	return 0;
}

_declspec(dllexport) int destroy()
{
	RWLockWrite(&returnlock);
	returnflag = 1;
	RWUnlock(&returnlock);
	RWLockWrite(&printingthreadlock);
	getFileName==NULL;
	RWUnlock(&printingthreadlock);
	sleep(10);
	return 0;
}

_declspec(dllexport)   int registerPrinter(const char * printername)
{
	int index;
	int i;

	DNSServiceErrorType	error;		
	char make_model[256],
		 rp[256],
		 product[256];	
	TXTRecordRef ipp_txt;
	char * ptr;
	char	hostname[256];
	const char *pdl = "application/pdf,image/urf,image/jpeg,image/png";

	httpGetHostname(NULL, hostname,sizeof(hostname));
	snprintf(make_model, sizeof(make_model), "%s",printername);
	snprintf(product, sizeof(product), "(%s)",printername);
	snprintf(rp, sizeof(rp), "%s",printername);
	ptr = rp;
	for(i=0; i<strlen(rp); i++)
	{
		if(*ptr == ' ')
		{
			*ptr='_';
		}
		ptr++;
	}

	TXTRecordCreate(&ipp_txt, 1024, NULL);
	TXTRecordSetValue(&ipp_txt, "txtvers", 1, "1");
	TXTRecordSetValue(&ipp_txt, "qtotal", 1, "1");
	TXTRecordSetValue(&ipp_txt, "rp", (uint8_t)strlen(rp), rp);
	TXTRecordSetValue(&ipp_txt, "ty", (uint8_t)strlen(make_model),make_model);
	TXTRecordSetValue(&ipp_txt, "note", (uint8_t)strlen(hostname),hostname);
	TXTRecordSetValue(&ipp_txt, "priority", 1, "0");
	TXTRecordSetValue(&ipp_txt, "product", (uint8_t)strlen(product),product);
	TXTRecordSetValue(&ipp_txt, "Transparent", 1, "T");
	TXTRecordSetValue(&ipp_txt, "Binary", 1, "T");
	TXTRecordSetValue(&ipp_txt, "Color", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Duplex", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Staple", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Copies", 1, "T");
	TXTRecordSetValue(&ipp_txt, "Collate", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Punch", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Bind", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Sort", 1, "F");
	TXTRecordSetValue(&ipp_txt, "Scan", 1, "F");
	TXTRecordSetValue(&ipp_txt, "pdl", (uint8_t)strlen(pdl),pdl);
	TXTRecordSetValue(&ipp_txt, "URF", 4, "none");
	TXTRecordSetValue(&ipp_txt, "printer-state", 1, "3");
	TXTRecordSetValue(&ipp_txt, "printer-type", 4, "0x4A");

	if(printername == NULL)
	{
		return 1;
	}

	for(i=0; i<sizeof(registerprinterinfo.printerlist.attr)/sizeof(registerprinterinfo.printerlist.attr[0]);i++)
	{
		if(registerprinterinfo.flag[i] && !strncmp(registerprinterinfo.printerlist.attr[i].name,printername,sizeof(registerprinterinfo.printerlist.attr[i].name) ) )
		{
			return 1;
		}
	}

	for(i=0; i<sizeof(registerprinterinfo.printerlist.attr)/sizeof(registerprinterinfo.printerlist.attr[0]);i++)
	{
		if(!registerprinterinfo.flag[i])
		{
			strncpy(registerprinterinfo.printerlist.attr[i].name,printername,sizeof(registerprinterinfo.printerlist.attr[i].name));
			registerprinterinfo.flag[i] = 1;

			if ((error = DNSServiceRegister(&(registerprinterinfo.ipp_ref[i]),0,
				kDNSServiceInterfaceIndexAny , registerprinterinfo.printerlist.attr[i].name,
				"_ipp._tcp,_universal,_sub,_ipp,_tcp", NULL ,
				NULL , htons(port),
				TXTRecordGetLength(&ipp_txt),
				TXTRecordGetBytesPtr(&ipp_txt),
				NULL,
				NULL)) != kDNSServiceErr_NoError)
			{
				return (1);
			}
			break;
		}
	}

	return 0;
}

_declspec(dllexport) int unregisterPrinter(const char * printername)
{
	DNSServiceErrorType	error;
	int index;
	int i;	
	
	if(printername == NULL)
	{
		return 1;
	}
	for(i=0; i<sizeof(registerprinterinfo.printerlist.attr)/sizeof(registerprinterinfo.printerlist.attr[0]);i++)
	{
		if(registerprinterinfo.flag[i] && strncmp(registerprinterinfo.printerlist.attr[i].name,printername,sizeof(registerprinterinfo.printerlist.attr[i].name)) == 0)
		{
			registerprinterinfo.flag[i] = 0;
			memset(registerprinterinfo.printerlist.attr[i].name,0,sizeof(registerprinterinfo.printerlist.attr[i].name));
			DNSServiceRefDeallocate(registerprinterinfo.ipp_ref[i]);
			break;
		}
	}
	return 0;
}

_declspec(dllexport) int enumPrinters(PrinterList  *pPrinterList)
{	
	DWORD Flags   =   PRINTER_ENUM_LOCAL;   //local   printers
	DWORD cbBuf;
	DWORD pcReturned   ;
	int i;

	DWORD Level   =   1;
	LPPRINTER_INFO_1 pPrinterEnum   =   NULL   ;

	EnumPrinters(Flags,   NULL,   Level,   NULL,   0,   &cbBuf,   &pcReturned);

	pPrinterEnum   =   (LPPRINTER_INFO_1)LocalAlloc(LPTR,   cbBuf   +   4);
	if(pPrinterEnum == NULL)
	{
		return 1;
	}

	if(!EnumPrinters(Flags,NULL, Level,(LPBYTE)pPrinterEnum, cbBuf, &cbBuf,&pcReturned))
	{
		return 1;
	}

	for(i=0;i<pcReturned && i<sizeof(pPrinterList->attr)/sizeof(pPrinterList->attr[0]); i++)
	{
		memcpy(pPrinterList->attr[i].name,pPrinterEnum[i].pName,sizeof(pPrinterList->attr[i].name));
	}
	pPrinterList->count=pcReturned;
	LocalFree(pPrinterEnum);
	return 0;
}

_declspec(dllexport) int printerState(const char *printername)
{
	LPPRINTER_DEFAULTS pDefault;
	DWORD	cbBuf=0,
		cbNeeded=0,cbUsed=0;
	HANDLE hPrinter;
	PRINTER_INFO_2 * pPrinterInfo = NULL;
	PRINTER_DEFAULTS  PrintDflt; 
	DWORD statue=0;

	memset(&PrintDflt,0,sizeof(PRINTER_DEFAULTS));
	PrintDflt.DesiredAccess = PRINTER_ACCESS_USE;
	if(!OpenPrinter(printername,&hPrinter,&PrintDflt))
	{
		return -1;
	}
	GetPrinter(hPrinter,2,NULL,0,&cbNeeded);
	pPrinterInfo   =   (PRINTER_INFO_2 *)malloc(cbNeeded);
	if(pPrinterInfo == NULL)
	{
		return -1;
	}
	memset(pPrinterInfo,0,sizeof(PRINTER_INFO_2));
	if(GetPrinter(hPrinter,2,pPrinterInfo,cbNeeded,&cbUsed))
	{
		statue = pPrinterInfo->Status;
		free(pPrinterInfo);
		pPrinterInfo = NULL;
		ClosePrinter(hPrinter);
		return statue;
	}
	return -1;
}

///////////////////////////////////kai.yan add///////////////////////////////////////
///get adobe app type
_declspec(dllexport) void getAdobeType(const int type)
{
	adobeType = type;
}
////////////////////////////////////////  end  //////////////////////////////////

void ippserver()
{	
	int ipv4,ipv6;
	const char	*servername = NULL,
		*name = "virtualprinter",
		*location = "",
		*make = "NETGEAR",		
		*model = "Printer",	
		*formats = "application/octet-stream,application/pdf,image/gif,image/urf,image/jpeg,image/png,text/html,text/plain";\
	int	duplex = 0,		
		ppm = 10,		
		ppm_color = 0;		
	char		directory[1024] = {0};	
	_ipp_printer_t * printer;

	WSADATA data;
	int ret;
	if( (ret = WSAStartup (MAKEWORD (2, 2), &data)) != 0)
	{
		fprintf(stderr,"WSAStartup failed with error %d\n", ret);
		return;
	}

	mutexInit(&sp_mutex);
	mutexInit(&ipp_mutex);

	if (!directory[0])
	{
		int size;
		int i;
		char tempPathBuffer[MAX_PATH];
		size = GetTempPathA(sizeof(tempPathBuffer), tempPathBuffer);

		snprintf(directory, sizeof(directory), "%sippserver%d", tempPathBuffer,getpid());

		if (mkdir(directory) && errno != EEXIST)
		{
			fprintf(stderr, "Unable to create spool directory \"%s\": %s\n",directory, strerror(errno));
		}
	}

	while((ipv4 = create_listener(AF_INET, &(port))) < 0)
	{
		port++;
	}

	if ((ipv6 = create_listener(AF_INET6, &(port))) < 0)
	{
		perror("Unable to create IPv6 listener");
	}

	if ((printer = create_printer(servername, name, location, make, model,
		formats, ppm, ppm_color, duplex, port,
		directory)) == NULL)
		return;

	//listen
	{
		int maxfdpl;
		fd_set rset;		  
		struct timeval timeout;
		_ipp_client_t	*client;
		int fd[2];
		int i = 0;
		int result;

		fd[0] = ipv4;
		fd[1] = ipv6;

		FD_ZERO(&rset);

		while(1)
		{
			FD_SET(fd[0], &rset);
			if(fd[1] > 0)
			{
				FD_SET(fd[1], &rset);
				maxfdpl = max(fd[0],fd[1]);
			}
			else
			{
				maxfdpl =fd[0];
			}

			maxfdpl++;

			if (arrayCount(printer->jobs))
			{
				timeout.tv_sec = 10;
				timeout.tv_usec = 0;
			}
			else
			{
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;
			}

			if ((result = select(maxfdpl, &rset, NULL, NULL, &timeout)) < 0)
			{
				printf("select() failed %d",WSAGetLastError());
				perror("select() failed");
				break;
			}

			if (FD_ISSET(fd[0],&rset))
			{
				if ((client = create_client(printer, ipv4)) != NULL)
				{
					if (!threadCreate((thread_func_t)process_client, client))
					{
						perror("Unable to create client thread");
						delete_client(client);
					}
				}
			}

			if ( fd[1]>0 && FD_ISSET(fd[1],&rset))
			{
				if ((client = create_client(printer, ipv6)) != NULL)
				{
					if (!threadCreate((thread_func_t)process_client, client))
					{
						perror("Unable to create client thread");
						delete_client(client);
					}
				}
			}

			clean_jobs(printer);

			RWLockWrite(&returnlock);
			if(returnflag)
			{
				RWUnlock(&returnlock);
				break;
			}
			RWUnlock(&returnlock);
		}
	}

	sleep(10);
	if (ipv4 >= 0)
	{
		closesocket(ipv4);
		ipv4 = -1;
	}

	if (ipv6 >= 0)
	{
		closesocket(ipv6);
		ipv4 = -1;
	}


	delete_printer(printer);
}

void
clean_jobs(_ipp_printer_t *printer)	
{
	_ipp_job_t	*job;			
	time_t	cleantime;		


	if (arrayCount(printer->jobs) == 0)
		return;

	cleantime = time(NULL) - 60;

	RWLockWrite(&(printer->rwlock));
	for (job = (_ipp_job_t *)arrayFirst(printer->jobs);
		job;
		job = (_ipp_job_t *)arrayNext(printer->jobs))
		if (job->completed && job->completed < cleantime)
		{
			arrayRemove(printer->jobs, job);
			delete_job(job);
		}
		else
			break;
	RWUnlock(&(printer->rwlock));
}

int
compare_jobs(_ipp_job_t *a,		
			 _ipp_job_t *b)		
{
	return (b->id - a->id);
}

ipp_attribute_t *		
copy_attribute(
			   ipp_t           *to,		
			   ipp_attribute_t *attr,		
			   ipp_tag_t       group_tag,		
			   int             quickcopy)		
{
	int			i;		
	ipp_attribute_t	*toattr;	

	switch (attr->value_tag & ~IPP_TAG_COPY)
	{
	case IPP_TAG_ZERO :
		toattr = ippAddSeparator(to);
		break;

	case IPP_TAG_INTEGER :
	case IPP_TAG_ENUM :
		toattr = ippAddIntegers(to, group_tag, attr->value_tag,
			attr->name, attr->num_values, NULL);

		for (i = 0; i < attr->num_values; i ++)
			toattr->values[i].integer = attr->values[i].integer;
		break;

	case IPP_TAG_BOOLEAN :
		toattr = ippAddBooleans(to, group_tag, attr->name,
			attr->num_values, NULL);

		for (i = 0; i < attr->num_values; i ++)
			toattr->values[i].boolean = attr->values[i].boolean;
		break;

	case IPP_TAG_TEXT :
	case IPP_TAG_NAME :
	case IPP_TAG_KEYWORD :
	case IPP_TAG_URI :
	case IPP_TAG_URISCHEME :
	case IPP_TAG_CHARSET :
	case IPP_TAG_LANGUAGE :
	case IPP_TAG_MIMETYPE :
		toattr = ippAddStrings(to, group_tag,
			(ipp_tag_t)(attr->value_tag | quickcopy),
			attr->name, attr->num_values, NULL, NULL);

		if (quickcopy)
		{
			for (i = 0; i < attr->num_values; i ++)
				toattr->values[i].string.text = attr->values[i].string.text;
		}
		else
		{
			for (i = 0; i < attr->num_values; i ++)
				toattr->values[i].string.text =
				stralloc(attr->values[i].string.text);
		}
		break;

	case IPP_TAG_DATE :
		toattr = ippAddDate(to, group_tag, attr->name,
			attr->values[0].date);
		break;

	case IPP_TAG_RESOLUTION :
		toattr = ippAddResolutions(to, group_tag, attr->name,
			attr->num_values, IPP_RES_PER_INCH,
			NULL, NULL);

		for (i = 0; i < attr->num_values; i ++)
		{
			toattr->values[i].resolution.xres  = attr->values[i].resolution.xres;
			toattr->values[i].resolution.yres  = attr->values[i].resolution.yres;
			toattr->values[i].resolution.units = attr->values[i].resolution.units;
		}
		break;

	case IPP_TAG_RANGE :
		toattr = ippAddRanges(to, group_tag, attr->name,
			attr->num_values, NULL, NULL);

		for (i = 0; i < attr->num_values; i ++)
		{
			toattr->values[i].range.lower = attr->values[i].range.lower;
			toattr->values[i].range.upper = attr->values[i].range.upper;
		}
		break;

	case IPP_TAG_TEXTLANG :
	case IPP_TAG_NAMELANG :
		toattr = ippAddStrings(to, group_tag,
			(ipp_tag_t)(attr->value_tag | quickcopy),
			attr->name, attr->num_values, NULL, NULL);

		if (quickcopy)
		{
			for (i = 0; i < attr->num_values; i ++)
			{
				toattr->values[i].string.charset = attr->values[i].string.charset;
				toattr->values[i].string.text    = attr->values[i].string.text;
			}
		}
		else
		{
			for (i = 0; i < attr->num_values; i ++)
			{
				if (!i)
					toattr->values[i].string.charset =
					stralloc(attr->values[i].string.charset);
				else
					toattr->values[i].string.charset =
					toattr->values[0].string.charset;

				toattr->values[i].string.text =
					stralloc(attr->values[i].string.text);
			}
		}
		break;

	case IPP_TAG_BEGIN_COLLECTION :
		toattr = ippAddCollections(to, group_tag, attr->name,
			attr->num_values, NULL);

		for (i = 0; i < attr->num_values; i ++)
		{
			toattr->values[i].collection = attr->values[i].collection;
			attr->values[i].collection->use ++;
		}
		break;

	case IPP_TAG_STRING :
		if (quickcopy)
		{
			toattr = ippAddOctetString(to, group_tag, attr->name, NULL, 0);
			toattr->value_tag = (ipp_tag_t)((int)toattr->value_tag | quickcopy);
			toattr->values[0].unknown.data   = attr->values[0].unknown.data;
			toattr->values[0].unknown.length = attr->values[0].unknown.length;
		}
		else
			toattr = ippAddOctetString(to, attr->group_tag, attr->name,
			attr->values[0].unknown.data,
			attr->values[0].unknown.length);
		break;

	default :
		toattr = ippAddIntegers(to, group_tag, attr->value_tag,
			attr->name, attr->num_values, NULL);

		for (i = 0; i < attr->num_values; i ++)
		{
			toattr->values[i].unknown.length = attr->values[i].unknown.length;

			if (toattr->values[i].unknown.length > 0)
			{
				if ((toattr->values[i].unknown.data =
					malloc(toattr->values[i].unknown.length)) == NULL)
					toattr->values[i].unknown.length = 0;
				else
					memcpy(toattr->values[i].unknown.data,
					attr->values[i].unknown.data,
					toattr->values[i].unknown.length);
			}
		}
		break; 
	}

	return (toattr);
}

void
copy_attributes(ipp_t        *to,	
				ipp_t        *from,	
				array_t *ra,	
				ipp_tag_t    group_tag,	
				int          quickcopy)	
{
	ipp_attribute_t	*fromattr;	


	if (!to || !from)
		return;

	for (fromattr = from->attrs; fromattr; fromattr = fromattr->next)
	{
		if ((group_tag != IPP_TAG_ZERO && fromattr->group_tag != group_tag &&
			fromattr->group_tag != IPP_TAG_ZERO) || !fromattr->name)
			continue;

		if (!ra || arrayFind(ra, fromattr->name))
			copy_attribute(to, fromattr, fromattr->group_tag, quickcopy);
	}
}

void
copy_job_attributes(
					_ipp_client_t *client,		
					_ipp_job_t    *job,			
					array_t  *ra)			
{
	copy_attributes(client->response, job->attrs, ra, IPP_TAG_ZERO, 0);

	if (!ra || arrayFind(ra, "job-printer-up-time"))
		ippAddInteger(client->response, IPP_TAG_JOB, IPP_TAG_INTEGER,
		"job-printer-up-time", (int)time(NULL));

	if (!ra || arrayFind(ra, "job-state"))
		ippAddInteger(client->response, IPP_TAG_JOB, IPP_TAG_ENUM,
		"job-state", job->state);

	if (!ra || arrayFind(ra, "job-state-reasons"))
	{
		switch (job->state)
		{
		case IPP_JOB_PENDING :
			ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "none");
			break;

		case IPP_JOB_HELD :
			if (job->fd >= 0)
				ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "job-incoming");
			else if (ippFindAttribute(job->attrs, "job-hold-until", IPP_TAG_ZERO))
				ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "job-hold-until-specified");
			break;

		case IPP_JOB_PROCESSING :
			if (job->cancel)
				ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "processing-to-stop-point");
			else
				ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "job-printing");
			break;

		case IPP_JOB_STOPPED :
			ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "job-stopped");
			break;

		case IPP_JOB_CANCELED :
			ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "job-canceled-by-user");
			break;

		case IPP_JOB_ABORTED :
			ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "aborted-by-system");
			break;

		case IPP_JOB_COMPLETED :
			ippAddString(client->response, IPP_TAG_JOB,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "job-state-reasons",
				NULL, "job-completed-successfully");
			break;
		}
	}

	if (!ra || arrayFind(ra, "time-at-completed"))
		ippAddInteger(client->response, IPP_TAG_JOB,
		job->completed ? IPP_TAG_INTEGER : IPP_TAG_NOVALUE,
		"time-at-completed", job->completed);

	if (!ra || arrayFind(ra, "time-at-processing"))
		ippAddInteger(client->response, IPP_TAG_JOB,
		job->processing ? IPP_TAG_INTEGER : IPP_TAG_NOVALUE,
		"time-at-processing", job->processing);
}

_ipp_client_t *			
create_client(_ipp_printer_t *printer,	
			  int            sock)	
{
	_ipp_client_t	*client;		
	int		val;			
	socklen_t	addrlen;		


	if ((client = (_ipp_client_t *)calloc(1, sizeof(_ipp_client_t))) == NULL)
	{
		perror("Unable to allocate memory for client");
		return (NULL);
	}

	client->printer       = printer;
	memset(&(client->http), 0, sizeof(client->http));
	client->http.activity = time(NULL);
	client->http.hostaddr = &(client->addr);
	client->http.blocking = 0;
	client->http.timeout_cb = NULL;

	addrlen = sizeof(http_addr_t);

	if ((client->http.fd = accept(sock, (struct sockaddr *)&(client->addr),
		&addrlen)) < 0)
	{
		perror("Unable to accept client connection");

		free(client);

		return (NULL);
	}

	httpAddrString(&(client->addr), client->http.hostname,
		sizeof(client->http.hostname));

	fprintf(stderr, "Accepted connection from %s (%s)\n", client->http.hostname,
		client->http.hostaddr->addr.sa_family == AF_INET ? "IPv4" : "IPv6");

	val = 1;
	setsockopt(client->http.fd, IPPROTO_TCP, TCP_NODELAY, (char *)&val,
		sizeof(val));

	return (client);
}

_ipp_job_t *			
create_job(_ipp_client_t *client)	
{
	_ipp_job_t		*job;		
	ipp_attribute_t	*attr;		
	char			uri[1024];	


	RWLockWrite(&(client->printer->rwlock));
	if (client->printer->active_job &&
		client->printer->active_job->state < IPP_JOB_CANCELED)
	{
		RWUnlock(&(client->printer->rwlock));
		return (NULL);
	}

	if ((job = (_ipp_job_t *)calloc(1, sizeof(_ipp_job_t))) == NULL)
	{
		perror("Unable to allocate memory for job");
		return (NULL);
	}

	job->printer    = client->printer;
	job->attrs      = client->request;
	job->state      = IPP_JOB_HELD;
	job->fd         = -1;
	client->request = NULL;


	for (attr = job->attrs->attrs->next->next; attr; attr = attr->next)
		attr->group_tag = IPP_TAG_JOB;

	if ((attr = ippFindAttribute(job->attrs, "requesting-user-name",
		IPP_TAG_NAME)) != NULL)
	{
		strfree(attr->name);
		attr->name = stralloc("job-originating-user-name");
		fprintf(stderr,"user name: %s\n", attr->name);
	}
	else
		attr = ippAddString(job->attrs, IPP_TAG_JOB, (ipp_tag_t)(IPP_TAG_NAME | IPP_TAG_COPY),
		"job-originating-user-name", NULL, "anonymous");

	if (attr)
		job->username = attr->values[0].string.text;
	else
		job->username = "anonymous";

	if ((attr = ippFindAttribute(job->attrs, "document-format",
		IPP_TAG_MIMETYPE)) != NULL)
		job->format = attr->values[0].string.text;
	else
		job->format = "application/octet-stream";

	job->id = client->printer->next_job_id ++;

	snprintf(uri, sizeof(uri), "%s/%d", client->printer->uri, job->id);

	ippAddInteger(job->attrs, IPP_TAG_JOB, IPP_TAG_INTEGER, "job-id", job->id);
	ippAddString(job->attrs, IPP_TAG_JOB, IPP_TAG_URI, "job-uri", NULL, uri);
	ippAddString(job->attrs, IPP_TAG_JOB, IPP_TAG_URI, "job-printer-uri", NULL,
		client->printer->uri);
	ippAddInteger(job->attrs, IPP_TAG_JOB, IPP_TAG_INTEGER, "time-at-creation",
		(int)time(NULL));

	arrayAdd(client->printer->jobs, job);
	client->printer->active_job = job;

	RWUnlock(&(client->printer->rwlock));

	return (job);
}

int				
create_listener(int family,		
				int *port)		
{
	int		sock,			
		val;			
	http_addr_t	address;		
	socklen_t	addrlen;		


	if ((sock = socket(family, SOCK_STREAM, 0)) < 0)
		return (-1);

	val = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));

#ifdef IPV6_V6ONLY
	if (family == AF_INET6)
		setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&val, sizeof(val));
#endif 

	if (!*port)
	{
		*port = 631;
	}

	memset(&address, 0, sizeof(address));
	address.addr.sa_family = family;
	_httpAddrSetPort(&address, *port);

	if (bind(sock, (struct sockaddr *)&address, httpAddrLength(&address)))
	{
		closesocket(sock);
		return (-1);
	}

	if (listen(sock, 5))
	{
		closesocket(sock);
		return (-1);
	}

	return (sock);
}

ipp_t *				
create_media_col(const char *media,	
				 const char *type,	
				 int        width,	
				 int        length,	
				 int        margins)	
{
	ipp_t	*media_col = ippNew(),		
		*media_size = ippNew();		
	char	media_key[256];			


	ippAddInteger(media_size, IPP_TAG_PRINTER, IPP_TAG_INTEGER, "x-dimension",
		width);
	ippAddInteger(media_size, IPP_TAG_PRINTER, IPP_TAG_INTEGER, "y-dimension",
		length);

	snprintf(media_key, sizeof(media_key), "%s_%s%s", media, type,
		margins == 0 ? "_borderless" : "");

	ippAddString(media_col, IPP_TAG_PRINTER, IPP_TAG_KEYWORD, "media-key", NULL,
		media_key);
	ippAddCollection(media_col, IPP_TAG_PRINTER, "media-size", media_size);
	ippAddInteger(media_col, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-bottom-margin", margins);
	ippAddInteger(media_col, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-left-margin", margins);
	ippAddInteger(media_col, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-right-margin", margins);
	ippAddInteger(media_col, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-top-margin", margins);
	ippAddString(media_col, IPP_TAG_PRINTER, IPP_TAG_KEYWORD, "media-type",
		NULL, type);

	ippDelete(media_size);

	return (media_col);
}

_ipp_printer_t *			
create_printer(const char *servername,	
			   const char *name,	
			   const char *location,	
			   const char *make,	
			   const char *model,	
			   const char *docformats,	
			   int        ppm,		
			   int        ppm_color,	
			   int        duplex,	
			   int        port,		
			   const char *directory)	
{
	int i, j;		
	_ipp_printer_t	*printer;	
	char			hostname[256],	
		uri[1024],	
		adminurl[1024],	
		device_id[1024],
		make_model[128];
	int			num_formats;	
	char			*defformat,	
		*formats[100],	
		*ptr;		
	const char		*prefix;	
	int			num_database;	
	ipp_attribute_t	*media_col_database;
	
	ipp_t			*media_col_default;
	
	ipp_value_t		*media_col_value;
	
	int			k_supported;	
	static const int	orients[4] =	
	{
		IPP_PORTRAIT,
		IPP_LANDSCAPE,
		IPP_REVERSE_LANDSCAPE,
		IPP_REVERSE_PORTRAIT
	};
	static const char * const versions[] =
	{
		"1.0",
		"1.1",
		"2.0"
	};
	static const int	ops[] =		
	{
		IPP_PRINT_JOB,
		IPP_VALIDATE_JOB,
		IPP_SEND_DOCUMENT,
		IPP_CANCEL_JOB,
		IPP_GET_JOB_ATTRIBUTES,
		IPP_GET_JOBS,
		IPP_GET_PRINTER_ATTRIBUTES
	};
	static const char * const charsets[] =
	{
		"us-ascii",
		"utf-8"
	};
	static const char * const job_creation[] =
	{					
		"copies",
		"ipp-attribute-fidelity",
		"job-name",
		"job-priority",
		"media",
		"media-col",
		"multiple-document-handling",
		"orientation-requested",
		"print-quality",
		"sides"
	};
	static const char * const media_col_supported[] =
	{					
		"media-bottom-margin",
		"media-left-margin",
		"media-right-margin",
		"media-size",
		"media-top-margin",
		"media-type"
	};
	static const int	media_xxx_margin_supported[] =
	{					
		0,
		635
	};
	static const char * const multiple_document_handling[] =
	{					
		"separate-documents-uncollated-copies",
		"separate-documents-collated-copies"
	};
	static const int	print_quality_supported[] =
	{					
		IPP_QUALITY_DRAFT,
		IPP_QUALITY_NORMAL,
		IPP_QUALITY_HIGH
	};
	static const char * const sides_supported[] =
	{					
		"one-sided",
		"two-sided-long-edge",
		"two-sided-short-edge"
	};
	static const char * const which_jobs[] =
	{					
		"completed",
		"not-completed",
		"aborted",
		"all",
		"canceled",
		"pending",
		"pending-held",
		"processing",
		"processing-stopped"
	};

	if ((printer = (_ipp_printer_t *)calloc(1, sizeof(_ipp_printer_t))) == NULL)
	{
		perror("Unable to allocate memory for printer");
		return (NULL);
	}

	printer->name          = stralloc(name);
	printer->directory     = stralloc(directory);
	printer->hostname      = stralloc(servername ? servername :httpGetHostname(NULL, hostname,sizeof(hostname)));
	printer->port          = port;
	printer->state         = IPP_PRINTER_IDLE;
	printer->state_reasons = _IPP_PRINTER_NONE;
	printer->jobs          = arrayNew((array_func_t)compare_jobs, NULL);
	printer->next_job_id   = 1;

	httpAssembleURI(HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp", NULL,
		printer->hostname, printer->port, "/ipp");
	printer->uri    = stralloc(uri);
	printer->urilen = strlen(uri);

	RWInit(&(printer->rwlock));

	httpAssembleURI(HTTP_URI_CODING_ALL, adminurl, sizeof(adminurl), "http", NULL,
		printer->hostname, printer->port, "/");

	snprintf(make_model, sizeof(make_model), "%s %s", make, model);

	num_formats = 1;
	formats[0]  = strdup(docformats);
	defformat   = formats[0];
	for (ptr = strchr(formats[0], ','); ptr; ptr = strchr(ptr, ','))
	{
		*ptr++ = '\0';
		formats[num_formats++] = ptr;

		if (!strcaseinsensitivecmp(ptr, "application/octet-stream"))
			defformat = ptr;
	}

	snprintf(device_id, sizeof(device_id), "MFG:%s;MDL:%s;", make, model);
	ptr    = device_id + strlen(device_id);
	prefix = "CMD:";
	for (i = 0; i < num_formats; i ++)
	{
		if (!strcaseinsensitivecmp(formats[i], "application/pdf"))
			snprintf(ptr, sizeof(device_id) - (ptr - device_id), "%sPDF", prefix);
		else if (!strcaseinsensitivecmp(formats[i], "application/postscript"))
			snprintf(ptr, sizeof(device_id) - (ptr - device_id), "%sPS", prefix);
		else if (!strcaseinsensitivecmp(formats[i], "application/vnd.hp-PCL"))
			snprintf(ptr, sizeof(device_id) - (ptr - device_id), "%sPCL", prefix);
		else if (!strcaseinsensitivecmp(formats[i], "image/jpeg"))
			snprintf(ptr, sizeof(device_id) - (ptr - device_id), "%sJPEG", prefix);
		else if (!strcaseinsensitivecmp(formats[i], "image/png"))
			snprintf(ptr, sizeof(device_id) - (ptr - device_id), "%sPNG", prefix);
		else if (strcaseinsensitivecmp(formats[i], "application/octet-stream"))
			snprintf(ptr, sizeof(device_id) - (ptr - device_id), "%s%s", prefix,
			formats[i]);

		ptr += strlen(ptr);
		prefix = ",";
	}
	strncat(device_id, ";", sizeof(device_id));

	k_supported = INT_MAX;

	printer->attrs = ippNew();

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_CHARSET | IPP_TAG_COPY),
		"charset-configured", NULL, "utf-8");

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_CHARSET | IPP_TAG_COPY),
		"charset-supported", sizeof(charsets) / sizeof(charsets[0]),
		NULL, charsets);

	
	ippAddBoolean(printer->attrs, IPP_TAG_PRINTER, "color-supported",
		ppm_color > 0);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"compression-supported", NULL, "none");

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"copies-default", 1);

	
	ippAddRange(printer->attrs, IPP_TAG_PRINTER, "copies-supported", 1, 999);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_MIMETYPE,
		"document-format-default", NULL, defformat);

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_MIMETYPE,
		"document-format-supported", num_formats, NULL,
		(const char * const *)formats);

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"finishings-default", IPP_FINISHINGS_NONE);

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"finishings-supported", IPP_FINISHINGS_NONE);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_LANGUAGE | IPP_TAG_COPY),
		"generated-natural-language-supported", NULL, "en");

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"ipp-versions-supported",
		sizeof(versions) / sizeof(versions[0]), NULL, versions);

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"job-creation-attributes-supported",
		sizeof(job_creation) / sizeof(job_creation[0]),
		NULL, job_creation);

	
	ippAddRange(printer->attrs, IPP_TAG_PRINTER, "job-k-octets-supported", 0,
		k_supported);

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"job-priority-default", 50);

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"job-priority-supported", 100);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_NAME | IPP_TAG_COPY),
		"job-sheets-default", NULL, "none");

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_NAME | IPP_TAG_COPY),
		"job-sheets-supported", NULL, "none");

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-bottom-margin-supported",
		(int)(sizeof(media_xxx_margin_supported) /
		sizeof(media_xxx_margin_supported[0])),
		media_xxx_margin_supported);

	
	for (num_database = 0, i = 0;
		i < (int)(sizeof(media_col_sizes) / sizeof(media_col_sizes[0]));
		i ++)
	{
		if (media_col_sizes[i][2] == _IPP_ENV_ONLY)
			num_database += 2;		
		else if (media_col_sizes[i][2] == _IPP_PHOTO_ONLY)
			num_database += 12;		
		else
			num_database += (int)(sizeof(media_type_supported) /
			sizeof(media_type_supported[0])) + 6;
		
	}

	media_col_database = ippAddCollections(printer->attrs, IPP_TAG_PRINTER,
		"media-col-database", num_database,
		NULL);
	for (media_col_value = media_col_database->values, i = 0;
		i < (int)(sizeof(media_col_sizes) / sizeof(media_col_sizes[0]));
		i ++)
	{
		for (j = 0;
			j < (int)(sizeof(media_type_supported) /
			sizeof(media_type_supported[0]));
		j ++)
		{
			if (media_col_sizes[i][2] == _IPP_ENV_ONLY &&
				strcmp(media_type_supported[j], "auto") &&
				strcmp(media_type_supported[j], "envelope"))
				continue;
			else if (media_col_sizes[i][2] == _IPP_PHOTO_ONLY &&
				strcmp(media_type_supported[j], "auto") &&
				strncmp(media_type_supported[j], "photographic-", 13))
				continue;

			media_col_value->collection =
				create_media_col(media_supported[i], media_type_supported[j],
				media_col_sizes[i][0], media_col_sizes[i][1],
				media_xxx_margin_supported[1]);
			media_col_value ++;

			if (media_col_sizes[i][2] != _IPP_ENV_ONLY &&
				(!strcmp(media_type_supported[j], "auto") ||
				!strncmp(media_type_supported[j], "photographic-", 13)))
			{
				media_col_value->collection =
					create_media_col(media_supported[i], media_type_supported[j],
					media_col_sizes[i][0], media_col_sizes[i][1],
					media_xxx_margin_supported[0]);
				media_col_value ++;
			}
		}
	}

	
	media_col_default = create_media_col(media_supported[0],
		media_type_supported[0],
		media_col_sizes[0][0],
		media_col_sizes[0][1],
		media_xxx_margin_supported[1]);

	ippAddCollection(printer->attrs, IPP_TAG_PRINTER, "media-col-default",
		media_col_default);
	ippDelete(media_col_default);

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"media-col-supported",
		(int)(sizeof(media_col_supported) /
		sizeof(media_col_supported[0])), NULL,
		media_col_supported);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"media-default", NULL, media_supported[0]);

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-left-margin-supported",
		(int)(sizeof(media_xxx_margin_supported) /
		sizeof(media_xxx_margin_supported[0])),
		media_xxx_margin_supported);

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-right-margin-supported",
		(int)(sizeof(media_xxx_margin_supported) /
		sizeof(media_xxx_margin_supported[0])),
		media_xxx_margin_supported);

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"media-supported",
		(int)(sizeof(media_supported) / sizeof(media_supported[0])),
		NULL, media_supported);

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"media-top-margin-supported",
		(int)(sizeof(media_xxx_margin_supported) /
		sizeof(media_xxx_margin_supported[0])),
		media_xxx_margin_supported);

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"multiple-document-handling-supported",
		sizeof(multiple_document_handling) /
		sizeof(multiple_document_handling[0]), NULL,
		multiple_document_handling);

	
	ippAddBoolean(printer->attrs, IPP_TAG_PRINTER,
		"multiple-document-jobs-supported", 0);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_LANGUAGE | IPP_TAG_COPY),
		"natural-language-configured", NULL, "en");

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"number-up-default", 1);

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"number-up-supported", 1);

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"operations-supported", sizeof(ops) / sizeof(ops[0]), ops);

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_NOVALUE,
		"orientation-requested-default", 0);

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"orientation-requested-supported", 4, orients);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"output-bin-default", NULL, "face-down");

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"output-bin-supported", NULL, "face-down");

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"pages-per-minute", ppm);

	
	if (ppm_color > 0)
		ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"pages-per-minute-color", ppm_color);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"pdl-override-supported", NULL, "attempted");

	
	ippAddInteger(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"print-quality-default", IPP_QUALITY_NORMAL);

	
	ippAddIntegers(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"print-quality-supported",
		(int)(sizeof(print_quality_supported) /
		sizeof(print_quality_supported[0])),
		print_quality_supported);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_TEXT,
		"printer-device-id", NULL, device_id);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_URI,
		"printer-icons", NULL, "/icon.png");

	
	ippAddBoolean(printer->attrs, IPP_TAG_PRINTER, "printer-is-accepting-jobs",
		1);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_TEXT, "printer-info",
		NULL, name);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_TEXT,
		"printer-location", NULL, location);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_TEXT,
		"printer-make-and-model", NULL, make_model);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_URI,
		"printer-more-info", NULL, adminurl);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_NAME, "printer-name",
		NULL, name);

	
	ippAddResolution(printer->attrs, IPP_TAG_PRINTER,
		"printer-resolution-default", IPP_RES_PER_INCH, 600, 600);

	
	ippAddResolution(printer->attrs, IPP_TAG_PRINTER,
		"printer-resolution-supported", IPP_RES_PER_INCH, 600, 600);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, IPP_TAG_URI,
		"printer-uri-supported", NULL, uri);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"sides-default", NULL, "one-sided");

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"sides-supported", duplex ? 3 : 1, NULL, sides_supported);

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"uri-authentication-supported", NULL, "none");

	
	ippAddString(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"uri-security-supported", NULL, "none");

	
	ippAddStrings(printer->attrs, IPP_TAG_PRINTER, (ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),
		"which-jobs-supported",
		sizeof(which_jobs) / sizeof(which_jobs[0]), NULL, which_jobs);

	free(formats[0]);

	debug_attributes("Printer", printer->attrs);

	return (printer);

bad_printer:

	delete_printer(printer);
	return (NULL);
}

array_t *			
create_requested_array(
					   _ipp_client_t *client)		
{
	int			i;		
	ipp_attribute_t	*requested;	
	array_t		*ra;		
	char			*value;		

	if ((requested = ippFindAttribute(client->request, "requested-attributes",
		IPP_TAG_KEYWORD)) == NULL)
		return (NULL);

	if (requested->num_values == 1 &&
		!strcmp(requested->values[0].string.text, "all"))
		return (NULL);

	ra = arrayNew((array_func_t)strcmp, NULL);

	for (i = 0; i < requested->num_values; i ++)
	{
		value = requested->values[i].string.text;

		if (!strcmp(value, "job-template"))
		{
			arrayAdd(ra, "copies");
			arrayAdd(ra, "copies-default");
			arrayAdd(ra, "copies-supported");
			arrayAdd(ra, "finishings");
			arrayAdd(ra, "finishings-default");
			arrayAdd(ra, "finishings-supported");
			arrayAdd(ra, "job-hold-until");
			arrayAdd(ra, "job-hold-until-default");
			arrayAdd(ra, "job-hold-until-supported");
			arrayAdd(ra, "job-priority");
			arrayAdd(ra, "job-priority-default");
			arrayAdd(ra, "job-priority-supported");
			arrayAdd(ra, "job-sheets");
			arrayAdd(ra, "job-sheets-default");
			arrayAdd(ra, "job-sheets-supported");
			arrayAdd(ra, "media");
			arrayAdd(ra, "media-col");
			arrayAdd(ra, "media-col-default");
			arrayAdd(ra, "media-col-supported");
			arrayAdd(ra, "media-default");
			arrayAdd(ra, "media-source-supported");
			arrayAdd(ra, "media-supported");
			arrayAdd(ra, "media-type-supported");
			arrayAdd(ra, "multiple-document-handling");
			arrayAdd(ra, "multiple-document-handling-default");
			arrayAdd(ra, "multiple-document-handling-supported");
			arrayAdd(ra, "number-up");
			arrayAdd(ra, "number-up-default");
			arrayAdd(ra, "number-up-supported");
			arrayAdd(ra, "orientation-requested");
			arrayAdd(ra, "orientation-requested-default");
			arrayAdd(ra, "orientation-requested-supported");
			arrayAdd(ra, "page-ranges");
			arrayAdd(ra, "page-ranges-supported");
			arrayAdd(ra, "printer-resolution");
			arrayAdd(ra, "printer-resolution-default");
			arrayAdd(ra, "printer-resolution-supported");
			arrayAdd(ra, "print-quality");
			arrayAdd(ra, "print-quality-default");
			arrayAdd(ra, "print-quality-supported");
			arrayAdd(ra, "sides");
			arrayAdd(ra, "sides-default");
			arrayAdd(ra, "sides-supported");
		}
		else if (!strcmp(value, "job-description"))
		{
			arrayAdd(ra, "date-time-at-completed");
			arrayAdd(ra, "date-time-at-creation");
			arrayAdd(ra, "date-time-at-processing");
			arrayAdd(ra, "job-detailed-status-message");
			arrayAdd(ra, "job-document-access-errors");
			arrayAdd(ra, "job-id");
			arrayAdd(ra, "job-impressions");
			arrayAdd(ra, "job-impressions-completed");
			arrayAdd(ra, "job-k-octets");
			arrayAdd(ra, "job-k-octets-processed");
			arrayAdd(ra, "job-media-sheets");
			arrayAdd(ra, "job-media-sheets-completed");
			arrayAdd(ra, "job-message-from-operator");
			arrayAdd(ra, "job-more-info");
			arrayAdd(ra, "job-name");
			arrayAdd(ra, "job-originating-user-name");
			arrayAdd(ra, "job-printer-up-time");
			arrayAdd(ra, "job-printer-uri");
			arrayAdd(ra, "job-state");
			arrayAdd(ra, "job-state-message");
			arrayAdd(ra, "job-state-reasons");
			arrayAdd(ra, "job-uri");
			arrayAdd(ra, "number-of-documents");
			arrayAdd(ra, "number-of-intervening-jobs");
			arrayAdd(ra, "output-device-assigned");
			arrayAdd(ra, "time-at-completed");
			arrayAdd(ra, "time-at-creation");
			arrayAdd(ra, "time-at-processing");
		}
		else if (!strcmp(value, "printer-description"))
		{
			arrayAdd(ra, "charset-configured");
			arrayAdd(ra, "charset-supported");
			arrayAdd(ra, "color-supported");
			arrayAdd(ra, "compression-supported");
			arrayAdd(ra, "document-format-default");
			arrayAdd(ra, "document-format-supported");
			arrayAdd(ra, "generated-natural-language-supported");
			arrayAdd(ra, "ipp-versions-supported");
			arrayAdd(ra, "job-impressions-supported");
			arrayAdd(ra, "job-k-octets-supported");
			arrayAdd(ra, "job-media-sheets-supported");
			arrayAdd(ra, "multiple-document-jobs-supported");
			arrayAdd(ra, "multiple-operation-time-out");
			arrayAdd(ra, "natural-language-configured");
			arrayAdd(ra, "notify-attributes-supported");
			arrayAdd(ra, "notify-lease-duration-default");
			arrayAdd(ra, "notify-lease-duration-supported");
			arrayAdd(ra, "notify-max-events-supported");
			arrayAdd(ra, "notify-events-default");
			arrayAdd(ra, "notify-events-supported");
			arrayAdd(ra, "notify-pull-method-supported");
			arrayAdd(ra, "notify-schemes-supported");
			arrayAdd(ra, "operations-supported");
			arrayAdd(ra, "pages-per-minute");
			arrayAdd(ra, "pages-per-minute-color");
			arrayAdd(ra, "pdl-override-supported");
			arrayAdd(ra, "printer-alert");
			arrayAdd(ra, "printer-alert-description");
			arrayAdd(ra, "printer-current-time");
			arrayAdd(ra, "printer-driver-installer");
			arrayAdd(ra, "printer-info");
			arrayAdd(ra, "printer-is-accepting-jobs");
			arrayAdd(ra, "printer-location");
			arrayAdd(ra, "printer-make-and-model");
			arrayAdd(ra, "printer-message-from-operator");
			arrayAdd(ra, "printer-more-info");
			arrayAdd(ra, "printer-more-info-manufacturer");
			arrayAdd(ra, "printer-name");
			arrayAdd(ra, "printer-state");
			arrayAdd(ra, "printer-state-message");
			arrayAdd(ra, "printer-state-reasons");
			arrayAdd(ra, "printer-up-time");
			arrayAdd(ra, "printer-uri-supported");
			arrayAdd(ra, "queued-job-count");
			arrayAdd(ra, "reference-uri-schemes-supported");
			arrayAdd(ra, "uri-authentication-supported");
			arrayAdd(ra, "uri-security-supported");
		}
		else if (!strcmp(value, "printer-defaults"))
		{
			arrayAdd(ra, "copies-default");
			arrayAdd(ra, "document-format-default");
			arrayAdd(ra, "finishings-default");
			arrayAdd(ra, "job-hold-until-default");
			arrayAdd(ra, "job-priority-default");
			arrayAdd(ra, "job-sheets-default");
			arrayAdd(ra, "media-default");
			arrayAdd(ra, "media-col-default");
			arrayAdd(ra, "number-up-default");
			arrayAdd(ra, "orientation-requested-default");
			arrayAdd(ra, "sides-default");
		}
		else if (!strcmp(value, "subscription-template"))
		{
			arrayAdd(ra, "notify-attributes");
			arrayAdd(ra, "notify-charset");
			arrayAdd(ra, "notify-events");
			arrayAdd(ra, "notify-lease-duration");
			arrayAdd(ra, "notify-natural-language");
			arrayAdd(ra, "notify-pull-method");
			arrayAdd(ra, "notify-recipient-uri");
			arrayAdd(ra, "notify-time-interval");
			arrayAdd(ra, "notify-user-data");
		}
		else
			arrayAdd(ra, value);
	}

	return (ra);
}

void
debug_attributes(const char *title,	
				 ipp_t      *ipp)	
{
	ipp_tag_t		group_tag;	
	ipp_attribute_t	*attr;		
	char			buffer[2048];	

	fprintf(stderr, "%s:\n", title);
	for (attr = ipp->attrs, group_tag = IPP_TAG_ZERO; attr; attr = attr->next)
	{
		if (attr->group_tag != group_tag)
		{
			group_tag = attr->group_tag;
			fprintf(stderr, "  %s\n", ippTagString(group_tag));
		}

		if (attr->name)
		{
			_ippAttrString(attr, buffer, sizeof(buffer));
			fprintf(stderr, "    %s (%s%s) %s\n", attr->name,
				attr->num_values > 1 ? "1setOf " : "",
				ippTagString(attr->value_tag), buffer);
		}
	}
}

void
delete_client(_ipp_client_t *client)	
{

	httpFlushWrite(&(client->http));

	if (client->http.fd >= 0)
		closesocket(client->http.fd);

	httpClearCookie(&(client->http));
	httpClearFields(&(client->http));

	ippDelete(client->request);

	ippDelete(client->response);

	free(client);
}

void
delete_job(_ipp_job_t *job)		
{
	ippDelete(job->attrs);

	if (job->filename)
	{
		//unlink(job->filename);
		free(job->filename);
	}

	free(job);
}

void
delete_printer(_ipp_printer_t *printer)	
{
	if (printer->name)
		strfree(printer->name);
	if (printer->directory)
		strfree(printer->directory);
	if (printer->hostname)
		strfree(printer->hostname);
	if (printer->uri)
		strfree(printer->uri);

	ippDelete(printer->attrs);
	arrayDelete(printer->jobs);

	free(printer);
}

_ipp_job_t *			
find_job(_ipp_client_t *client)		
{
	ipp_attribute_t	*attr;		
	_ipp_job_t		key,		
		*job;		


	key.id = 0;

	if ((attr = ippFindAttribute(client->request, "job-uri",
		IPP_TAG_URI)) != NULL)
	{
		if (!strncmp(attr->values[0].string.text, client->printer->uri,
			client->printer->urilen) &&
			attr->values[0].string.text[client->printer->urilen] == '/')
			key.id = atoi(attr->values[0].string.text + client->printer->urilen + 1);
	}
	else if ((attr = ippFindAttribute(client->request, "job-id",
		IPP_TAG_INTEGER)) != NULL)
		key.id = attr->values[0].integer;

	RWLockRead(&(client->printer->rwlock));
	job = (_ipp_job_t *)arrayFind(client->printer->jobs, &key);
	RWUnlock(&(client->printer->rwlock));

	return (job);
}

void
html_escape(_ipp_client_t *client,	
			const char    *s,		
			size_t        slen)		
{
	const char	*start,			
		*end;			


	start = s;
	end   = s + (slen > 0 ? slen : strlen(s));

	while (*s && s < end)
	{
		if (*s == '&' || *s == '<')
		{
			if (s > start)
				httpWrite2(&(client->http), start, s - start);

			if (*s == '&')
				httpWrite2(&(client->http), "&amp;", 5);
			else
				httpWrite2(&(client->http), "&lt;", 4);

			start = s + 1;
		}

		s ++;
	}

	if (s > start)
		httpWrite2(&(client->http), start, s - start);
}

void
html_printf(_ipp_client_t *client,	
			const char    *format,	
			...)			
{
	va_list	ap;			
	const char	*start;			
	char		size,			
		type;			
	int		width,			
		prec;			
	char		tformat[100],		
		*tptr,			
		temp[1024];		
	char		*s;			

	va_start(ap, format);
	start = format;

	while (*format)
	{
		if (*format == '%')
		{
			if (format > start)
				httpWrite2(&(client->http), start, format - start);

			tptr    = tformat;
			*tptr++ = *format++;

			if (*format == '%')
			{
				httpWrite2(&(client->http), "%", 1);
				format ++;
				continue;
			}
			else if (strchr(" -+#\'", *format))
				*tptr++ = *format++;

			if (*format == '*')
			{

				format ++;
				width = va_arg(ap, int);

				snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", width);
				tptr += strlen(tptr);
			}
			else
			{
				width = 0;

				while (isdigit(*format & 255))
				{
					if (tptr < (tformat + sizeof(tformat) - 1))
						*tptr++ = *format;

					width = width * 10 + *format++ - '0';
				}
			}

			if (*format == '.')
			{
				if (tptr < (tformat + sizeof(tformat) - 1))
					*tptr++ = *format;

				format ++;

				if (*format == '*')
				{
					/*
					* Get precision from argument...
					*/

					format ++;
					prec = va_arg(ap, int);

					snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", prec);
					tptr += strlen(tptr);
				}
				else
				{
					prec = 0;

					while (isdigit(*format & 255))
					{
						if (tptr < (tformat + sizeof(tformat) - 1))
							*tptr++ = *format;

						prec = prec * 10 + *format++ - '0';
					}
				}
			}

			if (*format == 'l' && format[1] == 'l')
			{
				size = 'L';

				if (tptr < (tformat + sizeof(tformat) - 2))
				{
					*tptr++ = 'l';
					*tptr++ = 'l';
				}

				format += 2;
			}
			else if (*format == 'h' || *format == 'l' || *format == 'L')
			{
				if (tptr < (tformat + sizeof(tformat) - 1))
					*tptr++ = *format;

				size = *format++;
			}
			else
				size = 0;


			if (!*format)
			{
				start = format;
				break;
			}

			if (tptr < (tformat + sizeof(tformat) - 1))
				*tptr++ = *format;

			type  = *format++;
			*tptr = '\0';
			start = format;

			switch (type)
			{
			case 'E' : 
			case 'G' :
			case 'e' :
			case 'f' :
			case 'g' :
				if ((width + 2) > sizeof(temp))
					break;

				sprintf(temp, tformat, va_arg(ap, double));

				httpWrite2(&(client->http), temp, strlen(temp));
				break;

			case 'B' : 
			case 'X' :
			case 'b' :
			case 'd' :
			case 'i' :
			case 'o' :
			case 'u' :
			case 'x' :
				if ((width + 2) > sizeof(temp))
					break;

#  ifdef HAVE_LONG_LONG
				if (size == 'L')
					sprintf(temp, tformat, va_arg(ap, long long));
				else
#  endif 
					if (size == 'l')
						sprintf(temp, tformat, va_arg(ap, long));
					else
						sprintf(temp, tformat, va_arg(ap, int));

				httpWrite2(&(client->http), temp, strlen(temp));
				break;

			case 'p' : 
				if ((width + 2) > sizeof(temp))
					break;

				sprintf(temp, tformat, va_arg(ap, void *));

				httpWrite2(&(client->http), temp, strlen(temp));
				break;

			case 'c' : 
				if (width <= 1)
				{
					temp[0] = va_arg(ap, int);
					temp[1] = '\0';
					html_escape(client, temp, 1);
				}
				else
					html_escape(client, va_arg(ap, char *), (size_t)width);
				break;

			case 's' : 
				if ((s = va_arg(ap, char *)) == NULL)
					s = "(null)";

				html_escape(client, s, strlen(s));
				break;
			}
		}
		else
			format ++;
	}

	if (format > start)
		httpWrite2(&(client->http), start, format - start);

	va_end(ap);
}

void
ipp_cancel_job(_ipp_client_t *client)	
{
	_ipp_job_t		*job;		

	if ((job = find_job(client)) == NULL)
		return;

	switch (job->state)
	{
	case IPP_JOB_CANCELED :
		respond_ipp(client, IPP_NOT_POSSIBLE,
			"Job #%d is already canceled - can\'t cancel.", job->id);
		break;

	case IPP_JOB_ABORTED :
		respond_ipp(client, IPP_NOT_POSSIBLE,
			"Job #%d is already aborted - can\'t cancel.", job->id);
		break;

	case IPP_JOB_COMPLETED :
		respond_ipp(client, IPP_NOT_POSSIBLE,
			"Job #%d is already completed - can\'t cancel.", job->id);
		break;

	default :
		RWLockWrite(&(client->printer->rwlock));

		if (job->state == IPP_JOB_PROCESSING ||
			(job->state == IPP_JOB_HELD && job->fd >= 0))
			job->cancel = 1;
		else
		{
			job->state     = IPP_JOB_CANCELED;
			job->completed = time(NULL);
		}

		RWUnlock(&(client->printer->rwlock));

		respond_ipp(client, IPP_OK, NULL);
		break;
	}
}

void
ipp_get_job_attributes(
					   _ipp_client_t *client)		
{
	_ipp_job_t	*job;			
	array_t	*ra;			


	if ((job = find_job(client)) == NULL)
	{
		respond_ipp(client, IPP_NOT_FOUND, "Job not found.");
		return;
	}

	respond_ipp(client, IPP_OK, NULL);

	ra = create_requested_array(client);
	copy_job_attributes(client, job, ra);
	arrayDelete(ra);
}

void
ipp_get_jobs(_ipp_client_t *client)	
{
	ipp_attribute_t	*attr;		
	int			job_comparison;	
	ipp_jstate_t		job_state;	
	int			first_job_id,	
		limit,		
		count;		
	const char		*username;	
	_ipp_job_t		*job;		
	array_t		*ra;		

	if ((attr = ippFindAttribute(client->request, "which-jobs",
		IPP_TAG_KEYWORD)) != NULL)
		fprintf(stderr, "%s Get-Jobs which-jobs=%s", client->http.hostname,
		attr->values[0].string.text);

	if (!attr || !strcmp(attr->values[0].string.text, "not-completed"))
	{
		job_comparison = -1;
		job_state      = IPP_JOB_STOPPED;
	}
	else if (!strcmp(attr->values[0].string.text, "completed"))
	{
		job_comparison = 1;
		job_state      = IPP_JOB_CANCELED;
	}
	else if (!strcmp(attr->values[0].string.text, "aborted"))
	{
		job_comparison = 0;
		job_state      = IPP_JOB_ABORTED;
	}
	else if (!strcmp(attr->values[0].string.text, "all"))
	{
		job_comparison = 1;
		job_state      = IPP_JOB_PENDING;
	}
	else if (!strcmp(attr->values[0].string.text, "canceled"))
	{
		job_comparison = 0;
		job_state      = IPP_JOB_CANCELED;
	}
	else if (!strcmp(attr->values[0].string.text, "pending"))
	{
		job_comparison = 0;
		job_state      = IPP_JOB_PENDING;
	}
	else if (!strcmp(attr->values[0].string.text, "pending-held"))
	{
		job_comparison = 0;
		job_state      = IPP_JOB_HELD;
	}
	else if (!strcmp(attr->values[0].string.text, "processing"))
	{
		job_comparison = 0;
		job_state      = IPP_JOB_PROCESSING;
	}
	else if (!strcmp(attr->values[0].string.text, "processing-stopped"))
	{
		job_comparison = 0;
		job_state      = IPP_JOB_STOPPED;
	}
	else
	{
		respond_ipp(client, IPP_ATTRIBUTES,
			"The which-jobs value \"%s\" is not supported.",
			attr->values[0].string.text);
		ippAddString(client->response, IPP_TAG_UNSUPPORTED_GROUP, IPP_TAG_KEYWORD,
			"which-jobs", NULL, attr->values[0].string.text);
		return;
	}

	if ((attr = ippFindAttribute(client->request, "limit",
		IPP_TAG_INTEGER)) != NULL)
	{
		limit = attr->values[0].integer;

		fprintf(stderr, "%s Get-Jobs limit=%d", client->http.hostname, limit);
	}
	else
		limit = 0;

	if ((attr = ippFindAttribute(client->request, "first-job-id",
		IPP_TAG_INTEGER)) != NULL)
	{
		first_job_id = attr->values[0].integer;

		fprintf(stderr, "%s Get-Jobs first-job-id=%d", client->http.hostname,
			first_job_id);
	}
	else
		first_job_id = 1;

	username = NULL;

	if ((attr = ippFindAttribute(client->request, "my-jobs",
		IPP_TAG_BOOLEAN)) != NULL)
	{
		fprintf(stderr, "%s Get-Jobs my-jobs=%s\n", client->http.hostname,
			attr->values[0].boolean ? "true" : "false");

		if (attr->values[0].boolean)
		{
			if ((attr = ippFindAttribute(client->request, "requesting-user-name",
				IPP_TAG_NAME)) == NULL)
			{
				respond_ipp(client, IPP_BAD_REQUEST,
					"Need requesting-user-name with my-jobs.");
				return;
			}

			username = attr->values[0].string.text;

			fprintf(stderr, "%s Get-Jobs requesting-user-name=\"%s\"\n",
				client->http.hostname, username);
		}
	}

	if ((ra = create_requested_array(client)) == NULL &&
		!ippFindAttribute(client->request, "requested-attributes",
		IPP_TAG_KEYWORD))
	{
		ra = arrayNew((array_func_t)strcmp, NULL);
		arrayAdd(ra, "job-id");
		arrayAdd(ra, "job-uri");
	}

	respond_ipp(client, IPP_OK, NULL);

	RWLockRead(&(client->printer->rwlock));

	for (count = 0, job = (_ipp_job_t *)arrayFirst(client->printer->jobs);
		(limit <= 0 || count < limit) && job;
		job = (_ipp_job_t *)arrayNext(client->printer->jobs))
	{
		if ((job_comparison < 0 && job->state > job_state) ||
			(job_comparison == 0 && job->state != job_state) ||
			(job_comparison > 0 && job->state < job_state) ||
			job->id < first_job_id ||
			(username && job->username && strcaseinsensitivecmp(username, job->username)))
			continue;

		if (count > 0)
			ippAddSeparator(client->response);

		count ++;
		copy_job_attributes(client, job, ra);
	}

	arrayDelete(ra);

	RWUnlock(&(client->printer->rwlock));
}

void
ipp_get_printer_attributes(
						   _ipp_client_t *client)		
{
	array_t		*ra;		
	_ipp_printer_t	*printer;	

	ra      = create_requested_array(client);
	printer = client->printer;

	respond_ipp(client, IPP_OK, NULL);

	RWLockRead(&(printer->rwlock));

	copy_attributes(client->response, printer->attrs, ra, IPP_TAG_ZERO,
		IPP_TAG_COPY);

	if (!ra || arrayFind(ra, "printer-state"))
		ippAddInteger(client->response, IPP_TAG_PRINTER, IPP_TAG_ENUM,
		"printer-state", printer->state);

	if (!ra || arrayFind(ra, "printer-state-reasons"))
	{
		if (printer->state_reasons == _IPP_PRINTER_NONE)
			ippAddString(client->response, IPP_TAG_PRINTER,
			(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY), "printer-state-reasons",
			NULL, "none");
		else
		{
			int			num_reasons = 0;
			const char		*reasons[32];	

			if (printer->state_reasons & _IPP_PRINTER_OTHER)
				reasons[num_reasons ++] = "other";
			if (printer->state_reasons & _IPP_PRINTER_COVER_OPEN)
				reasons[num_reasons ++] = "cover-open";
			if (printer->state_reasons & _IPP_PRINTER_INPUT_TRAY_MISSING)
				reasons[num_reasons ++] = "input-tray-missing";
			if (printer->state_reasons & _IPP_PRINTER_MARKER_SUPPLY_EMPTY)
				reasons[num_reasons ++] = "marker-supply-empty-warning";
			if (printer->state_reasons & _IPP_PRINTER_MARKER_SUPPLY_LOW)
				reasons[num_reasons ++] = "marker-supply-low-report";
			if (printer->state_reasons & _IPP_PRINTER_MARKER_WASTE_ALMOST_FULL)
				reasons[num_reasons ++] = "marker-waste-almost-full-report";
			if (printer->state_reasons & _IPP_PRINTER_MARKER_WASTE_FULL)
				reasons[num_reasons ++] = "marker-waste-full-warning";
			if (printer->state_reasons & _IPP_PRINTER_MEDIA_EMPTY)
				reasons[num_reasons ++] = "media-empty-warning";
			if (printer->state_reasons & _IPP_PRINTER_MEDIA_JAM)
				reasons[num_reasons ++] = "media-jam-warning";
			if (printer->state_reasons & _IPP_PRINTER_MEDIA_LOW)
				reasons[num_reasons ++] = "media-low-report";
			if (printer->state_reasons & _IPP_PRINTER_MEDIA_NEEDED)
				reasons[num_reasons ++] = "media-needed-report";
			if (printer->state_reasons & _IPP_PRINTER_MOVING_TO_PAUSED)
				reasons[num_reasons ++] = "moving-to-paused";
			if (printer->state_reasons & _IPP_PRINTER_PAUSED)
				reasons[num_reasons ++] = "paused";
			if (printer->state_reasons & _IPP_PRINTER_SPOOL_AREA_FULL)
				reasons[num_reasons ++] = "spool-area-full";
			if (printer->state_reasons & _IPP_PRINTER_TONER_EMPTY)
				reasons[num_reasons ++] = "toner-empty-warning";
			if (printer->state_reasons & _IPP_PRINTER_TONER_LOW)
				reasons[num_reasons ++] = "toner-low-report";

			ippAddStrings(client->response, IPP_TAG_PRINTER,
				(ipp_tag_t)(IPP_TAG_KEYWORD | IPP_TAG_COPY),  "printer-state-reasons",
				num_reasons, NULL, reasons);
		}
	}

	if (!ra || arrayFind(ra, "printer-up-time"))
		ippAddInteger(client->response, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"printer-up-time", (int)time(NULL));

	if (!ra || arrayFind(ra, "queued-job-count"))
		ippAddInteger(client->response, IPP_TAG_PRINTER, IPP_TAG_INTEGER,
		"queued-job-count",
		printer->active_job &&
		printer->active_job->state < IPP_JOB_CANCELED);

	RWUnlock(&(printer->rwlock));

	arrayDelete(ra);
}

void
ipp_print_job(_ipp_client_t *client)	
{
	_ipp_job_t		*job;		
	char filename[1024],	
		 buffer[4096];	
	ssize_t		bytes;		
	array_t		*ra;		
	int i;

	if (!valid_job_attributes(client))
	{
		httpFlush(&(client->http));
		return;
	}

	if (client->http.state == HTTP_POST_SEND)
	{
		respond_ipp(client, IPP_BAD_REQUEST, "No file in request.");
		return;
	}

	if ((job = create_job(client)) == NULL)
	{
		respond_ipp(client, IPP_PRINTER_BUSY, "Currently printing another job.");
		return;
	}

	if (!strcaseinsensitivecmp(job->format, "image/jpeg"))
		snprintf(filename, sizeof(filename), "%s/%d.jpg",
		client->printer->directory, job->id);
	else if (!strcaseinsensitivecmp(job->format, "image/png"))
		snprintf(filename, sizeof(filename), "%s/%d.png",
		client->printer->directory, job->id);
	else if (!strcaseinsensitivecmp(job->format, "application/pdf"))
		snprintf(filename, sizeof(filename), "%s/%d.pdf",
		client->printer->directory, job->id);
	else if (!strcaseinsensitivecmp(job->format, "application/postscript"))
		snprintf(filename, sizeof(filename), "%s/%d.ps",
		client->printer->directory, job->id);
	else
		snprintf(filename, sizeof(filename), "%s/%d.prn",
		client->printer->directory, job->id);

	if ((job->fd = open(filename, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC)) < 0)
	{
		job->state = IPP_JOB_ABORTED;

		respond_ipp(client, IPP_INTERNAL_ERROR,
			"Unable to create print file: %s", strerror(errno));
		return;
	}

	while ((bytes = httpRead2(&(client->http), buffer, sizeof(buffer))) > 0)
	{
		if (write(job->fd, buffer, bytes) < bytes)
		{
			int error = errno;		

			job->state = IPP_JOB_ABORTED;

			fclose((FILE *)(job->fd));
			job->fd = -1;

			unlink(filename);

			respond_ipp(client, IPP_INTERNAL_ERROR,
				"Unable to write print file: %s", strerror(error));
			return;
		}
	}

	if (bytes < 0)
	{
		job->state = IPP_JOB_ABORTED;

		fclose((FILE *)(job->fd));
		job->fd = -1;

		unlink(filename);

		respond_ipp(client, IPP_INTERNAL_ERROR, "Unable to read print file.");
		return;
	}

	if (close(job->fd))
	{
		int error = errno;		

		job->state = IPP_JOB_ABORTED;
		job->fd    = -1;

		unlink(filename);

		respond_ipp(client, IPP_INTERNAL_ERROR, "Unable to write print file: %s",
			strerror(error));
		return;
	}

	job->fd       = -1;
	job->filename = strdup(filename);
	job->state    = IPP_JOB_PENDING;

	if (!threadCreate((thread_func_t)process_job, job))
	{
		job->state = IPP_JOB_ABORTED;
		respond_ipp(client, IPP_INTERNAL_ERROR, "Unable to process job.");
		return;
	}

	respond_ipp(client, IPP_OK, NULL);

	ra = arrayNew((array_func_t)strcmp, NULL);
	arrayAdd(ra, "job-id");
	arrayAdd(ra, "job-state");
	arrayAdd(ra, "job-state-reasons");
	arrayAdd(ra, "job-uri");

	copy_job_attributes(client, job, ra);
	arrayDelete(ra);
}

void
ipp_validate_job(_ipp_client_t *client)	
{
	respond_ipp(client, IPP_OK, NULL);
}

void *				
process_client(_ipp_client_t *client)	
{
	while (httpWait(&(client->http), 30000))
		if (!process_http(client))
			break;

	delete_client(client);

	return (NULL);
}

int					
process_http(_ipp_client_t *client)	
{
	char			line[4096],	
		operation[64],	
		uri[1024],	
		version[64],	
		*ptr;		
	int			major, minor;	
	http_status_t		status;		
	ipp_state_t		state;		

	if (client->http.error)
		return (0);

	httpClearFields(&(client->http));
	ippDelete(client->request);
	ippDelete(client->response);

	client->http.activity       = time(NULL);
	client->http.version        = HTTP_1_1;
	client->http.keep_alive     = HTTP_KEEPALIVE_OFF;
	client->http.data_encoding  = HTTP_ENCODE_LENGTH;
	client->http.data_remaining = 0;
	client->request             = NULL;
	client->response            = NULL;
	client->operation           = HTTP_WAITING;

	while ((ptr = httpGets(line, sizeof(line) - 1, &(client->http))) != NULL)
		if (*ptr)
			break;

	if (!ptr)
		return (0);

	fprintf(stderr, "%s %s\n", client->http.hostname, line);

	switch (sscanf(line, "%63s%1023s%63s", operation, uri, version))
	{
	case 1 :
		fprintf(stderr, "%s Bad request line.\n", client->http.hostname);
		respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
		return (0);

	case 2 :
		client->http.version = HTTP_0_9;
		break;

	case 3 :
		if (sscanf(version, "HTTP/%d.%d", &major, &minor) != 2)
		{
			fprintf(stderr, "%s Bad HTTP version.\n", client->http.hostname);
			respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
			return (0);
		}

		if (major < 2)
		{
			client->http.version = (http_version_t)(major * 100 + minor);
			if (client->http.version == HTTP_1_1)
				client->http.keep_alive = HTTP_KEEPALIVE_ON;
			else
				client->http.keep_alive = HTTP_KEEPALIVE_OFF;
		}
		else
		{
			respond_http(client, HTTP_NOT_SUPPORTED, NULL, 0);
			return (0);
		}
		break;
	}

	if (!strncmp(client->uri, "http:", 5) || !strncmp(client->uri, "ipp:", 4))
	{
		char	scheme[32],		
			userpass[128],		
			hostname[HTTP_MAX_HOST];
		int		port;			

		if (httpSeparateURI(HTTP_URI_CODING_MOST, uri, scheme, sizeof(scheme),
			userpass, sizeof(userpass),
			hostname, sizeof(hostname), &port,
			client->uri, sizeof(client->uri)) < HTTP_URI_OK)
		{
			fprintf(stderr, "%s Bad URI \"%s\".\n", client->http.hostname, uri);
			respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
			return (0);
		}
	}
	else
	{
		if (!_httpDecodeURI(client->uri, uri, sizeof(client->uri)))
		{
			fprintf(stderr, "%s Bad URI \"%s\".\n", client->http.hostname, uri);
			respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
			return (0);
		}
	}

	if (!strcmp(operation, "GET"))
		client->http.state = HTTP_GET;
	else if (!strcmp(operation, "POST"))
		client->http.state = HTTP_POST;
	else if (!strcmp(operation, "OPTIONS"))
		client->http.state = HTTP_OPTIONS;
	else if (!strcmp(operation, "HEAD"))
		client->http.state = HTTP_HEAD;
	else
	{
		fprintf(stderr, "%s Bad operation \"%s\".\n", client->http.hostname,
			operation);
		respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
		return (0);
	}

	client->start       = time(NULL);
	client->operation   = client->http.state;
	client->http.status = HTTP_OK;

	while ((status = httpUpdate(&(client->http))) == HTTP_CONTINUE);

	if (status != HTTP_OK)
	{
		respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
		return (0);
	}

	if (!client->http.fields[HTTP_FIELD_HOST][0] &&
		client->http.version >= HTTP_1_1)
	{
		respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
		return (0);
	}

	if (!strcaseinsensitivecmp(client->http.fields[HTTP_FIELD_CONNECTION], "Upgrade"))
	{
	}

	if (client->http.expect &&
		(client->operation == HTTP_POST || client->operation == HTTP_PUT))
	{
		if (client->http.expect == HTTP_CONTINUE)
		{
			if (!respond_http(client, HTTP_CONTINUE, NULL, 0))
				return (0);
		}
		else
		{
			if (!respond_http(client, HTTP_EXPECTATION_FAILED, NULL, 0))
				return (0);

			httpPrintf(&(client->http), "Content-Length: 0\r\n");
			httpPrintf(&(client->http), "\r\n");
			httpFlushWrite(&(client->http));
			client->http.data_encoding = HTTP_ENCODE_LENGTH;
		}
	}

	switch (client->operation)
	{
	case HTTP_OPTIONS :
		return (respond_http(client, HTTP_OK, NULL, 0));
		break;

	case HTTP_HEAD :
		return (respond_http(client, HTTP_NOT_FOUND, NULL, 0));
		break;

	case HTTP_GET :
		return (respond_http(client, HTTP_NOT_FOUND, NULL, 0));
		break;

	case HTTP_POST :
		if (client->http.data_remaining < 0 ||
			(!client->http.fields[HTTP_FIELD_CONTENT_LENGTH][0] &&
			client->http.data_encoding == HTTP_ENCODE_LENGTH))
		{
			return (respond_http(client, HTTP_BAD_REQUEST, NULL, 0));
		}

		if (strcmp(client->http.fields[HTTP_FIELD_CONTENT_TYPE],
			"application/ipp"))
		{
			
			return (respond_http(client, HTTP_BAD_REQUEST, NULL, 0));
		}

		client->request = ippNew();

		while ((state = ippRead(&(client->http), client->request)) != IPP_DATA)
			if (state == IPP_ERROR)
			{
				fprintf(stderr, "%s IPP read error (%s).\n", client->http.hostname,
					ippOpString(client->request->request.op.operation_id));
				respond_http(client, HTTP_BAD_REQUEST, NULL, 0);
				return (0);
			}
			return (process_ipp(client));

	default :
		break; 
	}

	return (1);
}

int				
process_ipp(_ipp_client_t *client)	
{
	ipp_tag_t		group;		
	ipp_attribute_t	*attr;		
	ipp_attribute_t	*charset;	
	ipp_attribute_t	*language;	
	ipp_attribute_t	*uri;		


	debug_attributes("Request", client->request);

	client->operation_id = client->request->request.op.operation_id;
	client->response     = ippNew();

	client->response->request.status.version[0] =
		client->request->request.op.version[0];
	client->response->request.status.version[1] =
		client->request->request.op.version[1];
	client->response->request.status.request_id =
		client->request->request.op.request_id;

	if (client->request->request.any.version[0] < 1 ||
		client->request->request.any.version[0] > 2)
	{
		respond_ipp(client, IPP_VERSION_NOT_SUPPORTED,
			"Bad request version number %d.%d.",
			client->request->request.any.version[0],
			client->request->request.any.version[1]);
	}
	else if (client->request->request.any.request_id <= 0)
		respond_ipp(client, IPP_BAD_REQUEST, "Bad request-id %d.",
		client->request->request.any.request_id);
	else if (!client->request->attrs)
		respond_ipp(client, IPP_BAD_REQUEST, "No attributes in request.");
	else
	{
		for (attr = client->request->attrs, group = attr->group_tag;
			attr;
			attr = attr->next)
			if (attr->group_tag < group && attr->group_tag != IPP_TAG_ZERO)
			{
				respond_ipp(client, IPP_BAD_REQUEST,
					"Attribute groups are out of order (%x < %x).",
					attr->group_tag, group);
				break;
			}
			else
				group = attr->group_tag;

		if (!attr)
		{
			attr = client->request->attrs;
			if (attr && attr->name &&
				!strcmp(attr->name, "attributes-charset") &&
				(attr->value_tag & IPP_TAG_MASK) == IPP_TAG_CHARSET)
				charset = attr;
			else
				charset = NULL;

			if (attr)
				attr = attr->next;

			if (attr && attr->name &&
				!strcmp(attr->name, "attributes-natural-language") &&
				(attr->value_tag & IPP_TAG_MASK) == IPP_TAG_LANGUAGE)
				language = attr;
			else
				language = NULL;

			if ((attr = ippFindAttribute(client->request, "printer-uri",
				IPP_TAG_URI)) != NULL)
				uri = attr;
			else if ((attr = ippFindAttribute(client->request, "job-uri",
				IPP_TAG_URI)) != NULL)
				uri = attr;
			else
				uri = NULL;

			ippAddString(client->response, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
				"attributes-charset", NULL,
				charset ? charset->values[0].string.text : "utf-8");

			ippAddString(client->response, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
				"attributes-natural-language", NULL,
				language ? language->values[0].string.text : "en");

			if (charset &&
				strcaseinsensitivecmp(charset->values[0].string.text, "us-ascii") &&
				strcaseinsensitivecmp(charset->values[0].string.text, "utf-8"))
			{

				respond_ipp(client, IPP_BAD_REQUEST,
					"Unsupported character set \"%s\".",
					charset->values[0].string.text);
			}
			else if (!charset || !language || !uri)
			{
				respond_ipp(client, IPP_BAD_REQUEST, "Missing required attributes.");
			}
			else
			{
				if (client->http.expect == HTTP_CONTINUE)
				{
					if (!respond_http(client, HTTP_CONTINUE, NULL, 0))
						return (0);
				}

				switch (client->request->request.op.operation_id)
				{
				case IPP_PRINT_JOB :
					ipp_print_job(client);
					break;

				case IPP_VALIDATE_JOB :
					ipp_validate_job(client);
					break;

				case IPP_CANCEL_JOB :
					ipp_cancel_job(client);
					break;

				case IPP_GET_JOB_ATTRIBUTES :
					ipp_get_job_attributes(client);
					break;

				case IPP_GET_JOBS :
					ipp_get_jobs(client);
					break;

				case IPP_GET_PRINTER_ATTRIBUTES :
					ipp_get_printer_attributes(client);
					break;

				default :
					respond_ipp(client, IPP_OPERATION_NOT_SUPPORTED,
						"Operation not supported.");
					break;
				}
			}
		}
	}

	if (client->http.state != HTTP_POST_SEND)
		httpFlush(&(client->http));		

	return (respond_http(client, HTTP_OK, "application/ipp",
		ippLength(client->response)));
}

void *				
process_job(_ipp_job_t *job)		
{
	char filename[1024] = {0};
	char cmd[1024] = {0};

	void *minst;
	int code, code1;
	const char * gsargv[10];
	int gsargc;
	ipp_attribute_t	*uri,*attr;
	char rp[256];
	char *ptr;
	int i;
	int j=0;
	int printpagecnt = 1;

	job->state          = IPP_JOB_PROCESSING;
	job->printer->state = IPP_PRINTER_PROCESSING;

	if ((attr = ippFindAttribute(job->attrs, "printer-uri",
		IPP_TAG_URI)) != NULL)
		uri = attr;
	else if ((attr = ippFindAttribute(job->attrs, "job-uri",
		IPP_TAG_URI)) != NULL)
		uri = attr;
	else
		uri = NULL;

	if ((attr = ippFindAttribute(job->attrs, "copies",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_INTEGER ||
			attr->values[0].integer < 1 || attr->values[0].integer > 999)
		{
			printpagecnt = 1;
		}
		else
		{
			printpagecnt = attr->values[0].integer;
		}		
	}

	fprintf(stderr,"uri->values[0].string.text: %s\n",uri->values[0].string.text);
	ptr = strrchr(uri->values[0].string.text,'/');
	ptr++;
	snprintf(rp,sizeof(rp), "%s", ptr);

	for(i=0,ptr = rp; i < strlen(rp); i++,ptr++)
	{
		if(*ptr == '_')
		{
			*ptr = ' ';
		}
	}

	if (!strcaseinsensitivecmp(job->format, "image/jpeg"))
	{
		snprintf(filename, sizeof(filename), "%s/%d.jpg",
			job->printer->directory, job->id);
		RWLockWrite(&printingthreadlock);
			if(getFileName)
				getFileName(filename,rp,1);
		RWUnlock(&printingthreadlock);
	}
	else if (!strcaseinsensitivecmp(job->format, "image/png"))
	{
		snprintf(filename, sizeof(filename), "%s/%d.png",
			job->printer->directory, job->id);
		RWLockWrite(&printingthreadlock);
			if(getFileName)
				getFileName(filename,rp,2);
		RWUnlock(&printingthreadlock);
	}
	else if (!strcaseinsensitivecmp(job->format, "application/pdf"))
	{
#ifndef USING_POPPLER

		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si,0,sizeof(si));
		si.cb=sizeof(si);
		si.wShowWindow=SW_HIDE;
		si.dwFlags=STARTF_USESHOWWINDOW;

		snprintf(filename, sizeof(filename), "%s\\%d.pdf",
			job->printer->directory, job->id);
		fprintf(stderr, "%s/%d.pdf %s",
			job->printer->directory, job->id,filename);

		//snprintf(cmd, sizeof(cmd), "cmd /c start /B acrord32 /h /t \"%s\" \"%s\"", filename,rp);

		for(j;j<printpagecnt; j++)
		{
			if(adobeType == 1)
			{
				snprintf(cmd, sizeof(cmd), "cmd /c start /B acrord32 /o /t \"%s\" \"%s\"", filename,rp);
			}else
			{
				snprintf(cmd, sizeof(cmd), "cmd /c start /B Acrobat /o /t \"%s\" \"%s\"", filename,rp);
			}

			fprintf(stderr,"cmd: %s\n", cmd);
			CreateProcess(NULL,cmd,NULL,FALSE,NULL,NULL,NULL,NULL,&si,&pi);
		}

#else
		snprintf(filename, sizeof(filename), "%s\\%d.pdf",
			job->printer->directory, job->id);
		fprintf(stderr, "%s/%d.pdf %s",
			job->printer->directory, job->id,filename);

		Poppler::Document * doc = NULL;
		doc = Poppler::Document::load(filename);
		if (doc != NULL) {
			int startpage=1;
			int	endpage = doc->numPages();
			doc->setRenderHint(Poppler::Document::Antialiasing);
			doc->setRenderHint(Poppler::Document::TextAntialiasing);
			QPrinter printer;
			printer.setPrinterName(rp);
			QPainter painter(&printer);
			for(int i=startpage; i<=endpage && (i==startpage || printer.newPage()); ++i)
			{
				QImage image = doc->page(i-1)->renderToImage( 200,200);
				painter.drawImage( printer.pageRect() ,image);
			}
			if(doc != NULL)
			{
				delete doc;
			}
		}
#endif //USING_POPPLER
	}
	else if (!strcaseinsensitivecmp(job->format, "application/postscript"))
	{
		snprintf(filename, sizeof(filename), "%s/%d.ps",
			job->printer->directory, job->id);
	}
	else
		snprintf(filename, sizeof(filename), "%s/%d.prn",
		job->printer->directory, job->id);

END:
	if (job->cancel)
		job->state = IPP_JOB_CANCELED;
	else
		job->state = IPP_JOB_COMPLETED;

	job->completed           = time(NULL);
	job->printer->state      = IPP_PRINTER_IDLE;
	job->printer->active_job = NULL;

	return (NULL);
}

int					
respond_http(_ipp_client_t *client,	
			 http_status_t code,	
			 const char    *type,	
			 size_t        length)	
{
	char	message[1024];			
	char http_date[40] = {0};


	fprintf(stderr, "%s %s\n", client->http.hostname, httpStatus(code));

	if(code == HTTP_SWITCHING_PROTOCOLS)
	{
		return (httpPrintf(HTTP(client), "HTTP/%d.%d 101 Switching Protocols\r\n"
			"Connection: Upgrade\r\n"
			"Upgrade: TLS/1.0,HTTP/1.1\r\n"
			"Content-Length: 0\r\n"
			"\r\n",
			client->http.version / 100,
			client->http.version % 100) > 0);
	}

	if (code == HTTP_CONTINUE)
	{
		return (httpPrintf(&(client->http), "HTTP/%d.%d 100 Continue\r\n\r\n",
			client->http.version / 100,
			client->http.version % 100) > 0);
	}

	if (!type && !length && code != HTTP_OK)
	{
		snprintf(message, sizeof(message), "%d - %s\n", code, httpStatus(code));

		type   = "text/plain";
		length = strlen(message);
	}
	else
		message[0] = '\0';

	httpFlushWrite(&(client->http));

	client->http.data_encoding = HTTP_ENCODE_FIELDS;

	if (httpPrintf(&(client->http), "HTTP/%d.%d %d %s\r\n", client->http.version / 100,
		client->http.version % 100, code, httpStatus(code)) < 0)
		return (0);

	httpGetDateString(time(NULL), http_date, sizeof(http_date));
	if (httpPrintf(&(client->http), "Date: %s\r\n", http_date) < 0)
		return (0);

	if (client->http.keep_alive && client->http.version >= HTTP_1_0)
	{
		if (httpPrintf(&(client->http),
			"Connection: Keep-Alive\r\n"
			"Keep-Alive: timeout=10\r\n") < 0)
			return (0);
	}

	if (code == HTTP_METHOD_NOT_ALLOWED || client->operation == HTTP_OPTIONS)
	{
		if (httpPrintf(&(client->http), "Allow: GET, HEAD, OPTIONS, POST\r\n") < 0)
			return (0);
	}

	if (type)
	{
		if (!strcmp(type, "text/html"))
		{
			if (httpPrintf(&(client->http),
				"Content-Type: text/html; charset=utf-8\r\n") < 0)
				return (0);
		}
		else if (httpPrintf(&(client->http), "Content-Type: %s\r\n", type) < 0)
			return (0);
	}

	if (length == 0 && !message[0])
	{
		if (httpPrintf(&(client->http), "Transfer-Encoding: chunked\r\n\r\n") < 0)
			return (0);
	}
	else if (httpPrintf(&(client->http), "Content-Length: %ld\r\n\r\n",(long)length) < 0)
		return (0);

	if (httpFlushWrite(&(client->http)) < 0)
		return (0);

	if (message[0])
	{
		if (httpPrintf(&(client->http), "%s", message) < 0)
			return (0);
	}
	else if (client->response)
	{
		debug_attributes("Response", client->response);

		client->http.data_encoding  = HTTP_ENCODE_LENGTH;
		client->http.data_remaining = (off_t)ippLength(client->response);
		client->response->state     = IPP_IDLE;

		if (ippWrite(&(client->http), client->response) != IPP_DATA)
			return (0);
	}
	else
		client->http.data_encoding = HTTP_ENCODE_CHUNKED;

	return (httpFlushWrite(&(client->http)) >= 0);
}

void
respond_ipp(_ipp_client_t *client,	
			ipp_status_t  status,	
			const char    *message,	
			...)			
{
	va_list	ap;			
	char		formatted[1024];	


	client->response->request.status.status_code = status;

	if (!client->response->attrs)
	{
		ippAddString(client->response, IPP_TAG_OPERATION,
			(ipp_tag_t)(IPP_TAG_CHARSET | IPP_TAG_COPY), "attributes-charset", NULL,
			"utf-8");
		ippAddString(client->response, IPP_TAG_OPERATION,
			(ipp_tag_t)(IPP_TAG_LANGUAGE | IPP_TAG_COPY), "attributes-natural-language",
			NULL, "en-us");
	}

	if (message)
	{
		va_start(ap, message);
		vsnprintf(formatted, sizeof(formatted), message, ap);
		va_end(ap);

		ippAddString(client->response, IPP_TAG_OPERATION, IPP_TAG_TEXT,
			"status-message", NULL, formatted);
	}
	else
		formatted[0] = '\0';

	fprintf(stderr, "%s %s %s (%s)\n", client->http.hostname,
		ippOpString(client->operation_id), ippErrorString(status), formatted);
}

int				
valid_job_attributes(
					 _ipp_client_t *client)		
{
	int			i;		
	ipp_attribute_t	*attr,		
		*supported;	
	const char		*format = NULL;	
	int			valid = 1;	

#define respond_unsupported(client, attr) \
	if (valid) \
	{ \
	respond_ipp(client, IPP_ATTRIBUTES, "Unsupported %s %s%s value.", \
	attr->name, attr->num_values > 1 ? "1setOf " : "", \
	ippTagString(attr->value_tag)); \
	valid = 0; \
	} \
	copy_attribute(client->response, attr, IPP_TAG_UNSUPPORTED_GROUP, 0)

	if ((attr = ippFindAttribute(client->request, "compression",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_KEYWORD ||
			strcmp(attr->values[0].string.text, "none"))
		{
			respond_unsupported(client, attr);
		}
		else
			fprintf(stderr, "%s Print-Job compression=\"%s\"\n",
			client->http.hostname, attr->values[0].string.text);
	}

	if ((attr = ippFindAttribute(client->request, "document-format",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_MIMETYPE)
		{
			respond_unsupported(client, attr);
		}
		else
		{
			format = attr->values[0].string.text;

			fprintf(stderr, "%s %s document-format=\"%s\"\n",
				client->http.hostname,
				ippOpString(client->request->request.op.operation_id), format);
		}
	}
	else
		format = "application/octet-stream";

	if (!strcmp(format, "application/octet-stream") &&
		client->request->request.op.operation_id != IPP_VALIDATE_JOB)
	{
		unsigned char	header[4];	

		memset(header, 0, sizeof(header));
		_httpPeek(&(client->http), (char *)header, sizeof(header));

		if (!memcmp(header, "%PDF", 4))
			format = "application/pdf";
		else if (!memcmp(header, "%!", 2))
			format = "application/postscript";
		else if (!memcmp(header, "\377\330\377", 3) &&
			header[3] >= 0xe0 && header[3] <= 0xef)
			format = "image/jpeg";
		else if (!memcmp(header, "\211PNG", 4))
			format = "image/png";

		if (format)
			fprintf(stderr, "%s %s Auto-typed document-format=\"%s\"\n",
			client->http.hostname,
			ippOpString(client->request->request.op.operation_id), format);

		if (!attr)
			ippAddString(client->request, IPP_TAG_JOB, IPP_TAG_MIMETYPE,
			"document-format", NULL, format);
		else
		{
			strfree(attr->values[0].string.text);
			attr->values[0].string.text = stralloc(format);
		}
	}

	if ((supported = ippFindAttribute(client->printer->attrs,
		"document-format-supported",
		IPP_TAG_MIMETYPE)) != NULL)
	{
		for (i = 0; i < supported->num_values; i ++)
			if (!strcaseinsensitivecmp(format, supported->values[i].string.text))
				break;

		if (i >= supported->num_values)
		{
			respond_unsupported(client, supported);
		}
	}

	if ((attr = ippFindAttribute(client->request, "copies",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_INTEGER ||
			attr->values[0].integer < 1 || attr->values[0].integer > 999)
		{
			respond_unsupported(client, attr);
		}		
	}

	if ((attr = ippFindAttribute(client->request, "ipp-attribute-fidelity",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_BOOLEAN)
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "job-hold-until",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 ||
			(attr->value_tag != IPP_TAG_NAME &&
			attr->value_tag != IPP_TAG_NAMELANG &&
			attr->value_tag != IPP_TAG_KEYWORD) ||
			strcmp(attr->values[0].string.text, "no-hold"))
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "job-name",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 ||
			(attr->value_tag != IPP_TAG_NAME &&
			attr->value_tag != IPP_TAG_NAMELANG))
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "job-priority",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_INTEGER ||
			attr->values[0].integer < 1 || attr->values[0].integer > 100)
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "job-sheets",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 ||
			(attr->value_tag != IPP_TAG_NAME &&
			attr->value_tag != IPP_TAG_NAMELANG &&
			attr->value_tag != IPP_TAG_KEYWORD) ||
			strcmp(attr->values[0].string.text, "none"))
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "media",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 ||
			(attr->value_tag != IPP_TAG_NAME &&
			attr->value_tag != IPP_TAG_NAMELANG &&
			attr->value_tag != IPP_TAG_KEYWORD))
		{
			respond_unsupported(client, attr);
		}
		else
		{
			for (i = 0;
				i < (int)(sizeof(media_supported) / sizeof(media_supported[0]));
				i ++)
				if (!strcmp(attr->values[0].string.text, media_supported[i]))
					break;

			if (i >= (int)(sizeof(media_supported) / sizeof(media_supported[0])))
			{
				respond_unsupported(client, attr);
			}
		}
	}

	if ((attr = ippFindAttribute(client->request, "media-col",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_BEGIN_COLLECTION)
		{
			respond_unsupported(client, attr);
		}
		
	}

	if ((attr = ippFindAttribute(client->request, "multiple-document-handling",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_KEYWORD ||
			(strcmp(attr->values[0].string.text,
			"separate-documents-uncollated-copies") &&
			strcmp(attr->values[0].string.text,
			"separate-documents-collated-copies")))
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "orientation-requested",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_ENUM ||
			attr->values[0].integer < IPP_PORTRAIT ||
			attr->values[0].integer > IPP_REVERSE_PORTRAIT)
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "page-ranges",
		IPP_TAG_ZERO)) != NULL)
	{
		respond_unsupported(client, attr);
	}

	if ((attr = ippFindAttribute(client->request, "print-quality",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_ENUM ||
			attr->values[0].integer < IPP_QUALITY_DRAFT ||
			attr->values[0].integer > IPP_QUALITY_HIGH)
		{
			respond_unsupported(client, attr);
		}
	}

	if ((attr = ippFindAttribute(client->request, "printer-resolution",
		IPP_TAG_ZERO)) != NULL)
	{
		respond_unsupported(client, attr);
	}

	/*if ((attr = ippFindAttribute(client->request, "sides",
		IPP_TAG_ZERO)) != NULL)
	{
		if (attr->num_values != 1 || attr->value_tag != IPP_TAG_KEYWORD)
		{
			respond_unsupported(client, attr);
		}

		if ((supported = ippFindAttribute(client->printer->attrs, "sides",
			IPP_TAG_KEYWORD)) != NULL)
		{
			for (i = 0; i < supported->num_values; i ++)
				if (!strcmp(attr->values[0].string.text,
					supported->values[i].string.text))
					break;

			if (i >= supported->num_values)
			{
				respond_unsupported(client, attr);
			}
		}
		else
		{
			respond_unsupported(client, attr);
		}
	}*/
	return (valid);
}

#include "app.h"
#include "logger.h"

int main(int argc, char *argv[])
{
	DBGLOGINIT();
	int retval = LPCManager::main(argc, argv);
	DBGLOG(LOG_INFO, 1, QString::fromUtf8("Exited with value %1").arg(retval));
	DBGLOGTERM();
	return retval;
}

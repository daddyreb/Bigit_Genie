#include "stdafx.h"

bool ReadFile(const WCHAR *path, std::vector<char>& data);

const char *g_magic = "\xde\xc0\xd2\xba\xba\xd2\xc0\xde\xce\xfa\xbe\xba\xba\xbe\xfa\xce";

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3) {
		return -1;
	}

	std::vector<char> wpinst_bin;
	std::vector<char> wpinst64_bin;
	if (!ReadFile(argv[1], wpinst_bin) || !ReadFile(argv[2], wpinst64_bin)) {
		return -2;
	}

	std::string w32(&wpinst_bin[0], wpinst_bin.size());
	std::string w64(&wpinst64_bin[0], wpinst64_bin.size());

	size_t pos1 = w32.find(g_magic);
	size_t pos2 = w64.find(g_magic);
	if (pos1 == std::string::npos || pos2 == std::string::npos) {
		return -3;
	}

	*reinterpret_cast<DWORD*>(&wpinst_bin[0] + pos1) = pos2;
	*reinterpret_cast<DWORD*>(&wpinst_bin[0] + pos1 + 4) = pos1;
	std::ofstream fOut(argv[1], std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
	if (!fOut) {
		return -4;
	}

	fOut.write(&wpinst_bin[0], wpinst_bin.size());
	return 0;
}

bool ReadFile(const WCHAR *path, std::vector<char>& data)
{
	std::ifstream fIn(path, std::ios_base::in|std::ios_base::binary);
	if (!fIn) {
		return false;
	}

	fIn.seekg(0, std::ios_base::end);
	data.resize(fIn.tellg());
	fIn.seekg(0, std::ios_base::beg);
	fIn.read(&data[0], data.size());
	return true;
}

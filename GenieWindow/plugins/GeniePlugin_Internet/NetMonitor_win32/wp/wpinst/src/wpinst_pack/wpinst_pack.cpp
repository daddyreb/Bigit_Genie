#include "stdafx.h"

int pack(const std::wstring& outputFileName, const std::list<std::wstring>& inputFileList);

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 4) {
		return -1;
	}

	if (_tcsicmp(argv[1], _T("a")) != 0) {
		return -2;
	}

	std::list<std::wstring> ls;
	for (int i = 3; i < argc; i++) {
		ls.push_back(argv[i]);
	}

	return pack(argv[2], ls);
}

bool pack(const std::wstring& fileName, std::list<std::vector<char>>& ls, std::list<unsigned long>& lls)
{
	std::ifstream fIn(fileName.c_str(), std::ios_base::in|std::ios_base::binary);
	if (!fIn) {
		return false;
	}

	fIn.seekg(0, std::ios_base::end);
	size_t length = fIn.tellg();
	fIn.seekg(0, std::ios_base::beg);
	std::vector<char> data(length);
	fIn.read(&data[0], data.size());

	z_stream strm;
	memset(&strm, 0, sizeof(strm));
	if (Z_OK != deflateInit(&strm, 9)) {
		return false;
	}

	std::vector<char> zdata(data.size() * 11 / 10);
	strm.next_in = reinterpret_cast<unsigned char*>(&data[0]);
	strm.avail_in = data.size();
	strm.next_out = reinterpret_cast<unsigned char*>(&zdata[0]);
	strm.avail_out = zdata.size();
	int retval = deflate(&strm, Z_FINISH);
	deflateEnd(&strm);

	if (retval != Z_STREAM_END) {
		return false;
	}

	ls.push_back(std::vector<char>(zdata.begin(), zdata.begin() + strm.total_out));
	lls.push_back(data.size());
	return true;
}

int pack(const std::wstring& outputFileName, const std::list<std::wstring>& inputFileList)
{
	std::list<std::vector<char>> ls;
	std::list<unsigned long> lls;
	std::list<std::wstring>::const_iterator it = inputFileList.begin();
	std::list<std::wstring>::const_iterator ie = inputFileList.end();
	for (; it != ie; ++it) {
		if (!pack(*it, ls, lls)) {
			return -5;
		}
	}

	std::vector<DWORD> header;
	header.push_back(0xBAD2C0DE);
	header.push_back(0xDEC0D2BA);
	header.push_back(0xBABEFACE);
	header.push_back(0xCEFABEBA);
	std::vector<char> outputData(sizeof(DWORD) * (ls.size() * 3 + 4));
	DWORD offset = outputData.size();
	std::list<std::vector<char>>::const_iterator itL = ls.begin();
	std::list<std::vector<char>>::const_iterator ieL = ls.end();
	std::list<unsigned long>::const_iterator itLL = lls.begin();
	for (; itL != ieL; ++itL, ++itLL) {
		const std::vector<char>& zdata = *itL;
		outputData.insert(outputData.end(), zdata.begin(), zdata.end());
		header.push_back(offset);
		header.push_back(zdata.size());
		header.push_back(*itLL);
		offset += zdata.size();
	}

	memcpy(&outputData[0], &header[0], header.size() * sizeof(DWORD));

	FILE *fOut = _wfopen(outputFileName.c_str(), L"w");
	if (!fOut) {
		return -6;
	}

	fprintf(fOut, "static const unsigned char s_data_bin[] = {\n");

#define LINE_DIV 32

	const unsigned char *pData = reinterpret_cast<unsigned char*>(&outputData[0]);
	int DataLen = outputData.size();

	for (int i = 0; i < DataLen; i += LINE_DIV) {
		const unsigned char *ll = pData + i;
		if (i + LINE_DIV < DataLen) {
			fprintf(fOut, "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n",
				ll[0], ll[1], ll[2], ll[3], ll[4], ll[5], ll[6], ll[7],
				ll[8], ll[9], ll[10], ll[11], ll[12], ll[13], ll[14], ll[15],
				ll[16], ll[17], ll[18], ll[19], ll[20], ll[21], ll[22], ll[23],
				ll[24], ll[25], ll[26], ll[27], ll[28], ll[29], ll[30], ll[31]);
		} else {
			for (int j = i; j < DataLen; j++) {
				fprintf(fOut, "0x%02x,", ll[j - i]);
			}
			fprintf(fOut, "\n");
		}
	}

	fprintf(fOut, "};\n\nconst unsigned char *get_data_bin(unsigned long *len)\n{\n\tif (len) *len = sizeof(s_data_bin);\n\treturn s_data_bin;\n}\n");

	fclose(fOut);

	return 0;
}

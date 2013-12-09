#pragma once

#include "resource.h"

bool ExtractData(int index, std::vector<unsigned char>& data);
bool ExtractFile(const WCHAR *path, int index);
int Process(bool fUninstall);
int DoInstall();
int DoUninstall();

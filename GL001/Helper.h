#pragma once
#include<direct.h>
#include<io.h>
#include<iostream>
#include<string>
#include<regex>
#include<fstream>

using namespace std;
namespace Helper {

	inline bool StartWith(const string& str, const char* v) {
		return str.find(v) == 0;
	}

	inline bool EndWith(const string& str, const char* v) {
		return str.find(v) == str.length() - strlen(v);
	}

	inline bool Contains(const string& str, const char* v) {
		return str.find(v) != string::npos;
	}

	static std::string Relace(const std::string& str, const char* replace, const char* to) {
		std::string newStr = str;
		string::size_type  findidx = 0;
		while ((findidx = newStr.find(replace)) != string::npos) {
			newStr = newStr.replace(findidx, strlen(replace), to);
		}
		return newStr;
	}

	static bool IsAbsoluteUri(const std::string& strPath) {
		return strPath.find("://") != std::string::npos || strPath.find(":\\\\") != std::string::npos;
	}

	static std::string RepairUri(const string& uri, const string& exeDir) {
		std::string strRet = uri;

		if (!IsAbsoluteUri(uri)) {
			strRet = exeDir + uri;
		}

		strRet = Relace(strRet, "\\", "/");
		strRet = Relace(strRet, "//", "/");

		std::regex r(".*?/([^/]+?/\\.\\./).+?");
		std::smatch matches;
		do
		{
			std::regex_match(strRet, matches, r);
			if (matches.size() >= 2) {
				string rm = matches[1];
				strRet = Relace(strRet, rm.c_str(), "");
			}
		} while (matches.size() >= 2);

		return strRet;
	}

	static void SearchDir(std::string path, std::vector<std::string>& files)
	{
		intptr_t   hFile = 0;
		struct _finddata_t fileinfo;
		std::string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						SearchDir(p.assign(path).append("\\").append(fileinfo.name), files);
				}
				else
				{
					string filePath = p.assign(path).append("\\").append(fileinfo.name);
					filePath = Relace(filePath, "\\", "/");
					filePath = Relace(filePath, "//", "/");
					files.push_back(filePath);
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	static void DeleteDir(std::string path)
	{
		intptr_t   hFile = 0;
		struct _finddata_t fileinfo;
		std::string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
						string dirName = p.assign(path).append("\\").append(fileinfo.name);
						DeleteDir(dirName);
						_rmdir(dirName.c_str());
					}
				}
				else
				{
					string filePath = p.assign(path).append("\\").append(fileinfo.name);
					filePath = Relace(filePath, "\\", "/");
					filePath = Relace(filePath, "//", "/");
					remove(filePath.c_str());
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	static std::string GetNameFromPath(std::string path, bool withExtension) {
		std::string::size_type findIdx = path.rfind("/");
		if (findIdx == std::string::npos) {
			throw "get name from path failed: path=" + path;
		}

		std::string::size_type pIdx = path.rfind(".");
		if (pIdx != std::string::npos && !withExtension) {
			return path.substr(findIdx + 1, pIdx - findIdx - 1);
		}
		else {
			return path.substr(findIdx + 1, path.length() - findIdx - 1);
		}
	}

	static void CheckDir(const char* path, bool isFile = true) {
		size_t len = strlen(path);
		char* buffer = new char[len + 1UL];
		for (int i = 0; i < len; i++)
		{
			if (path[i] == '\\' || path[i] == '/') {
				if (_access(buffer, 0) != 0) {
					_mkdir(buffer);
				}
			}
			buffer[i] = path[i];
			buffer[i + 1] = 0;
		}
		if (!isFile && _access(buffer, 0) != 0) {
			_mkdir(buffer);
		}
		delete[] buffer;
	}

	static void CopyFile(const char* from, const char* to) {
		if (_access(from, 4) != 0) {
			return;
		}
		ifstream f;
		f.open(from);
		CheckDir(to);
		ofstream ostr;
		ostr.open(to, ios::out);
		ostr << f.rdbuf();
	}

}

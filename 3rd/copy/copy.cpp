/*
MIT License

Copyright(c) 2019 fangcun

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma warning(disable : 4996)

#include <iostream>
#include <stdio.h>


int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cout << "Error parameter count!" << std::endl;
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb");

	if (fp == nullptr) {
		std::cout << "File " << argv[1] << " open failed!" << std::endl;
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *mem = new char[file_size];
	if (mem == nullptr) {
		std::cout << "Memory alloc failed!" << std::endl;
		fclose(fp);
		return 1;
	}
	fread(mem, file_size, 1, fp);

	fclose(fp);

	fp = fopen(argv[2], "wb");

	if (fp == nullptr) {
		std::cout << "File " << argv[2] << " open failed!" << std::endl;
		return 1;
	}

	fwrite(mem, file_size, 1, fp);
	fclose(fp);

	delete[]mem;

	return 0;
}
#include <iostream>
#include <fstream>

#include <cassert>

#include <string>
#include <algorithm>
#include <cstring>

using namespace std;

typedef int_fast8_t s8;
typedef uint_fast8_t u8;
typedef int_fast16_t s16;
typedef uint_fast16_t u16;
typedef int_fast32_t s32;
typedef uint_fast32_t u32;



struct segment_t {
	s16 cs, ds, ss, es;
};

segment_t segments; // all set to 0 -PR-




int parser(const string IN, char* OUT) {
	string M = IN.substr(0, 3);

	if (M == "pus") { // push
		if (IN == "push ax") {
			OUT[0] = 32; // 50h
		} else if (IN == "push cx") {
			OUT[0] = 33;
		} else if (IN == "push dx") {
			OUT[0] = 34;
		} else if (IN == "push bx") {
			OUT[0] = 35;
		} else if (IN == "push sp") {
			OUT[0] = 36;
		} else if (IN == "push bp") {
			OUT[0] = 37;
		} else if (IN == "push si") {
			OUT[0] = 38;
		} else if (IN == "push di") {
			OUT[0] = 39;
		} else if (IN == "push cs") {
			OUT[0] = 0x0E;
		} else if (IN == "push ds") {
			OUT[0] = 0x1E;
		} else if (IN == "push ss") {
			OUT[0] = 0x16;
		} else if (IN == "push es") {
			OUT[0] = 0x06;
		} else {
			return 0; // error
		}
		return 1;
	} else if (M == "pop") { // pop
		if (IN == "pop ax") {
			OUT[0] = 40; // 58h
		} else if (IN == "pop cx") {
			OUT[0] = 41;
		} else if (IN == "pop dx") {
			OUT[0] = 42;
		} else if (IN == "pop bx") {
			OUT[0] = 43;
		} else if (IN == "pop sp") {
			OUT[0] = 44;
		} else if (IN == "pop bp") {
			OUT[0] = 45;
		} else if (IN == "pop si") {
			OUT[0] = 46;
		} else if (IN == "pop di") {
			OUT[0] = 47;
		} else if (IN == "pop cs") {
			assert(1); return 0; // INVALID
		} else if (IN == "pop ds") {
			OUT[0] = 0x1F;
		} else if (IN == "pop ss") {
			OUT[0] = 0x17;
		} else if (IN == "pop es") {
			OUT[0] = 0x07;
		} else {
			return 0; // error
		}
		return 1;
	}
	return 0;
}




















int main(int argc, char* argv[]) {

	if (argc < 3) { cout << argv[0] << " <filename.in> <filename.out>\n"; return 1; }
	const char* filenamein  = argv[1];
	const char* filenameout = argv[2];
	ifstream in(filenamein);
	ofstream out(filenameout, ios::binary);
	if (!in) {
		return 1; // error opening file
	}
	if (!out) {
		return 1; // error opening file
	}



	string line;
	char output_buf[] = {};
	int i = 0;
	while (getline(in, line)) {
		i = parser(line, output_buf);
		out.write(reinterpret_cast<char*>(&output_buf), i);
	}

	in.close();
	out.close();

	return 0;
}







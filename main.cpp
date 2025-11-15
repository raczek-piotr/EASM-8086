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
			OUT[0] = 0x50;
		} else if (IN == "push cx") {
			OUT[0] = 0x51;
		} else if (IN == "push dx") {
			OUT[0] = 0x52;
		} else if (IN == "push bx") {
			OUT[0] = 0x53;
		} else if (IN == "push sp") {
			OUT[0] = 0x54;
		} else if (IN == "push bp") {
			OUT[0] = 0x55;
		} else if (IN == "push si") {
			OUT[0] = 0x56;
		} else if (IN == "push di") {
			OUT[0] = 0x57;
		} else if (IN == "push cs") {
			OUT[0] = 0x0E;
		} else if (IN == "push ds") {
			OUT[0] = 0x1E;
		} else if (IN == "push ss") {
			OUT[0] = 0x16;
		} else if (IN == "push es") {
			OUT[0] = 0x06;
		} else {
			return 0;
		}
		return 1;
	} else if (M == "pop") { // pop
		if (IN == "pop ax") {
			OUT[0] = 0x58;
		} else if (IN == "pop cx") {
			OUT[0] = 0x59;
		} else if (IN == "pop dx") {
			OUT[0] = 0x5A;
		} else if (IN == "pop bx") {
			OUT[0] = 0x5B;
		} else if (IN == "pop sp") {
			OUT[0] = 0x5C;
		} else if (IN == "pop bp") {
			OUT[0] = 0x5D;
		} else if (IN == "pop si") {
			OUT[0] = 0x5E;
		} else if (IN == "pop di") {
			OUT[0] = 0x5F;
		} else if (IN == "pop cs") {
			assert(1); return 0; // INVALID
		} else if (IN == "pop ds") {
			OUT[0] = 0x1F;
		} else if (IN == "pop ss") {
			OUT[0] = 0x17;
		} else if (IN == "pop es") {
			OUT[0] = 0x07;
		} else {
			return 0;
		}
		return 1;
	} else if (M == "inc") { // inc
		if (IN == "inc ax") {
			OUT[0] = 0x40;
		} else if (IN == "inc cx") {
			OUT[0] = 0x41;
		} else if (IN == "inc dx") {
			OUT[0] = 0x42;
		} else if (IN == "inc bx") {
			OUT[0] = 0x43;
		} else if (IN == "inc sp") {
			OUT[0] = 0x44;
		} else if (IN == "inc bp") {
			OUT[0] = 0x45;
		} else if (IN == "inc si") {
			OUT[0] = 0x46;
		} else if (IN == "inc di") {
			OUT[0] = 0x47;
		} else {
			return 0;
		}
		return 1;
	} else if (M == "dec") { // dec
		if (IN == "dec ax") {
			OUT[0] = 0x48;
		} else if (IN == "dec cx") {
			OUT[0] = 0x49;
		} else if (IN == "dec dx") {
			OUT[0] = 0x4A;
		} else if (IN == "dec bx") {
			OUT[0] = 0x4B;
		} else if (IN == "dec sp") {
			OUT[0] = 0x4C;
		} else if (IN == "dec bp") {
			OUT[0] = 0x4D;
		} else if (IN == "dec si") {
			OUT[0] = 0x4E;
		} else if (IN == "dec di") {
			OUT[0] = 0x4F;
		} else {
			return 0;
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







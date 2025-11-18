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


char extract(int a, int b) {
	while (a > b) a -= b;
	return a;
}






char get_seg(string SEG) {
	if (SEG == "cs") return 0x2E;
	if (SEG == "ds") return 0x3E;
	if (SEG == "ss") return 0x36;
	if (SEG == "es") return 0x26;
	return 0x00; // error
}

int get_reg_size(string REG) {
	if (REG[1] == 'x') return 1; // ax, bx, cx, dx
	if (REG[1] == 'i') return 1; // si, di
	if (REG[1] == 'p') return 1; // sp, bp
	return 0;	
}

char get_reg(string REG) { // xxx reg
	char reg = 0;
	if (REG[1] == 'h') reg = 4;
	if (REG[0] == 'a') return reg;
	if (REG[0] == 'c') return reg+1;
	if (REG == "di")   return 7;
	if (REG[0] == 'd') return reg+2;
	if (REG == "bp")   return 5;
	if (REG[0] == 'b') return reg+3;
	if (REG == "si")   return 6;
	if (REG == "sp")   return 4;
	assert(1);
	return 0;
}





int tail(const string TAIL, unsigned char OUT[8]) {
	size_t pos = TAIL.find(' ');
	if (pos == string::npos) return 0;

	string A = TAIL.substr(0, pos);
	string B = TAIL.substr(pos + 1);
	int flag = 0, len = 0, has_prefix = 0;

	// validation -PR-
	if (A[A.size() - 1] == ']') flag += 1;
	if (B[B.size() - 1] == ']') flag += 2;
	if (flag == 3) return 0; // no mem ← mem

	if (flag & 1) { // „H” must be big… (in numbers) -PR-
		if (A[A.size() - 2] == 'H') flag += 4;
	} else {
		if (A[A.size() - 1] == 'H') flag += 4;
	}
	if (flag & 2) {
		if (B[B.size() - 2] == 'H') flag += 8;
	} else {
		if (B[B.size() - 1] == 'H') flag += 8;
	}
	if (flag >= 12) return 0; // one i at once

	// search for segment (only for one)
	pos = A.find(':');
	if (pos == string::npos) {
		flag += 16;
		pos = B.find(':');
		if (pos == string::npos)
			flag += 16; // so… +32 -PR-
	}
	if (flag < 32) {
		string SEG = A;
		if (flag & 16)
			SEG = B;
		SEG = SEG.substr(0, pos);
		OUT[0] = get_seg(SEG);
		if (OUT[0] != 0) {
			len++; // ok, first byte set
			has_prefix = 1; //len += 32; // SEG_FLAG (in len; not if flag) [will I have to make 5th type of programming language to make it clearer?] -PR-
		}
	}
	// OUT[len] will be instruction set after return of this function [will I have to make 5th type…] flag 32 on OUT[0] or on OUT[1] -PR-

	// check if it is byte or word
	{
		string reg = A;
		if (flag & 5 != 0) reg = B;   // if A is not a register
		if (flag & 10 == 0) return 0; // can't specified size → return -PR-
		if (get_reg_size(reg) == 1) {
			OUT[len] = 1;
		} else {
			OUT[len] = 0;
		}
		if (flag & 5) {
			OUT[len] += 2;
		}
	}
	len++; // space for the command -PR-

	if ((flag & 15) == 0) { // r, r
		unsigned char byte2 = 3;
		byte2 <<= 3;
		byte2 += get_reg(B);
		byte2 <<= 3;
		byte2 += get_reg(A);
		OUT[len] = byte2;
	}






	cout << (flag & 32)/32 << (flag & 16)/16 << (flag & 8)/8 << (flag & 4)/4 << (flag & 2)/2 << (flag & 1) << '\n';
	
	
	// len & 32 → segment is on OUT[0]
	return ++len + 32*has_prefix;
}




int parser(const string IN, unsigned char OUT[8]) {
	size_t pos = IN.find(' ');
	string HEAD = IN.substr(0, pos);
	string TAIL = IN.substr(pos + 1);

	if (HEAD== "push") { // push
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
	} else if (HEAD == "pop") { // pop
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
	} else if (HEAD == "inc") { // inc
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
	} else if (HEAD == "dec") { // dec
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
	} else if (HEAD == "mov") { // mov
		//OUT[0] = 0x0; // or (08h, 09h, 0Ah, 0Bh) → (m,r B; m,r W; r,r/m B; r,r/m W) -PR-
		int len = tail(TAIL, OUT);

		if (len & 32) { // has_prefix = 1 -PR-
			OUT[1] += 0x88;
		} else {
			OUT[0] += 0x88;
		}



		// for debug [will be removed] -PR-
		for (int i = 0; i < extract(len, 8) ; i++)
			cout << (int)OUT[i] << ' ';
		cout << '\n';
		if (len <= 8) return len;

		return 0;
	}
	return 0;
}




















int main(int argc, char* argv[]) {

	if (argc != 4) { cout << argv[0] << " <filename-in> <filename-cfg> <filename-out>\n"; return 1; }
	const char* filenamein  = argv[1];
	const char* filenameout = argv[3];
	ifstream in(filenamein);
	ofstream out(filenameout, ios::binary);
	if (!in)  return 1; // error opening file
	if (!out) return 1; // error opening file



	string line;
	unsigned char output_buf[8] = {};
	int i = 0;
	while (getline(in, line)) {
		i = parser(line, output_buf);
		if (i <= 8)
			out.write(reinterpret_cast<char*>(&output_buf), i);
		else {
			
		}
	}



	in.close();
	out.close();

	return 0;
}







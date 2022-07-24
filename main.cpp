//=============================================================================//
/*! @file
    @brief  utils::format クラス・テストケース
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2021, 2022 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=============================================================================//
#include <iostream>
#include <boost/format.hpp>
#include <limits>
#include <cmath>

// mingw64 環境では、標準の「write」関数より「putchar」を使った方が高速です。
#define USE_PUTCHAR
#include "format.hpp"

namespace {

	void list_error_(int cmp, utils::format::error errc)
	{
		if(cmp == 0) {
			std::cout << "  Error: string miss match.";
		}
		switch(errc) {
		case utils::format::error::none:
			break;
		case utils::format::error::null:
			std::cout << "  Error: null ptr.";
			break;
		case utils::format::error::unknown:
			std::cout << "  Error: unknown type.";
			break;
		case utils::format::error::different:
			std::cout << "  Error: different type.";
			break;
		case utils::format::error::over:
			std::cout << "  Error: over range.";
			break;
		case utils::format::error::out_null:
			std::cout << "  Error: output NULL.";
			break;
		case utils::format::error::out_overflow:
			std::cout << "  Error: output overflow.";
			break;
		default:
			break;
		}
		std::cout << std::endl;
	}

	int list_result_(int no, int subidx, int subnum, const std::string& title, const std::string& errmsg, bool error)
	{
		int ret;
		if(error) ret = 0; else ret = 1;

		if(ret != 0) {
			std::cout << "\x1B[32;1m";
		} else {
			std::cout << "\x1B[31;1m";
		}

		std::cout << boost::format("(%2d) ") % no;
		std::cout << boost::format("(%2d/%2d) ") % subidx % subnum;
		std::cout << title;

		if(ret != 0) {
			std::cout << "  Pass." << std::endl;
		} else {
			std::cout << "  " << errmsg << std::endl;
		}
		std::cout << "\x1B[37;m";
		return ret;
	}

	int list_result_(int no, int subidx, int subnum, const std::string& title, const std::string& ref, const std::string& res, utils::format::error err)
	{
		int ret;
		if(ref.empty()) {
			if(err == utils::format::error::none) {
				ret = 1;
			} else {
				ret = 0;
			}
		} else {
			if(ref == res) {
				ret = 1;
			} else {
				ret = 0;
			}
		}
		if(ret != 0) {
			std::cout << "\x1B[32;1m";
		} else {
			std::cout << "\x1B[31;1m";
		}
	
		std::cout << boost::format("(%2d) ") % no;
		std::cout << boost::format("(%2d/%2d) ") % subidx % subnum;

		if(!ref.empty()) {
			std::cout << title << " Ref: '" << ref << "' <-> Res: '" << res << "'";
		}
		if(ret != 0) {
			std::cout << "  Pass." << std::endl;
		} else {
			list_error_(ret, err);
		}
		std::cout << "\x1B[37;m";
		return ret;
	} 

	// 速度検査用
	void test_speed_(const std::string& s, long count)
	{
		auto a = sqrtf(2.0f);
		auto b = sqrtf(5.0f);
		if(s == "printf") {
        	// libc version
        	for(long i = 0; i < count; ++i)
            	printf("%11.10f:%04d:%+g:%s:%p:%c:%%\n",
                	a, 42, b, "str", (void*)1000, (int)'X');
		} else if(s == "boost") {
			for(long i = 0; i < count; ++i) {
				std::cout << boost::format("%11.10f:%04d:%+g:%s:%p:%c:%%\n")
					% a % 42 % b % "str" % (void*)1000 % (int)'X';
			}
		} else if(s == "format") {
        	for(long i = 0; i < count; ++i) {
            	utils::format("%11.10f:%04d:%+g:%s:%p:%c:%%\n")
                	% a % 42 % b % "str" % (void*)1000 % (int)'X';
			}
		}
	}
}

static constexpr long speed_count_ = 500000;

int main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	using namespace utils;

	int pass = 0;
	int total = 0;
	uint32_t exec = 0b0111'1111'1111'1111'1111'1111;

	bool start = false;
	if(argc > 1) {
		bool init = false;
		for(int i = 1; i < argc; ++i) {
			if(argv[i] == nullptr) continue;
			std::string s = argv[i];
			if(s == "-start") {
				start = true;
			} else if(s == "-printf") {
				test_speed_("printf", speed_count_);
			} else if(s == "-boost") {
				test_speed_("boost", speed_count_);
			} else if(s == "-format") {
				test_speed_("format", speed_count_);
			} else if(s[0] == '-') {
				auto n = std::stoi(argv[i] + 1);
				if(n >= 1 && n <= 32) {
					if(!init) {
						exec = 0;
						init = true;
					}
					exec |= 1 << (n - 1);
				}
			}
		}
	}
	if(!start) {
		std::cout << "Test for 'format class'" << std::endl;
		std::cout << "'format class' version: " << utils::format::VERSION << std::endl;
		std::cout << "    -start     Start test" << std::endl;
		std::cout << "    -printf    Speed test 'printf'" << std::endl;
		std::cout << "    -boost     Speed test 'boost' (iostream)" << std::endl;
		std::cout << "    -format    Speed test 'format'" << std::endl;
		std::cout << "    -(1 to 32) Select test (1 to 32)" << std::endl;
		return 0;
	}

	if(exec & (1 << 0)) {  // Test01: 文字列制限の検査
		char res[16];
		memset(res, '.', 16);
		int size = 8;
		static const char* ref = { "0123456789" };
		auto err = (utils::sformat("%s", res, size) % ref).get_error();
		bool ef = true;
		if(err == format::error::none && strncmp(ref, res, size - 1) == 0
			&& res[size - 1] == 0 && res[size] == '.') {
			ef = false;
		}
		pass += list_result_(total + 1, 1, 1, "output buffer size check.", "Fail: output buffer size check.", ef);
		++total;
	}

	if(exec & (1 << 1)) {  // Test02: %d の 数値をデコードする検査
		int a = 12345678;
		static const char* form[] = {
			"form=%d", "form=%13d", "form=%012d", "form=%6d", "form=%07d"
		};

		int sub = 0;
		int num = 10;
		for(int i = 0; i < num; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "decimal check. ", ref, res, err);
			a = -a;
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 2)) {  // Test03: %o の 数値をデコードする検査
		uint32_t a = 01245667;
		static const char* form[] = {
			"form=%o", "form=%10o", "form=%09o", "form=%6o", "form=%07o"
		};

		int sub = 0;
		int num = 5;
		for(int i = 0; i < 5; ++i) {
			char ref[64];
			sprintf(ref, form[i], a);
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "octal check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 3)) {  // Test04: %b の 数値をデコードする検査
		int a = 0b10101110;
		static const char* form[] = {
			"form=%b", "form=%12b", "form=%013b", "form=%6b", "form=%07b"
		};
		static const char* ref[] = {
			"form=10101110", "form=    10101110",  "form=0000010101110",
			"form=10101110", "form=10101110" };

		int sub = 0;
		int num = 5;
		for(int i = 0; i < num; ++i) {
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "binary check. ", ref[i], res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 4)) {  // Test05: %x, %X の 数値をデコードする検査
		uint32_t a = 0x12A4BF9C;
		static const char* form[] = {
			"form=%x", "form=%10x", "form=%09x", "form=%6x", "form=%07x",
			"form=%X", "form=%10X", "form=%09X", "form=%6X", "form=%07X"
		};

		int sub = 0;
		int num = 10;
		for(int i = 0; i < num; ++i) {
			char ref[64];
			sprintf(ref, form[i], a);
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "hex-decimal check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 5)) {  // Test06: %u の 数値をデコードする検査
		int a = 12345678;
		static const char* form[] = {
			"form=%u", "form=%13u", "form=%012u", "form=%6u", "form=%07u"
		};

		int sub = 0;
		int num = 10;
		for(int i = 0; i < num; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "positive decimal check. ", ref, res, err);
			a = -a;
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 6)) {  // Test07: %f の 数値をデコードする検査
		float a = sqrtf(5.0f);
		static const char* form[] = {
			"form=%f", "form=%9.8f", "form=%07.6f", "form=%5.4f", "form=%05.4f", "form=%6.0f"
		};

		int sub = 0;
		int num = 12;
		for(int i = 0; i < num; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "floating point check. ", ref, res, err);
			a = -a;
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 7)) {  // Test08: %e の 数値をデコードする検査
		float a[] = { 102500.125f, 0.0000000325f, -107500.125f, -0.0000000625f };
		static const char* form[] = {
			"form=%e", "form=%7.6e", "form=%07.6e", "form=%5.4e", "form=%05.4e",
		};

		int sub = 0;
		int num = 20;
		for(int i = 0; i < num; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 2], a[i & 3]);
			char res[64];
			auto err = (sformat(form[i >> 2], res, sizeof(res)) % a[i & 3]).get_error();
			sub += list_result_(total + 1, i + 1, num, "floating point (exponent) check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 8)) {  // Test09: %s の 文字列をデコードする検査
		static const char* inp = {
			"AbcdEFG"
		};
		static const char* form[] = {
			"%s", "%10s", "%09s", "%6s", "%05s"
		};

		int sub = 0;
		int num = 5;
		for(int i = 0; i < 5; ++i) {
			char ref[64];
			sprintf(ref, form[i], inp);
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % inp).get_error();
			sub += list_result_(total + 1, i + 1, num, "string format check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 9)) {  // Test10: フォーマットに nullptr を与えた場合のエラー検査。
		auto err = (utils::format(nullptr)).get_error();
		pass += list_result_(total + 1, 1, 1, "format poniter to nullptr, error code check. ", "format error code fail.", err != format::error::null);
		++total;
	}

	if(exec & (1 << 10)) {  // Test11: 型が異なる場合のエラー検査。（different エラーになる事を確認）
		float a = 0.0f;
		static const char* form[] = { "%s", "%d", "%c", "%u", "%p" };
		int sub = 0;
		int num = 5;
		for(int i = 0; i < num; ++i) {
			char res[64];
			auto err = (utils::sformat(form[i], res, sizeof(res)) % a).get_error();
			auto title = (boost::format("different type (float): '%s' error check. ") % form[i]).str();
			sub += list_result_(total + 1, i + 1, num, title, "error type of different fail.", err != format::error::different);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 11)) {  // Test12: ポインター型検査。
		float a = 0.0f;
		static const char* form = { "%p" };
		char res[64];
		auto err = (utils::sformat(form, res, sizeof(res)) % &a).get_error();
		char ref[64];
		sprintf(ref, form, &a);
		pass += list_result_(total + 1, 1, 1, "pointer type check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 12)) {  // Test13: 浮動小数点、inf 表現検査。
		float inf = std::numeric_limits<float>::infinity();
		static const char* form = { "%f" };
		char res[64];
		auto err = (utils::sformat(form, res, sizeof(res)) % inf).get_error();
		char ref[64];
		sprintf(ref, form, inf);
		pass += list_result_(total + 1, 1, 1, "floating point 'inf' (infinity) check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 13)) {  // Test14: 型が異なる場合に適切にエラーになるか検査。
		int a = 0;
		static const char* form[] = { "%s", "%f", "%p", "%g" };
		int sub = 0;
		int num = 4;
		for(int i = 0; i < num; ++i) {
			char res[64];
			auto err = (utils::sformat(form[i], res, sizeof(res)) % a).get_error();
			auto title = (boost::format("different type (int): '%s' error check. ") % form[i]).str();
			sub += list_result_(total + 1, i + 1, num, title, "error type of different fail. ", err != format::error::different);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 14)) {  // Test15: %y 固定小数点の検査（１０ビット）
		static const uint16_t val[] = { 17, 61, 100, 500, 750, 896, 1000, 1024 };
		int sub = 0;
		int num = 8;
		for(int i = 0; i < num; ++i) {
			char res[64];
			auto err = (sformat("%4.3:10y", res, sizeof(res)) % val[i]).get_error();
			char ref[64];
			int a = static_cast<int>(val[i]) >> 10;
			int b = ((static_cast<int>(val[i]) & 0x3ff) * 1000) >> 10;
			sprintf(ref, "%d.%03d", a, b);
			sub += list_result_(total + 1, i + 1, num, "fixed point check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 15)) {  // Test16: %7.6f -1 表示
		char res[64];
		float val = -99.0f;
		auto err = (sformat("%7.6f", res, sizeof(res)) % val).get_error();
		char ref[64];
		sprintf(ref, "%7.6f", val);
		pass += list_result_(total + 1, 1, 1, "floating point '-1' check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 16)) {  // Test17: %-7.6f -1 表示
		char res[64];
		float val = -99.0f;
		auto err = (sformat("%-7.6f", res, sizeof(res)) % val).get_error();
		char ref[64];
		sprintf(ref, "%-7.6f", val);
		pass += list_result_(total + 1, 1, 1, "floating point '%-' check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 17)) {  // Test18: ポインターアドレス表示 (char*)
		char res[64];
		static const char* val = res;
		auto err = (sformat("%p", res, sizeof(res)) % val).get_error();
		char ref[64];
		sprintf(ref, "%p", val);
		pass += list_result_(total + 1, 1, 1, "report pointer (char*) '%p' check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 18)) {  // Test19: ポインターアドレス表示 (int*)
		char res[64];
		static const int dec = 1234;
		static const int* val = &dec;
		auto err = (sformat("%p", res, sizeof(res)) % val).get_error();
		char ref[64];
		sprintf(ref, "%p", val);
		pass += list_result_(total + 1, 1, 1, "report pointer (int*) '%p' check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 19)) {  // Test20: %g 表示
		int sub = 0;
		static const char* form[] = { "%g", "%G" };
		int num = 22;
		static const float mul[] = { 1e7f, 1e6f, 1e5f, -1e5f, 1e3f, 1.0f, 1e-3f, -1e-5f, 1e-5f, 1e-6f, 1e-7f };
		for(int i = 0; i < num; ++i) {
			auto a = mul[i >> 1];
			if((i & 1) != 0) a *= sqrtf(2.0f);
			char ref[64];
			sprintf(ref, form[i & 1], a);
			char res[64];
			auto err = (sformat(form[i & 1], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "floating point '%g' check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 20)) {  // Test21: %8g 表示
		int sub = 0;
		static const char* form[] = { "%8g", "%8G" };
		int num = 22;
		static const float mul[] = { 1e7f, 1e6f, 1e5f, -1e5f, 1e3f, 1.0f, 1e-3f, -1e-5f, 1e-5f, 1e-6f, 1e-7f };
		for(int i = 0; i < num; ++i) {
			auto a = mul[i >> 1];
			if((i & 1) != 0) a *= sqrtf(2.0f);
			else a += mul[i >> 1] * 0.666f;
			char ref[64];
			sprintf(ref, form[i & 1], a);
			char res[64];
			auto err = (sformat(form[i & 1], res, sizeof(res)) % a).get_error();
			sub += list_result_(total + 1, i + 1, num, "floating point '%8g' check. ", ref, res, err);
		}
		if(sub == num) {
			++pass;
		}
		++total;
	}

	if(exec & (1 << 21)) {  // Test22: 浮動小数点、nan 表現検査。
		float inf = std::numeric_limits<float>::infinity();
		static const char* form = { "%6.3f" };
		float nan = inf - inf;
		char res[64];
		auto err = (utils::sformat(form, res, sizeof(res)) % nan).get_error();
		char ref[64];
		sprintf(ref, form, nan);
		pass += list_result_(total + 1, 1, 1, "floating point 'nan' (not a number) check. ", ref, res, err);
		++total;
	}

	if(exec & (1 << 22)) {  // Test23: %% の表示検査
		static const char* form = { "%dabcdefg%%ABCDEFG%d" };
		int a = 1234;
		int b = 5678;
		char res[64];
		auto err = (utils::sformat(form, res, sizeof(res)) % a % b).get_error();
		char ref[64];
		sprintf(ref, form, a, b);
		pass += list_result_(total + 1, 1, 1, "'%' check. ", ref, res, err);
		++total;
	}

	std::cout << std::endl;
	std::cout << "format class Version: " << format::VERSION << std::endl;
	if(pass == total) {
		std::cout << "All Pass: " << pass << '/' << total << std::endl;
	} else {
		std::cout << "Pass for: " << pass << '/' << total << std::endl;
		return -1;
	}
}

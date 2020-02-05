#include <iostream>
#include <boost/format.hpp>
#include <limits>

#include "format.hpp"

namespace {

// 速度検査
// #define SPEED_TEST
#ifdef SPEED_TEST
	std::string s;
	if(argc >= 2) s = argv[1];
//	const long maxIter = 2000000L;
//	const long maxIter = 50000L;
	const long maxIter = 1;
	if(s == "printf") {
        // libc version
        for(long i = 0; i < maxIter; ++i)
            printf("%0.10f:%04d:%+g:%s:%p:%c:%%\n",
                1.234, 42, 3.13, "str", (void*)1000, (int)'X');
	}
	else if(s == "boost") {
		for(long i = 0; i < maxIter; ++i) {
			std::cout << boost::format("%0.10f:%04d:%+g:%s:%p:%c:%%\n")
			% 1.234 % 42 % 3.13 % "str" % (void*)1000 % (int)'X';
		}
	}
	else if(s == "format") {
        for(long i = 0; i < maxIter; ++i) {
            utils::format("%0.10f:%04d:%+g:%s:%x:%c:%%\n")
                % 1.234 % 42 % 3.13 % "str" % (uint32_t)1000 % (int)'X';
		}
	}
	return 0;
#endif

#if 0
	{
		float a = 1234.567890123f;
		float b = 0.567890123f;
		float c = 1234567890123.0f;
		printf("%g, %g, %g\n", a, b, c);
// 1234.57, 0.56789, 1.23457e+12
	}
#endif


	void list_error_(utils::format::error errc)
	{
		switch(errc) {
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
		default:
			std::cout << "  Error: (" << static_cast<int>(errc) << ") other error.";
			break;
		}
		std::cout << std::endl;
	}


	void list_result_(const std::string& ref, const std::string& res)
	{
		std::cout << " Ref: '" << ref << "' <-> Res: '" << res << "'";
	} 

}

int main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	using namespace utils;

	int pass = 0;
	int total = 0;

	{  // Test01: 文字列制限の検査
		char res[16];
		memset(res, '.', 16);
		int size = 8;
		static const char* inp = { "0123456789" };
		auto err = (utils::sformat("%s", res, size) % inp).get_error();
		std::cout << "Test01, output buffer size check: ("
			<< static_cast<int>(err) << ") ";
		list_result_(inp, res);
		if(err == format::error::none && strncmp(inp, res, size - 1) == 0
			&& res[size - 1] == 0 && res[size] == '.') {
			std::cout << "  Pass." << std::endl;
			++pass;
		} else {
			std::cout << "  " << static_cast<int>(res[size - 1]) << ", " << static_cast<int>(res[size]);
			std::cout << "  Error. " << std::endl;
		}
		++total;
	}


	{  // Test02: %d の 数値をデコードする検査
		int a = 12345678;
		static const char* form[] = {
			"form=%d", "form=%13d", "form=%012d", "form=%6d", "form=%07d"
		};

		int sub = 0;
		for(int i = 0; i < 10; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			std::cout << "Test02(" << i << "), decimal check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
			a = -a;
		}
		if(sub == 10) {
			++pass;
		}
		++total;
	}


	{  // Test03: %o の 数値をデコードする検査
		uint32_t a = 01245667;
		static const char* form[] = {
			"form=%o", "form=%10o", "form=%09o", "form=%6o", "form=%07o"
		};

		int sub = 0;
		for(int i = 0; i < 5; ++i) {
			char ref[64];
			sprintf(ref, form[i], a);
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % a).get_error();
			std::cout << "Test03(" << i << "), octal check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
		}
		if(sub == 5) {
			++pass;
		}
		++total;
	}


	{  // Test04: %b の 数値をデコードする検査
		int a = 0b10101110;
		static const char* form[] = {
			"form=%b", "form=%12b", "form=%013b", "form=%6b", "form=%07b"
		};
		static const char* ref[] = {
			"form=10101110", "form=    10101110",  "form=0000010101110",
			"form=10101110", "form=10101110" };

		int sub = 0;
		for(int i = 0; i < 5; ++i) {
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % a).get_error();
			std::cout << "Test04(" << i << "), binary check.";
			list_result_(ref[i], res);
			if(strcmp(ref[i], res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
		}
		if(sub == 5) {
			++pass;
		}
		++total;
	}


	{  // Test05: %x, %X の 数値をデコードする検査
		uint32_t a = 0x12A4BF9C;
		static const char* form[] = {
			"form=%x", "form=%10x", "form=%09x", "form=%6x", "form=%07x",
			"form=%X", "form=%10X", "form=%09X", "form=%6X", "form=%07X"
		};

		int sub = 0;
		for(int i = 0; i < 10; ++i) {
			char ref[64];
			sprintf(ref, form[i], a);
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % a).get_error();
			std::cout << "Test05(" << i << "), hex-dedcimal check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
		}
		if(sub == 10) {
			++pass;
		}
		++total;
	}


	{  // Test06: %u の 数値をデコードする検査
		int a = 12345678;
		static const char* form[] = {
			"form=%u", "form=%13u", "form=%012u", "form=%6u", "form=%07u"
		};

		int sub = 0;
		for(int i = 0; i < 10; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			std::cout << "Test06(" << i << "), positive decimal check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
			a = -a;
		}
		if(sub == 10) {
			++pass;
		}
		++total;
	}


	{  // Test07: %f の 数値をデコードする検査
		float a = 1.00625f;
		static const char* form[] = {
			"form=%f", "form=%7.6f", "form=%07.6f", "form=%5.4f", "form=%05.4f",
		};

		int sub = 0;
		for(int i = 0; i < 10; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			std::cout << "Test07(" << i << "), floating point check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
			a = -a;
		}
		if(sub == 10) {
			++pass;
		}
		++total;
	}


	{  // Test08: %e の 数値をデコードする検査
		float a[] = { 102500.125f, 0.0000000325f, -107500.125f, -0.0000000625f };
		static const char* form[] = {
			"form=%e", "form=%7.6e", "form=%07.6e", "form=%5.4e", "form=%05.4e",
		};

		int sub = 0;
		for(int i = 0; i < 20; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 2], a[i & 3]);
			char res[64];
			auto err = (sformat(form[i >> 2], res, sizeof(res)) % a[i & 3]).get_error();
			std::cout << "Test08(" << i << "), floating point (exponent) check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
		}
		if(sub == 20) {
			++pass;
		}
		++total;
	}


	{  // Test09: %s の 文字列をデコードする検査
		static const char* inp = {
			"AbcdEFG"
		};
		static const char* form[] = {
			"%s", "%10s", "%09s", "%6s", "%05s"
		};

		int sub = 0;
		for(int i = 0; i < 5; ++i) {
			char ref[64];
			sprintf(ref, form[i], inp);
			char res[64];
			auto err = (sformat(form[i], res, sizeof(res)) % inp).get_error();
			std::cout << "Test09(" << i << "), text check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
		}
		if(sub == 5) {
			++pass;
		}
		++total;
	}


	{  // Test10: フォーマットに nullptr を与えた場合のエラー検査。
		auto err = (utils::format(nullptr)).get_error();
		std::cout << "Test10, format poniter to nullptr, error code: ("
			<< static_cast<int>(err) << ")";
		if(err == format::error::null) {
			std::cout << "  Pass." << std::endl;
			++pass;
		} else {
			std::cout << "  Error." << std::endl;
		}
		++total;
	}


	{  // Test11: 型が異なる場合のエラー検査。
		float a = 0.0f;
		static const char* form[] = { "%s", "%d", "%c", "%u", "%p" };
		int sub = 0;
		for(int i = 0; i < 5; ++i) {
			char res[64];
			auto err = (utils::sformat(form[i], res, sizeof(res)) % a).get_error();
			std::cout << "Test11, different type, error code: ("
				<< static_cast<int>(err) << ") '" << form[i] << "' (target float)";
			if(err == format::error::different) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				std::cout << "  Error." << std::endl;
			}
		}
		if(sub == 5) {
			++pass;
		}
		++total;
	}


	{  // Test12: ポインター型検査。
		float a = 0.0f;
		static const char* form = { "%p" };
		char res[64];
		auto err = (utils::sformat(form, res, sizeof(res)) % &a).get_error();
		char ref[64];
		sprintf(ref, form, &a);
		std::cout << "Test12, pointer type check: ("
			<< static_cast<int>(err) << ") ";
		if(err == format::error::none && strcmp(ref, res) == 0) {
			list_result_(ref, res);
			std::cout << "  Pass." << std::endl;
			++pass;
		} else {
			std::cout << "  Error." << std::endl;
		}
		++total;
	}


	{  // Test13: 浮動小数点、無限大表現検査。
		float a = std::numeric_limits<float>::infinity();
		static const char* form = { "%f" };
		char res[64];
		auto err = (utils::sformat(form, res, sizeof(res)) % a).get_error();
		char ref[64];
		sprintf(ref, form, a);
		std::cout << "Test13, floating point infinity check: ("
			<< static_cast<int>(err) << ")";
		if(err == format::error::none && strcmp(ref, res) == 0) {
			list_result_(ref, res);
			std::cout << "  Pass." << std::endl;
			++pass;
		} else {
			std::cout << "  Error." << std::endl;
		}
		++total;
	}


	{  // Test14: 型が異なる場合のエラー検査。
		int a = 0;
		static const char* form[] = { "%s", "%f", "%p", "%g" };
		int sub = 0;
		for(int i = 0; i < 4; ++i) {
			char res[64];
			auto err = (utils::sformat(form[i], res, sizeof(res)) % a).get_error();
			std::cout << "Test14, different type, error code: ("
				<< static_cast<int>(err) << ") '" << form[i] << "' (target integer)";
			if(err == format::error::different) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				std::cout << "  Error." << std::endl;
			}
		}
		if(sub == 4) {
			++pass;
		}
		++total;
	}


	{  // Test15: %y 固定小数点の検査
		static const uint16_t val[] = { 100, 500, 750, 1000, 1024 };
		int sub = 0;
		for(int i = 0; i < 5; ++i) {
			char res[64];
			auto err = (sformat("%3.2:10y", res, sizeof(res)) % val[i]).get_error();
			char ref[64];
			sprintf(ref, "%3.2f", static_cast<float>(val[i]) / 1024.0f);
			std::cout << "Test15(" << i << "), fixed point check.";
			list_result_(ref, res);
			if(strcmp(ref, res) == 0) {
				std::cout << "  Pass." << std::endl;
				++sub;
			} else {
				list_error_(err);
			}
		}
		if(sub == 5) {
			++pass;
		}
		++total;
	}


	std::cout << std::endl;
	std::cout << "format class Version: " << format::VERSION << std::endl;
	if(pass == total) {
		std::cout << "All Pass: " << pass << '/' << total << std::endl;
	} else {
		std::cout << "Pass for " << pass << '/' << total << std::endl;
		return -1;
	}
}

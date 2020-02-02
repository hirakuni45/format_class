#include <iostream>
#include <boost/format.hpp>

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


	void list_error_(utils::format::error errc)
	{
		switch(errc) {
		case utils::format::error::null:
			std::cout << "Error: null ptr.";
			break;
		case utils::format::error::unknown:
			std::cout << "Error: unknown type.";
			break;
		case utils::format::error::different:
			std::cout << "Error: different type.";
			break;
		case utils::format::error::over:
			std::cout << "Error: over range.";
			break;
		default:
			std::cout << "Error: (" << static_cast<int>(errc) << ") other error.";
			break;
		}
	}


	void list_result_(const std::string& ref, const std::string& res)
	{
		std::cout << "Ref: '" << ref << "' <-> '" << res << "'";
	} 

}

int main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	using namespace utils;

	int pass = 0;
	int total = 0;

	{  // Test01: %d の 数値をデコードする検査
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
			std::cout << "Test01(" << i << "), decimal check.";
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


	{  // Test02: %o の 数値をデコードする検査
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
			std::cout << "Test02(" << i << "), octal check.";
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


	{  // Test03: %b の 数値をデコードする検査
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
			std::cout << "Test03(" << i << "), binary check.";
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


	{  // Test04: %x, %X の 数値をデコードする検査
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
			std::cout << "Test04(" << i << "), hex-dedcimal check.";
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


	{  // Test05: %u の 数値をデコードする検査
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
			std::cout << "Test05(" << i << "), positive decimal check.";
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


	{  // Test06: %f の 数値をデコードする検査
		float a = 1.00625f;
		static const char* form[] = {
			"form=%f", "form=%7.6f", "form=%07.6f", "form=%5.4f", "form=%05.4f"
		};

		int sub = 0;
		for(int i = 0; i < 10; ++i) {
			char ref[64];
			sprintf(ref, form[i >> 1], a);
			char res[64];
			auto err = (sformat(form[i >> 1], res, sizeof(res)) % a).get_error();
			std::cout << "Test06(" << i << "), floating point check.";
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







	{  // Test07: フォーマットに nullptr を与える。
		auto err = (utils::format(nullptr)).get_error();
		std::cout << "Test07, format poniter to nullptr, error code: ("
			<< static_cast<int>(err) << ")";
		if(err == format::error::null) {
			std::cout << "  Pass." << std::endl;
			++pass;
		} else {
			std::cout << "  Error." << std::endl;
		}
		++total;
	}




#if 0
	{
		utils::format("test for 'different type (int to float)':\n  ");
		float a = 1.0f;
		auto errcode = (utils::format("int: %d\n") % a).get_error();
		list_error_(errcode);
	}

	{
		utils::format("test for 'floating point auto(%%g) format':\n");
		float a = 1210.05f;
		printf("  printf: %%g(1210.05f): %g\n", a);
		fflush(stdout);
		utils::format("  format: %%g(1210.05f): %g\n") % a;
	}

	{
		float a = 1000000.0f;
		printf("  printf: %%e(1000000.0f): %e\n", a);
		fflush(stdout);
		utils::format("  format: %%e(1000000.0f): %e\n") % a;
	}

	{
		sqri sq;
		int a = sq(100);
		utils::format("%d\n") % a;
	}

	using namespace utils;
	{
		static const char* t = { "Asdfg" };
		format("'%s'\n") % t;
	}

	{
		format("\nTest for char[]\n");
		char tmp[100];
		strcpy(tmp, "Qwert");
		format("Qwert: '%s'\n\n") % tmp;
	}

	{
		char str[] = { "Asdfg" };
		int i = 123;
		float a = 1000.105f;
		float b = 0.0f;
		format("%d, %d, '%s', %7.3f, %6.2f\n") % i % -i % str % a % b;
	}

	{  // 固定小数点
		uint16_t val = (1 << 10) + 500; 
		format("Fixed point: %4.3:10y\n") % val;
	}

	{  // sformat
		format("\nsformat pass 1: in\n");
		int a = 9817;
		sformat("9817: %d\n") % a;

		char tmp[4];
		sformat("%d", tmp, sizeof(tmp)) % a;
		format("'%s'\n\n") % tmp;
	}

	{
		float x = 1.0f;
		float y = 2.0f;
		float z = 3.0f;
		char tmp[512];
		sformat("Value: %f, %f, %f\n", tmp, sizeof(tmp)) % x % y % z;
		sformat("Value: %f, %f, %f\n", tmp, sizeof(tmp), true) % y % z % x;
		sformat("Value: %f, %f, %f\n", tmp, sizeof(tmp), true) % z % x % y;
		int size = sformat::chaout().size();
		format("(%d)\n%s") % size % tmp;
	}


	{  // 異なったプロトタイプ
		static const char* str = { "Poiuytrewq" };
		int val = 1921;
		format("%s, %d\n") % val % str;
	}

	{  // pointer
		format("\nPointer test:\n");
		const char* s = "ABCDEFG";
		format("    Pointer(const char*): %p (%s)\n") % s % s;
	}

	format("\n");
	{  // 0.01 * 10000
		float a = 0.0f;
		for(int i = 0; i < 10000; ++i) {
			a += 0.01f;
		}
		format("0.01 * 10000 (float): %6.3f\n") % a;
	}
	{  // 0.01 * 10000
		double a = 0.0f;
		for(int i = 0; i < 10000; ++i) {
			a += 0.01;
		}
		std::cout << boost::format("0.01 * 10000 (double): %6.3f\n") % a;
	}
#endif

	std::cout << std::endl;
	std::cout << "format class Version: " << format::VERSION << std::endl;
	if(pass == total) {
		std::cout << "All Pass: " << pass << '/' << total << std::endl;
	} else {
		std::cout << "Pass for " << pass << '/' << total << std::endl;
		return -1;
	}
}

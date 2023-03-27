#include <iostream>
#include <stdint.h>
#include <windows.h>
#include <sstream>
#include <dwmapi.h>
#include <Uxtheme.h>
#include <cassert>
#define MAX64 18446744073709551615
#define MAXS32 2147483648

#define p_newline() std::cout << "\n"

// using namespace std;
// ok no im joking i would never do that

void set_rename() {
	// redraw the name of the console window since windows loves to delete it
	HWND hwnd = GetConsoleWindow();
	SetWindowText(hwnd, "Collatz Conjecture Simulation");
}

static void set_console_size(HANDLE screen_buffer, SHORT width, SHORT height) { 
	assert(screen_buffer != NULL);
	assert(width > 0);
	assert(height > 0);

	COORD const size = { width, height };

	SMALL_RECT const minimal_window = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(screen_buffer, TRUE, &minimal_window);

	SetConsoleScreenBufferSize(screen_buffer, size);

	SMALL_RECT const window = { 0, 0, size.X - 1, size.Y - 1 };
	SetConsoleWindowInfo(screen_buffer, TRUE, &window);

	set_rename();
}

VOID WINAPI set_colors(int attribs) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, attribs);

	set_rename();
}

void clear_screen(char fill = ' ') {
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);

	set_rename();
}

bool isNumber(const std::string& str)
{
	for (char const& c : str) {
		if (std::isdigit(c) == 0) return false;
	}

	set_rename();
	return true;
}

void startloop();

void calculate(uint64_t& num) {

	if (num <= 0) {
		return;
	}

	const uint64_t onum = num;


	p_newline();

	int cycles = 0;
	while (num != 1) {

		set_rename();
		cycles++;
		if (num % 2 == 0) {
			// even
			num /= 2;
		}
		else {
			// odd

			num = (3 * num) + 1;
		}
		if (num % 2 == 0) {
			set_colors(0x000B); // blue
		}
		else {
			if (num != 1) {
				set_colors(0x000C); // red
			}
			else {
				// is one
				set_colors(0x000A); // green
			}
		}
		std::cout << num << " ";
	}
	p_newline();
	set_colors(0x00A0);

	std::cout << "Number Calculated: " << onum;

	set_colors(0x0000);
	p_newline();
	set_colors(0x00A0);
	std::cout << cycles << " Cycles";
	set_colors(0x0000);
	p_newline();

	set_rename();
}

int iteration(int specialnum = 0) {

	set_colors(0x0E);
	std::cout << "Enter a number or command: ";
	set_colors(0x0F);

	std::string input_str;
	if (!specialnum) {
		std::cin >> input_str;
	}


	if (input_str == "loop") {
		startloop();
		return 0;
		
	}

	if (input_str == "clear" || input_str == "cls" || input_str == "clr" || input_str == "clear_screen") {
		clear_screen();
		return 0;
	}

	if (input_str == "exit") {
		return -1;
	}


	if (input_str == "ls" || input_str == "dir") {
		set_colors(0x0F);
		std::cout << "This isn't a file manager\n";
		return 0;
	}

	if (input_str == "hello" || input_str == "hi") {
		set_colors(0x0F);
		std::cout << "hello there\n";
		return 0;
	}

	if (input_str == "random" || input_str == "rand") {
		uint64_t randomnum = rand() % (MAXS32 - 1); // lvalue
		calculate(randomnum);
		return 0;
	}

	if (input_str == "help") {
		set_colors(0x00);
		p_newline();
		p_newline();
		set_colors(0xA0);
		std::cout << "--Help--";
		set_colors(0x00);
		p_newline();
		p_newline();
		set_colors(0x03);

		std::cout << "List of commands:\n\nhelp\nclear/cls/clr/clear_screen\nhello/hi\nrandom/rand\n\n";

		return 0;
	}

	uint64_t num = 0;

	std::istringstream strs(input_str);

	strs >> num;

	if (specialnum) {
		num = specialnum;
	}

	if (!isNumber(input_str)) {
		set_colors(0x04);
		std::cout << "Error: Input is not a number or command\n";
		return 0;
	}
	if (num == 0) {
		set_colors(0x04);
		std::cout << "Error: Input is zero\n";
		return 0;
	}
	if (num >= MAX64) {
		set_colors(0x04);
		std::cout << "Error: Input is too big, sorry.\n";
		return 0;
	}

	calculate(num);

	return 0;
}

int main(){

	set_rename();
	HWND hwnd = GetConsoleWindow();
	
	BOOL enable = 1;
	BOOL notenable = 0;
	DwmSetWindowAttribute(hwnd, 20, &enable, sizeof(enable));


	SetWindowPos(hwnd, 0, 0, 0, 848, 480, SWP_NOMOVE);

	SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_VISIBLE);


	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	set_console_size(h, 80, 25);

	UpdateWindow(hwnd);


	set_colors(0x000A);

	clear_screen();

	std::cout << "Collatz Conjecture\n";

	set_colors(0x0008);
	std::cout << "Conhost HWND ID: " << hwnd << "\n\n";

	set_colors(0x000F);

	bool running = true;

	while (running) {
		int res = iteration();
		if (res == -1) running = false;
	}

	set_colors(0x000F);
	clear_screen();
	std::cout << "Exited loop";

	DwmSetWindowAttribute(hwnd, 20, &notenable, sizeof(notenable));
	SetWindowPos(hwnd, 0, 0, 0, 848, 480, SWP_NOMOVE);
	SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

	set_rename();
	return 0;
}

void startloop() {
	std::cout << "loop";
	for (uint64_t i = 0; i < 0b0100; i++) {
		std::cout << "iter";
		calculate(i);
	}
	std::cout << "Done";
}
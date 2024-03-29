#include <ice/Lexer.hpp>

#include <Windows.h>

#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
	SetConsoleOutputCP(CP_UTF8);

	// Test and Benchmark Codes
	auto test = u8"0 0.5 0e5 0e+5 0e-5 0.0e5";
	auto lexer = ice::Lexer();
	auto messages = ice::Messages();

	std::cout << "<Test Code>\n" << test << "\n\n\n";

	auto pointLexingBegin = std::chrono::high_resolution_clock::now();
	lexer.Lex("", test, messages);
	auto pointLexingEnd = std::chrono::high_resolution_clock::now();
	auto timeLexing = (pointLexingEnd - pointLexingBegin).count() / 1000;

	std::cout << "<Lexing>\n" << timeLexing << "micro-seconds\nResult:\n";
	auto tokens = lexer.Tokens();
	for (const auto& token : tokens) {
		std::cout << token.ToString() << '\n';
	}
	std::cout << "\nMessages:\n";
	messages.Print();

	return 0;
}
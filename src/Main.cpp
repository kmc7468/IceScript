#include <ice/Lexer.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
	auto test = u8"10e+4 10e-4 10ex4 10e+ 10e+5'";
	auto lexer = ice::Lexer();
	auto messages = ice::Messages();

	lexer.Lex("", test, messages);

	std::cout << "<Messages>\n";
	messages.Print();

	std::cout << "\n<Tokens>\n";
	auto tokens = lexer.Tokens();
	for (const auto& token : tokens) {
		std::cout << token.ToString() << '\n';
	}

	return 0;
}
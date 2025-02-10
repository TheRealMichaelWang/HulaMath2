// HulaMath2.cpp : Defines the entry point for the application.
//

#include "HulaMath.hpp"
#include "matrix.hpp"
#include <iostream>

using namespace HulaMath;

DYNALO_EXPORT const char** DYNALO_CALL HulaMath::manifest(HulaScript::instance::foreign_object* foreign_obj)
{
	std::cout << 
		"HulaMath 1.0\n"
		"(C) Michael Wang, 2024-2025\n\n"
		"Call help(), credits(), or license() for more information. Call quit() to exit HulaScript."
	<< std::endl;

	static const char* my_functions[] = {
		"mat",
		"vec",
		"vect",
		NULL
	};

	HulaScript::library_owner = foreign_obj;
	return my_functions;
}

DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL HulaMath::mat(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance)
{
	HULASCRIPT_EXPECT_ARGS(2);

	const size_t rows = args.at(0).size(instance);
	const size_t cols = args.at(1).size(instance);
	std::unique_ptr<matrix::element[]> elems(new matrix::element[rows * cols]);
	for (size_t i = 0; i < rows * cols; i++)
	{
		elems[i] = instance.rational_integer(0);
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, cols, std::move(elems)));
}

DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL HulaMath::vec(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance)
{
	return instance.add_foreign_object(std::make_unique<matrix>(args.size(), 1, args));
}

DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL HulaMath::vect(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance)
{
	return instance.add_foreign_object(std::make_unique<matrix>(1, args.size(), args));
}

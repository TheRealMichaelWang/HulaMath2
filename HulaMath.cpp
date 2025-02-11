// HulaMath2.cpp : Defines the entry point for the application.
//

#include "HulaMath.hpp"
#include "matrix.hpp"
#include <iostream>
#include <cstring>

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
		"ident",
		"catByRow",
		"catByCol",
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

DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL HulaMath::ident(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance)
{
	HULASCRIPT_EXPECT_ARGS(1);

	const size_t size = args.at(0).size(instance);
	std::unique_ptr<matrix::element[]> elems(new matrix::element[size * size]);
	for (size_t i = 0; i < size * size; i++)
	{
		elems[i] = instance.rational_integer(0);
	}
	for (size_t i = 0; i < size; i++)
	{
		elems[i * size + i] = instance.rational_integer(1);
	}

	return instance.add_foreign_object(std::make_unique<matrix>(size, size, std::move(elems)));
}

DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL HulaMath::catByRow(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance)
{
	std::optional<size_t> common;
	size_t total_rows = 0;
	for (auto& arg : args) {
		matrix* current;
		current = matrix::match_matrix_dim(arg, instance, std::nullopt, common);
		common = current->get_cols();
		total_rows += current->get_rows();
	}

	if (common.has_value()) {
		std::unique_ptr<matrix::element[]> elems(new matrix::element[common.value() * total_rows]);

		size_t offset = 0;
		for (auto& arg : args) {
			matrix* current = matrix::match_matrix_dim(arg, instance, std::nullopt, std::nullopt);

			std::memcpy(&elems.get()[offset], current->data(), current->get_size() * sizeof(matrix::element));
			offset += current->get_size();
		}

		return instance.add_foreign_object(std::make_unique<matrix>(total_rows, common.value(), std::move(elems)));
	}
	return HulaScript::instance::value();
}

DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL HulaMath::catByCol(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance)
{
	std::optional<size_t> common;
	size_t total_cols = 0;
	for (auto& arg : args) {
		matrix* current;
		current = matrix::match_matrix_dim(arg, instance, common, std::nullopt);
		common = current->get_rows();
		total_cols += current->get_cols();
	}

	if (common.has_value()) {
		std::unique_ptr<matrix::element[]> elems(new matrix::element[common.value() * total_cols]);

		size_t col_offset = 0;
		for (auto& arg : args) {
			matrix* current = matrix::match_matrix_dim(arg, instance, std::nullopt, std::nullopt);
			
			for (size_t row = 0; row < current->get_rows(); row++)
			{
				for (size_t col = 0; col < current->get_cols(); col++)
				{
					elems[row * total_cols + col_offset + col] = current->at(row, col);
				}
			}
			col_offset += current->get_cols();
		}

		return instance.add_foreign_object(std::make_unique<matrix>(common.value(), total_cols, std::move(elems)));
	}
	return HulaScript::instance::value();
}

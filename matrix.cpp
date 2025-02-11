#include "matrix.hpp"
#include "errors.hpp"
#include <cassert>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace HulaMath;

matrix::element HulaMath::matrix::get_element(std::vector<element>& args, HulaScript::instance& instance)
{
	HULASCRIPT_EXPECT_ARGS(2);
	size_t row = args.at(0).size(instance);
	size_t col = args.at(1).size(instance);
	validate_index(row, col, instance);

	return at(row - 1, col - 1);
}

matrix::element HulaMath::matrix::set_element(std::vector<element>& args, HulaScript::instance& instance)
{
	HULASCRIPT_EXPECT_ARGS(3);
	size_t row = args.at(0).size(instance);
	size_t col = args.at(1).size(instance);
	validate_index(row, col, instance);

	at(row - 1, col - 1) = args.at(2);
	return args.at(2);
}

matrix::element HulaMath::matrix::to_print_str(std::vector<element>& args, HulaScript::instance& instance)
{
	std::stringstream ss;
	ss << '[';

	for (size_t row = 0; row < rows; row++)
	{
		ss << "  ";
		for (size_t col = 0; col < cols; col++)
		{
			ss << std::setw(col == 0 && row == 0 ? 5 : 6);
			ss << instance.get_value_print_string(at(row, col));
		}

		if (row == rows - 1) {
			ss << std::setw(6) << ']';
		}
		else {
			ss << std::endl;
		}
	}

	return instance.make_string(ss.str());
}

std::unique_ptr<matrix::element[]> HulaMath::matrix::allocate_elements(size_t rows, size_t cols, std::vector<element>& vec)
{
	assert(rows * cols == vec.size());

	std::unique_ptr<element[]> elements(new element[rows * cols]);
	std::memcpy(elements.get(), vec.data(), vec.size() * sizeof(element));

	return elements;
}

void HulaMath::matrix::validate_index(size_t row, size_t col, HulaScript::instance& instance) const
{
	if (row < 1 || row > rows) {
		std::stringstream ss;
		ss << "Row out of bounds: got " << row << ", expected row in [1, " << rows << "].";
		instance.panic(ss.str(), HulaScript::error_code::ERROR_INDEX_OUT_OF_RANGE);
	}
	if (col < 1 || col > cols) {
		std::stringstream ss;
		ss << "Col out of bounds: got " << col << ", expected col in [1, " << cols << "].";
		instance.panic(ss.str(), HulaScript::error_code::ERROR_INDEX_OUT_OF_RANGE);
	}
}

matrix* HulaMath::matrix::match_matrix_dim(element& matrix_value, HulaScript::instance& instance, std::optional<size_t> expected_rows, std::optional<size_t> expected_cols)
{
	auto* mat = dynamic_cast<matrix*>(matrix_value.foreign_obj(instance));
	if (mat == nullptr) {
		instance.panic("Expected matrix, got another object instead.", HulaScript::error_code::ERROR_TYPE);
		return nullptr;
	}

	if (expected_rows.has_value() && mat->rows != expected_rows) {
		std::stringstream ss;
		ss << "Expected matrix with " << expected_rows.value() << " row(s), but got one with " << mat->rows << " row(s) instead.";
		instance.panic(ss.str(), error_code::MATRIX_DIM_MISMATCH);
	}

	if (expected_cols.has_value() && mat->cols != expected_cols) {
		std::stringstream ss;
		ss << "Expected matrix with " << expected_cols.value() << " row(s), but got one with " << mat->cols << " row(s) instead.";
		instance.panic(ss.str(), error_code::MATRIX_DIM_MISMATCH);
	}

	return mat;
}

HulaMath::matrix::matrix(size_t rows, size_t cols, std::unique_ptr<element[]>&& elements) : rows(rows), cols(cols), elements(std::move(elements))
{	
	declare_getter("rows", &matrix::get_rows);
	declare_getter("cols", &matrix::get_cols);
	declare_getter("size", &matrix::get_size);
	declare_method("get", &matrix::get_element);
	declare_method("set", &matrix::set_element);
	declare_method("printStr", &matrix::to_print_str);
}

HulaMath::matrix::matrix(size_t rows, size_t cols, std::vector<HulaScript::instance::value> elements) : matrix(rows, cols, std::move(allocate_elements(rows, cols, elements)))
{

}

void HulaMath::matrix::trace(std::vector<element>& to_trace)
{
	for (size_t i = 0; i < rows * cols; i++) 
	{
		to_trace.push_back(this->elements[i]);
	}
}

std::string HulaMath::matrix::to_string()
{
	std::stringstream ss;
	ss << rows << " x " << cols << " HulaMath matrix";
	return ss.str();
}

HulaScript::instance::value HulaMath::matrix::add_operator(HulaScript::instance::value& operand, HulaScript::instance& instance)
{
	matrix* mat = match_matrix_dim(operand, instance, rows, cols);

	std::unique_ptr<element[]> new_elems(new element[rows * cols]);

	std::vector<HulaScript::instance::instruction> ins;
	ins.push_back({ .operation = HulaScript::instance::opcode::ADD });
	std::vector<element> operands;
	operands.reserve(2);
	for (size_t i = 0; i < rows * cols; i++)
	{
		operands.push_back(elements[i]);
		operands.push_back(mat->elements[i]);
		new_elems[i] = instance.execute_arbitrary(ins, operands, true).value();
		operands.clear();
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, cols, std::move(new_elems)));
}

HulaScript::instance::value HulaMath::matrix::subtract_operator(HulaScript::instance::value& operand, HulaScript::instance& instance)
{
	matrix* mat = match_matrix_dim(operand, instance, rows, cols);

	std::unique_ptr<element[]> new_elems(new element[rows * cols]);

	std::vector<HulaScript::instance::instruction> ins;
	ins.push_back({ .operation = HulaScript::instance::opcode::SUBTRACT });
	std::vector<element> operands;
	operands.reserve(2);
	for (size_t i = 0; i < rows * cols; i++)
	{
		operands.push_back(elements[i]);
		operands.push_back(mat->elements[i]);
		new_elems[i] = instance.execute_arbitrary(ins, operands, true).value();
		operands.clear();
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, cols, std::move(new_elems)));
}

HulaScript::instance::value HulaMath::matrix::multiply_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) {
	matrix* mat = match_matrix_dim(operand, instance, cols, std::nullopt);
	
	const size_t common = rows;
	std::unique_ptr<element[]> new_elems(new element[rows * mat->cols]);
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t col = 0; col < mat->cols; col++)
		{
			std::vector<HulaScript::instance::instruction> ins;
			ins.push_back({ .operation = HulaScript::instance::opcode::MULTIPLY });

			std::vector<element> operands;
			std::vector<element> products;
			operands.reserve(2);
			products.reserve(common);
			for (size_t i = 0; i < common; i++)
			{
				operands.push_back(at(row, i));
				operands.push_back(mat->at(i, col));

				products.push_back(instance.execute_arbitrary(ins, operands, true).value());
				operands.clear();
			}

			ins.clear();

			if (products.size() > 1) {
				for (size_t i = 1; i < products.size(); i++) {
					ins.push_back({ .operation = HulaScript::instance::opcode::ADD });
				}
				new_elems[rows * common + col] = instance.execute_arbitrary(ins, products, true).value();
			}
			else {
				new_elems[rows * common + col] = products.at(0);
			}
		}
	}

	return instance.add_foreign_object(std::make_unique<matrix>(rows, cols, std::move(new_elems)));
}

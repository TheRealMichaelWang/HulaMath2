#pragma once

#include "HulaScript.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <string>

namespace HulaMath {
	class matrix : public HulaScript::foreign_getter_object<matrix> {
	public:
		using element = HulaScript::instance::value;

	private:
		size_t rows, cols;
		std::unique_ptr<element[]> elements;

		element get_rows(HulaScript::instance& instance) {
			return instance.rational_integer(rows);
		}

		element get_cols(HulaScript::instance& instance) {
			return instance.rational_integer(cols);
		}

		element get_size(HulaScript::instance& instance) {
			return instance.rational_integer(rows * cols);
		}

		element get_element(std::vector<element>& args, HulaScript::instance& instance);
		element set_element(std::vector<element>& args, HulaScript::instance& instance);

		element to_print_str(std::vector<element>& args, HulaScript::instance& instance);

		static std::unique_ptr<element[]> allocate_elements(size_t rows, size_t cols,  std::vector<element>& vec);
		void validate_index(size_t row, size_t col, HulaScript::instance& instance) const;

	public:
		static matrix* match_matrix_dim(element& matrix_value, HulaScript::instance& instance, std::optional<size_t> expected_rows, std::optional<size_t> expected_cols);

		matrix(size_t rows, size_t cols, std::vector<element> elements);
		matrix(size_t rows, size_t cols, std::unique_ptr<element[]>&& elements);

		element& at(size_t row, size_t col) {
			return elements[row * cols + col];
		}

		void trace(std::vector<element>& to_trace) override;
		std::string to_string() override;

		HulaScript::instance::value add_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override;
		HulaScript::instance::value subtract_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override;
		HulaScript::instance::value multiply_operator(HulaScript::instance::value& operand, HulaScript::instance& instance) override;

		const size_t get_rows() const noexcept {
			return rows;
		}

		const size_t get_cols() const noexcept {
			return cols;
		}

		const size_t get_size() const noexcept {
			return rows * cols;
		}

		const element* data() const noexcept {
			return elements.get();
		}
	};
}
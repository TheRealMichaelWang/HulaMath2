#pragma once

#include "HulaScript.hpp"

#define DYNALO_EXPORT_SYMBOLS
#include "dynalo/symbol_helper.hpp"

namespace HulaMath {
	DYNALO_EXPORT const char** DYNALO_CALL manifest(HulaScript::instance::foreign_object* foreign_obj);

	DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL mat(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance);
	DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL vec(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance);
	DYNALO_EXPORT HulaScript::instance::value DYNALO_CALL vect(std::vector<HulaScript::instance::value>& args, HulaScript::instance& instance);
}
#include <iostream>
#include "ScriptInterface.h"
#include "compiler.h"
#include "config.h"

namespace client {
	ScriptInterface::ScriptInterface() : ninput(0) { }

	ScriptInterface::ScriptInterface(const std::string& script,
		const std::vector<std::string>& name,
		const std::vector<std::vector<double> >& value) : ninput(0)
	{
		using client::parser::iterator_type;
		iterator_type iter = script.begin();
		iterator_type end = script.end();

		ast::compound_expression ast;
		using boost::spirit::x3::with;
		using parser::error_handler_type;
		error_handler_type error_handler(iter, end, std::cerr); // Our error handler

		// Our parser
		auto const parser =
			// we pass our error handler to the parser so we can access
			// it later on in our on_error and on_sucess handlers
			with<client::parser::error_handler_tag>(std::ref(error_handler))
			[
				client::compound_expression()
			];

		code_gen::compiler cmp(code, index, variable, constant, loopInfo, error_handler);				// Our compiler
		if (name.size() != value.size()) {
			throw std::invalid_argument("name and value array size must be the same");
		}
		ninput = name.size();
		for (size_t i = 0; i < ninput; ++i) {
			cmp.add(name[i], value[i]);
		}
		bool success = phrase_parse(iter, end, parser, client::parser::skipper, ast);
		if (success && iter == end) {
			if (!cmp(ast)) {
				throw std::invalid_argument("script compilation failure");
			}
		}
		else {
			throw std::invalid_argument("script parsing failure point ... " + std::string(iter, end));
		}
	}

	void ScriptInterface::update(const std::string& name, const std::vector<double>& value) {
		auto item = variable.find(name);
		if (item != variable.end()) {
			constant[item->second] = value;
		}
	}

	void ScriptInterface::retrieve(const std::string& name, std::vector<double>& value) const {
		auto item = variable.find(name);
		if (item != variable.end()) {
			value = local[item->second];
		}
		else {
			throw std::runtime_error("variable " + name + " not found in script results");
		}
	}

	std::vector<std::string> ScriptInterface::getDataName() const {
		std::vector<std::string> name(variable.size());
		size_t i = 0;
		for (auto item = variable.begin(); item != variable.end(); ++item) {
			name[i] = item->first;
			++i;
		}
		return name;
	}

	void ScriptInterface::retrieve(std::vector<std::string>& name, std::vector<std::vector<double> >& value) const {
		name.resize(variable.size());
		value.resize(variable.size());
		size_t i = 0;
		for (auto item = variable.begin(); item != variable.end(); ++item) {
			name[i] = item->first;
			value[i] = local[item->second];
			++i;
		}
	}

	void ScriptInterface::run(std::vector<double>& result)
	{
		client::vmachine vm;
		vm.execute(code, index, variable, constant, loopInfo, ninput);
		local = vm.get_local();
		vm.get_stack_top(result);
	}
}
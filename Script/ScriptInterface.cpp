#include <iostream>
#include "ScriptInterface.h"
#include "compiler.h"
#include "config.h"

namespace client {
	ScriptInterface::ScriptInterface(unsigned stackSize) : ninput(0), stack(stackSize), size(stackSize) { }

	ScriptInterface::ScriptInterface(const std::string& script,
		const std::vector<std::string>& name,
		const std::vector<std::vector<double> >& value,
		const std::vector<std::string>& array2dname,
		const std::vector<size_t>& array2ddim,
		unsigned stackSize) : ninput(0), stack(stackSize), size(stackSize)
	{
		std::map<std::string, size_t> array2d;
		if (array2dname.size() != array2ddim.size()) {
			throw std::invalid_argument("array2d name and dim array size must be the same");
		}
		for (size_t i = 0; i < array2dname.size(); ++i) {
			std::map<std::string, size_t>::iterator iter = array2d.find(array2dname[i]);
			if (iter == array2d.end()) {
				array2d.insert({ array2dname[i], array2ddim[i] });
			}
			else {
				iter->second = array2ddim[i];
			}
		}

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

		code_gen::compiler cmp(code, index, variable, array2d, constant, loopInfo, error_handler);				// Our compiler
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
			throw std::runtime_error("variable " + name + " not found in script data");
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
		client::vmachine vm(stack, size, local);
		vm.execute(code, index, variable, constant, loopInfo, ninput, workspace);
		vm.get_stack_top(result);
	}

	BlockScriptInterface::BlockScriptInterface(const std::string& script,
		const std::vector<std::string>& name,
		const std::vector<std::vector<double> >& value,
		const std::vector<std::string>& array2dname,
		const std::vector<size_t>& array2ddim,
		unsigned stackSize)
	{
		using client::parser::iterator_type;
		iterator_type iter = script.begin();
		iterator_type end = script.end();

		std::list<ast::expression_block> ast;
		using boost::spirit::x3::with;
		using parser::error_handler_type;
		error_handler_type error_handler(iter, end, std::cerr); // Our error handler

		// Our parser
		auto const parser =
			// we pass our error handler to the parser so we can access
			// it later on in our on_error and on_sucess handlers
			with<client::parser::error_handler_tag>(std::ref(error_handler))
			[
				*client::expression_block()
			];
		
		bool success = phrase_parse(iter, end, parser, client::parser::skipper, ast);
		if (!success || iter != end) {
			throw std::invalid_argument("script parsing failure point ... " + std::string(iter, end));
		}

		if (name.size() != value.size()) {
			throw std::invalid_argument("name and value array size must be the same");
		}

		std::map<std::string, size_t> array2d;
		if (array2dname.size() != array2ddim.size()) {
			throw std::invalid_argument("array2d name and dim array size must be the same");
		}
		for (size_t i = 0; i < array2dname.size(); ++i) {
			std::map<std::string, size_t>::iterator iter = array2d.find(array2dname[i]);
			if (iter == array2d.end()) {
				array2d.insert({ array2dname[i], array2ddim[i] });
			}
			else {
				iter->second = array2ddim[i];
			}
		}

		size_t i = 0;
		for (auto it = ast.begin(); it != ast.end(); ++it, ++i) {
			std::string var;
			if (it->var) {
				var = it->var->name;
			}
			scriptBlock.push_back({ it->name.name, it->group.name, var, ScriptInterface(stackSize) });
			ScriptInterface& si = std::get<3>(scriptBlock.back());
			code_gen::compiler cmp(si.code, si.index, si.variable, array2d, si.constant, si.loopInfo, error_handler);				// Our compiler
			si.ninput = name.size();
			for (size_t i = 0; i < name.size(); ++i) {
				cmp.add(name[i], value[i]);
			}
			if (!cmp(it->body)) {
				throw std::invalid_argument("script compilation failure for block " + it->name.name);
			}
		}
	}
}
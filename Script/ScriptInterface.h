#if !defined(SCRIPTINTERFACE_H)
#define SCRIPTINTERFACE_H

#include "vm.h"
#include <map>

namespace client {
	class ScriptInterface {
	public:
		ScriptInterface(unsigned stackSize = 4096);
		ScriptInterface(const std::string& script,
			const std::vector<std::string>& name,
			const std::vector<std::vector<double> >& value,
			const std::vector<std::string>& array2dname = {},
			const std::vector<size_t>& array2ddim = {},
			unsigned stackSize = 4096);
		const std::vector<bytecode>& getCode() const { return code; }
		const std::vector<int>& getIndex() const { return index; }
		void update(const std::string& name, const std::vector<double>& value);
		std::vector<std::string> getDataName() const;
		void retrieve(const std::string& name, std::vector<double>& value) const;
		void retrieve(std::vector<std::string>& name, std::vector<std::vector<double> >& value) const;
		void run(std::vector<double>& result);

	protected:
		std::vector<bytecode> code;
		std::vector<int> index;
		std::map<std::string, size_t> variable;
		std::map<std::string, size_t> array2d;
		size_t ninput;
		std::vector<std::vector<double> > constant;
		std::vector<loopData> loopInfo;
		std::vector<double> stack;
		std::vector<size_t> size;
		std::vector<std::vector<double> > local;
	};
}

#endif

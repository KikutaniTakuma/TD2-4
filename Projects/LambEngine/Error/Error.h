#pragma once
#include <string>
#include <typeinfo>
#include "Utils/ExecutionLog/ExecutionLog.h"

namespace Lamb {
	class Error {
	public:
		class Function{};
		class Value{};

	public:
		Error() = default;
		Error(const Error&) = default;
		Error(Error&&) = default;
		~Error() = default;

		Error& operator=(const Error&) = default;
		Error& operator=(Error&&) = default;

	public:
		const std::string& What() const {
			return errorCode_;
		}

		const std::string& FunctionName() const {
			return functionName_;
		}

		const std::string& ClassName() const {
			return className_;
		}

	private:
		std::string className_;
		std::string errorCode_;
		std::string functionName_;

	public:
		template<class T>
		static const Error& Code(const std::string& errorCode, const std::string& functionName) {
			static Error err;
			err = Error{};

			err.errorCode_ = errorCode;
			err.className_ = typeid(T).name();
			err.functionName_ = functionName + "()";
			Lamb::DebugLog(std::string{ typeid(Error).name() } + " " + err.ClassName() + " : " + err.FunctionName() + " : " + err.What());

			return err;
		}
	};
}
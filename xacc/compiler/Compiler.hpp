/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef XACC_COMPILER_HPP_
#define XACC_COMPILER_HPP_

#include <iostream>
#include <memory>
#include "AbstractFactory.hpp"
#include "QCIError.hpp"
#include "IR.hpp"
#include "Accelerator.hpp"
#include <ostream>

using namespace qci::common;

namespace xacc {

/**
 * The ICompiler provides an interface for clients to realize to
 * provide a mechanism for compiling a provided source string to
 * the XACC Intermediate Representation (IR).
 */
class ICompiler : public qci::common::QCIObject {
public:
	virtual ~ICompiler() {}

	/**
	 *
	 * @param src
	 * @return
	 */
	virtual std::shared_ptr<IR> compile(const std::string& src,
			const std::shared_ptr<Accelerator>& accelerator) = 0;
};

/**
 * The Compiler provides implements the ICompiler interface by
 * delegating to available concrete language compilers. This class employs
 * the curiously recurring template pattern to delegate (static polymorphism)
 * compilation requests to subclasses that have knowledge of the source
 * string language. This interface is a subclass of the QCIObject to enable
 * dynamic injection into the QCI Common AbstractFactory.
 *
 */
template<typename Derived>
class Compiler: public ICompiler {

public:

	/**
	 * The Compiler.compile method is in charge of modifying
	 * the source code to be amenable to compilation by derived
	 * types.
	 *
	 * @param src The kernel source string.
	 * @return ir Intermediate representation for provided source kernel code.
	 */
	virtual std::shared_ptr<IR> compile(const std::string& src,
			const std::shared_ptr<Accelerator>& acc) {

		// Set the provided kernel source string
		// so derived types can have reference to it
		kernelSource =  src;

		hardware = acc;

		// Xacc requires that clients provide
		// only the body code for an attached
		// accelerator... Some language compilers
		// need to make updates to make that code
		// amenable to their specific compilation.
		modifySource();

		return getAsDerived().compile();
	}

	virtual ~Compiler() {}

protected:

protected:

	/**
	 *
	 */
	std::string kernelSource;

	std::shared_ptr<Accelerator> hardware;

	/**
	 *
	 * Derived types implementing compile should perform language
	 * specific compilation and return a shared pointer to an
	 * intermediate representation IR instance.
	 *
	 * @return
	 */
	virtual std::shared_ptr<IR> compile() {
		QCIError("Compile must be overridden by derived types.\n");
	}

	/**
	 *
	 */
	virtual void modifySource() {
		return;
	}

	/**
	 * Get a reference to this Compiler as its Derived
	 * type.
	 *
	 * @return
	 */
	Derived& getAsDerived() {
		return *static_cast<Derived*>(this);
	}


};

}
#endif
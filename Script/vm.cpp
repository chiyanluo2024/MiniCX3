#include "vm.h"
#include <boost/assert.hpp>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "Normal.h"

namespace client
{
    /*static*/ const double vmachine::dbl_eps = 1.0e-12;

	void vmachine::proc_op1_(bytecode c, std::vector<double>::iterator& iter) {
		switch (c) {
		case op_neg:
			*iter = -*iter;
			break;
		case op_zero:
			*iter = 0.0;
			break;
		case op_floor:
			if (*iter < 0.0) {
				*iter = 0.0;
			}
			break;
		case op_cap:
			if (*iter > 0.0) {
				*iter = 0.0;
			}
			break;
		case op_not:
			if (fabs(*iter) > dbl_eps) {
				*iter = 0.0;
			}
			else {
				*iter = 1.0;
			}
			break;
		case op_abs:
			*iter = fabs(*iter);
			break;
		case op_sqr:
			*iter *= *iter;
			break;
		case op_sqrt:
			if (*iter < 0.0) {
				throw std::runtime_error("cannot calculate sqrt of a negative number");
			}
			*iter = sqrt(*iter);
			break;
		case op_exp:
			*iter = exp(*iter);
			break;
		case op_log:
			if (*iter <= 0.0) {
				throw std::runtime_error("cannot calculate log of a negative number or zero");
			}
			*iter = log(*iter);
			break;
		case op_norm:
			*iter = CML::NormalDistribution::CDF_Cephes(*iter, 0);
			break;
		case op_norminv:
			*iter = CML::NormalDistribution::InverseCDF_Cephes(*iter);
			break;
		default:
			throw std::runtime_error("unsupported op code " + std::to_string(int(c)) + " input for proc_op1");
		}
	}

	void vmachine::proc_op2_(bytecode c, std::vector<double>::iterator& iter1, std::vector<double>::iterator& iter2, const double* tmp) {
		double op1 = tmp ? *tmp : *iter1;
		switch (c) {
		case op_add:
			*iter1 = op1 + *iter2;
			break;
		case op_sub:
			*iter1 = op1 - *iter2;
			break;
		case op_mul:
			*iter1 = op1 * *iter2;
			break;
		case op_div:
			*iter1 = op1 / *iter2;
			break;
		case op_pow:
			*iter1 = std::pow(op1, *iter2);
			break;
		case op_eq:
			if (fabs(op1 - *iter2) > dbl_eps) {
				*iter1 = 0.0;
			}
			else {
				*iter1 = 1.0;
			}
			break;
		case op_neq:
			if (fabs(op1 - *iter2) > dbl_eps) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		case op_lt:
			if (op1 + dbl_eps < *iter2) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		case op_lte:
			if (op1 < *iter2 + dbl_eps) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		case op_gt:
			if (op1 > *iter2 + dbl_eps) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		case op_gte:
			if (op1 + dbl_eps > *iter2) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		case op_emax:
			*iter1 = std::max(op1, *iter2);
			break;
		case op_emin:
			*iter1 = std::min(op1, *iter2);
			break;
		case op_and:
			if (fabs(op1) > dbl_eps && fabs(*iter2) > dbl_eps) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		case op_or:
			if (fabs(op1) > dbl_eps || fabs(*iter2) > dbl_eps) {
				*iter1 = 1.0;
			}
			else {
				*iter1 = 0.0;
			}
			break;
		default:
			throw std::runtime_error("unsupported op code " + std::to_string(int(c)) + " input for proc_op2");
		}
	}

	void vmachine::add(const std::vector<double>& v, std::vector<double>::iterator& stack_ptr, std::vector<size_t>::iterator& size_ptr) {
		if (size_ptr == size.end()) {
			size.resize(size.size() + 1);
			size_ptr = size.begin() + size.size() - 1;
		}
		*size_ptr = v.size();
		size_t ii = stack_ptr - stack.begin() + *size_ptr;
		if (ii > stack.size()) {
			stack.resize(ii);
			stack_ptr = stack.begin() + ii - *size_ptr;
		}
		++size_ptr;
		std::vector<double>::const_iterator iter2 = v.begin();
		while (iter2 != v.end()) {
			*stack_ptr++ = *iter2++;
		}
	}

	void vmachine::get_stack_top(std::vector<double>& val) const {
		if (size_ptr != size.begin()) {
			val.resize(*(size_ptr - 1));
			std::copy(stack_ptr - val.size(), stack_ptr, val.begin());
		}
	}

    void vmachine::execute(
        const std::vector<bytecode>& code,
        const std::vector<int>& index,
		const std::map<std::string, size_t>& variable,
		const std::vector<std::vector<double>>& data,
		std::vector<loopData>& loopInfo,
        size_t ninput
    )
    {
		static const std::vector<double> vtrue(1, 1.0);
		static const std::vector<double> vfalse(1, 0.0);
		stack_ptr = stack.begin();
        size_ptr = size.begin();
        std::vector<bytecode>::const_iterator pc = code.begin();
        std::vector<int>::const_iterator pi = index.begin();
        if (ninput > data.size()) {
            throw std::runtime_error("Number of inputs is larger than the actual data");
        }
        std::vector<std::vector<double> >::const_iterator pv = data.begin();
        local.resize(variable.size());
        std::copy(pv, pv + ninput, local.begin());
        pv += ninput;
        bool cond;
        double tmp;
        size_t sizetmp;
        std::vector<double>::iterator iter1, iter2;
        std::vector<double>::const_iterator itertmp;
        static std::vector<double> stepCopy;

        while (pc != code.end())
        {
            switch (*pc)
            {
            case op_neg:
            case op_not:
            case op_floor:
            case op_cap:
            case op_zero:
            case op_abs:
            case op_sqr:
            case op_sqrt:
            case op_exp:
            case op_log:
			case op_norm:
			case op_norminv:
                if (*pc == op_zero && *(size_ptr - 1) == 1) {
                    size_t n = std::max(1, static_cast<int>(*(stack_ptr - 1) + 0.5));
                    size_t ii = stack_ptr - stack.begin();
                    if (ii + n > stack.size()) {
                        stack.resize(ii + n);
                        stack_ptr = stack.begin() + ii;
                    }
                    stack_ptr += n - 1;
                    *(size_ptr - 1) = n;
                }
                iter1 = stack_ptr - *(size_ptr - 1);
                while (iter1 != stack_ptr) {
                    proc_op1_(*pc, iter1);
                    ++iter1;
                }
                if (*pc != op_neg && *pc != op_not) {
                    ++pi;
                }
                break;

			case op_len:
				stack_ptr -= *(size_ptr - 1) - 1;
				*(stack_ptr - 1) = static_cast<double>(*(size_ptr - 1));
				*(size_ptr - 1) = 1;
				break;

			case op_sum:
			case op_prod:
			case op_min:
			case op_max:
				itertmp = stack_ptr;
				stack_ptr -= *(size_ptr - 1);
				if (stack_ptr != itertmp) {
					iter1 = stack_ptr + 1;
					while (iter1 != itertmp) {
						switch (*pc) {
						case op_sum:
							*stack_ptr += *iter1++;
							break;
						case op_prod:
							*stack_ptr *= *iter1++;
							break;
						case op_min:
							*stack_ptr = std::min(*stack_ptr, *iter1++);
							break;
						default:
							*stack_ptr = std::max(*stack_ptr, *iter1++);
						}
					}
				}
				else {
					size_t ii = stack_ptr - stack.begin();
					if (ii > stack.size()) {
						stack.resize(ii);
						stack_ptr = stack.begin() + ii;
					}
					switch (*pc) {
					case op_sum:
						*stack_ptr = 0.0;
						break;
					case op_prod:
						*stack_ptr = 1.0;
						break;
					case op_min:
						*stack_ptr = std::numeric_limits<double>::infinity();
						break;
					default:
						*stack_ptr = -std::numeric_limits<double>::infinity();
					}
				}
				++stack_ptr;
				*(size_ptr - 1) = 1;
				++pi;
				break;

			case op_add:
			case op_sub:
			case op_mul:
			case op_div:
			case op_pow:
			case op_eq:
			case op_neq:
			case op_lt:
			case op_lte:
			case op_gt:
			case op_gte:
			case op_emax:
			case op_emin:
			case op_and:
			case op_or:
				--size_ptr;
				iter2 = stack_ptr - *size_ptr;
				iter1 = iter2 - *(size_ptr - 1);
				if (*(size_ptr - 1) == *size_ptr) {
					stack_ptr = iter2;
					while (iter1 != stack_ptr) {
						proc_op2_(*pc, iter1, iter2, 0);
						++iter1;
						++iter2;
					}
				}
				else if (*size_ptr == 1) {
					stack_ptr = iter2;
					while (iter1 != stack_ptr) {
						proc_op2_(*pc, iter1, iter2, 0);
						++iter1;
					}
				}
				else if (*(size_ptr - 1) == 1) {
					*(size_ptr - 1) = *size_ptr;
					tmp = *iter1;
					--stack_ptr;
					while (iter1 != stack_ptr) {
						proc_op2_(*pc, iter1, iter2, &tmp);
						++iter1;
						++iter2;
					}
				}
				else {
					throw std::runtime_error("cannot run this op on two arrays of different sizes");
				}
				if (*pc == op_emax || *pc == op_emin) {
					++pi;
				}
				break;

			case op_interp:
				--size_ptr;
				iter2 = stack_ptr - *size_ptr;
				--size_ptr;
				iter1 = iter2 - *size_ptr;
				if (*(size_ptr + 1) > 1 && *size_ptr != *(size_ptr + 1)) {
					throw std::runtime_error("the x and y array sizes for interp must be consistent");
				}
				stack_ptr = iter1 - *(size_ptr - 1);
				if (*(size_ptr + 1) == 1) {
					for (; stack_ptr != iter1; ++stack_ptr) {
						*stack_ptr = *iter2;
					}
				}
				else {
					itertmp = iter1;
					for (; stack_ptr != iter1; ++stack_ptr) {
						while (itertmp > iter1 && *stack_ptr + dbl_eps < *(itertmp - 1)) {
							--itertmp;
						}
						while (itertmp < iter1 + *size_ptr && *stack_ptr > *itertmp + dbl_eps) {
							++itertmp;
						}
						if (itertmp == iter1) {
							++itertmp;
						}
						else if (itertmp == iter1 + *size_ptr) {
							--itertmp;
						}
						tmp = (*stack_ptr - *(itertmp - 1)) / (*itertmp - *(itertmp - 1));
						sizetmp = itertmp - iter1;
						*stack_ptr = tmp * *(iter2 + sizetmp) + (1.0 - tmp) * *(iter2 + sizetmp - 1);
					}
				}
				++pi;
				break;

			case op_append:
				--size_ptr;
				*(size_ptr - 1) += *size_ptr;
				break;

			case op_seq:
			{
				if (*pi) {
					if (*pi > 1) {
						--size_ptr;
						stack_ptr -= *size_ptr;
						stepCopy.resize(*size_ptr);
						std::copy(stack_ptr, stack_ptr + *size_ptr, stepCopy.begin());
					}
					else {
						stepCopy.resize(1);
						stepCopy[0] = 1.0;
					}
					--size_ptr;
					stack_ptr -= *size_ptr;
					iter1 = stack_ptr;
					--size_ptr;
					stack_ptr -= *size_ptr;
					size_t nd = *size_ptr;
					if (*size_ptr != 1) {
						if (*(size_ptr + 1) > 1 && *(size_ptr + 1) != *size_ptr) {
							throw std::runtime_error("inconsistent start and end array sizes");
						}
						if (stepCopy.size() > 1 && stepCopy.size() != *size_ptr) {
							throw std::runtime_error("inconsistent start and step array sizes");
						}
					}
					else {
						if (*(size_ptr + 1) > 1 && stepCopy.size() > 1 && *(size_ptr + 1) != stepCopy.size()) {
							throw std::runtime_error("inconsistent end and step array sizes");
						}
						if (*(size_ptr + 1) > 1) {
							nd = std::max(*(size_ptr + 1), nd);
						}
						if (stepCopy.size() > 1) {
							nd = std::max(stepCopy.size(), nd);
						}
					}
					size_t nstep = 0;
					iter2 = stepCopy.begin();
					for (size_t id = 0; id < nd; ++id) {
						size_t n1 = std::max(0, static_cast<int>((*iter1 - *stack_ptr) / *iter2 + 1e-7));
						if (!id || n1 < nstep) {
							nstep = n1;
						}
						if (*size_ptr > 1) {
							++stack_ptr;
						}
						if (*(size_ptr + 1) > 1) {
							++iter1;
						}
						if (stepCopy.size() > 1) {
							++iter2;
						}
					}
					if (*size_ptr > 1) {
						stack_ptr -= nd;
					}
					if (*size_ptr < nd) {
						std::fill(stack_ptr + *size_ptr, stack_ptr + nd, *stack_ptr);
					}
					*size_ptr = nd * (nstep + 1);
					sizetmp = stack_ptr - stack.begin() + *size_ptr;
					if (sizetmp > stack.size()) {
						stack.resize(sizetmp);
						stack_ptr = stack.begin() + sizetmp - *size_ptr;
					}
					stack_ptr += nd;
					++size_ptr;
					sizetmp = 0;
					while (sizetmp < nstep) {
						iter2 = stepCopy.begin();
						for (size_t id = 0; id < nd; ++id) {
							*stack_ptr = *(stack_ptr - nd) + *iter2;
							++stack_ptr;
							if (stepCopy.size() > 1) {
								++iter2;
							}
						}
						++sizetmp;
					}
				}
				++pi;
			}
			break;

			case op_index:
			{
				iter1 = stack_ptr - *(size_ptr - 1);
				const std::vector<double>& arr = local[*pi++];
				while (iter1 != stack_ptr) {
					size_t ns = static_cast<size_t>(*iter1 + 0.5);
					if (ns >= arr.size()) {
						throw std::runtime_error("array out of bounds");
					}
					*iter1++ = arr[ns];
				}
			}
			break;

			case op_load:
				if (pi == index.end() || *pi == local.size()) {
					throw std::runtime_error("variable loading out of bounds");
				}
				add(local[*pi++], stack_ptr, size_ptr);
				break;

			case op_store:
				--size_ptr;
				local[*pi].resize(*size_ptr);
				iter1 = local[*pi].begin();
				iter2 = stack_ptr - *size_ptr;
				while (iter2 != stack_ptr) {
					*iter1++ = *iter2++;
				}
				stack_ptr -= *size_ptr;
				++pi;
				break;

			case op_indexstore:
			{
				--size_ptr;
				iter1 = stack_ptr - *size_ptr;
				--size_ptr;
				iter2 = iter1 - *size_ptr;
				std::vector<double>& arr = local[*pi++];
				if (*(size_ptr + 1) != *size_ptr && *size_ptr != 1) {
					throw std::runtime_error("assignment of arrays of different sizes");
				}
				while (iter1 != stack_ptr) {
					size_t ns = static_cast<size_t>(*(iter1++) + 0.5);
					if (ns >= arr.size()) {
						throw std::runtime_error("array out of bounds");
					}
					arr[ns] = *iter2;
					if (*size_ptr != 1) {
						++iter2;
					}
				}
				stack_ptr -= *(size_ptr + 1) + *size_ptr;
			}
			break;

			case op_push:
				add(*pv++, stack_ptr, size_ptr);
				break;

			case op_true:
				add(vtrue, stack_ptr, size_ptr);
				break;

			case op_false:
				add(vfalse, stack_ptr, size_ptr);
				break;

			case op_jump:
				pc += *pi;
				pv += *(pi + 1);
				pi += *(pi + 2);
				break;

			case op_jump_if:
				--size_ptr;
				iter2 = stack_ptr - *size_ptr;
				cond = false;
				while (iter2 != stack_ptr) {
					if (fabs(*iter2) > dbl_eps) {
					}
					else {
						cond = true;
						break;
					}
					++iter2;
				}
				if (cond) {
					pc += *pi;
					pv += *(pi + 1);
					pi += *(pi + 2);
				}
				else {
					pi += 3;
				}
				stack_ptr -= *size_ptr;
				break;

			case op_loop_start:
				--size_ptr;
				stack_ptr -= *size_ptr;
				loopInfo[*pi].range.assign(stack_ptr, stack_ptr + *size_ptr);
				loopInfo[*pi].iCurr = *size_ptr;
				loopInfo[*pi].resSize = 0;
				if (!*size_ptr) {
					++pi;
					pc += *pi;
					pv += *(pi + 1);
					pi += *(pi + 2);
				}
				else {
					local[loopInfo[*pi].iVar].resize(1);
					local[loopInfo[*pi].iVar][0] = *stack_ptr;
					pi += 4;
				}
				break;

			case op_loop_end:
				loopInfo[*pi].resSize += *(size_ptr - 1);
				--loopInfo[*pi].iCurr;
				if (loopInfo[*pi].iCurr) {
					local[loopInfo[*pi].iVar][0] = loopInfo[*pi].range[loopInfo[*pi].range.size() - loopInfo[*pi].iCurr];
					++pi;
					pc += *pi;
					pv += *(pi + 1);
					pi += *(pi + 2);
				}
				else {
					size_ptr -= loopInfo[*pi].range.size();
					*size_ptr++ = loopInfo[*pi].resSize;
					pi += 4;
				}
				break;

			default:
				throw std::runtime_error("unrecognized code found");
			}
			++pc;
        }
    }
}
#pragma once

#include "factor/base_factor.h"
#include "node/base_node.h"
#include "matrix/matrixx.h"
#include "matrix/matrix.h"

namespace hitnlls {
namespace factor {

template <int nnodes, int ndim>
class BaseFactorImpl : public BaseFactor {
public:
    BaseFactorImpl() : BaseFactor(nnodes, ndim) { info_ = ::hitnlls::matrix::Matrix<float, ndim, ndim>::Identity(); }
    virtual ~BaseFactorImpl() {}

    virtual ::hitnlls::matrix::Matrix<float, ndim, 1> Evaluate() = 0;
    virtual ::hitnlls::matrix::Matrixxf Jacobian(int nidx) = 0;

    virtual float ComputeError() override final {
        residual_ = meas_ - Evaluate();
        ::hitnlls::matrix::Matrix<float, 1, 1> error = residual_.Transpose() * info_ * residual_;
        if (kernel_) {
            robust_info_ = kernel_->Robustify(error(0, 0));
            return robust_info_[0];
        }
        return error(0, 0);
    }
    virtual ::hitnlls::matrix::Matrixxf GetJacobTInfoRes(int nidx) override final {
        if (kernel_) {
            return Jacobian(nidx).Transpose() * info_ * residual_ * robust_info_[1];
        } else {
            return Jacobian(nidx).Transpose() * info_ * residual_;
        }
    }
    virtual ::hitnlls::matrix::Matrixxf GetJacobTInfoJacob(int nidx1, int nidx2) override final {
        if (kernel_) {
            return Jacobian(nidx1).Transpose() * info_ * Jacobian(nidx2) * robust_info_[1];
        } else {
            return Jacobian(nidx1).Transpose() * info_ * Jacobian(nidx2);
        }
    }

    virtual bool SetInformation(::hitnlls::matrix::Matrix<float, ndim, ndim> info) final { info_ = info; }
    virtual bool SetMeasurement(::hitnlls::matrix::Matrix<float, ndim, 1> meas) final { meas_ = meas; }

protected:
    ::hitnlls::matrix::Matrix<float, ndim, 1> meas_;
    ::hitnlls::matrix::Matrix<float, ndim, 1> residual_;
    ::hitnlls::matrix::Matrix<float, ndim, ndim> info_;
    ::hitnlls::matrix::Vector3f robust_info_;
};

} // namespace factor
} // namespace hitnlls
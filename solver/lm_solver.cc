#include "solver/lm_solver.h"
#include <limits>
#include <algorithm>

namespace hitnlls {
namespace solver {

void LmSolver::Optimize() {
    const int lm_iter_num = 6;
    float error_bef = std::numeric_limits<float>::max();
    float error_aft = std::numeric_limits<float>::max();
    float lambda = 0;
    for (int sidx = 0; sidx < steps_; ++sidx) {
        ::hitnlls::matrix::Matrixs<::hitnlls::matrix::Matrixxf> mat_a;
        ::hitnlls::matrix::Vecxs<::hitnlls::matrix::Matrixxf> vecb;
        error_bef = graph_->ComputeGraphError();
        graph_->BuildProblem(mat_a, vecb);

        if (0 == sidx) {
            for (int ridx = 0; ridx < mat_a.Rows(); ++ridx) {
                lambda = ::std::max(lambda, mat_a(ridx, ridx).MaxDiagonalValue());
            }
            lambda *= 1e-4;
        }
        mat_a.SetLambdalm(lambda);
        ::hitnlls::matrix::Vecxs<::hitnlls::matrix::Matrixxf> inc = mat_a.SolveWithlm(vecb);
        int lm_idx = 0;
        while (!graph_->UpdateInc(inc) && lm_idx < lm_iter_num) {
            lambda *= 10;
            mat_a.SetLambdalm(lambda);
            inc = mat_a.SolveWithlm(vecb);
            ++lm_idx;
        }
        if (lm_iter_num == lm_idx) {
            return;
        }
        lambda /= 10;

        error_aft = graph_->ComputeGraphError();
        if (early_stop_) {
            if (fabs(error_aft - error_bef) / error_bef < error_reduction_) {
                return;
            }
        }
    }
}

HITNLLS_REGISTER_SOLVER(LmSolver);

} // namespace solver
} // namespace hitnlls
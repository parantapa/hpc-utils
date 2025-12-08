#pragma once

#include <cstdint>
#include <random>

#include <Random123/philox.h>
#include <Random123/conventional/Engine.hpp>

#include <hpc_utils/allocator.hpp>
#include <hpc_utils/containers.hpp>

namespace hpc_utils {

using Rng = r123::Philox4x32;
using RngEngine = r123::Engine<Rng>;

template <typename FloatType = float>
FloatType sample_uniform(RngEngine& rng) {
    std::uniform_real_distribution<FloatType> dist(0.0, 1.0);
    return dist(rng);
}

template <typename FloatType = float>
FloatType sample_normal(RngEngine& rng) {
    std::normal_distribution<FloatType> dist(0.0, 1.0);
    return dist(rng);
}

template <typename IntType = std::int32_t>
IntType sample_poisson(RngEngine& rng, auto rate) {
    std::poisson_distribution<std::int32_t> dist(rate);
    return dist(rng);
}

template <typename FloatType = float, typename IndexType = std::int32_t>
struct VoseAliasSampler1d {
    IndexType size;

    Vector<FloatType> likelihoods;
    Vector<IndexType> alias_index;
    Vector<FloatType> alias_prob;

    Vector<IndexType> small_q;
    Vector<IndexType> large_q;

    explicit VoseAliasSampler1d(IndexType s)
        : size{s}
        , likelihoods(s)
        , alias_index(s)
        , alias_prob(s)
        , small_q()
        , large_q() {
        small_q.reserve(s);
        large_q.reserve(s);
    }

    VoseAliasSampler1d() = delete;                                     // default constructor
    VoseAliasSampler1d(const VoseAliasSampler1d&) = delete;            // copy constructor
    VoseAliasSampler1d(VoseAliasSampler1d&&) = default;                // move constructor
    VoseAliasSampler1d& operator=(const VoseAliasSampler1d&) = delete; // copy assignment
    VoseAliasSampler1d& operator=(VoseAliasSampler1d&&) = default;     // move assignment

    void reset() {
        for (IndexType i = 0; i < size; i++) {
            likelihoods[i] = 0.0;
            alias_index[i] = i;
            alias_prob[i] = 1.0;
        }
        small_q.clear();
        large_q.clear();
    }

    void update(IndexType i, FloatType likelihood) {
        likelihoods[i] = likelihood;
    }

    void prepare() {
        double lhsum = 0;
        for (IndexType i = 0; i < size; i++) {
            lhsum += likelihoods[i];
        }

        lhsum = std::max<double>(lhsum, std::numeric_limits<double>::epsilon());

        for (IndexType i = 0; i < size; i++) {
            alias_prob[i] = likelihoods[i] / lhsum * size;
            if (alias_prob[i] < 1.0) {
                small_q.push_back(i);
            } else {
                large_q.push_back(i);
            }
        }

        while (!small_q.empty() && !large_q.empty()) {
            auto s = small_q.back();
            small_q.pop_back();

            auto l = large_q.back();
            large_q.pop_back();

            alias_index[s] = l;
            alias_prob[l] = alias_prob[l] + alias_prob[s] - 1.0;

            if (alias_prob[l] < 1.0) {
                small_q.push_back(l);
            } else {
                large_q.push_back(l);
            }
        }

        while (!large_q.empty()) {
            auto l = large_q.back();
            large_q.pop_back();

            alias_prob[l] = 1.0;
        }

        while (!small_q.empty()) {
            auto s = small_q.back();
            small_q.pop_back();

            alias_prob[s] = 1.0;
        }
    }

    IndexType sample(RngEngine& rng) const {
        std::uniform_real_distribution<FloatType> dist(0.0, 1.0);

        IndexType i = dist(rng) * size;
        FloatType x = dist(rng);

        if (x < alias_prob[i]) {
            return i;
        } else {
            return alias_index[i];
        }
    }
};

template <typename FloatType = float, typename IndexType = std::int32_t>
struct VoseAliasSampler2d {
    IndexType rows;
    IndexType cols;
    VoseAliasSampler1d<FloatType, IndexType> sampler1d;

    explicit VoseAliasSampler2d(IndexType r, IndexType c)
        : rows{r}
        , cols{c}
        , sampler1d(r * c) {}

    VoseAliasSampler2d() = delete;                                     // default constructor
    VoseAliasSampler2d(const VoseAliasSampler2d&) = delete;            // copy constructor
    VoseAliasSampler2d(VoseAliasSampler2d&&) = default;                // move constructor
    VoseAliasSampler2d& operator=(const VoseAliasSampler2d&) = delete; // copy assignment
    VoseAliasSampler2d& operator=(VoseAliasSampler2d&&) = default;     // move assignment

    void reset() {
        sampler1d.reset();
    };

    void update(IndexType i, IndexType j, FloatType likelihood) {
        sampler1d.update(i * cols + j, likelihood);
    }

    void prepare() {
        sampler1d.prepare();
    }

    std::pair<IndexType, IndexType> sample(RngEngine& rng) const {
        IndexType samp = sampler1d.sample(rng);
        IndexType i = samp / cols;
        IndexType j = samp % cols;
        return std::make_pair(i, j);
    }
};

} // namespace hpc_utils

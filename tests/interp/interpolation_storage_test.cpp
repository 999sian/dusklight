// Include the implementations to inspect storage without a production diagnostics API.
#include "global.h"
#include "JSystem/J3DGraphAnimator/J3DMaterialAnm.h"
#include "JSystem/J3DGraphBase/J3DMatBlock.h"
#include <cstdint>
#include <memory>
#include <vector>
// Inspect the binding vectors as well as their allocation count after reset.
#define private public
#include "dusk/interp/material.h"
#undef private
#include "dusk/interp/frame_interpolation.cpp"
#include "dusk/interp/material.cpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <new>

size_t allocations = 0;
void* operator new(size_t size) {
    ++allocations;
    if (void* result = std::malloc(size ? size : 1)) return result;
    throw std::bad_alloc();
}
void operator delete(void* ptr) noexcept { std::free(ptr); }
void operator delete(void* ptr, size_t) noexcept { std::free(ptr); }

namespace dusk::game_clock {
FrameTiming g_frameTiming{};
bool is_sim_frame() { return true; }
}
namespace dusk::interp {
void camera_on_sim_tick() {}
void camera_on_begin_record() {}
bool camera_apply_presentation() { return false; }
void camera_restore_presentation() {}
void camera_invalidate_snapshots() {}
void clear_owned_samples() {}
void clear_weather_samples() {}
namespace particle { void clear() {} void prune() {} }
namespace vertex { void clear() {} void prune() {} }
}
extern "C" void PSMTXCopy(const Mtx src, Mtx dst) { std::memcpy(dst, src, sizeof(Mtx)); }

void matrices() {
    using namespace dusk::interp;
    dusk::game_clock::g_frameTiming.interpolating = true;
    Mtx matrix{};
    matrix[0][0] = matrix[1][1] = matrix[2][2] = 1.0f;
    // Warm both snapshots and the replacement map before checking recurring allocations.
    for (int tick = 0; tick < 3; ++tick) {
        begin_record();
        matrix[0][3] = tick * 10;
        for (uintptr_t key = 1; key <= 256; ++key) record_final_mtx(matrix, reinterpret_cast<void*>(key));
        end_record();
        begin_presentation(0.5f);
        end_presentation();
    }
    const auto capacity = s_previousRecording.matrix_values.capacity();
    const auto before = allocations;
    for (int tick = 0; tick < 20; ++tick) {
        begin_record();
        assert(s_currentRecording.matrix_values.empty());
        assert(s_currentRecording.matrix_values.capacity() == capacity);
        matrix[0][3] = 30 + tick * 10;
        for (uintptr_t key = 1; key <= 256; ++key) record_final_mtx(matrix, reinterpret_cast<void*>(key));
        end_record();
        begin_presentation(0.5f);
        Mtx out;
        assert(lookup_replacement(reinterpret_cast<void*>(1), out));
        assert(out[0][3] == 25 + tick * 10);
        end_presentation();
    }
    assert(allocations == before);

    // Removed keys must vanish even when their buffer is reused.
    begin_record();
    record_final_mtx(matrix, reinterpret_cast<void*>(1));
    end_record();
    begin_presentation(0.5f);
    Mtx out;
    assert(!lookup_replacement(reinterpret_cast<void*>(2), out));
    forget_mtx(reinterpret_cast<void*>(1));
    assert(!lookup_replacement(reinterpret_cast<void*>(1), out));
    end_presentation();
    begin_record();
    matrix[0][3] = 1000;
    record_final_mtx(matrix, reinterpret_cast<void*>(1));
    end_record();
    begin_presentation(0.5f);
    assert(!lookup_replacement(reinterpret_cast<void*>(1), out));
    end_presentation();

    begin_record();
    record_final_mtx(matrix, reinterpret_cast<void*>(1));
    end_record();
    request_presentation_sync();
    begin_presentation(0.5f);
    assert(!lookup_replacement(reinterpret_cast<void*>(1), out));
    assert(get_interpolation_step() == 1.0f);
    end_presentation();

    ++dusk::game_clock::g_frameTiming.presentationEpoch;
    begin_frame(0.5f);
    assert(s_currentRecording.matrix_values.empty());
    assert(s_previousRecording.matrix_values.empty());
    dusk::game_clock::g_frameTiming.interpolating = false;
    begin_frame(0.5f);
    assert(!lookup_replacement(reinterpret_cast<void*>(1), out));
    dusk::game_clock::g_frameTiming.interpolating = true;
}

void materials() {
    using namespace dusk::interp;
    using namespace dusk::interp::material;
    auto first = acquire_recording();
    auto* address = first.get();
    auto& state = *first->bindings.m_state;
    u16 oldTarget = 7;
    auto& values = std::get<Values<u16>>(state.values);
    assert(values.capture(&oldTarget));
    assert(!values.capture(&oldTarget));
    oldTarget = 9;
    values.apply();
    assert(oldTarget == 7);
    values.restore();
    assert(oldTarget == 9);
    state.resources.resize(1);
    state.projections.resize(1);
    state.lights.resize(1);
    auto checkVectors = [&](bool empty) {
        assert(values.entries.empty() == empty && values.entries.capacity() >= 1);
        assert(state.resources.empty() == empty && state.resources.capacity() >= 1);
        assert(state.projections.empty() == empty && state.projections.capacity() >= 1);
        assert(state.lights.empty() == empty && state.lights.capacity() >= 1);
    };
    checkVectors(false);
    // The actual callback owner prevents recycling, even after the local owner is released.
    add_interpolation_callback([](void*) {}, first.get(), first);
    first.reset();
    prune();
    auto second = acquire_recording();
    assert(second.get() != address);
    checkVectors(false);
    second.reset();
    begin_sim_tick();
    const auto before = allocations;
    auto reused = acquire_recording();
    assert(reused.get() == address);
    assert(allocations == before);
    checkVectors(true);
    u16 newTarget = 11;
    assert(values.capture(&newTarget));
    newTarget = 13;
    values.apply();
    assert(newTarget == 11 && oldTarget == 9);
    values.restore();
    assert(newTarget == 13 && oldTarget == 9);
    assert(allocations == before);

    // Scene invalidation releases retained objects but honors outstanding shared owners.
    std::weak_ptr weak = reused;
    clear();
    assert(recordings().entries.empty());
    assert(!weak.expired());
    reused.reset();
    assert(weak.expired());
}

int main() {
    matrices();
    materials();
    std::puts("interpolation storage checks passed");
}

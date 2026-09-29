#pragma once

#include "JSystem/J3DGraphAnimator/J3DMaterialAnm.h"
#include "JSystem/J3DGraphBase/J3DMatBlock.h"

#include <cstdint>
#include <memory>
#include <vector>

class J3DAnmBase;
class J3DFrameCtrl;
class J3DModel;

namespace dusk::interp::material {

class ModelBindings;

struct Frames {
    f32 previous = 0.0f;
    f32 current = 0.0f;
    uint64_t tick = 0;
    uint64_t epoch = 0;
    bool valid = false;
    bool smooth = false;

    bool loop = false;
    f32 travel = 0.0f;
    f32 loopStart = 0.0f;
    f32 loopEnd = 0.0f;

    f32 read(f32 requested) const;
};

class Update {
public:
    explicit Update(J3DFrameCtrl& controller);
    ~Update();
    Update(const Update&) = delete;
    Update& operator=(const Update&) = delete;

private:
    J3DFrameCtrl& m_controller;
    f32 m_before;
    bool m_continuous;
    f32 m_travel = 0.0f;
    f32 m_loopStart = 0.0f;
    f32 m_loopEnd = 0.0f;
};

class Sample {
public:
    explicit Sample(J3DAnmBase* animation);
    ~Sample();
    Sample(const Sample&) = delete;
    Sample& operator=(const Sample&) = delete;

private:
    J3DAnmBase* m_animation;
    f32 m_frame;
};

class ModelBindings {
public:
    ModelBindings();
    ~ModelBindings();
    ModelBindings(const ModelBindings&) = delete;
    ModelBindings& operator=(const ModelBindings&) = delete;
    void capture(J3DMaterial* material);
    void apply();
    void restore();

private:
    void capture(J3DAnmBase* resource);
    struct State;
    std::unique_ptr<State> m_state;
};

class ModelScope {
public:
    explicit ModelScope(ModelBindings& bindings) : m_bindings(bindings) { m_bindings.apply(); }
    ~ModelScope() { m_bindings.restore(); }
    ModelScope(const ModelScope&) = delete;
    ModelScope& operator=(const ModelScope&) = delete;
private:
    ModelBindings& m_bindings;
};

void record_model(J3DModel* model);

// Re-runs every recorded model's material replay (interpolated values,
// view-relative lights re-aimed via LightView, calcMaterial(), diff())
// against the CURRENT j3dSys view matrix. The normal replay runs once in
// begin_presentation() against the flatscreen camera; VR calls this again
// inside each eye/stereo pass after installing the headset view, otherwise
// replayed models (NPCs, objects) keep lights aimed for the invisible
// flatscreen camera -- lighting that swings as the head turns or Link moves.
void replay_models_for_current_view();

// While set, the once-per-frame replay in begin_presentation() skips model
// recordings. m_Do_main.cpp sets it when VR rendered last frame (the eye
// passes replay per view instead), and replays for the flatscreen view
// itself if VR then doesn't render this frame -- so each frame replays
// exactly once per view actually drawn.
void set_defer_model_replay(bool defer);
bool is_model_replay_deferred();

// Accumulated model-replay cost since the last call (performance logging).
struct ReplayStats {
    double ms = 0.0;  // total time in calcMaterial/diff replays
    int models = 0;   // model replays run
    int passes = 0;   // replay_models_for_current_view() calls
};
ReplayStats take_replay_stats();

void set_view_projection(J3DTexMtxInfo* info, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
void record_light_view(J3DMaterial* material);

void sample_texture(const J3DAnmTextureSRTKey* animation, u16 track, J3DTextureSRTInfo* result);

Frames get_frames(const J3DFrameCtrl* controller);
Frames get_frames(const J3DAnmBase* resource);
void reset(const J3DFrameCtrl* controller);
void reset(const J3DAnmBase* resource);
void bind(J3DAnmBase* resource, const J3DFrameCtrl* controller);
void swap_frames(J3DAnmBase* resource, Frames& frames);
void prune();
void clear();

}  // namespace dusk::interp::material

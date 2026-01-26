# Step 3: Sprites with Animations

**Task:** Create animated game sprites (Sonic, enemies, rings, etc.) using the engine's animation system. Use the provided `BasicSprite` template as a reference.

---

## Animation System Overview

The engine uses a **two-part animation system**:

1. **Animation** (data) - Defines WHAT happens (frame ranges, timing, movement deltas)
2. **Animator** (controller) - Controls WHEN it happens (progress, callbacks, state)

Additionally:
- **AnimationFilm** - A sprite sheet with frame rectangles (the visual frames)
- **AnimationFilmHolder** - Singleton that loads and stores all films
- **AnimatorManager** - Singleton that updates all running animators

---

## Core Classes

### AnimationFilm (the sprite sheet)

```cpp
// Defined in Engine/Animations/AnimationFilm.h
class AnimationFilm {
    struct Data {
        std::string id;           // Unique identifier e.g. "sonic_walk"
        std::string path;         // Path to sprite sheet PNG
        std::vector<Rect> rects;  // Frame rectangles within the sheet
    };

    byte GetTotalFrames() const;
    Bitmap GetBitmap() const;
    const Rect& GetFrameBox(byte frameNo) const;
    void DisplayFrame(Bitmap dest, const Point& at, byte frameNo) const;
};
```

### AnimationFilmHolder (singleton - loads & stores films)

```cpp
// Defined in Engine/Animations/AnimationFilmHolder.h
class AnimationFilmHolder {
    static AnimationFilmHolder& Get();  // Singleton access

    // Load films using a parser function
    void Load(const std::string& text, const EntryParser& parser);
    void Load(const std::string& text, const FullParser& parser);

    // Retrieve a loaded film by ID
    const AnimationFilm* GetFilm(const std::string& id);

    void CleanUp();  // Free all films
};

// Parser signatures:
using EntryParser = std::function<int(int startPos, const std::string& input,
    std::string& idOutput, std::string& pathOutput, std::vector<Rect>& rectsOutput)>;

using FullParser = std::function<bool(std::list<AnimationFilm::Data>& output,
    const std::string& input)>;
```

### Animation Types

All animations derive from `Animation` base class.

#### FrameRangeAnimation (most common - sequential frames)
```cpp
// Constructor: id, startFrame, endFrame, reps, dx, dy, delay
FrameRangeAnimation(
    const std::string& id,
    unsigned startFrame,   // First frame index
    unsigned endFrame,     // Last frame index
    unsigned reps,         // Number of repetitions (0 = forever via SetForever())
    int dx, int dy,        // Movement per frame
    unsigned delay         // Milliseconds between frames
);

// Inherited from MovingAnimation:
// GetDx(), GetDy(), GetDelay(), GetReps(), IsForever(), SetForever()
```

#### FrameListAnimation (custom frame order)
```cpp
// Constructor: id, frames, reps, dx, dy, delay
FrameListAnimation(
    const std::string& id,
    const std::vector<unsigned>& frames,  // Custom frame sequence e.g. {0,1,2,1,0}
    unsigned reps,
    int dx, int dy,
    unsigned delay
);
```

#### FlashAnimation (visibility toggling)
```cpp
// Constructor: id, reps, showDelay, hideDelay
FlashAnimation(
    const std::string& id,
    unsigned reps,
    unsigned showDelay,   // ms visible
    unsigned hideDelay    // ms hidden
);
```

#### TickAnimation (simple timer callback)
```cpp
// Constructor: id, delay, reps, discrete
TickAnimation(
    const std::string& id,
    unsigned delay,
    unsigned reps,
    bool discrete  // true = fire once per delay, false = continuous
);
```

### Animator Types

Each animation type has a corresponding animator.

#### FrameRangeAnimator
```cpp
class FrameRangeAnimator : public Animator {
    void Start(FrameRangeAnimation* anim, TimeStamp startTime);
    void Progress(TimeStamp currTime);  // Called by AnimatorManager

    unsigned GetCurrFrame() const;
    unsigned GetPrevFrame() const;
    unsigned GetCurrRep() const;
    const FrameRangeAnimation& GetAnim();
};
```

#### Animator Base Class (callbacks)
```cpp
class Animator {
    enum animatorstate_t { ANIMATOR_FINISHED, ANIMATOR_RUNNING, ANIMATOR_STOPPED };

    // Callbacks - set these before Start()
    void SetOnStart(std::function<void(Animator*)> f);
    void SetOnAction(std::function<void(Animator*, Animation*)> f);  // Called each frame
    void SetOnFinish(std::function<void(Animator*)> f);

    void Stop();
    bool HasFinished() const;
};
```

**Important:** When an Animator is constructed, it auto-registers with `AnimatorManager`. When destroyed, it auto-unregisters.

### AnimatorManager (singleton - updates all animators)

```cpp
class AnimatorManager {
    static AnimatorManager& Get();

    void Progress(TimeStamp currTime);  // Call this every frame!
    void TimeShift(TimeStamp dt);       // Shift all animator times (for pause/resume)
};
```

---

## Sprite Class Integration

```cpp
// From Engine/Scene/Sprite.h
class Sprite : public LatelyDestroyable {
    // Animation film
    void SetFilm(AnimationFilm* film);
    const AnimationFilm& GetFilm() const;
    void SetFrame(byte frameNo);
    byte GetFrame();

    // Movement
    void Move(int dx, int dy);           // Through mover (with collision)
    void MoveDirect(int dx, int dy);     // Bypass mover
    void SetMover(const Mover& f);

    // Gravity
    GravityHandler& GetGravityHandler();

    // Collision
    void SetBoundingArea(BoundingArea* area);
    bool CollisionCheck(const Sprite* s) const;

    // Display
    void Display(Bitmap dest, const Rect& dpyArea, const Clipper& clipper) const;
    void SetVisibility(bool v);
};
```

---

## Reference Implementation: BasicSprite.cpp

Your classmate provided this template in `Application/Sprites/BasicSprite.h/.cpp`:

```cpp
class Sonic : public scene::Sprite {
public:
    Sonic(int x, int y, scene::GridMap* map, scene::TileLayer* layer);
    ~Sonic();

private:
    // Multiple animations for different states
    anim::FrameRangeAnimation* m_movingLeft = nullptr;
    anim::FrameRangeAnimation* m_movingRight = nullptr;
    anim::FrameRangeAnimation* m_crouch = nullptr;
    anim::FrameRangeAnimation* m_falling = nullptr;

    // Single animator (can switch between animations)
    anim::FrameRangeAnimator* m_animator = nullptr;
};
```

**Constructor pattern:**
```cpp
Sonic::Sonic(int x, int y, GridMap* map, TileLayer* layer)
    : Sprite(x, y, "Sonic")
{
    // 1. Create animations (placeholder values - replace with real frame data)
    m_movingLeft = new FrameRangeAnimation("walk_left", 0, 5, 0, -2, 0, 100);
    m_movingRight = new FrameRangeAnimation("walk_right", 0, 5, 0, 2, 0, 100);
    m_falling = new FrameRangeAnimation("fall", 0, 3, 0, 0, 4, 80);

    // 2. Create animator
    m_animator = new FrameRangeAnimator();

    // 3. Set bounding box for collisions
    m_BoundingArea = new physics::BoundingBox(
        GetBox().x, GetBox().y,
        GetBox().x + GetBox().w, GetBox().y + GetBox().h
    );

    // 4. Set mover (handles grid collision)
    SetMover(MakeSpriteGridLayerMoverWithCamera(map, this, layer));

    // 5. Configure gravity
    GetGravityHandler().SetGravityAddected(true);
    GetGravityHandler().SetOnSolidGround([map](Rect& r) {
        return map->IsOnSolidGround(r);
    });
    GetGravityHandler().SetOnStartFalling([this]() {
        m_animator->Start(m_falling, core::SystemClock::Get().GetCurrTime());
    });
    GetGravityHandler().SetOnStopFalling([this]() {
        m_animator->Stop();
    });

    // 6. Set animator callbacks
    m_animator->SetOnAction([this](Animator* anim, Animation* animation) {
        auto* frameAnim = static_cast<FrameRangeAnimation*>(animation);
        auto* frameAnimator = static_cast<FrameRangeAnimator*>(anim);

        // Update sprite frame from animator
        SetFrame(frameAnimator->GetCurrFrame());

        // Move sprite by animation's dx/dy
        Move(frameAnim->GetDx(), frameAnim->GetDy());
    });

    // 7. Start initial animation
    m_animator->Start(m_movingRight, core::SystemClock::Get().GetCurrTime());
}
```

---

## Complete Animation Flow

### 1. Load Animation Films (once at startup)

```cpp
// Option A: Simple hardcoded loading
void LoadFilms() {
    auto& holder = AnimationFilmHolder::Get();

    // Create film data manually
    std::vector<Rect> sonicWalkFrames = {
        {0, 0, 32, 40},    // Frame 0
        {32, 0, 32, 40},   // Frame 1
        {64, 0, 32, 40},   // Frame 2
        // ... more frames
    };

    // You'd need to add a method or use the parser approach
}

// Option B: Using a parser (recommended)
void LoadFilms() {
    std::string filmData = ReadFile("Assets/animations.txt");

    AnimationFilmHolder::Get().Load(filmData, [](
        std::list<AnimationFilm::Data>& output,
        const std::string& input
    ) -> bool {
        // Parse your format here
        // Each entry needs: id, path, vector of Rects
        // Push to output list
        return true;  // success
    });
}
```

### 2. Create Sprite with Film

```cpp
// Get film from holder
const AnimationFilm* walkFilm = AnimationFilmHolder::Get().GetFilm("sonic_walk");

// Create sprite and set film
Sonic* sonic = new Sonic(100, 200, &grid, &tileLayer);
sonic->SetFilm(const_cast<AnimationFilm*>(walkFilm));
sonic->SetVisibility(true);
```

### 3. Game Loop Integration

```cpp
// In your GameScene::Load()
m_Game.SetAnimationLoop([this]() {
    // Update all animators
    AnimatorManager::Get().Progress(core::SystemClock::Get().GetCurrTime());
});

m_Game.SetRenderLoop([this]() {
    // Clear screen
    gfx::BitmapClear(screenBuffer, backgroundColor);

    // Display all sprites
    for (Sprite* sprite : m_Sprites) {
        sprite->Display(screenBuffer, viewRect, clipper);
    }

    gfx::Flush();
});

m_Game.SetInputLoop([this]() {
    core::Input::UpdateInputEvents();

    // Handle input to switch animations
    if (core::Input::IsKeyPressed(io::Key::Left)) {
        sonic->GetAnimator()->Start(walkLeftAnim, SystemClock::Get().GetCurrTime());
    }
    // etc.
});

m_Game.SetCollisionsCheckingLoop([this]() {
    physics::CollisionChecker::Get().Check();
});
```

---

## What You Need to Create

### 1. Animation Data File

Create `Assets/Animations/films.json` or `films.txt` with your sprite sheet data:

```json
{
  "films": [
    {
      "id": "sonic_idle",
      "path": "Assets/Textures/sonic_sprites.png",
      "frames": [
        {"x": 0, "y": 0, "w": 32, "h": 40},
        {"x": 32, "y": 0, "w": 32, "h": 40}
      ]
    },
    {
      "id": "sonic_walk",
      "path": "Assets/Textures/sonic_sprites.png",
      "frames": [
        {"x": 0, "y": 40, "w": 32, "h": 40},
        {"x": 32, "y": 40, "w": 32, "h": 40},
        {"x": 64, "y": 40, "w": 32, "h": 40},
        {"x": 96, "y": 40, "w": 32, "h": 40}
      ]
    }
  ]
}
```

### 2. Film Parser

Create `Utilities/FilmParser.h/.cpp`:

```cpp
#include "Animations/AnimationFilmHolder.h"
#include <nlohmann/json.hpp>

bool ParseFilmsFromJson(
    std::list<anim::AnimationFilm::Data>& output,
    const std::string& jsonText
) {
    auto json = nlohmann::json::parse(jsonText);

    for (auto& film : json["films"]) {
        anim::AnimationFilm::Data data;
        data.id = film["id"];
        data.path = film["path"];

        for (auto& frame : film["frames"]) {
            data.rects.push_back({
                frame["x"], frame["y"],
                frame["w"], frame["h"]
            });
        }

        output.push_back(data);
    }

    return true;
}

// Usage:
// std::string json = ReadFile("Assets/Animations/films.json");
// AnimationFilmHolder::Get().Load(json, ParseFilmsFromJson);
```

### 3. Sprite Classes

Create in `Sprites/` folder:

| File | Description |
|------|-------------|
| `Sonic.h/.cpp` | Player character (expand from BasicSprite) |
| `Ring.h/.cpp` | Collectible ring with spin animation |
| `Badnik.h/.cpp` | Enemy base class |
| `MotoBug.h/.cpp` | Specific enemy type |

### 4. Sprite Manager Integration

```cpp
// In GameScene
class GameScene : public core::Context {
private:
    std::vector<scene::Sprite*> m_Sprites;
    Sonic* m_Sonic = nullptr;
    std::vector<Ring*> m_Rings;
    std::vector<Badnik*> m_Enemies;
};
```

---

## File Structure After Implementation

```
Application/
├── Sprites/
│   ├── BasicSprite.h/.cpp    # (existing template)
│   ├── Sonic.h/.cpp          # Player character
│   ├── Ring.h/.cpp           # Collectible
│   └── Badnik.h/.cpp         # Enemy base
├── Utilities/
│   ├── MoverUtilities.h/.cpp # (existing)
│   └── FilmParser.h/.cpp     # Animation film loader
└── Assets/
    ├── Textures/
    │   └── sonic_sprites.png # Sprite sheet
    └── Animations/
        └── films.json        # Film definitions
```

---

## Animation State Machine Pattern

For complex sprites like Sonic:

```cpp
class Sonic : public scene::Sprite {
public:
    enum class State { IDLE, WALKING, RUNNING, JUMPING, FALLING, CROUCHING };

    void SetState(State newState) {
        if (m_State == newState) return;
        m_State = newState;

        // Stop current animation
        m_animator->Stop();

        // Start new animation based on state
        switch (newState) {
            case State::IDLE:
                m_animator->Start(m_idleAnim, GetTime());
                break;
            case State::WALKING:
                m_animator->Start(m_walkAnim, GetTime());
                break;
            case State::JUMPING:
                m_animator->Start(m_jumpAnim, GetTime());
                break;
            // etc.
        }
    }

private:
    State m_State = State::IDLE;
    FrameRangeAnimation* m_idleAnim;
    FrameRangeAnimation* m_walkAnim;
    FrameRangeAnimation* m_jumpAnim;
    // etc.
};
```

---

## Engine Headers to Include

```cpp
// Animations
#include "Animations/Animation.h"
#include "Animations/Animator.h"
#include "Animations/AnimatorManager.h"
#include "Animations/AnimationFilm.h"
#include "Animations/AnimationFilmHolder.h"
#include "Animations/FrameRangeAnimation.h"
#include "Animations/FrameRangeAnimator.h"
#include "Animations/FrameListAnimation.h"
#include "Animations/FlashAnimation.h"

// Scene
#include "Scene/Sprite.h"
#include "Scene/GridLayer.h"
#include "Scene/GravityHandler.h"

// Physics
#include "Physics/BoundingArea.h"
#include "Physics/CollisionChecker.h"

// Core
#include "Core/SystemClock.h"
```

---

## Success Criteria

1. Animation films loaded from file into `AnimationFilmHolder`
2. Sonic sprite with multiple animation states (idle, walk, jump, fall)
3. Animation state changes based on input and physics
4. `AnimatorManager::Get().Progress()` called each frame
5. Sprites render correctly with current animation frame
6. At least one enemy type with simple patrol animation
7. Ring collectible with spinning animation
8. Collision detection between Sonic and enemies/rings working

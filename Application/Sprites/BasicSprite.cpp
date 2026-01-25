#include "BasicSprite.h"
#include "Animations/FrameRangeAnimation.h"
#include "Core/SystemClock.h"
#include "Utilities/MoverUtilities.h"

Sonic::Sonic(int x, int y, scene::GridMap* map, scene::TileLayer* layer)
	: scene::Sprite(x, y, "Sonic")
{

	// we need to load this animations either by hand or by parsing 
	m_movingLeft = new anim::FrameRangeAnimation("kappa", 1, 1, 1, 1, 1, 1);
	m_movingRight = new anim::FrameRangeAnimation("kappa", 1, 1, 1, 1, 1, 1);
	m_crouch = new anim::FrameRangeAnimation("kappa", 1, 1, 1, 1, 1, 1);
	m_falling = new anim::FrameRangeAnimation("kappa", 1, 1, 1, 1, 1, 1);

	// we might need more animators
	// but for the perpuse of the demoe im using 1
	m_animator = new anim::FrameRangeAnimator();

	m_BoundingArea = new physics::BoundingBox(GetBox().x, GetBox().y, GetBox().w, GetBox().h);

	// Pick one mover / make a mover inspired from the utils/MoverUitilities.h funcitons
	SetMover(MakeSpriteGridLayerMoverWithCamera(map, this, layer));

	// settings for gravity
	GetGravityHandler().SetGravityAddected(true);

	GetGravityHandler().SetOnSolidGround([map](Rect& r) {
		return map->IsOnSolidGround(r);
	});

	GetGravityHandler().SetOnStartFalling([this]() {
		// set functionality that we need to happen wen this animations is playing
		// this->m_animator->SetOnStart();
		// this->m_animator->SetOnAction();
		// this->m_animator->SetOnFinish();

		this->m_animator->Start(this->m_falling, core::SystemClock::Get().GetCurrTime());
	});

	GetGravityHandler().SetOnStopFalling([this]() {
		this->m_animator->Stop();
	});

	// now for moving left we will update that through the animator
	// becuase we need everything to be in sync

	// TODO: MOVE THIS FUNCTION IN THE UPDATE OF THE LOGIC
	// THIS IS FOR EXAMPLE ONLY

	// Moving to one direction code (let's say left)
	this->m_animator->SetOnStart([this](anim::Animator* anim) { 
		auto frameRange = static_cast<anim::FrameRangeAnimator*>(anim);
	});

	this->m_animator->SetOnAction([this](anim::Animator* mator, anim::Animation* mation) {
		auto frameMator = static_cast<anim::FrameRangeAnimator*>(mator);
		auto frameMation = static_cast<anim::FrameRangeAnimation*>(mation);

		this->Move(frameMation->GetDx(), frameMation->GetDy());
	});

	this->m_animator->SetOnFinish([this](anim::Animator* anim) { 
		auto frameRange = static_cast<anim::FrameRangeAnimator*>(anim);
	});
	this->m_animator->Start(this->m_movingLeft, core::SystemClock::Get().GetCurrTime());
	// --------------------------------------------

	// duplicate the above to simulate the motion in every direction
}

Sonic::~Sonic()
{
	m_movingLeft->Destroy();
	m_movingRight->Destroy();
	m_crouch->Destroy();

	m_animator->Destroy();
}
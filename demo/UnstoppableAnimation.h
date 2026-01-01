#pragma once
#include <utils/Tools.h>
#include <utils/Text.h>
#include <Pandemic/core/ParticleEffect.h>
#include <Pandemic/core/IAnimationRenderer.h>
#include <Pandemic/renderers/ParticleOverlayRenderer.h>
#include <Pandemic/core/TextFlasher.h>

class UnstoppableAnimation final : public virtual IAnimationRenderer
{
private:

	// Members
	Canvas tempcanvas;
	Text text;
	TweenInt textprogress;
	TweenFloat fadeprogress;
	TweenInt flashprogress;
	const Image& texture;
	const Image& bolt1;
	const Image& bolt2;
	const Image& distort1;
	const Image& electcolors;
	TimePoint starttime;
	TimePoint laststeptime;
	TimePoint bolttime1;
	TimePoint bolttime2;
	TimePoint bolttimeoff;

public:

	UnstoppableAnimation(ParticleOverlayRenderer& particlesoverlay);
	virtual ~UnstoppableAnimation();

	// Methods
	virtual void Render(Canvas& canvas) override final;
	virtual void Start() override final;
	virtual bool HasFinished() override final { return !ch::IsTimeSet(starttime); }
};

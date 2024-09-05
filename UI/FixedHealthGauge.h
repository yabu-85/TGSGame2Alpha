#pragma once
#include "HealthGauge.h"

class FixedHealthGauge : public HealthGauge
{
	
public:
	FixedHealthGauge(Character* parent, XMFLOAT2 size = XMFLOAT2(1.0f, 1.0f) );
	~FixedHealthGauge() override;
	void Draw(int index) override;

};
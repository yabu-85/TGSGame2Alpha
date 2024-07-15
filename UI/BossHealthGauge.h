#pragma once
#include "HealthGauge.h"

class BossHealthGauge : public HealthGauge
{
	
public:
	BossHealthGauge(Character* parent);
	~BossHealthGauge() override;
	void Draw(int index) override;

};
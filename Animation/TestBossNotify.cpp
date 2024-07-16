#include "TestBossNotify.h"
#include "../Enemy/TestBoss.h"
#include "../Action/MoveAction.h"
#include "../Action/RotateAction.h"

void TestBossAttackNotify::OnFrame(GameObject* obj)
{
	TestBoss* boss = static_cast<TestBoss*>(obj);
	//boss->CalcPoly();
	//boss->CalcCollider();
}

void TestBossAttackNotify::OnEnter(GameObject* obj)
{
	TestBoss* boss = static_cast<TestBoss*>(obj);
	//boss->SetAllAttackColliderValid(true);
}

void TestBossAttackNotify::OnExit(GameObject* obj)
{
	TestBoss* boss = static_cast<TestBoss*>(obj);
	//boss->AttackEnd();
}

void TestBossRotateNotify::OnFrame(GameObject* obj)
{
	TestBoss* boss = static_cast<TestBoss*>(obj);
	//boss->GetRotateAction()->Update();
	//boss->GetOrientedMoveAction()->SetTarget(GameManager::GetPlayer()->GetPosition());
}
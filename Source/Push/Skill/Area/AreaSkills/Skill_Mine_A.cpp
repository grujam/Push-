#include "Skill/Area/AreaSkills/Skill_Mine_A.h"
#include "Components/CapsuleComponent.h"
#include "Global.h"
#include "Buffs/BuffInstance.h"
#include "Character/PushCharacter.h"
#include "Components/BuffComponent.h"
#include "Components/SkillComponent.h"
#include "Particles/ParticleSystemComponent.h"

ASkill_Mine_A::ASkill_Mine_A()
{
	//2024.01.15 ������
	//���� �ݸ���, ���߹��� �ݸ���(�����Ǳ� ������ visibility = false)
	Helpers::CreateComponent<UCapsuleComponent>(this, &ActivationCollision, "ActivationCollision", Root);
	Helpers::CreateComponent<UCapsuleComponent>(this, &ExplosionCollision, "ExplosionCollision", Root);
	Helpers::CreateComponent<UParticleSystemComponent>(this, &BaseParticleComponent, "BaseParticleComponent", Root);
	Helpers::CreateComponent<UParticleSystemComponent>(this, &ExplosionParticleComponent, "ExplosionParticleComponent", Root);

	//���� ��ƼŬ ������ ���� �����Լ� ���ε�
	ExplosionParticleComponent->SetVisibility(false);
	ExplosionParticleComponent->OnSystemFinished.AddDynamic(this, &ASkill_Mine_A::OnFinishParticle);
}

void ASkill_Mine_A::BeginPlay()
{
	Super::BeginPlay();

	USkillComponent* skillComponent = Helpers::GetComponent<USkillComponent>(Owner);
	if (!skillComponent)
		return;

	DecalLocation = skillComponent->SpawnLocation;

	ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ActivationCollision->OnComponentBeginOverlap.AddDynamic(this, &ASkill_Mine_A::OnActivateOverlap);
	ExplosionCollision->OnComponentBeginOverlap.AddDynamic(this, &ASkill_Mine_A::OnExplosionOverlap);
	BaseParticleComponent->SetTemplate(BaseParticle);
}

//������ ���ݴ������� ����
void ASkill_Mine_A::Hit(AActor* InAttacker, const FHitData& InHitData)
{
	ActivationCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ExplosionParticleComponent)
	{
		BaseParticleComponent->DestroyComponent(true);
		ExplosionParticleComponent->SetVisibility(true);
		ExplosionParticleComponent->SetTemplate(ExplosionParticle);
	}
}

//�÷��̾ ���ڸ� �������
void ASkill_Mine_A::OnActivateOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<IDamageable>(OtherActor) || OtherActor->IsA(StaticClass()))
		return;

	

	CLog::LogAndPrint(Cast<APushCharacter>(OtherActor)->CustomPlayerName);
	ActivationCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//DelayTime���� ActiveExplosion�Լ�(���� ����Ʈ�� ����) ȣ��
	FTimerHandle tempTimer;
	GetWorldTimerManager().SetTimer(tempTimer, this, &ASkill_Mine_A::ActiveExplosion, DelayTime, false, DelayTime);
}

//�����ϴ½ñ⿡ ������ �ִ� ������ ����� �ο�
void ASkill_Mine_A::OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (hitted.Find(OtherActor) >= 0 && hitted.Num() > 0)
		return;

	UBuffComponent* buffComponent = Helpers::GetComponent<UBuffComponent>(OtherActor);
	if (!buffComponent)
		return;

	buffComponent->AddBuff(BuffClass);
	hitted.AddUnique(OtherActor);
}

//���� ��ƼŬ ������ �ı�
void ASkill_Mine_A::OnFinishParticle(class UParticleSystemComponent* PSystem)
{
	Destroy();
}

//�������� ��ƼŬ ȣ��`
void ASkill_Mine_A::ActiveExplosion()
{
	ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BaseParticleComponent->DestroyComponent(true);
	ExplosionParticleComponent->SetVisibility(true);
	ExplosionParticleComponent->SetTemplate(ExplosionParticle);
}

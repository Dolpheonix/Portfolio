#include "MainCharacter.h"

void AMainCharacter::Fist()
{

}

void AMainCharacter::Fire()
{
	if (ProjectileClass)
	{
		if (FireAudio)
		{
			FireAudio->Play();
		}
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(Muzzle); // muzzle location을 camera space에서 world space로 변환
		FRotator MuzzleRotation = CameraRotation;

		MuzzleRotation.Pitch += 10.0f; // 살짝 위쪽을 조준
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator(); // instigator : spawn을 trigger한 주체
			ABullet* Projectile = World->SpawnActor<ABullet>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams); // world에 actor를 스폰
			if (Projectile)
			{
				FVector LaunchDirection = MuzzleRotation.Vector(); // 총구 방향
				Projectile->FireInDirection(LaunchDirection); // 발사체 velocity 결정
			}
		}

	}
}
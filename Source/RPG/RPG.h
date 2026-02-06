#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"


#define DRAW_SPHERE(actorLoc) if(GetWorld()) DrawDebugSphere(GetWorld(), actorLoc, 30.f, 16, FColor::Red, true);
#define DRAW_SPHERE_TEMP(actorLoc, color) if(GetWorld()) DrawDebugSphere(GetWorld(), actorLoc, 7.f, 16, color, false, 5.f);
#define DRAW_SPHERE_SINGLE_FRAME(actorLoc) if(GetWorld()) DrawDebugSphere(GetWorld(), actorLoc, 30.f, 16, FColor::Red, false);
#define DRAW_LINE(actorLoc) if(GetWorld()) DrawDebugLine(GetWorld(), actorLoc, actorLoc + FVector(100.f, 0.f, 0.f), FColor::Red, true);
#include "MyEStates.h"

void PrintMyVector(FVector v, FString txt, float time, FColor color) {
	float x = v.X;
	float y = v.Y;
	float z = v.Z;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, time, color, txt + FString::Printf(TEXT(" : [ %f ] [ %f ] [ %f ]"), x, y, z), false); }
}

void PrintMyVector(FVector v1, FVector v2, FString txt1, FString txt2, float time, FColor color) {
	float x1 = v1.X;
	float y1 = v1.Y;
	float z1 = v1.Z;

	float x2 = v2.X;
	float y2 = v2.Y;
	float z2 = v2.Z;
	FString res = "";
	res += txt1 + FString::Printf(TEXT("\t : [ %f ] [ %f ] [ %f ]\n"), x1, y1, z1);
	res += txt2 + FString::Printf(TEXT("\t : [ %f ] [ %f ] [ %f ]\n"), x2, y2, z2);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, time, color, res, false); }
}

void PrintMyVector(FVector v1, FVector v2, FVector v3, FString txt1, FString txt2, FString txt3, float time, FColor color) {
	float x1 = v1.X;
	float y1 = v1.Y;
	float z1 = v1.Z;

	float x2 = v2.X;
	float y2 = v2.Y;
	float z2 = v2.Z;

	float x3 = v3.X;
	float y3 = v3.Y;
	float z3 = v3.Z;

	FString res = "";
	res += txt1 + FString::Printf(TEXT("\t : [ %f ] [ %f ] [ %f ]\n"), x1, y1, z1);
	res += txt2 + FString::Printf(TEXT("\t : [ %f ] [ %f ] [ %f ]\n"), x2, y2, z2);
	res += txt3 + FString::Printf(TEXT("\t : [ %f ] [ %f ] [ %f ]\n"), x3, y3, z3);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, time, color, res, false); }
}
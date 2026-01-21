// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameMode.h"
#include "SpartaPawn.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ASpartaPawn::StaticClass();
}

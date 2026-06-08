# Instructions

## Background

This is a Skyrim SKSE mod called Apprentice that adds new elements to the ScaleForm UI. The mod uses SKSE to interact with and change the RaceSexMenu, which contains player creation options, to allow for choosing new Classes and Traits.

This mod also provides support for the popular RaceMenu mod, the files of which can be found under `skyui`. RaceMenu overwrites the RaceSexMenu with its own Scaleform system and custom Actionscript based on SkyUI.

The SKSE in this mod is based off of reverse engineering from https://github.com/alandtse/CommonLibVR

## Goals

This SKSE mod aims to add Class and Trait as categories to the RaceSexMenu, with selection menus under those categories that allow the player to select a Class and Trait. In providing support for RaceMenu, this mod *MUST NOT* make changes to the RaceMenu files -- we do not have permissions to modify the files and re-release. Instead, this project takes a SKSE-injection approach to modify the RaceMenu Scaleform at runtime.
So im leaving hacks developing coz i wanted to update my ipad to ios 26 and i decide to share this source. 

Game: PUBG MOBILE
Engine: UE 4.23

Cheat features: ESP, AIM, items ESP, grenade warning.
Work on jailbreak/non jailbreak both.
Requirements: Theos, SDK 15.6+

Main file structure:

crossoffsets.mm - contains function for cross-supporting of different game versions (Global, Vietnam, Korea, Japan). Function checks app's bundle ID and choosing right offsets.
ImGuiDrawView.mm - contains some ImGui setup, menu and esp drawing based on ImGui, functions for working with textures.
functions/ESP/main.h - in fact this is main file of hack, contains ESP and AIM logic and a few structs
functions/ESP/mem.h - contains memory functions(read, write, address validation.
functions/ESP/structs.h - contains all hack structs and some fucntions fpr esp logic, i bet you're not interested in it.
functions/ESP/var.h - just variables.

If you still have any questions about cheat you can contact me in telegram @jailbreaked or ask in chat @hehipa


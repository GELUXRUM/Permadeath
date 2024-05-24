#pragma once

#include "Utils.h"

extern RE::PlayerCharacter* player;
extern bool filthyCasualMode;

namespace Events
{
	class TESDeathEventWatcher : public RE::BSTEventSink<RE::TESDeathEvent>
	{
		virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent& evn, RE::BSTEventSource<RE::TESDeathEvent>*) override
		{
			if (evn.actorDying.get() == player && player->IsDead(false) == false) {
				RE::BGSSaveLoadManager* manager = RE::BGSSaveLoadManager::GetSingleton();

				// get the save location for later
				char saveDirectoryPath[260];
				manager->GetSaveDirectoryPath(saveDirectoryPath);

				// build a list of all saves that belong to this character
				std::unordered_set<std::string> saveNames;
				std::vector<RE::BGSSaveLoadFileEntry*> savePtrs;

				std::string playerFileName;
				std::string playerName;

				// go through the saves and populate the lists
				manager->BuildSaveGameList(manager->currentPlayerID);
				for (RE::BGSSaveLoadFileEntry* savePtr : manager->saveGameList) {
					// get their metadata to access their PlayerIDs
					RE::SavefileMetadata metadata;
					metadata.FillDataFromFileName(savePtr->fileName);

					// check if this save file belongs to the current character
					if (metadata.PlayerID == manager->currentPlayerID) {
						// populate the name used for the character's .txt file
						if (playerFileName.empty()) {
							playerFileName += metadata.PlayerName.c_str();
							playerFileName += "_";
							playerFileName += std::to_string(metadata.PlayerID);
						}

						if (playerName.empty()) {
							playerName = metadata.PlayerName.c_str();
						}

						// the name will be used to copy the save files to a different folder
						saveNames.insert(savePtr->fileName);
						// the save ptr itself will be used to delete the files safely
						savePtrs.push_back(savePtr);
					}
				}

				
				// once we have the lists we can now mess with the files
				// we must copy both the .fos and .f4se files
				constexpr const char* saveCopyPath = R"(Data\Permadeath\)";

				// create the directory for the player
				Utils::CreateDir(saveCopyPath + playerFileName);

				Utils::WriteTextFile(saveCopyPath + playerFileName, playerName, evn.actorKiller.get());

				if (filthyCasualMode) {
					// make sure we have saves to work with
					if (saveNames.size() != 0) {
						for (std::string saveName : saveNames) {
							// copy the save
							Utils::CopyFileToDir(saveDirectoryPath + saveName + ".fos", saveCopyPath + playerFileName);
							// copy the co-save created by F4SE
							Utils::CopyFileToDir(saveDirectoryPath + saveName + ".f4se", saveCopyPath + playerFileName);
						}

						logger::warn("---------------------");
					}
				}

				for (auto savePtr : savePtrs) {
					logger::warn("Deleting {}", savePtr->fileName);
					savePtr->DeleteGame();
				}

				logger::warn("---------------------------------------------------------------------");
			}

			return RE::BSEventNotifyControl::kContinue;
		}
	};
}

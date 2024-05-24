#pragma once

#include <iostream>
#include <fstream>

extern RE::PlayerCharacter* player;

namespace Utils
{
	struct timeStruct
	{
		timeStruct()
		{
			years = 0;
			months = 0;
			days = 0;
			hours = 0;
			minutes = 0;
		}

		int years;
		int months;
		int days;
		int hours;
		int minutes;

		operator bool() const
		{
			return years != 0 || months != 0 || days != 0 || hours != 0 || minutes != 0;
		}
	};

	void CreateDir(const std::string path)
	{
		std::filesystem::path dirPath(path);

		// check if the directory already exists
		if (std::filesystem::exists(dirPath)) {
			logger::warn("Directory already exists: {}", dirPath.string());

			logger::warn("---------------------");

			return;
		}

		if (std::filesystem::create_directories(dirPath)) {
			logger::warn("Directory successfully created: {}", dirPath.string());
		} else {
			logger::warn("Failed to create directory: {}", dirPath.string());
		}

		logger::warn("---------------------");

		return;
	}

	void CopyFileToDir(const std::string sourceFilePath, const std::string destDirPath)
	{
		std::filesystem::path sourcePath(sourceFilePath);
		std::filesystem::path destDir(destDirPath);

		if (!std::filesystem::exists(sourcePath) || !std::filesystem::is_regular_file(sourcePath)) {
			logger::warn("Source file does not exist or is not a file: {}", sourceFilePath);
			return;
		}

		if (!std::filesystem::exists(destDir)) {
			std::filesystem::create_directories(destDir);
		}

		std::filesystem::path destPath = destDir / sourcePath.filename();

		std::filesystem::copy(sourcePath, destPath, std::filesystem::copy_options::overwrite_existing);
		logger::warn("Copied {}", sourceFilePath);
	}

	timeStruct ConvertHoursToTime(float hours)
	{
		timeStruct result;

		constexpr int minutesPerHour = 60;
		constexpr int minutesPerDay = 24 * minutesPerHour;
		constexpr int minutesPerMonth = 30 * minutesPerDay;
		constexpr int minutesPerYear = 12 * minutesPerMonth;

		float remainingMinutes = hours * minutesPerHour;

		result.years = static_cast<int>(std::floor(remainingMinutes / minutesPerYear));
		remainingMinutes -= result.years * minutesPerYear;

		result.months = static_cast<int>(std::floor(remainingMinutes / minutesPerMonth));
		remainingMinutes -= result.months * minutesPerMonth;

		result.days = static_cast<int>(std::floor(remainingMinutes / minutesPerDay));
		remainingMinutes -= result.days * minutesPerDay;

		result.hours = static_cast<int>(std::floor(remainingMinutes / minutesPerHour));
		remainingMinutes -= result.hours * minutesPerHour;

		result.minutes = static_cast<int>(remainingMinutes);

		return result;
	}

	void WriteTextFile(const std::string a_filePath, std::string a_playerName, RE::TESObjectREFR* a_killer)
	{
		// workaround for random Obituary.txt that gets created
		if (a_playerName.empty() == false) {
			std::ofstream txtFile;

			std::string fullPath = a_filePath + "\\Obituary.txt";

			// clear the file if it already exists
			// player is a filthy casual who restored deleted saves
			txtFile.open(fullPath, std::ofstream::out | std::ofstream::trunc);

			if (!txtFile) {
				logger::warn("Couldn't create Obituary.txt file!");
				return;
			}

			int statValue{ 0 };
			RE::BSFixedString statString{ "Did you ever hear the tragedy of Darth Plagueis the Wise?" };
			RE::Calendar* calendar = RE::Calendar::GetSingleton();

			txtFile << "=== Fallout 4 Permadeath Obituary ===" << std::endl;

			txtFile << "\n";

			txtFile << "Player Name: " << a_playerName << std::endl;

			txtFile << "Level: " << player->GetLevel() << std::endl;

			if (a_killer) {
				txtFile << "Killed by: " << a_killer->GetDisplayFullName() << std::endl;
			}

			if (player->currentLocation) {
				txtFile << "Death Location: " << player->currentLocation->GetFullName() << std::endl;
			}

			txtFile << "Time of Death: " << calendar->gameDay->GetValue() << "/" << calendar->gameMonth->GetValue() << "/" << calendar->gameYear->GetValue() + 2000.0f << std::endl;
			txtFile << "Game Time Survived: ";

			timeStruct timeSurvived = ConvertHoursToTime(calendar->GetHoursPassed());

			if (timeSurvived.years != 0) {
				txtFile << timeSurvived.years << " years ";
			}

			if (timeSurvived.months != 0) {
				txtFile << timeSurvived.months << " months ";
			}

			if (timeSurvived.days != 0) {
				txtFile << timeSurvived.days << " days ";
			}

			if (timeSurvived.hours != 0) {
				txtFile << timeSurvived.hours << " hours ";
			}

			if (timeSurvived.minutes != 0) {
				txtFile << timeSurvived.minutes << " minutes ";
			}

			if (timeSurvived) {
				txtFile << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Chems ---" << std::endl;

			statString = "Chems Taken";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Chems Taken: " << statValue << std::endl;
			}

			statString = "Times Addicted";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Times Addicted: " << statValue << std::endl;
			}

			statString = "Stimpaks Taken";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Stimpaks Taken: " << statValue << std::endl;
			}

			statString = "Rad-X Taken";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Rad-X Taken: " << statValue << std::endl;
			}

			statString = "RadAway Taken";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "RadAway Taken: " << statValue << std::endl;
			}

			statString = "Food Eaten";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Food Eaten: " << statValue << std::endl;
			}

			statString = "Fusion Cores Consumed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Fusion Cores Consumed: " << statValue << std::endl;
			}

			statString = "Corpses Eaten";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Corpses Eaten: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Combat ---" << std::endl;

			statString = "People Killed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "People Killed: " << statValue << std::endl;
			}

			statString = "Animals Killed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Animals Killed: " << statValue << std::endl;
			}

			statString = "Creatures Killed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Creatures Killed: " << statValue << std::endl;
			}

			statString = "Robots Killed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Robots Killed: " << statValue << std::endl;
			}

			statString = "Synths Killed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Synths Killed: " << statValue << std::endl;
			}

			statString = "Turrets Killed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Turrets Destroyed: " << statValue << std::endl;
			}

			statString = "Critical Strikes";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Critical Strikes: " << statValue << std::endl;
			}

			statString = "Sneak Attacks";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Sneak Attacks: " << statValue << std::endl;
			}

			statString = "Backstabs";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Backstabs: " << statValue << std::endl;
			}

			statString = "Fatman Deaths";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Fatman Deaths: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Crafting ---" << std::endl;

			statString = "Weapons Improved";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Weapons Improved: " << statValue << std::endl;
			}

			statString = "Armor Improved";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Armor Improved: " << statValue << std::endl;
			}

			statString = "Chems Crafted";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Chems Cooked: " << statValue << std::endl;
			}

			statString = "Food Cooked";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Food Cooked: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Crime ---" << std::endl;

			statString = "Locks Picked";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Locks Picked: " << statValue << std::endl;
			}

			statString = "Pockets Picked";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Pockets Picked: " << statValue << std::endl;
			}

			statString = "Computers Hacked";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Computers Hacked: " << statValue << std::endl;
			}

			statString = "Items Stolen";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Items Stolen: " << statValue << std::endl;
			}

			statString = "Assaults";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Assaults: " << statValue << std::endl;
			}

			statString = "Murders";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Murders: " << statValue << std::endl;
			}

			statString = "Trespasses";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Trespasses: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Exploration ---" << std::endl;

			statString = "Locations Discovered";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Locations Discovered: " << statValue << std::endl;
			}

			statString = "Dungeons Cleared";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Dungeons Cleared: " << statValue << std::endl;
			}

			statString = "Hours Slept";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Hours Slept: " << statValue << std::endl;
			}

			statString = "Hours Waiting";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Hours Waiting: " << statValue << std::endl;
			}

			statString = "Speech Successes";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Speech Successes: " << statValue << std::endl;
			}

			statString = "Mines Disarmed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Mines Disarmed: " << statValue << std::endl;
			}

			statString = "Robots Disabled";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Robots Disabled: " << statValue << std::endl;
			}

			statString = "Plants Harvested";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Plants Harvested: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Loot ---" << std::endl;

			txtFile << "Caps Held: " << player->GetGoldAmount() << std::endl;

			statString = "Most Caps Carried";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Most Caps Carried: " << statValue << std::endl;
			}

			statString = "Caps Found";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Caps Found: " << statValue << std::endl;
			}

			statString = "Junk Collected";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Junk Collected: " << statValue << std::endl;
			}

			statString = "Chests Looted";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Containers Looted: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Perks ---" << std::endl;

			statString = "Skill Books Read";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Skill Books Read: " << statValue << std::endl;
			}

			statString = "Bobbleheads Collected";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Bobbleheads Collected: " << statValue << std::endl;
			}

			statString = "Intimidations";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Intimidations: " << statValue << std::endl;
			}

			statString = "Animals Friended";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Animals Friended: " << statValue << std::endl;
			}

			statString = "Wasteland Whispers";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Wasteland Whispers: " << statValue << std::endl;
			}

			statString = "Grim Reaper Sprints";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Grim Reaper Sprints: " << statValue << std::endl;
			}

			statString = "Four Leaf Clovers";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Four Leaf Clovers: " << statValue << std::endl;
			}

			statString = "Weapons Disarmed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Weapons Disarmed: " << statValue << std::endl;
			}

			statString = "Grand Slams";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Grand Slams: " << statValue << std::endl;
			}

			statString = "Sandman Kills";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Sandman Kills: " << statValue << std::endl;
			}

			statString = "Mysterious Stranger Visits";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Mysterious Stranger Visits: " << statValue << std::endl;
			}

			statString = "Paralyzing Punches";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Paralyzing Punches: " << statValue << std::endl;
			}

			statString = "Pants Exploded";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Pants Exploded: " << statValue << std::endl;
			}

			statString = "Bright Ideas";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Bright Ideas: " << statValue << std::endl;
			}

			statString = "Investments Made";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Investments Made: " << statValue << std::endl;
			}

			statString = "Fits of Rage";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Fits of Rage: " << statValue << std::endl;
			}

			statString = "Money Shots";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Money Shots: " << statValue << std::endl;
			}

			statString = "Ricochets";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Ricochets: " << statValue << std::endl;
			}

			statString = "Cores Ejected";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Cores Ejected: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Quests ---" << std::endl;

			statString = "Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Quests Completed: " << statValue << std::endl;
			}

			statString = "Main Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Main Quests Completed: " << statValue << std::endl;
			}

			statString = "Side Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Side Quests Completed: " << statValue << std::endl;
			}

			statString = "Misc Objectives Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Misc Objectives Completed: " << statValue << std::endl;
			}

			statString = "Brotherhood of Steel Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Brotherhood of Steel Quests Completed: " << statValue << std::endl;
			}

			statString = "Institute Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Institute Quests Completed: " << statValue << std::endl;
			}

			statString = "Minutemen Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Minutemen Quests Completed: " << statValue << std::endl;
			}

			statString = "Railroad Quests Completed";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Railroad Quests Completed: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Workshops ---" << std::endl;

			statString = "Workshops Unlocked";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Workshops Unlocked: " << statValue << std::endl;
			}

			statString = "Items Scrapped";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Items Scrapped: " << statValue << std::endl;
			}

			statString = "Objects Built";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Objects Built: " << statValue << std::endl;
			}

			statString = "Supply Lines Created";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Supply Lines Created: " << statValue << std::endl;
			}

			statString = "People";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "People: " << statValue << std::endl;
			}

			statString = "Buildings";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Buildings: " << statValue << std::endl;
			}

			statString = "Power";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Power: " << statValue << std::endl;
			}

			statString = "Food";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Food: " << statValue << std::endl;
			}

			statString = "Water";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Water: " << statValue << std::endl;
			}

			statString = "Happiness";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Happiness: " << statValue << std::endl;
			}

			txtFile << "\n";

			txtFile << "--- Video Game High-Scores ---" << std::endl;

			statString = "HRedMenace";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Red Menace: " << statValue << std::endl;
			}

			statString = "HSAtomicCommand";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Atomic Command: " << statValue << std::endl;
			}

			statString = "HSZetaInvaders";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Zeta Invaders: " << statValue << std::endl;
			}

			statString = "HSPipfall";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Pipfall: " << statValue << std::endl;
			}

			statString = "HSGrognak";
			RE::MiscStatManager::QueryStat(&statString, &statValue);
			if (statValue != 0) {
				txtFile << "Grognak & the Ruby Ruins: " << statValue << std::endl;
			}

			txtFile << "Fallout 4: 0" << std::endl;

			txtFile.close();
		}
	}
}

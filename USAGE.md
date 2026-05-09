# First Launch

First, you'll need to specify your Rocket League installation folder:

- **Windows**: Default is `C:\Program Files\Epic Games\rocketleague`
- **Linux**: Default is `~/Games/Heroic/rocketleague`

If the default is not correct, type or paste in the correct path directly and press **Confirm**

If the path is correct, the rest of the UI will be visible and you can proceed to add custom maps.

# Adding custom maps

Drag-and-drop `.udk` or `.upk` files directly into the app window.

Do not drag-and-drop folders, `.zip`/`.rar` files etc. or any other file types. They will be rejected by the application.

Drag-and-dropping the map files is the only way to add maps to the list. Do not place them in the Rocket League install folder or any other folder manually, as the app will not recognize them.

# Loading a custom map

1. Click on a map from the "Custom Maps" list
2. Click the "Load Selected Map" button.

# Loading custom map in-game

1. Launch Rocket League

> You should be able to launch it with Anti-Cheat (EAC), but just in case, make sure to launch it without EAC enabled.
  I am not responsible for any issues caused by launching with EAC, so proceed with caution if you choose to do so.

2. Open free-play training and select map **Underpass - Soccar**

Technically, you can load maps while the game is running, provided the Underpass map is not currently loaded.

But to avoid any issues, it's recommended to load the custom map before launching the game and not change it while the game is running.

# Reverting to original Underpass map

Click "Restore Original" to revert to the original Underpass map.

# Removing maps from the list

Select a map and click "Remove selected" to delete it. This will not affect the original file you dragged into the app (eg. in the Downloads folder) - a copy was created beforehand.

# Troubleshooting

- **"Invalid Rocket League installation path"**: Make sure the path points to your Rocket League install folder and contains the `TAGame/CookedPCConsole` directory

- **Permission denied errors**: On Windows, you can try running the app as administrator. On Linux, make sure your user has the necessary permissions for the install directory.

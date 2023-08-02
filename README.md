
# DIAVOLO : Multiplayer Boss Battles
## **Table Of Contents**
- [Main Menu & Lobby](#main-menu-and-lobby)
- [Movement](#movement-system-top-down)
- [Character Controls](#controls)
- [Character States](#character-states)
- [Characters](#characters)
	- [The Beast (2099)](#the-beast-spiderman-2099-reimagined)
	- [Micheal Moss](#micheal-moss--weapon-master)
- [Crimson Queen Boss](#crimson-queen-the-first-boss)
- [Copyright](#copyright--music-used)

# Main Menu and Lobby
This game uses a simple lobby system where the player sets thier username at the start of the game and is able to host or join from that menu.
> This game also uses *Listen Servers* Where The Hosting Player Is The Server and A Client. If Hosting, Host On Port 7777 // Port Forward On Port 7777

Once the player either joins or hosts a game `Can Join Locally [LAN] By Using 127.0.0.1 as IP Address`, The Server waits for all players to select a character, this is also indicated by the icon to the right of the players name *Left Side Of The Lobby Screen.* Once all players choose a character, the game would then wait for players to vote on a boss to fight together. Then load up the game.
  

# Movement System (Top Down)

The Movement in this game is a bit sketch. Basically The *Player Controller* Controls A Pawn That Was Spawned For The Player That Controls Another Pawn (**Server Pawn**) That Holds All The Abilities And Animations. The **SERVER** Pawn Is Controlled By An AI Controller In Order To Use The "Move To" Function. This Allows The Player To Move Their Character (TOP-DOWN) While Replicating To The Server

# Controls
In game controls take a mix from **Diablo** and **League of Legends**
**Movement:** To move the Character, You hold or press **LMB** `Left Mouse Button` Where You Want To Move.
Additionally The Player Has A ***Dodge Roll*** Ability Where The Player Gains *I-Frames* `Invisibility Frames` During 80% of the Roll.
The Players Also Have 4 Abilities, 3 Basic Abilities **(Q W E)** and an Ultimate Ability, **R**.

**Full Control List:**
| Q  L-Shift  E  | F | WASD | SPACE | Mouse | G | Scroll Wheel |
|---|---|---|---|---|---|---|
| Basic Abilities | Ultimate Ability | Movement | Dodge | Aiming | Emote | Camera Zoom|

| L-Bumper  L-Trigger R-Bumper  | R-Trigger | L-Stick | A // X // Face Button Down | R-Stick | Left Stick Click // Right Stick Click | D-Pad Up // D-Pad Down |
|---|---|---|---|---|---|---|
| Basic Abilities | Ultimate Ability | Movement | Dodge | Aiming | Emote | Camera Zoom|

  

# Character States

The Player has a few states that they can be in currently:

**E_IDLE, E_MOVE, E_MOVE_ATTACK, E_ATTACK_WINDUP,
E_ATTACK_FULL, E_ABILITY**

  

Each of these states determine what the player is currently doing.

IE: If the player is moving to a point on the map, their state would be

E_MOVE.

  

# Characters

  

## **The Beast** Spiderman 2099 ReImagined

**The Beast** is a mock recreation of Spiderman 2099 from the hit movie *Spiderman: Across The Spiderverse*. In this game, **The Beast** is a high damage, low health, burst assassin who plays an in-and-out playstyle meaning the player should go in, deal high damage, and get out.

### Abilities
|![Shadow Claws](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_Q.png)|![Demonic Spin](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_W.png)|![Grapple](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_E.png)|![True Power](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_R.png)|
|---|---|---|---|
|The Beast Sends Out Shadow Claws In Aimed Direction (Mouse) That Will Damage Enemies. ReActivating The Key Will Teleport The User To The Claws If Claws Have Not Hit Enemy|The Beast Starts Spinning At High Speeds Damaging Enemies All Around Them Additionally Granting `Invisibility` During The Duration|The Beast Sends Out A Grapple Hook, If Hits Wall Or Enemy, The Beast Pulls Himself Towards The Hook|The Beast Releases 4 Spinning Blades That Rotate Around Them Damaging All Enemies It comes In Contact With|

## **Micheal Moss**  Weapon Master
Former Ex-Military now Gun For Hire, Micheal Moss is a playable ranged character in Diavolo. His play style is semi-ranged, the player should keep thier distance from enemies but close enough to be in range.

### Abilities
|![Rocket-Propelled Grenade](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_Q.png)|![Combat Stim](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_W.png)|![Combat Shotty](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_E.png)|![Air Strike](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_R.png)|
|---|---|---|---|
|Moss Pulls His Rocket Launcher From His Back, and Launches An Explosive That Deal Massive Damage|While Holding This Ability, Moss Uses This Stim To Heal Himself Constently|Moss Equips His Shotgun And Shoots In A Large Arc Aimed At The Mouse|Moss Calls An Airstrike At Mouse Location |

# Crimson Queen: The First Boss
Crimson Queen is a the broke-mans version of Lilith from [Diablo IV](https://diablo4.blizzard.com/en-us/). A Demon from the burring pits of Hell, Crimson Queen sends her foes back to Hell where she can endlessly control them from thier mind, to thier body.
As a character, Crimson Queen uses a ton of projectiles and beam attacks. When fighting her, Id recommend using [**The Beast**](#the-beast-spiderman-2099-reimagined) as his Ultimate ***Razor Blossom*** can take care of ***Crimson Queens Hounds***, speaking of ***Crimson Hounds*** Here are all of **Crimson Queen's** Abilities:
| **Crimson Hounds** |  **Hells Pit** |  **Deadly Scythes** |  **Crimson Blast** |  **FINAL STRIKE** |
|---|---|---|---|---|
| **Crimson Queen** Summons *6 to 12* **Crimson Hounds** to bite the Player | **Crimson Queen** Summons A *Magic Circle* That Unleashes <1% of Hells Energy *(Vertical Attack)* | **Crimson Queen** Spins Her Scythes In Her Hand | **Crimson Queen** Summons A *Magic Circle* Unleashing A Hellish Beam Spinning And Dealing Damage | **Crimson Queen** Summons A *Large Beam* From The Center Of The Map Turning Clockwise Dealing ***MASSIVE DAMAGE*** `UnDodgeable` |

Now that you understand Crimson Queens Attacks, you can try and beat her. Good Luck Gamers!

## Copyright & Music Used
-   [Diablo IV Main Menu Theme](https://www.youtube.com/watch?v=5Eae_WdwxVA&ab_channel=Ceburekas)
-   [Super Smash Bros Melee Character Select Theme](https://www.youtube.com/watch?v=0q_MSMUEhoU&ab_channel=VideoGameSoundtracks)
-   [NOOT NOOT Boss Theme](https://www.youtube.com/watch?v=9X7WME84NZo&t=9s&ab_channel=Carameii)
- [Micheal Moss Theme](https://www.tiktok.com/@iamtresorr/video/7250566605086706950)
- [The Beast Theme](https://www.youtube.com/watch?v=_Qq1B5na--s&ab_channel=Anzkuki)

## Copyright
If anyone wants to use this games code or anything like that, please read the [license](https://github.com/ChezyName/DIAVOLO/blob/main/LICENSE).


#### Copyright © 2023 ChezyName

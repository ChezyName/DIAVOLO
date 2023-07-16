
# DIAVOLO : Multiplayer Boss Battles

Diablo IV Came Out A Few Days, So I Decided To Remake The Game As A Dungeon Crawler. Think Of League of Legends x Binding Of Issac.

# Main Menu & Lobby
This game uses a simple lobby system where the player sets thier username at the start of the game and is able to host or join from that menu.
> This game also uses *Listen Servers* Where The Hosting Player Is The Server and A Client. If Hosting, Host On Port 7777 // Port Forward On Port 7777
  

# Movement

The Movement in this game is a bit sketch. Basically The *Player Controller* Controls A Pawn That Was Spawned For The Player That Controls Another Pawn (**Server Pawn**) That Holds All The Abilities And Animations. The **SERVER** Pawn Is Controlled By An AI Controller In Order To Use The "Move To" Function. This Allows The Player To Move Their Character (TOP-DOWN) While Replicating To The Server

  

# Character States

The Player has a few states that they can be in currently:

*E_IDLE, E_MOVE, E_MOVE_ATTACK, E_ATTACK_WINDUP,

E_ATTACK_FULL, E_ABILITY*

  

Each of these states determine what the player is currently doing.

IE: If the player is moving to a point on the map, their state would be

E_MOVE.

  

# Characters

  

## **The Beast** : Spiderman 2099 ReImagined

**The Beast** is a mock recreation of Spiderman 2099 from the hit movie *Spiderman: Across The Spiderverse*. In this game, **The Beast** is a high damage, low health, burst assassin who plays an in-and-out playstyle meaning the player should go in, deal high damage, and get out.

### Abilities
|![Shadow Claws](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_Q.png)|![Demonic Spin](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_W.png)|![Grapple](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_E.png)|![True Power](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/2099_R.png)|
|---|---|---|---|
|The Beast Sends Out Shadow Claws In Aimed Direction (Mouse) That Will Damage Enemies. ReActivating The Key Will Teleport The User To The Claws If Claws Have Not Hit Enemy|The Beast Starts Spinning At High Speeds Damaging Enemies All Around Them|The Beast Sends Out A Grapple Hook, If Hits Wall Or Enemy, The Beast Pulls Himself Towards The Hook|The Beast Releases 4 Spinning Blades That Rotate Around Them Damaging All Enemies It omes In Contact With|

## **Micheal Moss** : Weapon Master
Former Ex-Military now Gun For Hire, Micheal Moss is a playable ranged character in Diavolo. His play style is semi-ranged, the player should keep thier distance from enemies but close enough to be in range.

### Abilities
|![Rocket-Propelled Grenade](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_Q.png)|![Combat Stim](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_W.png)|![Combat Shotty](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_E.png)|![Air Strike](https://raw.githubusercontent.com/ChezyName/DIAVOLO/main/GitImages/Moss_R.png)|
|---|---|---|---|
|Moss Pulls His Rocket Launcher From His Back, and Launches An Explosive That Deal Massive Damage|While Holding This Ability, Moss Uses This Stim To Heal Himself Constently|Moss Equips His Shotgun And Shoots In A Large Arc Aimed At The Mouse|Moss Calls An Airstrike At Mouse Location |

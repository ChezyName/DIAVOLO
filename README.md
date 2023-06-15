# DIAVOLO : Multiplayer Dungeon Crawler
Diablo IV Came Out A Few Days, So I Decided To Remake The Game As A Dungeon Crawler. Think Of League of Legends x Binding Of Issac.

# Movement
The Movement in this game is a bit sketch. Basically The *Player Controller* Controls A Pawn That Was Spawned For The Player That Controls Another Pawn (**Server Pawn**) That Holds All The Abilities And Animations. The **SERVER** Pawn Is Controlled By An AI Controller In Order To Use The "Move To" Function. This Allows The Player To Move Their Character (TOP-DOWN) While Replicating To The Server

# Character States
The Player has a few states that they can be in currently: 
*E_IDLE, E_MOVE, E_MOVE_ATTACK, E_ATTACK_WINDUP, 
E_ATTACK_FULL, E_ABILITY*

Each of these states determine what the player is currently doing.
IE: If the player is moving to a point on the map, thier state would be
E_MOVE.


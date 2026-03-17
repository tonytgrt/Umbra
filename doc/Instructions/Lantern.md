## Lantern — Editor Guide

### What It Is

`AUmbraLantern` is a player-draggable point light source that casts real-time shadows. Players click and drag the lantern to reposition it, creating or shifting shadows used to solve puzzles.

---

### 1. Place the Lantern Actor

1. Open the **Place Actors** panel (press `A` or go to **Window → Place Actors**).
2. In the search bar, type `UmbraLantern`.
3. **Drag** `UmbraLantern` from the list into the viewport where you want the lantern's starting position.

### 2. Position the Actor

- Use the **Move** tool (W) to place the lantern at the desired location.
- During gameplay the lantern's **Z height locks on drag start** — the player can only move it on the **XY plane**. Place it at the Z height you want it to stay at.

### 3. Default Properties

| Property | Default Value | Notes |
|---|---|---|
| **Intensity** | 5000 | Point light brightness |
| **Attenuation Radius** | 1000 UU | How far the light reaches |
| **Casts Shadows** | ☑ Enabled | Must stay enabled for shadow puzzles |
| **Collision Sphere Radius** | 40 UU | Controls the click-target size for player interaction |

### 4. Player Interaction

- The player **clicks and holds the left mouse button** on the lantern to grab it.
- While held, the lantern follows the cursor on the **XY plane** (Z stays fixed).
- Releasing the mouse button drops the lantern at its new position.

### 5. Light Subsystem Registration

The lantern **auto-registers** with `UUmbraLightSubsystem` at play start — no manual wiring is needed. The subsystem uses the lantern's light data to compute shadow detection for the pawn and shadow bridges.

### 6. Tuning Tips

- **Attenuation Radius** controls how far the light reaches. A larger radius means shadows are cast over a wider area, which affects the detection range for both the pawn's shadow state and any nearby `UmbraShadowBridge` actors.
- **Intensity** affects visual brightness but does not change shadow detection behavior.
- **Collision Sphere Radius** (40 UU default) determines how close the player's cursor must be to grab the lantern. Increase it if players have trouble clicking the lantern; decrease it for tighter precision.
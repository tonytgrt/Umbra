## Shadow Bridge — Editor Guide

### What It Is

`AUmbraShadowBridge` is an invisible walkable surface designed to span void gaps. The bridge's collision is only active while the player pawn is standing in shadow. If the player loses shadow coverage while on the bridge, they are safely glided back to the last solid ground.

---

### 1. Place the Shadow Bridge Actor

1. Open the **Place Actors** panel (press `A` or go to **Window → Place Actors**).
2. In the search bar, type `UmbraShadowBridge`.
3. **Drag** `UmbraShadowBridge` from the list into the viewport over the void gap you want to span.

### 2. Position the Actor

- Use the **Move** tool (W) to align the bridge with the adjacent ground surfaces.
- Set the bridge's **Z position flush with the ground** on either side of the gap so the player walks onto it seamlessly.

### 3. Scale to Fit the Gap

- Use the **Scale** tool (R) to stretch the bridge so it covers the entire void gap.
- The default box extent is **200×200×10 UU** (X × Y × Z). Scale X and Y to match the gap width and depth; keep Z thin.

### 4. Debug Visuals (Editor & PIE)

During **Play In Editor** the bridge draws debug information to help you verify placement:

| Visual | Meaning |
|---|---|
| **Red** outline | Bridge collision is **disabled** (player not in shadow) |
| **Green** outline | Bridge collision is **enabled** (player is in shadow) |
| **Purple dots** | Shadow coverage sample points on the bridge surface |

Use these visuals to confirm that your light/shadow setup actually covers the bridge area.

### 5. Respawn Behavior

If the player **loses shadow while standing on the bridge**:

1. A **shrink animation** plays on the pawn.
2. The pawn **glides back** to the last safe ground position.
3. A **grow-back animation** restores the pawn to normal size.

No additional configuration is needed — this is handled automatically by the bridge.

### 6. Example Setup

A typical shadow bridge puzzle requires four elements:

1. **Lantern** (`UmbraLantern`) — the light source the player can reposition.
2. **Shadow-casting wall** — any static mesh with **Cast Shadow** enabled (☑), placed between the lantern and the bridge.
3. **Void gap** — an opening in the ground geometry.
4. **Shadow bridge** — `UmbraShadowBridge` spanning the void gap, positioned in the shadow cast by the wall.

The player drags the lantern so the wall's shadow falls across the bridge, then walks over the gap while staying in shadow.

### 7. Save the Level

Press **Ctrl+S** to save.

## 8. Place and Configure a Pillar in the Editor

### 8.1 Open the Prototype Level

Open `Content/Levels/Level_Prototype.umap` in the editor.

### 8.2 Place the Pillar Actor

1. Open the **Place Actors** panel (press `A` or go to **Window → Place Actors**).
2. In the search bar, type `UmbraPillar`.
3. **Drag** `UmbraPillar` from the list into the viewport. Drop it on the ground platform where you want the pillar's **starting position** (stop 0).

### 8.3 Position the Actor

- Use the **Move** tool (W) to place the pillar exactly where you want it to start.
- Use the **Scale** tool (R) if you need to resize the placeholder cube. (You'll replace the mesh later.)
- The pillar's placed location in the world becomes the **origin** for all stop positions.

### 8.4 Configure Stop Positions

1. With the pillar selected, look at the **Details** panel on the right.
2. Find the **Umbra | Pillar** category.
3. Find the **Stop Positions** array. It will be empty.
4. Click the **+** button to add a new entry. Repeat for each stop you need.

**Important concept**: Each entry is a **relative offset** from the actor's placed position. So:
- Entry 0 = `(0, 0, 0)` means "the pillar's placed position" (starting spot).
- Entry 1 = `(300, 0, 0)` means "300 UU to the right of where you placed the actor".
- Entry 2 = `(300, 300, 0)` means "300 right and 300 forward".

**Visual positioning with MakeEditWidget:**

When you add entries to `StopPositions`, you'll see **diamond-shaped 3D widgets** appear in the viewport at each offset position. You can **click and drag** these widgets to reposition them visually. The vector values in the Details panel update automatically as you drag.

This is much easier than typing coordinates manually. Just drag the diamonds to where you want each stop to be.

**Example for a simple two-position pillar:**
| Index | Relative Offset | Meaning |
|---|---|---|
| 0 | `(0, 0, 0)` | Starting position (where you placed the actor) |
| 1 | `(400, 0, 0)` | 400 UU to the right |

**Example for a three-position pillar:**
| Index | Relative Offset | Meaning |
|---|---|---|
| 0 | `(0, 0, 0)` | Starting position |
| 1 | `(0, 300, 0)` | 300 UU forward |
| 2 | `(0, 300, 200)` | 300 UU forward and 200 UU up |

### 8.5 Set Slide Speed

- In the **Umbra | Pillar** category, find **Slide Speed**.
- Default is **400 UU/s**. Increase for faster sliding, decrease for slower.
- For reference: the pawn's `MaxWalkSpeed` is 400 UU/s, so 400 means the pillar slides at the same speed the player walks.

### 8.6 Replace the Placeholder Mesh (Optional)

The pillar spawns with a default cube. To use a real pillar mesh:

1. Select the pillar actor in the viewport.
2. In the **Details** panel, find **Pillar Mesh** (the `UStaticMeshComponent`).
3. Click the **Static Mesh** dropdown and search for one of:
   - `SM_Pillar_01` through `SM_Pillar_04` (from `FloatingIslandPack`)
   - Any mesh from the `Content/Pillar/Meshes/` folder
4. Adjust scale as needed.
5. Ensure **Cast Shadow** is still **checked** (☑) — this is essential for the game mechanic.

### 8.7 Save the Level

Press **Ctrl+S** to save.

---

## 9. Place and Configure a Pressure Plate in the Editor

### 9.1 Place the Pressure Plate Actor

1. In the **Place Actors** panel, search for `UmbraPressurePlate`.
2. **Drag** it into the viewport onto the ground surface where the player should walk over it.

### 9.2 Position the Plate

- Place it **flush with the ground**. Since the `TriggerBox` has a Z half-extent of 10 UU, the trigger volume extends 10 UU above and 10 UU below the actor's position. If your ground surface is at Z=0, place the plate at Z=0 (or slightly above so the visual mesh is visible).
- The plate's visual is a flattened cube (120x120x4 UU). It should look like a thin slab on the ground.

### 9.3 Adjust the Trigger Box Size (If Needed)

The default trigger box is 120x120x20 UU (from `SetBoxExtent(60, 60, 10)`). If this doesn't match your level's scale:

1. Select the pressure plate actor.
2. In the **Details** panel, find **Trigger Box** (the `UBoxComponent`).
3. Under **Shape**, adjust **Box Extent** (X, Y, Z half-extents).
   - Larger X/Y = wider trigger area on the ground.
   - Z should stay small (10–20) since it's a floor trigger.

### 9.4 Set the Plate Mode

In the **Details** panel, find **Umbra | PressurePlate** category:

- **Plate Mode** dropdown:
  - **Advance** (default): Each time the pawn walks over the plate, the linked pillar(s) advance to the next stop. Walking over again advances to the next stop after that, etc. After the last stop, wraps back to stop 0.
  - **Toggle**: First walk-over sends the pillar(s) to `ToggleTargetStopIndex`. Second walk-over sends them back to stop 0. Repeats.
  - **OneShot**: First walk-over advances the pillar(s) once. All subsequent walk-overs are ignored.

- **Toggle Target Stop Index** (only visible in Toggle mode): Which stop index to send pillars to on odd-numbered activations. Default is `1`.

---

## 10. Link the Pressure Plate to the Pillar

This is the critical step that connects the two actors:

1. Select the **pressure plate** actor in the viewport (or in the World Outliner).
2. In the **Details** panel, find **Umbra | PressurePlate → Linked Pillars**.
3. The array is empty. Click the **+** button to add a new entry.
4. A new row appears with an empty actor reference. **Assign it** using one of these methods:

   **Method A — Eyedropper (Recommended):**
   - Click the **eyedropper icon** next to the empty slot.
   - Click on the **pillar actor** in the viewport.
   - The slot fills with the pillar's name.

   **Method B — Dropdown:**
   - Click the dropdown on the empty slot.
   - Search for and select the `UmbraPillar` actor by name from the list.
   - (This list shows all actors of type `AUmbraPillar` in the level.)

5. To link **multiple pillars** to one plate, click **+** again and assign another pillar. All linked pillars will move simultaneously when the plate fires.

6. **Verify** the assignment: the linked pillars entry should show a valid actor name (not "None").

### Save Again

Press **Ctrl+S**.

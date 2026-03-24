## Lighthouse System — Editor Guide

The lighthouse system consists of three actors that work together: a **Battery** the player picks up, a **Drop-Off Zone** where the player delivers it, and a **Lighthouse** that activates when the battery is delivered. This guide explains how to place and configure each one.

---

### Overview

```
Player walks over BP_Battery
        │
        ▼
Battery shrinks & floats above player
        │
        ▼
Player walks into BP_Battery_Dropoff
        │
        ▼
Battery placed at drop-off zone ──► BP_Light_House activates
```

---

## 1. BP_Light_House

### Placing

1. In the **Content Browser**, navigate to `Content/` and find `BP_Light_House`.
2. **Drag** it into the viewport where you want the lighthouse to stand.
3. Use the **Move** (W) and **Rotate** (E) tools to position it. The spotlight emits from the bulb at the top.

### Fields — Umbra | Lighthouse

| Field | Default | Description |
|---|---|---|
| **Auto Activate** | `false` | If checked, the lighthouse turns on at the start of the level without needing a battery. Useful for debugging or for lighthouses that are always on. |
| **Light Intensity** | `8000` | Brightness of the spotlight beam. Higher values cast stronger, more defined shadows. |
| **Inner Cone Angle** | `22` | The angle (degrees) of the bright center of the spotlight cone. Objects inside this cone receive full illumination. |
| **Outer Cone Angle** | `30` | The angle (degrees) of the full cone including the soft falloff edge. Must be >= Inner Cone Angle. A larger gap between inner and outer creates softer shadow edges. |
| **Attenuation Radius** | `2000` | How far the spotlight beam reaches (in Unreal units). Objects beyond this distance are not lit and do not cast shadows from this light. |

### Fields — Umbra | Lighthouse | Rotation

Controls the spotlight beam sweeping back and forth (ping-pong) between two yaw angles.

| Field | Default | Description |
|---|---|---|
| **Enable Rotation** | `true` | Toggles the beam sweep on/off. When disabled, the spotlight points at a fixed angle (Start Yaw). |
| **Start Yaw** | `-45` | Starting yaw angle in degrees (relative to the actor's forward direction). The beam begins here and sweeps toward End Yaw. |
| **End Yaw** | `45` | Ending yaw angle in degrees. The beam reverses direction when it reaches this angle. |
| **Spot Pitch** | `-30` | Pitch angle of the beam (negative = angled downward). A value of `-30` means the light points 30 degrees below horizontal. Use `-90` for straight down. |
| **Rotation Speed** | `30` | How fast the beam sweeps, in degrees per second. At the default, a full 90-degree sweep takes 3 seconds. |

**Tip:** To make the beam sweep a full 180 degrees, set Start Yaw to `-90` and End Yaw to `90`. Adjust Rotation Speed to control difficulty — faster sweeps give the player less time to move through shadows.

### Fields — Umbra | Lighthouse | Translation

Controls the entire lighthouse actor physically moving back and forth between two positions.

| Field | Default | Description |
|---|---|---|
| **Enable Translation** | `false` | Toggles physical movement on/off. Off by default — most lighthouses are stationary. |
| **Translation Offset** | `(0, 0, 0)` | The end position as an **offset** from the lighthouse's placed location. For example, `(500, 0, 0)` means the lighthouse slides 500 units along the X axis from where you placed it, then slides back. |
| **Translation Speed** | `200` | Movement speed in units per second. |

**Tip:** Rotation and translation are independent — you can enable both for a lighthouse that sweeps its beam while sliding along a rail, or enable only translation for a lighthouse that moves but always points in one direction.

---

## 2. BP_Battery

### Placing

1. In the **Content Browser**, find `BP_Battery` under `Content/`.
2. **Drag** it into the viewport where you want the player to find it.
3. Position it on the ground where the player can easily walk over it.

### Behavior

- The battery **spins slowly** in place as a visual cue to the player.
- When the player's pawn overlaps the battery, it is **picked up automatically** — no button press required.
- While carried, the battery **shrinks and floats above the player**, spinning slowly as an indicator.
- The player **cannot pick up a second battery** while already carrying one.

### Components

| Component | Description |
|---|---|
| **PickupSphere** (root) | Sphere collision trigger (radius 50). The player picks up the battery by walking into this sphere. |
| **BatteryMesh** | The visual static mesh. Set this to your battery model in the Blueprint's Components panel. |

### Customizing the Mesh

1. Open `BP_Battery` in the Blueprint Editor.
2. Select `BatteryMesh` in the Components panel.
3. In the Details panel, set **Static Mesh** to your desired mesh asset.
4. Adjust **Relative Scale** and **Relative Location** so it looks correct relative to the trigger sphere.

---

## 3. BP_Battery_Dropoff

### Placing

1. In the **Content Browser**, find `BP_Battery_Dropoff` under `Content/`.
2. **Drag** it into the viewport near the lighthouse it should activate.
3. Position it where you want the player to deliver the battery (e.g., at the base of the lighthouse, or at a pedestal nearby).

### Fields — Umbra | Battery

| Field | Description |
|---|---|
| **Linked Lighthouse** | **Required.** Click the dropdown or use the eyedropper to select which `BP_Light_House` instance this drop-off activates. If not set, the drop-off will accept the battery but nothing will activate (a warning is logged). |

### Behavior

- When the player carrying a battery walks into the drop-off zone, the battery is **automatically delivered**.
- The battery detaches from the player, returns to its **original size**, and is placed at the center of the drop-off zone.
- The battery **cannot be picked up again** after delivery.
- The linked lighthouse **activates immediately** upon delivery.
- Each drop-off zone accepts **only one battery** — subsequent deliveries are ignored.

### Components

| Component | Description |
|---|---|
| **DropOffZone** (root) | Box collision trigger (120x120x20 UU). The delivery happens when the player overlaps this box. |
| **DropOffMesh** | Optional visual mesh to indicate the drop-off area to the player. Set this in the Blueprint's Components panel. |

### Customizing the Mesh

1. Open `BP_Battery_Dropoff` in the Blueprint Editor.
2. Select `DropOffMesh` in the Components panel.
3. Set **Static Mesh** to a pedestal, platform, or marker mesh.
4. Adjust scale and position as needed.

---

## Wiring It All Together

Here is a step-by-step checklist for setting up a complete lighthouse puzzle:

1. **Place `BP_Light_House`** in the level. Configure its rotation/translation/spotlight settings.
2. **Place `BP_Battery`** somewhere the player can reach it. Set the visual mesh.
3. **Place `BP_Battery_Dropoff`** near the lighthouse. Set the visual mesh.
4. **Select the drop-off instance** in the viewport. In the Details panel under **Umbra | Battery**, set **Linked Lighthouse** to the lighthouse instance from step 1.
5. **Play-test:**
   - Walk the pawn over the battery — it should shrink and float above the player.
   - Walk into the drop-off zone — the battery should appear at the drop-off at full size, and the lighthouse should activate (light turns on, beam starts sweeping).
6. **Iterate** on spotlight angles, rotation speed, and translation to get the shadow puzzle difficulty right.

### Quick Debugging

- Check **Auto Activate** on the lighthouse to test shadow behavior without placing a battery/drop-off.
- Watch the **Output Log** (Window > Developer Tools > Output Log) for messages prefixed with `LogUmbra` — these log pickup, delivery, and activation events.
- Use the debug draw lines (visible in PIE) to verify the spotlight is being traced correctly by the shadow system: green lines = line of sight to light, red lines = blocked (in shadow).

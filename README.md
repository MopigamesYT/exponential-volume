# Exponential Volume

A [Geode](https://geode-sdk.org/) mod for Geometry Dash that makes the Music and SFX volume sliders exponential instead of linear.

## Why?

GD's default linear volume sliders mean that most of the perceptible volume range is crammed into the first ~20% of the slider. An exponential curve spreads it evenly, giving you much finer control at low volumes.

## Settings

- **Exponent** (1.0 - 5.0, default 3.0): Controls the curve steepness. Set to 1.0 for vanilla linear behavior.
- **Enable Logging**: Log volume changes to the Geode console for debugging.

## Building

```bash
geode build
```

Or use CMake directly:

```bash
cmake -B build -DGEODE_SDK=$GEODE_SDK
cmake --build build
```

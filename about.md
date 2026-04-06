# Exponential Volume

Makes the Music and SFX volume sliders use an exponential curve instead of linear scaling.

## Why?

GD's default linear volume sliders mean that most of the perceptible volume range is crammed into the first ~20% of the slider. The difference between 50% and 100% is barely noticeable, while the difference between 0% and 10% is huge.

An exponential curve spreads the perceptible range evenly across the slider, giving you much finer control at low volumes.

## Settings

- **Exponent** (1.0 - 5.0, default 3.0): Controls the curve steepness. Set to 1.0 for vanilla linear behavior.

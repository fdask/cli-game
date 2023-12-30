# HOW TO PLAY

You are mycellium, the fibrous fungus network in the ground.

At this point in time, your mission is to simply stay alive,
and try for a high score.  That, and quests.

## Environment

### Sky

Sky, (~), is where the rain and meteors come from.

The player cannot leave the dirt.

Raindrops are shown in the sky, represented as (|)
Raindrops vary in size, which is transferred into the
dirt when they land.

Meteors are represented in the sky as ¤ character.
They leave a trail as they fall, making it easy to
spot where they will land.

### Dirt

Dirt, (.) makes up the bottom half of the playfield.

Dirt can get wet, shown in the (h2o view) as a number
1-9 indicating how wet it is.

Dirt can also grow plants.  (plant view).
Plants appear on the top layer of soil and are numbers
representing the plant health.   Go too long without 
water, and a plant will die.

Dirt can be toxix to varying degrees, indicated as a (T)
in the plant view.

### Rock

Indicated by a ▙ character (in all views)
Rocks are immovable, and impassable.

### Toxic Waste

Dirt squares become toxic when a meteor crashes into them.
Passing through toxic dirt, harms the player character 
(decreases health), but also clear out the toxicity.

Toxic waste is shown in the plant view as a 'T'.

### Rain

Shown as | in the sky.  Passes moisture to the dirt
it lands on.

### Plants

Plants grow randomly on the top layer of dirt (closest to the sky).

Plants are shown in the plant view as a number from 1-9 representing
the plants health.  Without water, a plant gradually dies,
eventually breaking down into minerals in the dirt.

Every time a plant is restored to full health (by hitting water),
it grows more wise.

At a certain point, a long living plant will become enlightened.
These plants will be shown as glowing (on the plant view).

### Minerals

Minerals, shown in the mineral view as a number from 1-9 representing
the concentration of it, are key to staying alive for long times.

The player absorbs any minerals it touches, turning them points, and
a health bonus.

### Crystals

Crystals appear randomly in the depths of the soil.

Collect as many of them as you can, as they are great
for your score (and health!).

### Portal

Shown on the map as a glowing @, the portal only appears
once all tasks for a level are complete.  See the status
of these tasks on your journal screen 'j'.

## Mechanics

### Rain

Rain falls down from the sky.
When it hits the ground, the dirt gets wet, indicated on the h2o view
with a number (moistness) of that dirt.

Rain that encounters minerals, take one mineral and washes it down to the 
next square.

Rain will trickle into the earth until it reaches 0.

### Meteors

Every so often a meteor might fly by and crash!
Meteors crash into the ground, leaving traces of
toxic waste in the soil in it's crash path.

Touching toxic waste will hurt your health a lot!

### Health

Your health decrements an amount equal to your size, 
every single time you move.  The longer you are, the
quicker your health depletes!

Game over when health reaches 0.

The main way of keeping your health up, is collecting minerals
(shown in the mineral view as a number representing mineral
concentration).

As well, each crystal collected gives a large health bonus.

### Moistness

Mycellium has specific dampness it likes to live at.  Your
character is no different!   Keep your moistness at 0 for
a perfect healthy balance.   Get too dry (every move through
dirt that isn't wet), or get too wet (by collecting rain), and
it's game over for you!

### Soil Toxicity (TW%)

As meteors keep falling to earth, they leave streaks of toxic
waste in their wake.  The more dirt polluted with toxic waste,
the more toxic overall the soil becomes.

### Quests and XP

Check your active quests on the journal screen, (j).
They will be checked off when they are complete.

Complete quests to receive XP and special powerups.

When all objectives for a level are complete, look for
the exit portal, marked with a '@' to proceed.

## Keys

wasd - moves your character a space
WASD - grows your character in the given direction
x - shrink your character by one
t - toggle between showing three screens, or just one
123 - when in single view, toggle which view is shown
      when in triple view, toggle which view is selected
5 - save the game
9 - show the score screen
q - quit the game
zc - scroll the viewport left or right
h - when in single view, toggles visibility of your character
    when in triple view, toggles visibility of your character on
    the selected screen
j - show journal screen
r - reverses your head and tail
? - show help screen
n - new game

## Views

1. h2o

This view shows the wetness of the earth.
A number on a square represents the dampness of that dirt.

2. minerals

Numbers here represent a mineral deposit concentration. 
The higher the number, the more of a health benefit you get!

3. plants and toxic waste

Plants grow randomly on the surface (first layer of dirt),
and are shown in this view as a number 1-9 representing
the plants health.

Go too many turns without rain, and the plant dies (X), 
turning into a mineral deposit.

Toxic waste in this view is shown as a letter T.

## Saving

You can save your current game by pressing the '5' key.
A savegame file will be created on disk.

On startup, if a savegame file is detected, it will be loaded.
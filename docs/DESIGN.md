# Silph Architecture

### `libsilph` library


Goal: Provide an abstraction for a Pokemon, Save File, PC and Items.


#### `scope` CLI

Goal: Simple scriptable CLI to read and write from a PKMN save file (Initial support: Gen 3 - 5)

Design:
```
scope --help
box
  view <ID>
  edit <box> <ID>
  new <species> <box> <id>
party
 view
 edit <ID>
 new <species> <id>
bag
 view <section>
 new <item> <quantity>
```

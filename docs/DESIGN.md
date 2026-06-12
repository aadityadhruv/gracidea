# Silph Architecture

### `libsilph` library


Goal: Provide an abstraction for a Pokemon, Save File, PC and Items, and save
specific interface as well.

When a savefile is loaded the following happens:

1. User provides only path to file
2. Savefile type is autodetected, and the appropriate `SubSaveFile` class is
   returned. However, there is an abstract higher `SaveFile` it can be cast to
4. Provide `SaveFile` object, with `get/set` methods. Provide `Pokemon`, `Item`
   interfaces as well.
5. Each class has save specific methods, that can be called. 

Because a savefile is binary data, any `SubSaveFile` should:

1. Store the state of the save file as is, byte by byte. This is a private
   member
2. Interface with higher level objects allows modifications to private save file
3. This ensures unparsed data isn't affected


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

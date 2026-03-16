# Api Docs

## Config file

first of all, you config file should be named `samp-node.json` and placed in your server root directory

### keys and values

Now there are two ways to configure your samp-node environment

| key          |   type   | value                                                                                         |
| ------------ | :------: | --------------------------------------------------------------------------------------------- |
| `log_level`  | integer  | 1: ERROR, 2: WARNING, 3: DEBUG, 4: INFO. <br /> the higher you set, the more logs you'll see. |
| `entry_file` |  string  | like `dist/bundle.js`                                                                         |
| `node_flags` | string[] | like `["--inspect"]`                                                                          |

examples:

```json
{
  "entry_file": "dist/bundle.js",
  "log_level": 4
}
```

- NOTE: `entry_file` is required, otherwise your resource will fail getting loaded

First of all, you can already use all samp callback names while adding an event listener; so if you don't want to use our predefined definitions, you'll be fine.

## Event functions

- on(eventName, function) `// adds a new listener to the given event`
- addEventListener `// acts like *on*`
- addListener `// acts like *on*`
- removeEventListener(eventName) `// removes all listeners`
- removeEventListener(eventName, function) `// removes specific listener`
- removeEventListener(eventName, functions[]) `// removes array of specific listeners`
- removeListener `// acts like *removeEventListener*`
- registerEvent(callback/eventName, paramTypes) `// register a new event, e.g: from other plugins`

### SAMPNode_CallEvent

This is pawn native, made to be used for calling custom registered **events** in `samp-node` environment, and it calls and passes your args to your event according to specifiers you specified while registering it.

#### NOTE

There's something you should know if you are going to use `a` or `v`, array of integers and array of floats, you **have** to specify the array size in the next parameter, don't worry it gets ignored so you don't need an `i` specifier on event registration, so without passing array size, you'll face a crash. (This doesn't happen with strings since we know the exact size, unlike arrays)

#### Example

JS code:

```js
// note that there's only 2 specifiers, that means this event takes two parameters
// an array of integers, and an integer
samp.registerEvent("MyTestEvent", "ai");
samp.on("MyTestEvent", (array, myint) => {
  console.log(array);
  console.log("My int" + myint);
  return 8;
});
```

Pawn code:

```pawn
new array[8] = {0, 1, 4, 6, 7, 8, 9, 10};
new integer = 4;
new test = SAMPNode_CallEvent("MyTestEvent", array, sizeof(array), integer);
```

## Native caller

```js
samp.callNative(nativeName, paramTypes, args...)
```

a function to call samp natives from your JavaScript code.

### Native usage

```js
samp.callNative('native name here', 'param types (specifiers)', args...);
```

#### Examples

```js
samp.on("OnPlayerConnect", (playerid) => {
  samp.callNative(
    "SendClientMessage",
    "iis",
    playerid,
    0xff00ffff,
    "Welcome to the server player " + playerid
  );
});
```

```js
samp.on("OnPlayerDisconnect", (playerid) => {
  let name = samp.callNative("GetPlayerName", "iSi", playerid, 24);
  samp.callNative(
    "SendClientMessageToAll",
    "is",
    -1,
    "{006600}[JOIN] {ffffff}" + name + " has joined the server"
  );
});
```

### Native specifiers

\***_Note:_** _lower-case specifiers are used for passing values, in order to call a native with reference parameters, use upper-case specifiers, then they will be returned as return value of your call_

| Specifiers | Info                                                                         |
| ---------- | ---------------------------------------------------------------------------- |
| `i, d`     | integer (both do the same thing, both can take unsigned and signed integers) |
| `f`        | float                                                                        |
| `s`        | string                                                                       |
| `a`        | array of integers                                                            |
| `v`        | array of floats                                                              |
| `I, D`     | (Return value) integer                                                       |
| `F`        | (Return value) float                                                         |
| `S`        | (Return value) string                                                        |
| `A`        | (Return value) array of integers                                             |
| `V`        | (Return value) array of floats                                               |

## Public caller

```js
samp.callPublic(publicName, paramTypes, args...)
```

a function to call samp publics registered in all AMX instances from your JavaScript code.

**_Note_**: This wont fire/trigger registered JS events.

### Public usage

```js
samp.callPublic('public name here', 'param types (specifiers)', args...);
```

### Public specifiers

| Specifiers | Info                                                                         |
| ---------- | ---------------------------------------------------------------------------- |
| `i, d`     | integer (both do the same thing, both can take unsigned and signed integers) |
| `f`        | float                                                                        |
| `s`        | string                                                                       |
| `a`        | array of integers                                                            |
| `v`        | array of floats                                                              |
